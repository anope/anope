/* Initalization and related routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

Uplink *uplink_server;

extern void moduleAddMsgs();
extern void moduleAddIRCDMsgs();

/*************************************************************************/

void introduce_user(const Anope::string &user)
{
	/* Watch out for infinite loops... */
#define LTSIZE 20
	static int lasttimes[LTSIZE];
	if (lasttimes[0] >= time(NULL) - 3)
		fatal("introduce_user() loop detected");
	memmove(lasttimes, lasttimes + 1, sizeof(lasttimes) - sizeof(int));
	lasttimes[LTSIZE - 1] = time(NULL);
#undef LTSIZE
	/* We make the bots go online */

	/* XXX: it might be nice to have this inside BotInfo's constructor, or something? */
	for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
	{
		BotInfo *bi = it->second;

		if (user.empty() || bi->nick.equals_ci(user))
		{
			ircdproto->SendClientIntroduction(bi->nick, bi->GetIdent(), bi->host, bi->realname, ircd->pseudoclient_mode, bi->GetUID());
			XLine x(bi->nick, "Reserved for services");
			ircdproto->SendSQLine(&x);
		}
	}
}

/*************************************************************************/

/* Set GID if necessary.  Return 0 if successful (or if RUNGROUP not
 * defined), else print an error message to logfile and return -1.
 */

static int set_group()
{
#if defined(RUNGROUP) && defined(HAVE_SETGRENT)
	struct group *gr;

	setgrent();
	while ((gr = getgrent()))
	{
		if (!strcmp(gr->gr_name, RUNGROUP))
			break;
	}
	endgrent();
	if (gr)
	{
		setgid(gr->gr_gid);
		return 0;
	}
	else
	{
		Alog() << "Unknown group `" << RUNGROUP << "'";
		return -1;
	}
#else
	return 0;
#endif
}

/*************************************************************************/

/* Vector of pairs of command line arguments and their params */
static std::vector<std::pair<Anope::string, Anope::string> > CommandLineArguments;

/** Called on startup to organize our starting arguments in a better way
 * and check for errors
 * @param ac number of args
 * @param av args
 */
static void ParseCommandLineArguments(int ac, char **av)
{
	for (int i = 1; i < ac; ++i)
	{
		Anope::string option = av[i];
		Anope::string param;
		while (!option.empty() && option[0] == '-')
			option.erase(option.begin());
		size_t t = option.find('=');
		if (t != Anope::string::npos)
		{
			param = option.substr(t + 1);
			option.erase(t);
		}

		if (option.empty())
			continue;

		CommandLineArguments.push_back(std::make_pair(option, param));
	}
}

/** Check if an argument was given on startup
 * @param name The argument name
 * @param shortname A shorter name, eg --debug and -d
 * @return true if name/shortname was found, false if not
 */
bool GetCommandLineArgument(const Anope::string &name, char shortname)
{
	Anope::string Unused;
	return GetCommandLineArgument(name, shortname, Unused);
}

/** Check if an argument was given on startup and its parameter
 * @param name The argument name
 * @param shortname A shorter name, eg --debug and -d
 * @param param A string to put the param, if any, of the argument
 * @return true if name/shortname was found, false if not
 */
bool GetCommandLineArgument(const Anope::string &name, char shortname, Anope::string &param)
{
	param.clear();

	for (std::vector<std::pair<Anope::string, Anope::string> >::iterator it = CommandLineArguments.begin(), it_end = CommandLineArguments.end(); it != it_end; ++it)
	{
		if (it->first.equals_ci(name) || it->first[0] == shortname)
		{
			param = it->second;
			return true;
		}
	}

	return false;
}

/*************************************************************************/

/* Remove our PID file.  Done at exit. */

static void remove_pidfile()
{
	remove(Config.PIDFilename.c_str());
}

/*************************************************************************/

/* Create our PID file and write the PID to it. */

static void write_pidfile()
{
	FILE *pidfile;

	pidfile = fopen(Config.PIDFilename.c_str(), "w");
	if (pidfile)
	{
#ifdef _WIN32
		fprintf(pidfile, "%d\n", static_cast<int>(GetCurrentProcessId()));
#else
		fprintf(pidfile, "%d\n", static_cast<int>(getpid()));
#endif
		fclose(pidfile);
		atexit(remove_pidfile);
	}
	else
		log_perror("Warning: cannot write to PID file %s", Config.PIDFilename.c_str());
}

/*************************************************************************/

/* Overall initialization routines.  Return 0 on success, -1 on failure. */

int openlog_failed = 0, openlog_errno = 0;

int init_primary(int ac, char **av)
{
	int started_from_term = isatty(0) && isatty(1) && isatty(2);

	/* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
	umask(DEFUMASK);
#endif
	if (set_group() < 0)
		return -1;

	/* Parse command line arguments */
	ParseCommandLineArguments(ac, av);

	if (GetCommandLineArgument("version", 'v'))
	{
		Alog(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::Build();
		return -1;
	}

	if (GetCommandLineArgument("help", 'h'))
	{
		Alog(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::Build();
		Alog(LOG_TERMINAL) << "Anope IRC Services (http://www.anope.org)";
		Alog(LOG_TERMINAL) << "Usage ./" << SERVICES_BIN << " [options] ...";
		Alog(LOG_TERMINAL) << "-c, --config=filename.conf";
		Alog(LOG_TERMINAL) << "-d, --debug[=level]";
		Alog(LOG_TERMINAL) << "    --dir=services_directory";
		Alog(LOG_TERMINAL) << "-h, --help";
		Alog(LOG_TERMINAL) << "    --log=log_filename";
		Alog(LOG_TERMINAL) << "-e, --noexpire";
		Alog(LOG_TERMINAL) << "-n, --nofork";
		Alog(LOG_TERMINAL) << "    --nothird";
		Alog(LOG_TERMINAL) << "    --protocoldebug";
		Alog(LOG_TERMINAL) << "-r, --readonly";
		Alog(LOG_TERMINAL) << "-s, --support";
		Alog(LOG_TERMINAL) << "-v, --version";
		Alog(LOG_TERMINAL) << "";
		Alog(LOG_TERMINAL) << "Further support is available from http://www.anope.org";
		Alog(LOG_TERMINAL) << "Or visit us on IRC at irc.anope.org #anope";
		return -1;
	}

	if (GetCommandLineArgument("nofork", 'n'))
		nofork = true;

	if (GetCommandLineArgument("support", 's'))
	{
		nofork = nothird = true;
		++debug;
	}

	if (GetCommandLineArgument("readonly", 'r'))
		readonly = true;

	if (GetCommandLineArgument("nothird"))
		nothird = true;

	if (GetCommandLineArgument("noexpire", 'e'))
		noexpire = true;

	if (GetCommandLineArgument("protocoldebug"))
		protocoldebug = true;

	Anope::string Arg;
	if (GetCommandLineArgument("debug", 'd', Arg))
	{
		if (!Arg.empty())
		{
			int level = Arg.is_number_only() ? convertTo<int>(Arg) : -1;
			if (level > 0)
				debug = level;
			else
			{
				Alog(LOG_TERMINAL) << "Invalid option given to --debug";
				return -1;
			}
		}
		else
			++debug;
	}

	if (GetCommandLineArgument("config", 'c', Arg))
	{
		if (Arg.empty())
		{
			Alog(LOG_TERMINAL) << "The --config option requires a file name";
			return -1;
		}
		services_conf = Arg;
	}

	if (GetCommandLineArgument("dir", 0, Arg))
	{
		if (Arg.empty())
		{
			Alog(LOG_TERMINAL) << "The --dir option requires a directory name";
			return -1;
		}
		services_dir = Arg;
	}

	if (GetCommandLineArgument("log", 0, Arg))
	{
		if (Arg.empty())
		{
			Alog(LOG_TERMINAL) << "The --log option requires a file name";
			return -1;
		}
		log_filename = Arg;
	}

	/* Chdir to Services data directory. */
	if (chdir(services_dir.c_str()) < 0)
	{
		fprintf(stderr, "chdir(%s): %s\n", services_dir.c_str(), strerror(errno));
		return -1;
	}

	/* Open logfile, and complain if we didn't. */
	if (open_log() < 0)
	{
		openlog_errno = errno;
		if (started_from_term)
			fprintf(stderr, "Warning: unable to open log file %s: %s\n", log_filename.c_str(), strerror(errno));
		else
			openlog_failed = 1;
	}

	/* Read configuration file; exit if there are problems. */
	if (!read_config(0))
		return -1;

	/* Add IRCD Protocol Module; exit if there are errors */
	if (protocol_module_init())
		return -1;

	/* Create me */
	Me = new Server(NULL, Config.ServerName, 0, Config.ServerDesc, Config.Numeric);

	/* First thing, add our core bots internally. Before modules are loaded and before the database is read
	 * This is used for modules adding commands and for the BotInfo* poiners in the command classes.
	 * When these bots are loaded from the databases the proper user/host/rname are added.
	 *
	 * If a user renames a bot in the configuration file, the new bot gets created here, and the old bot
	 * that is in the database gets created aswell, on its old nick. The old nick remains in all the channels
	 * etc and the new bot becomes the new client to accept commands. The user can use /bs bot del later
	 * if they want the old bot deleted.
	 *
	 * Note that it is important this is after loading the protocol module. The ircd struct must exist for
	 * the ts6_ functions
	 */
	if (!Config.s_OperServ.empty())
		new BotInfo(Config.s_OperServ, Config.ServiceUser, Config.ServiceHost, Config.desc_OperServ);
	if (!Config.s_NickServ.empty())
		new BotInfo(Config.s_NickServ, Config.ServiceUser, Config.ServiceHost, Config.desc_NickServ);
	if (!Config.s_ChanServ.empty())
		new BotInfo(Config.s_ChanServ, Config.ServiceUser, Config.ServiceHost, Config.desc_ChanServ);
	if (!Config.s_HostServ.empty())
		new BotInfo(Config.s_HostServ, Config.ServiceUser, Config.ServiceHost, Config.desc_HostServ);
	if (!Config.s_MemoServ.empty())
		new BotInfo(Config.s_MemoServ, Config.ServiceUser, Config.ServiceHost, Config.desc_MemoServ);
	if (!Config.s_BotServ.empty())
		new BotInfo(Config.s_BotServ, Config.ServiceUser, Config.ServiceHost, Config.desc_BotServ);
	if (!Config.s_GlobalNoticer.empty())
		new BotInfo(Config.s_GlobalNoticer, Config.ServiceUser, Config.ServiceHost, Config.desc_GlobalNoticer);

	/* Add Encryption Modules */
	ModuleManager::LoadModuleList(Config.EncModuleList);

	/* Add Database Modules */
	ModuleManager::LoadModuleList(Config.DBModuleList);

	/* Load the socket engine */
	if (ModuleManager::LoadModule(Config.SocketEngine, NULL))
	{
		Alog(LOG_TERMINAL) << "Unable to load socket engine " << Config.SocketEngine;
		return -1;
	}

	return 0;
}

int init_secondary(int ac, char **av)
{
#ifndef _WIN32
	int started_from_term = isatty(0) && isatty(1) && isatty(2);
#endif

	/* Add Core MSG handles */
	moduleAddMsgs();

#ifndef _WIN32
	if (!nofork)
	{
		int i;
		if ((i = fork()) < 0)
		{
			perror("fork()");
			return -1;
		}
		else if (i != 0)
		{
			Alog(LOG_TERMINAL) << "PID " << i;
			exit(0);
		}
		if (started_from_term)
		{
			close(0);
			close(1);
			close(2);
		}
		if (setpgid(0, 0) < 0)
		{
			perror("setpgid()");
			return -1;
		}
	}
#else
	if (!SupportedWindowsVersion())
	{
		Alog() << GetWindowsVersion() << " is not a supported version of Windows";

		return -1;
	}
	if (!nofork)
	{
		Alog(LOG_TERMINAL) << "PID " << GetCurrentProcessId();
		Alog() << "Launching Anope into the background";
		FreeConsole();
	}
#endif

	/* Write our PID to the PID file. */
	write_pidfile();

	/* Announce ourselves to the logfile. */
	Alog() << "Anope " << Anope::Version() << " (ircd protocol: " << version_protocol << ") starting up" << (debug || readonly ? " (options:" : "") << (debug ? " debug" : "") << (readonly ? " readonly" : "") << (debug || readonly ? ")" : "");
	start_time = time(NULL);

	/* If in read-only mode, close the logfile again. */
	if (readonly)
		close_log();

	/* Set signal handlers.  Catch certain signals to let us do things or
	 * panic as necessary, and ignore all others.
	 */
#ifndef _WIN32
	signal(SIGHUP, sighandler);
#endif
	signal(SIGTERM, sighandler);
	signal(SIGINT, sighandler);

	/* Initialize multi-language support */
	lang_init();
	Alog(LOG_DEBUG) << "Loaded languages";

	/* Initialize subservices */
	ns_init();
	cs_init();
	ms_init();
	bs_init();
	os_init();
	hostserv_init();

	/* load any custom modules */
	if (!nothird)
		ModuleManager::LoadModuleList(Config.ModulesAutoLoad);

	/* Initialize random number generator */
	rand_init();
	add_entropy_userkeys();

	/* Load up databases */
	Alog() << "Loading databases...";
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnLoadDatabase, OnLoadDatabase());
	Alog() << "Databases loaded";

	FOREACH_MOD(I_OnPostLoadDatabases, OnPostLoadDatabases());

	return 0;
}

/*************************************************************************/
