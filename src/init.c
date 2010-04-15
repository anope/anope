/* Initalization and related routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "pseudo.h"
Uplink *uplink_server;

extern void moduleAddMsgs();
extern void moduleAddIRCDMsgs();

/*************************************************************************/

void introduce_user(const std::string &user)
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
	BotInfo *bi;
	int i;

	/* XXX: it might be nice to have this inside BotInfo's constructor, or something? */
	for (i = 0; i < 256; ++i)
	{
		for (bi = botlists[i]; bi; bi = bi->next)
		{
			ci::string ci_bi_nick(bi->nick.c_str());
			if (user.empty() || ci_bi_nick == user)
				ircdproto->SendClientIntroduction(bi->nick, bi->user, bi->host, bi->real, ircd->pseudoclient_mode, bi->uid);
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
	while ((gr = getgrent()) != NULL) {
		if (strcmp(gr->gr_name, RUNGROUP) == 0)
			break;
	}
	endgrent();
	if (gr) {
		setgid(gr->gr_gid);
		return 0;
	} else {
		Alog() << "Unknown group `" << RUNGROUP << "'";
		return -1;
	}
#else
	return 0;
#endif
}

/*************************************************************************/

/* Vector of pairs of command line arguments and their params */
static std::vector<std::pair<std::string, std::string> > CommandLineArguments;

/** Called on startup to organize our starting arguments in a better way
 * and check for errors
 * @param ac number of args
 * @param av args
 */
static void ParseCommandLineArguments(int ac, char **av)
{
	for (int i = 1; i < ac; ++i)
	{
		std::string option = av[i];
		std::string param = "";
		while (!option.empty() && option[0] == '-')
			option.erase(option.begin());
		size_t t = option.find('=');
		if (t != std::string::npos)
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
bool GetCommandLineArgument(const std::string &name, char shortname)
{
	std::string Unused;
	return GetCommandLineArgument(name, shortname, Unused);
}

/** Check if an argument was given on startup and its parameter
 * @param name The argument name
 * @param shortname A shorter name, eg --debug and -d
 * @param param A string to put the param, if any, of the argument
 * @return true if name/shortname was found, false if not
 */
bool GetCommandLineArgument(const std::string &name, char shortname, std::string &param)
{
	param.clear();
	
	for (std::vector<std::pair<std::string, std::string> >::iterator it = CommandLineArguments.begin(); it != CommandLineArguments.end(); ++it)
	{
		if (it->first == name || it->first[0] == shortname)
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
	remove(Config.PIDFilename);
}

/*************************************************************************/

/* Create our PID file and write the PID to it. */

static void write_pidfile()
{
	FILE *pidfile;

	pidfile = fopen(Config.PIDFilename, "w");
	if (pidfile) {
#ifdef _WIN32
		fprintf(pidfile, "%d\n", static_cast<int>(GetCurrentProcessId()));
#else
		fprintf(pidfile, "%d\n", static_cast<int>(getpid()));
#endif
		fclose(pidfile);
		atexit(remove_pidfile);
	} else {
		log_perror("Warning: cannot write to PID file %s", Config.PIDFilename);
	}
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
		Alog(LOG_TERMINAL) << "Anope-" << version_number << version_flags << " -- " << version_build;
		return -1;
	}

	if (GetCommandLineArgument("help", 'h'))
	{
		Alog(LOG_TERMINAL) << "Anope-" << version_number << version_flags << " -- " << version_build;
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
	{
		nofork = 1;
	}

	if (GetCommandLineArgument("support", 's'))
	{
		nofork = nothird = 1;
		++debug;
	}

	if (GetCommandLineArgument("readonly", 'r'))
	{
		readonly = 1;
	}

	if (GetCommandLineArgument("nothird"))
	{
		nothird = 1;
	}

	if (GetCommandLineArgument("noexpire", 'e'))
	{
		noexpire = 1;
	}

	if (GetCommandLineArgument("protocoldebug"))
	{
		protocoldebug = 1;
	}

	std::string Arg;
	if (GetCommandLineArgument("debug", 'd', Arg))
	{
		 if (!Arg.empty())
		 {
		 	int level = atoi(Arg.c_str());
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
	if (chdir(services_dir.c_str()) < 0) {
		fprintf(stderr, "chdir(%s): %s\n", services_dir.c_str(), strerror(errno));
		return -1;
	}

	/* Open logfile, and complain if we didn't. */
	if (open_log() < 0) {
		openlog_errno = errno;
		if (started_from_term) {
			fprintf(stderr, "Warning: unable to open log file %s: %s\n",
					log_filename.c_str(), strerror(errno));
		} else {
			openlog_failed = 1;
		}
	}

	/* Read configuration file; exit if there are problems. */
	if (!read_config(0)) {
		return -1;
	}

	/* Add IRCD Protocol Module; exit if there are errors */
	if (protocol_module_init()) {
		return -1;
	}

	/* Add Encryption Modules */
	ModuleManager::LoadModuleList(Config.EncModuleList);

	/* Add Database Modules */
	ModuleManager::LoadModuleList(Config.DBModuleList);

	return 0;
}

int init_secondary(int ac, char **av)
{
#ifndef _WIN32
	int i;
	int started_from_term = isatty(0) && isatty(1) && isatty(2);
#endif

	/* Add Core MSG handles */
	moduleAddMsgs();

#ifndef _WIN32
	if (!nofork) {
		if ((i = fork()) < 0) {
			perror("fork()");
			return -1;
		} else if (i != 0) {
			Alog(LOG_TERMINAL) << "PID " << i;
			exit(0);
		}
		if (started_from_term) {
			close(0);
			close(1);
			close(2);
		}
		if (setpgid(0, 0) < 0) {
			perror("setpgid()");
			return -1;
		}
	}
#else
	if (!SupportedWindowsVersion()) {

		char *winver = GetWindowsVersion();

		Alog() << winver << " is not a supported version of Windows";

		delete [] winver;

		return -1;

	}
	if (!nofork) {
		Alog(LOG_TERMINAL) << "PID " << GetCurrentProcessId();
		Alog() << "Launching Anope into the background";
		FreeConsole();
	}
#endif

	/* Write our PID to the PID file. */
	write_pidfile();

	/* Announce ourselves to the logfile. */
	Alog() << "Anope " << version_number << " (ircd protocol: " << version_protocol << ") starting up" 
		<< (debug || readonly ? " (options:" : "") << (debug ? " debug" : "") 
		<< (readonly ? " readonly" : "") << (debug || readonly ? ")" : "");
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

	// XXX: this is duplicated in type loading.
	for (std::list<std::pair<std::string, std::string> >::iterator it = Config.Opers.begin(); it != Config.Opers.end(); it++)
	{
		std::string nick = it->first;
		std::string type = it->second;

		NickAlias *na = findnick(nick);
		if (!na)
		{
			// Nonexistant nick
			Alog() << "Oper nick '" << nick << "' is not registered";
			continue;
		}

		if (!na->nc)
		{
			// Nick with no core (wtf?)
			abort();
		}

		for (std::list<OperType *>::iterator tit = Config.MyOperTypes.begin(); tit != Config.MyOperTypes.end(); tit++)
		{
			OperType *ot = *tit;
			if (ot->GetName() == type)
			{
				Alog() << "Tied oper " << na->nc->display << " to type " << type;
				na->nc->ot = ot;
			}
		}
	}
	// END DUPLICATION


	/* this is only used on the first run of Anope. */
	if (!nbots)
	{
		if (Config.s_OperServ)
			new BotInfo(Config.s_OperServ, Config.ServiceUser, Config.ServiceHost, Config.desc_OperServ);
		if (Config.s_NickServ)
			new BotInfo(Config.s_NickServ, Config.ServiceUser, Config.ServiceHost, Config.desc_NickServ);
		if (Config.s_ChanServ)
			new BotInfo(Config.s_ChanServ, Config.ServiceUser, Config.ServiceHost, Config.desc_ChanServ);
		if (Config.s_HostServ)
			new BotInfo(Config.s_HostServ, Config.ServiceUser, Config.ServiceHost, Config.desc_HostServ);
		if (Config.s_MemoServ)
			new BotInfo(Config.s_MemoServ, Config.ServiceUser, Config.ServiceHost, Config.desc_MemoServ);
		if (Config.s_BotServ)
			new BotInfo(Config.s_BotServ, Config.ServiceUser, Config.ServiceHost, Config.desc_BotServ);
		if (Config.s_GlobalNoticer)
			new BotInfo(Config.s_GlobalNoticer, Config.ServiceUser, Config.ServiceHost, Config.desc_GlobalNoticer);
	}

	FOREACH_MOD(I_OnPostLoadDatabases, OnPostLoadDatabases());

	return 0;
}

/*************************************************************************/
