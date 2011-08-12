/* Services -- main source file.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "services.h"
#include "timers.h"
#include "modules.h"

// getrlimit.
#ifndef _WIN32
# include <sys/time.h>
# include <sys/resource.h>
#endif

/******** Global variables! ********/

/* Command-line options: (note that configuration variables are in config.c) */
Anope::string services_dir;	/* -dir dirname */
Anope::string services_bin;	/* Binary as specified by the user */
Anope::string orig_cwd;		/* Original current working directory */
int debug = 0;				/* -debug */
bool readonly = false;		/* -readonly */
bool nofork = false;		/* -nofork */
bool nothird = false;		/* -nothrid */
bool noexpire = false;		/* -noexpire */
bool protocoldebug = false;	/* -protocoldebug */

Anope::string binary_dir; /* Used to store base path for Anope */
#ifdef _WIN32
# include <process.h>
# define execve _execve
#endif

/* Set to 1 if we are to quit */
bool quitting = false;

/* Set to true if we are restarting */
bool restarting = false;

/* Contains a message as to why services is terminating */
Anope::string quitmsg;

/* At what time were we started? */
time_t start_time = time(NULL);

time_t Anope::CurTime = time(NULL);

/******** Local variables! ********/

/* Set to 1 after we've set everything up */
static bool started = false;

/*************************************************************************/

class UpdateTimer : public Timer
{
 public:
	UpdateTimer(time_t timeout) : Timer(timeout, Anope::CurTime, true) { }

	void Tick(time_t)
	{
		save_databases();
	}
};

ConnectionSocket *UplinkSock = NULL;
int CurrentUplink = 0;

static void Connect();

class ReconnectTimer : public Timer
{
 public:
	ReconnectTimer(int wait) : Timer(wait) { }

	void Tick(time_t)
	{
		try
		{
			Connect();
		}
		catch (const SocketException &ex)
		{
			quitmsg = ex.GetReason();
			quitting = true;
		}
	}
};

class UplinkSocket : public ConnectionSocket
{
 public:
	UplinkSocket() : ConnectionSocket(Config->Uplinks[CurrentUplink]->ipv6)
	{
		UplinkSock = this;
	}

	~UplinkSocket()
	{
		if (Me && !Me->GetLinks().empty() && Me->GetLinks()[0]->IsSynced())
		{
			FOREACH_MOD(I_OnServerDisconnect, OnServerDisconnect());

			for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u = it->second;

				if (u->server == Me)
					/* Don't use quitmsg here, it may contain information you don't want people to see */
					ircdproto->SendQuit(u, "Shutting down");
			}

			ircdproto->SendSquit(Config->ServerName, quitmsg);

			this->ProcessWrite(); // Write out the last bit
		}

		for (unsigned i = Me->GetLinks().size(); i > 0; --i)
			if (!Me->GetLinks()[i - 1]->HasFlag(SERVER_JUPED))
				Me->GetLinks()[i - 1]->Delete(Me->GetName() + " " + Me->GetLinks()[i - 1]->GetName());

		UplinkSock = NULL;

		Me->SetFlag(SERVER_SYNCING);

		if (!quitting)
		{
			int Retry = Config->RetryWait;
			if (Retry <= 0)
				Retry = 60;

			Log() << "Retrying in " << Retry << " seconds";
			new ReconnectTimer(Retry);
		}
	}

	bool Read(const Anope::string &buf)
	{
		process(buf);
		return true;
	}

	void OnConnect()
	{
		Log() << "Successfully connected to " << Config->Uplinks[CurrentUplink]->host << ":" << Config->Uplinks[CurrentUplink]->port;
		ircdproto->SendConnect();
		FOREACH_MOD(I_OnServerConnect, OnServerConnect());
	}

	void OnError(const Anope::string &error)
	{
		Log() << "Unable to connect to server " << Config->Uplinks[CurrentUplink]->host << ":" << Config->Uplinks[CurrentUplink]->port << (!error.empty() ? (": " + error) : "");
		this->SetFlag(SF_DEAD);
	}
};

static void Connect()
{
	if (static_cast<unsigned>(++CurrentUplink) >= Config->Uplinks.size())
		CurrentUplink = 0;

	Uplink *u = Config->Uplinks[CurrentUplink];

	new UplinkSocket();
	if (!Config->LocalHost.empty())
		UplinkSock->Bind(Config->LocalHost);
	FOREACH_MOD(I_OnPreServerConnect, OnPreServerConnect());
	UplinkSock->Connect(u->host, u->port);
}

/*************************************************************************/

void save_databases()
{
	if (readonly)
		return;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnSaveDatabase, OnSaveDatabase());
	Log(LOG_DEBUG) << "Saving databases";
}

/*************************************************************************/

std::vector<Signal *> Signal::SignalHandlers;

void Signal::SignalHandler(int signal)
{
	for (unsigned i = 0, j = SignalHandlers.size(); i < j; ++i)
		if (SignalHandlers[i]->signal == signal)
			SignalHandlers[i]->called = true;
}

void Signal::Process()
{
	for (unsigned i = 0, j = SignalHandlers.size(); i < j; ++i)
		if (SignalHandlers[i]->called == true)
		{
			Signal *s = SignalHandlers[i];
			s->called = false;
			s->OnSignal();
		}
}

Signal::Signal(int s) : called(false), signal(s)
{
	memset(&this->old, 0, sizeof(this->old));

	this->action.sa_flags = 0;
	sigemptyset(&this->action.sa_mask);
	this->action.sa_handler = SignalHandler;
	
	if (sigaction(s, &this->action, &this->old) == -1)
		throw CoreException("Unable to install signal " + stringify(s) + ": " + Anope::LastError());

	SignalHandlers.push_back(this);
}

Signal::~Signal()
{
	std::vector<Signal *>::iterator it = std::find(SignalHandlers.begin(), SignalHandlers.end(), this);
	if (it != SignalHandlers.end())
		SignalHandlers.erase(it);
	
	sigaction(this->signal, &this->old, NULL);
}


/*************************************************************************/

/** The following comes from InspIRCd to get the full path of the Anope executable
 */
Anope::string GetFullProgDir(const Anope::string &argv0)
{
	char buffer[PATH_MAX];
#ifdef _WIN32
	/* Windows has specific API calls to get the EXE path that never fail.
	 * For once, Windows has something of use, compared to the POSIX code
	 * for this, this is positively neato.
	 */
	if (GetModuleFileName(NULL, buffer, PATH_MAX))
	{
		Anope::string fullpath = buffer;
		Anope::string::size_type n = fullpath.rfind("\\");
		services_bin = fullpath.substr(n + 1, fullpath.length());
		return fullpath.substr(0, n);
	}
#else
	// Get the current working directory
	if (getcwd(buffer, PATH_MAX))
	{
		Anope::string remainder = argv0;

		services_bin = remainder;
		Anope::string::size_type n = services_bin.rfind("/");
		Anope::string fullpath;
		if (services_bin[0] == '/')
			fullpath = services_bin.substr(0, n);
		else
			fullpath = Anope::string(buffer) + "/" + services_bin.substr(0, n);
		services_bin = services_bin.substr(n + 1, remainder.length());
		return fullpath;
	}
#endif
	return "/";
}

/*************************************************************************/

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
{
	int return_code = 0;

	char cwd[PATH_MAX] = "";
#ifdef _WIN32
	GetCurrentDirectory(PATH_MAX, cwd);
#else
	getcwd(cwd, PATH_MAX);
#endif
	orig_cwd = cwd;

#ifndef _WIN32
	/* If we're root, issue a warning now */
	if (!getuid() && !getgid())
	{
		fprintf(stderr, "WARNING: You are currently running Anope as the root superuser. Anope does not\n");
		fprintf(stderr, "    require root privileges to run, and it is discouraged that you run Anope\n");
		fprintf(stderr, "    as the root superuser.\n");
	}
#endif

	binary_dir = GetFullProgDir(av[0]);
	if (binary_dir[binary_dir.length() - 1] == '.')
		binary_dir = binary_dir.substr(0, binary_dir.length() - 2);
#ifdef _WIN32
	Anope::string::size_type n = binary_dir.rfind('\\');
	services_dir = binary_dir.substr(0, n) + "\\data";
#else
	Anope::string::size_type n = binary_dir.rfind('/');
	services_dir = binary_dir.substr(0, n) + "/data";
#endif

	/* Clean out the module runtime directory prior to running, just in case files were left behind during a previous run */
	ModuleManager::CleanupRuntimeDirectory();

	try
	{
		/* General initialization first */
		Init(ac, av);
	}
	catch (const FatalException &ex)
	{
		Log() << ex.GetReason();
		return -1;
	}

	try
	{
		Connect();
	}
	catch (const SocketException &ex)
	{
		quitmsg = ex.GetReason();
		quitting = true;
		return_code = -1;
	}

	started = true;

	/* Set up timers */
	time_t last_check = Anope::CurTime;
	UpdateTimer updateTimer(Config->UpdateTimeout);

	/*** Main loop. ***/
	while (!quitting)
	{
		Log(LOG_DEBUG_2) << "Top of main loop";

		/* Process signals */
		Signal::Process();

		/* Process timers */
		if (Anope::CurTime - last_check >= Config->TimeoutCheck)
		{
			TimerManager::TickTimers(Anope::CurTime);
			last_check = Anope::CurTime;
		}

		/* Free up any finished threads */
		threadEngine.Process();

		/* Process any modes that need to be (un)set */
		if (Me->IsSynced())
			ModeManager::ProcessModes();

		/* Process the socket engine */
		SocketEngine::Process();
	}

	if (restarting)
	{
		FOREACH_MOD(I_OnRestart, OnRestart());
	}
	else
	{
		FOREACH_MOD(I_OnShutdown, OnShutdown());
	}

	if (quitmsg.empty())
		quitmsg = "Terminating, reason unknown";
	Log() << quitmsg;

	delete UplinkSock;

	ModuleManager::UnloadAll();
	SocketEngine::Shutdown();
	for (Module *m; (m = ModuleManager::FindFirstOf(PROTOCOL)) != NULL;)
		ModuleManager::UnloadModule(m, NULL);

	ModuleManager::CleanupRuntimeDirectory();

#ifdef _WIN32
	OnShutdown();
#endif

	if (restarting)
	{
		chdir(binary_dir.c_str());
		av[0] = const_cast<char *>(("./" + services_bin).c_str());
		execve(services_bin.c_str(), av, envp);
		Log() << "Restart failed";
		return_code = -1;
	}

	return return_code;
}
