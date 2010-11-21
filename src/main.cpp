/* Services -- main source file.
 *
 * (C) 2003-2010 Anope Team
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
#include "version.h"

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

/* Set to 1 if we are to quit after saving databases */
bool shutting_down = false;

/* Contains a message as to why services is terminating */
Anope::string quitmsg;

/* Should we update the databases now? */
bool save_data = false;

/* At what time were we started? */
time_t start_time;

/* Parameters and environment */
char **my_av, **my_envp;

time_t Anope::CurTime = time(NULL);

/******** Local variables! ********/

/* Set to 1 after we've set everything up */
static bool started = false;

/*************************************************************************/

class ExpireTimer : public Timer
{
 public:
	ExpireTimer(time_t timeout, time_t now) : Timer(timeout, now, true) { }

	void Tick(time_t)
	{
		if (!readonly && !noexpire)
			expire_all();
	}
};

class UpdateTimer : public Timer
{
 public:
	UpdateTimer(time_t timeout, time_t now) : Timer(timeout, now, true) { }

	void Tick(time_t)
	{
		if (!readonly)
			save_databases();
	}
};

ConnectionSocket *UplinkSock = NULL;

UplinkSocket::UplinkSocket(bool ipv6) : ConnectionSocket(ipv6)
{
	UplinkSock = this;
}

UplinkSocket::~UplinkSocket()
{
	UplinkSock = NULL;
}

bool UplinkSocket::Read(const Anope::string &buf)
{
	process(buf);
	return true;
}

/*************************************************************************/

/* Run expiration routines */

extern void expire_all()
{
	if (noexpire || readonly)
		// Definitely *do not* want.
		return;

	FOREACH_MOD(I_OnPreDatabaseExpire, OnPreDatabaseExpire());

	Log(LOG_DEBUG) << "Running expire routines";
	expire_nicks();
	expire_chans();
	expire_requests();
	expire_exceptions();

	FOREACH_MOD(I_OnDatabaseExpire, OnDatabaseExpire());
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

/* Restarts services */
void do_restart_services()
{
	if (!readonly)
	{
		expire_all();
		save_databases();
	}
	Log() << "Restarting";

	FOREACH_MOD(I_OnPreRestart, OnPreRestart());

	if (quitmsg.empty())
		quitmsg = "Restarting";
	/* Send a quit for all of our bots */
	for (patricia_tree<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
	{
		BotInfo *bi = *it;

		/* Don't use quitmsg here, it may contain information you don't want people to see */
		ircdproto->SendQuit(bi, "Restarting");
		/* Erase bots from the user list so they don't get nuked later on */
		UserListByNick.erase(bi->nick);
		if (!bi->GetUID().empty())
			UserListByUID.erase(bi->GetUID());
	}
	ircdproto->SendSquit(Config->ServerName, quitmsg);
	SocketEngine->Process();
	delete UplinkSock;
	ModuleManager::UnloadAll();
	chdir(binary_dir.c_str());
	my_av[0] = const_cast<char *>(("./" + services_bin).c_str());
	execve(services_bin.c_str(), my_av, my_envp);
	if (!readonly)
	{
		throw FatalException("Restart failed");
	}

	FOREACH_MOD(I_OnRestart, OnRestart());

	exit(1);
}

/*************************************************************************/

/* Terminates services */

static void services_shutdown()
{
	FOREACH_MOD(I_OnPreShutdown, OnPreShutdown());

	if (quitmsg.empty())
		quitmsg = "Terminating, reason unknown";
	Log() << quitmsg;
	if (started && UplinkSock)
	{
		/* Send a quit for all of our bots */
		for (patricia_tree<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = *it;

			/* Don't use quitmsg here, it may contain information you don't want people to see */
			ircdproto->SendQuit(bi, "Shutting down");
			/* Erase bots from the user list so they don't get nuked later on */
			UserListByNick.erase(bi->nick);
			if (!bi->GetUID().empty())
				UserListByUID.erase(bi->GetUID());
		}

		ircdproto->SendSquit(Config->ServerName, quitmsg);

		while (!UserListByNick.empty())
			delete UserListByNick.front();
	}
	SocketEngine->Process();
	delete UplinkSock;
	FOREACH_MOD(I_OnShutdown, OnShutdown());
	ModuleManager::UnloadAll();
	/* just in case they weren't all removed at least run once */
	ModuleRunTimeDirCleanUp();
}

/*************************************************************************/

/* If we get a weird signal, come here. */

void sighandler(int signum)
{
	if (quitmsg.empty())
#ifndef _WIN32
		quitmsg = Anope::string("Services terminating via signal ") + strsignal(signum) + " (" + stringify(signum) + ")";
#else
		quitmsg = Anope::string("Services terminating via signal ") + stringify(signum);
#endif
	bool fatal = false;

	if (started)
	{
		switch (signum)
		{
#ifndef _WIN32
			case SIGHUP:
				Log() << "Received SIGHUP: Saving databases & rehashing configuration";

				expire_all();
				save_databases();

				try
				{
					ServerConfig *newconfig = new ServerConfig();
					delete Config;
					Config = newconfig;
					FOREACH_MOD(I_OnReload, OnReload(true));
				}
				catch (const ConfigException &ex)
				{
					Log() << "Error reloading configuration file: " << ex.GetReason();
				}
				break;
#endif
			case SIGINT:
			case SIGTERM:
				signal(signum, SIG_IGN);
#ifndef _WIN32
				signal(SIGHUP, SIG_IGN);
#endif

#ifndef _WIN32
				Log() << "Received " << strsignal(signum) << " signal (" << signum << "), exiting.";
#else
				Log() << "Received signal " << signum << ", exiting.";
#endif

				if (Config->GlobalOnCycle)
					oper_global("", "%s", Config->GlobalOnCycleMessage.c_str());

				expire_all();
				save_databases();
			default:
				fatal = true;
				break;
		}
	}

	FOREACH_MOD(I_OnSignal, OnSignal(signum, quitmsg));

	if (fatal)
		throw FatalException(quitmsg);
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

static bool Connect()
{
	/* Connect to the remote server */
	int servernum = 1;
	for (std::list<Uplink *>::iterator curr_uplink = Config->Uplinks.begin(), end_uplink = Config->Uplinks.end(); curr_uplink != end_uplink; ++curr_uplink, ++servernum)
	{
		uplink_server = *curr_uplink;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreServerConnect, OnPreServerConnect(*curr_uplink, servernum));
		if (MOD_RESULT != EVENT_CONTINUE)
		{
			if (MOD_RESULT == EVENT_STOP)
				continue;
			return true;
		}

		try
		{
			new UplinkSocket(uplink_server->ipv6);
			UplinkSock->Connect(uplink_server->host, uplink_server->port, Config->LocalHost);
		}
		catch (const SocketException &ex)
		{
			Log() << "Unable to connect to server" << servernum << " (" << uplink_server->host << ":" << uplink_server->port << "), " << ex.GetReason();
			continue;
		}

		Log() << "Connected to server " << servernum << " (" << uplink_server->host << ":" << uplink_server->port << ")";
		return true;
	}

	uplink_server = NULL;

	return false;
}

/*************************************************************************/

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
{
	try
	{
		my_av = av;
		my_envp = envp;

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
		ModuleRunTimeDirCleanUp();

		/* General initialization first */
		Init(ac, av);

		/* If the first connect fails give up, don't sit endlessly trying to reconnect */
		if (!Connect())
		{
			Log() << "Can't connect to any servers";
			return 0;
		}

		ircdproto->SendConnect();
		FOREACH_MOD(I_OnServerConnect, OnServerConnect());

		started = true;

		/* Set up timers */
		time_t last_check = Anope::CurTime;
		ExpireTimer expireTimer(Config->ExpireTimeout, Anope::CurTime);
		UpdateTimer updateTimer(Config->UpdateTimeout, Anope::CurTime);

		/*** Main loop. ***/
		while (!quitting)
		{
			while (!quitting && UplinkSock)
			{
				Log(LOG_DEBUG_2) << "Top of main loop";

				if (!readonly && (save_data || shutting_down))
				{
					if (!noexpire)
						expire_all();
					if (shutting_down)
						ircdproto->SendGlobops(NULL, "Updating databases on shutdown, please wait.");
					save_databases();
					save_data = false;
				}

				if (shutting_down)
				{
					quitting = true;
					break;
				}

				if (Anope::CurTime - last_check >= Config->TimeoutCheck)
				{
					TimerManager::TickTimers(Anope::CurTime);
					last_check = Anope::CurTime;
				}

				/* Free up any finished threads */
				threadEngine.Process();

				/* Process any modes that need to be (un)set */
				ModeManager::ProcessModes();

				/* Process the socket engine */
				SocketEngine->Process();
			}

			if (quitting)
				/* Disconnect and exit */
				services_shutdown();
			else
			{
				FOREACH_MOD(I_OnServerDisconnect, OnServerDisconnect());

				/* Clear all of our users, but not our bots */
				for (patricia_tree<User *>::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end;)
				{
					User *u = *it;
					++it;

					if (u->server != Me)
						delete u;
				}

				Me->SetFlag(SERVER_SYNCING);
				for (unsigned i = Me->GetLinks().size(); i > 0; --i)
					if (!Me->GetLinks()[i - 1]->HasFlag(SERVER_JUPED))
						delete Me->GetLinks()[i - 1];

				unsigned j = 0;
				for (; j < (Config->MaxRetries ? Config->MaxRetries : j + 1); ++j)
				{
					Log() << "Disconnected from the server, retrying in " << Config->RetryWait << " seconds";

					sleep(Config->RetryWait);
					if (Connect())
					{
						ircdproto->SendConnect();
						FOREACH_MOD(I_OnServerConnect, OnServerConnect());
						break;
					}
				}
				if (Config->MaxRetries && j == Config->MaxRetries)
				{
					Log() << "Max connection retry limit exceeded";
					quitting = true;
				}
			}
		}
	}
	catch (const FatalException &ex)
	{
		if (!ex.GetReason().empty())
			Log(LOG_TERMINAL) << ex.GetReason();
		if (started)
			services_shutdown();
		return -1;
	}

	return 0;
}

Anope::string Anope::Version()
{
	return stringify(VERSION_MAJOR) + "." + stringify(VERSION_MINOR) + "." + stringify(VERSION_PATCH) + VERSION_EXTRA + " (" + stringify(VERSION_BUILD) + ")";
}

Anope::string Anope::Build()
{
	return "build #" + stringify(BUILD) + ", compiled " + Anope::compiled;
}


