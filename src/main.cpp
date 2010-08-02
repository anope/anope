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
Anope::string log_filename = "services.log"; /* -log filename */
int debug = 0;				/* -debug */
bool readonly = false;		/* -readonly */
bool LogChan = false;		/* -logchan */
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

Socket *UplinkSock = NULL;

class UplinkSocket : public ClientSocket
{
 public:
	UplinkSocket(const Anope::string &nTargetHost, int nPort, const Anope::string &nBindHost = "", bool nIPv6 = false) : ClientSocket(nTargetHost, nPort, nBindHost, nIPv6)
	{
		UplinkSock = this;
	}

	~UplinkSocket()
	{
		UplinkSock = NULL;
	}

	bool Read(const Anope::string &buf)
	{
		process(buf);
		return true;
	}
};

/*************************************************************************/

/* Run expiration routines */

extern void expire_all()
{
	if (noexpire || readonly)
		// Definitely *do not* want.
		return;

	FOREACH_MOD(I_OnPreDatabaseExpire, OnPreDatabaseExpire());

	Alog(LOG_DEBUG) << "Running expire routines";
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
	Alog(LOG_DEBUG) << "Saving FFF databases";
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
	Alog() << "Restarting";

	FOREACH_MOD(I_OnPreRestart, OnPreRestart());

	if (quitmsg.empty())
		quitmsg = "Restarting";
	/* Send a quit for all of our bots */
	for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
	{
		/* Don't use quitmsg here, it may contain information you don't want people to see */
		ircdproto->SendQuit(it->second, "Restarting");
		/* Erase bots from the user list so they don't get nuked later on */
		UserListByNick.erase(it->second->nick);
		if (!it->second->GetUID().empty())
			UserListByUID.erase(it->second->GetUID());
	}
	ircdproto->SendSquit(Config.ServerName, quitmsg);
	SocketEngine->Process();
	delete UplinkSock;
	close_log();
	/* First don't unload protocol module, then do so */
	ModuleManager::UnloadAll(false);
	ModuleManager::UnloadAll(true);
	chdir(binary_dir.c_str());
	execve(services_bin.c_str(), my_av, my_envp);
	if (!readonly)
	{
		open_log();
		log_perror("Restart failed");
		close_log();
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
	Alog() << quitmsg;
	if (started && UplinkSock)
	{
		/* Send a quit for all of our bots */
		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			/* Don't use quitmsg here, it may contain information you don't want people to see */
			ircdproto->SendQuit(it->second, "Shutting down");
			/* Erase bots from the user list so they don't get nuked later on */
			UserListByNick.erase(it->second->nick);
			if (!it->second->GetUID().empty())
				UserListByUID.erase(it->second->GetUID());
		}

		ircdproto->SendSquit(Config.ServerName, quitmsg);

		while (!UserListByNick.empty())
			delete UserListByNick.begin()->second;
	}
	SocketEngine->Process();
	delete UplinkSock;
	FOREACH_MOD(I_OnShutdown, OnShutdown());
	/* First don't unload protocol module, then do so */
	ModuleManager::UnloadAll(false);
	ModuleManager::UnloadAll(true);
	/* just in case they weren't all removed at least run once */
	ModuleRunTimeDirCleanUp();
}

/*************************************************************************/

/* If we get a weird signal, come here. */

void sighandler(int signum)
{
	/* We set the quit message to something default, just to be sure it is
	 * always set when we need it. It seems some signals slip through to the
	 * QUIT code without having a valid quitmsg. -GD
	 */
	if (quitmsg.empty())
		quitmsg = "Services terminating via a signal.";

	if (started)
	{
#ifndef _WIN32
		if (signum == SIGHUP)
		{
			Alog() << "Received SIGHUP: Saving Databases & Rehash Configuration";

			expire_all();
			save_databases();

			if (!read_config(1))
			{
				quitmsg = "Error Reading Configuration File (Received SIGHUP)";
				quitting = true;
			}

			FOREACH_MOD(I_OnReload, OnReload(true));
			return;

		}
		else
#endif
		if (signum == SIGTERM)
		{
			signal(SIGTERM, SIG_IGN);
#ifndef _WIN32
			signal(SIGHUP, SIG_IGN);
#endif

			Alog() << "Received SIGTERM, exiting.";

			expire_all();
			save_databases();
			quitmsg = "Shutting down on SIGTERM";
			services_shutdown();
			exit(0);
		}
		else if (signum == SIGINT)
		{
			if (nofork)
			{
				signal(SIGINT, SIG_IGN);
				Alog() << "Received SIGINT, exiting.";
				expire_all();
				save_databases();
				quitmsg = "Shutting down on SIGINT";
				services_shutdown();
				exit(0);
			}
		}
	}

	/* Should we send the signum here as well? -GD */
	FOREACH_MOD(I_OnSignal, OnSignal(quitmsg));

	if (started)
	{
		services_shutdown();
		exit(0);
	}
	else
	{
		if (isatty(2))
			fprintf(stderr, "%s\n", quitmsg.c_str());
		else
			Alog() << quitmsg;

		exit(1);
	}
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
		Anope::string::size_type n = fullpath.rfind("\\" SERVICES_BIN);
		services_bin = fullpath.substr(n + 1, fullpath.length());
		return fullpath.substr(0, n);
	}
#else
	// Get the current working directory
	if (getcwd(buffer, PATH_MAX))
	{
		Anope::string remainder = argv0;

		/* Does argv[0] start with /? If so, it's a full path, use it */
		if (remainder[0] == '/')
		{
			Anope::string::size_type n = remainder.rfind("/" SERVICES_BIN);
			services_bin = remainder.substr(n + 1, remainder.length());
			return remainder.substr(0, n);
		}

		services_bin = remainder;
		if (services_bin.substr(0, 2).equals_cs("./"))
			services_bin = services_bin.substr(2);
		Anope::string fullpath = Anope::string(buffer) + "/" + remainder;
		Anope::string::size_type n = fullpath.rfind("/" SERVICES_BIN);
		return fullpath.substr(0, n);
	}
#endif
	return "/";
}

/*************************************************************************/

static bool Connect()
{
	/* Connect to the remote server */
	int servernum = 1;
	for (std::list<Uplink *>::iterator curr_uplink = Config.Uplinks.begin(), end_uplink = Config.Uplinks.end(); curr_uplink != end_uplink; ++curr_uplink, ++servernum)
	{
		uplink_server = *curr_uplink;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreServerConnect, OnPreServerConnect(*curr_uplink, servernum));
		if (MOD_RESULT != EVENT_CONTINUE)
		{
			if (MOD_RESULT == EVENT_STOP)
				break;
			return true;
		}

		try
		{
			new UplinkSocket(uplink_server->host, uplink_server->port, Config.LocalHost, uplink_server->ipv6);
		}
		catch (const SocketException &ex)
		{
			Alog() << "Unable to connect to server" << servernum << " (" << uplink_server->host << ":" << uplink_server->port << "), " << ex.GetReason();
			continue;
		}

		Alog() << "Connected to Server " << servernum << " (" << uplink_server->host << ":" << uplink_server->port << ")";
		return true;
	}

	uplink_server = NULL;

	return false;
}

/*************************************************************************/

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
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
	int i = init_primary(ac, av);
	if (i)
		return i;

	Alog(LOG_TERMINAL) << "Anope " << Anope::Version() << ", " << Anope::Build();
#ifdef _WIN32
	Alog(LOG_TERMINAL) << "Using configuration file " << services_dir << "\\" << services_conf;
#else
	Alog(LOG_TERMINAL) << "Using configuration file " << services_dir << "/" << services_conf;
#endif

	/* Initialization stuff. */
	i = init_secondary(ac, av);
	if (i)
		return i;

	/* If the first connect fails give up, don't sit endlessly trying to reconnect */
	if (!Connect())
		fatal_perror("Can't connect to any servers");

	ircdproto->SendConnect();
	FOREACH_MOD(I_OnServerConnect, OnServerConnect());

	started = true;

#ifndef _WIN32
	if (Config.DumpCore)
	{
		rlimit rl;
		if (getrlimit(RLIMIT_CORE, &rl) == -1)
			Alog() << "Failed to getrlimit()!";
		else
		{
			rl.rlim_cur = rl.rlim_max;
			if (setrlimit(RLIMIT_CORE, &rl) == -1)
				Alog() << "setrlimit() failed, cannot increase coredump size";
		}
	}
#endif

	/* Set up timers */
	time_t last_check = time(NULL);
	ExpireTimer expireTimer(Config.ExpireTimeout, last_check);
	UpdateTimer updateTimer(Config.UpdateTimeout, last_check);

	/*** Main loop. ***/
	while (!quitting)
	{
		while (!quitting && UplinkSock)
		{
			time_t t = time(NULL);

			Alog(LOG_DEBUG_2) << "Top of main loop";

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

			if (t - last_check >= Config.TimeoutCheck)
			{
				TimerManager::TickTimers(t);
				last_check = t;
			}

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
			for (user_map::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; )
			{
				User *u = it->second;
				++it;

				if (!findbot(u->nick))
					delete u;
			}

			/* Nuke all channels */
			for (channel_map::const_iterator it = ChannelList.begin(); it != ChannelList.end();)
			{
				Channel *c = it->second;
				++it;

				delete c;
			}

			Me->SetFlag(SERVER_SYNCING);
			Me->ClearLinks();

			unsigned j = 0;
			for (; j < (Config.MaxRetries ? Config.MaxRetries : j + 1); ++j)
			{
				Alog() << "Disconnected from the server, retrying in " << Config.RetryWait << " seconds";

				sleep(Config.RetryWait);
				if (Connect())
				{
					ircdproto->SendConnect();
					FOREACH_MOD(I_OnServerConnect, OnServerConnect());
					break;
				}
			}
			if (Config.MaxRetries && j == Config.MaxRetries)
			{
				Alog() << "Max connection retry limit exceeded";
				quitting = true;
			}
		}
	}

	return 0;
}

inline Anope::string Anope::Version()
{
	return stringify(VERSION_MAJOR) + "." + stringify(VERSION_MINOR) + "." + stringify(VERSION_PATCH) + VERSION_EXTRA + " (" + stringify(VERSION_BUILD) + ")";
}

inline Anope::string Anope::Build()
{
	return "build #" + stringify(BUILD) + ", compiled " + Anope::compiled;
}
