/* Initalization and related routines.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "config.h"
#include "extern.h"
#include "users.h"
#include "protocol.h"
#include "bots.h"
#include "oper.h"
#include "signals.h"
#include "socketengine.h"
#include "servers.h"

#ifndef _WIN32
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <grp.h>
#endif

Anope::string conf_dir = "conf", db_dir = "data", modules_dir = "lib", locale_dir = "locale", log_dir = "logs";

ServerConfig::Uplink *uplink_server;

void introduce_user(const Anope::string &user)
{
	/* Watch out for infinite loops... */
	time_t now = Anope::CurTime;
	static time_t lasttime = now - 4;
	if (lasttime >= now - 3)
	{
		quitmsg = "introduce_user loop detected";
		quitting = true;
		return;
	}
	lasttime = now;

	User *u = finduser(user);
	if (u)
	{
		ircdproto->SendClientIntroduction(u);

		BotInfo *bi = findbot(u->nick);
		if (bi)
		{
			bi->introduced = true;

			XLine x(bi->nick, "Reserved for services");
			ircdproto->SendSQLine(NULL, &x);

			for (UChannelList::const_iterator cit = bi->chans.begin(), cit_end = bi->chans.end(); cit != cit_end; ++cit)
				ircdproto->SendJoin(bi, (*cit)->chan, &Config->BotModeList);
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
	}
	else
	{
		Log() << "Unknown run group '" << RUNGROUP << "'";
		return -1;
	}
#endif
	return 0;
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
		if (it->first.equals_ci(name) || (it->first.length() == 1 && it->first[0] == shortname))
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
	remove(Config->PIDFilename.c_str());
}

/*************************************************************************/

/* Create our PID file and write the PID to it. */

static void write_pidfile()
{
	FILE *pidfile = fopen(Config->PIDFilename.c_str(), "w");
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
		throw FatalException("Can not write to PID file " + Config->PIDFilename);
}

/*************************************************************************/

class SignalReload : public Signal
{
 public:
	SignalReload(int sig) : Signal(sig) { }

	void OnNotify()
	{
		Log() << "Received SIGHUP: Saving databases & rehashing configuration";

		save_databases();

		ServerConfig *old_config = Config;
		try
		{
			Config = new ServerConfig();
			FOREACH_MOD(I_OnReload, OnReload());
			delete old_config;
		}
		catch (const ConfigException &ex)
		{
			Config = old_config;
			Log() << "Error reloading configuration file: " << ex.GetReason();
		}
	}
};

class SignalExit : public Signal
{
 public:
	SignalExit(int sig) : Signal(sig) { }

	void OnNotify()
	{
#ifndef _WIN32
		Log() << "Received " << strsignal(this->signal) << " signal (" << this->signal << "), exiting.";
		quitmsg = Anope::string("Services terminating via signal ") + strsignal(this->signal) + " (" + stringify(this->signal) + ")";
#else
		Log() << "Received signal " << this->signal << ", exiting.";
		quitmsg = Anope::string("Services terminating via signal ") + stringify(this->signal);
#endif

		quitting = true;
		save_databases();
	}
};

class SignalNothing : public Signal
{
 public:
	SignalNothing(int sig) : Signal(sig) { }

	void OnNotify() { }
};

bool AtTerm()
{
	return isatty(fileno(stdout)) && isatty(fileno(stdin)) && isatty(fileno(stderr));
}

void Fork()
{
#ifndef _WIN32
	kill(getppid(), SIGUSR2);
	
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	setpgid(0, 0);
#else
	FreeConsole();
#endif
}

#ifndef _WIN32
static void parent_signal_handler(int signal)
{
	if (signal == SIGUSR2)
	{
		quitting = true;
		return_code = 0;
	}
	else if (signal == SIGCHLD)
	{
		quitting = true;
		return_code = -1;
		int status = 0;
		wait(&status);
		if (WIFEXITED(status))
			return_code = WEXITSTATUS(status);
	}
}
#endif

void Init(int ac, char **av)
{
	/* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
	umask(DEFUMASK);
#endif
	if (set_group() < 0)
		throw FatalException("set_group() fail");
	
	RegisterTypes();

	/* Parse command line arguments */
	ParseCommandLineArguments(ac, av);

	if (GetCommandLineArgument("version", 'v'))
	{
		Log(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::VersionBuildString();
		throw FatalException();
	}

	if (GetCommandLineArgument("help", 'h'))
	{
		Log(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::VersionBuildString();
		Log(LOG_TERMINAL) << "Anope IRC Services (http://www.anope.org)";
		Log(LOG_TERMINAL) << "Usage ./" << services_bin << " [options] ...";
		Log(LOG_TERMINAL) << "-c, --config=filename.conf";
		Log(LOG_TERMINAL) << "    --confdir=conf file direcory";
		Log(LOG_TERMINAL) << "    --dbdir=database directory";
		Log(LOG_TERMINAL) << "-d, --debug[=level]";
		Log(LOG_TERMINAL) << "-h, --help";
		Log(LOG_TERMINAL) << "    --localedir=locale directory";
		Log(LOG_TERMINAL) << "    --logdir=logs directory";
		Log(LOG_TERMINAL) << "    --modulesdir=modules directory";
		Log(LOG_TERMINAL) << "-e, --noexpire";
		Log(LOG_TERMINAL) << "-n, --nofork";
		Log(LOG_TERMINAL) << "    --nothird";
		Log(LOG_TERMINAL) << "    --protocoldebug";
		Log(LOG_TERMINAL) << "-r, --readonly";
		Log(LOG_TERMINAL) << "-s, --support";
		Log(LOG_TERMINAL) << "-v, --version";
		Log(LOG_TERMINAL) << "";
		Log(LOG_TERMINAL) << "Further support is available from http://www.anope.org";
		Log(LOG_TERMINAL) << "Or visit us on IRC at irc.anope.org #anope";
		throw FatalException();
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
				throw FatalException("Invalid option given to --debug");
		}
		else
			++debug;
	}

	if (GetCommandLineArgument("config", 'c', Arg))
	{
		if (Arg.empty())
			throw FatalException("The --config option requires a file name");
		services_conf = ConfigurationFile(Arg, false);
	}

	if (GetCommandLineArgument("confdir", 0, Arg))
	{
		if (Arg.empty())
			throw FatalException("The --confdir option requires a path");
		conf_dir = Arg;
	}

	if (GetCommandLineArgument("dbdir", 0, Arg))
	{
		if (Arg.empty())
			throw FatalException("The --dbdir option requires a path");
		db_dir = Arg;
	}

	if (GetCommandLineArgument("localedir", 0, Arg))
	{
		if (Arg.empty())
			throw FatalException("The --localedir option requires a path");
		locale_dir = Arg;
	}

	if (GetCommandLineArgument("modulesdir", 0, Arg))
	{
		if (Arg.empty())
			throw FatalException("The --modulesdir option requires a path");
		modules_dir = Arg;
	}

	if (GetCommandLineArgument("logdir", 0, Arg))
	{
		if (Arg.empty())
			throw FatalException("The --logdir option requires a path");
		log_dir = Arg;
	}

	/* Chdir to Services data directory. */
	if (chdir(services_dir.c_str()) < 0)
	{
		throw FatalException("Unable to chdir to " + services_dir + ": " + Anope::LastError());
	}

	Log(LOG_TERMINAL) << "Anope " << Anope::Version() << ", " << Anope::VersionBuildString();
#ifdef _WIN32
	Log(LOG_TERMINAL) << "Using configuration file " << conf_dir << "\\" << services_conf.GetName();
#else
	Log(LOG_TERMINAL) << "Using configuration file " << conf_dir << "/" << services_conf.GetName();
#endif

	/* Read configuration file; exit if there are problems. */
	try
	{
		Config = new ServerConfig();
	}
	catch (const ConfigException &ex)
	{
		Log(LOG_TERMINAL) << ex.GetReason();
		Log(LOG_TERMINAL) << "*** Support resources: Read through the services.conf self-contained";
		Log(LOG_TERMINAL) << "*** documentation. Read the documentation files found in the 'docs'";
		Log(LOG_TERMINAL) << "*** folder. Visit our portal located at http://www.anope.org/. Join";
		Log(LOG_TERMINAL) << "*** our support channel on /server irc.anope.org channel #anope.";
		throw FatalException("Configuration file failed to validate");
	}

#ifdef _WIN32
	if (!SupportedWindowsVersion())
		throw FatalException(GetWindowsVersion() + " is not a supported version of Windows");
#else
	if (!nofork && AtTerm())
	{
		/* Install these before fork() - it is possible for the child to
		 * connect and kill() the parent before it is able to install the
		 * handler.
		 */
		struct sigaction sa, old_sigusr2, old_sigchld;

		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sa.sa_handler = parent_signal_handler;

		sigaction(SIGUSR2, &sa, &old_sigusr2);
		sigaction(SIGCHLD, &sa, &old_sigchld);

		int i = fork();
		if (i > 0)
		{
			sigset_t mask;

			sigemptyset(&mask);
			sigsuspend(&mask);

			exit(return_code);
		}
		else if (i == -1)
		{
			Log() << "Error, unable to fork: " << Anope::LastError();
			nofork = true;
		}

		/* Child doesn't need these */
		sigaction(SIGUSR2, &old_sigusr2, NULL);
		sigaction(SIGCHLD, &old_sigchld, NULL);
	}
#endif

	/* Initialize the socket engine */
	SocketEngine::Init();

	/* Write our PID to the PID file. */
	write_pidfile();

	/* Create me */
	Me = new Server(NULL, Config->ServerName, 0, Config->ServerDesc, Config->Numeric);
	for (botinfo_map::const_iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
		it->second->server = Me;

	/* Announce ourselves to the logfile. */
	Log() << "Anope " << Anope::Version() << " starting up" << (debug || readonly ? " (options:" : "") << (debug ? " debug" : "") << (readonly ? " readonly" : "") << (debug || readonly ? ")" : "");

	new SignalReload(SIGHUP);
	new SignalExit(SIGTERM);
	new SignalExit(SIGINT);
	new SignalNothing(SIGPIPE);

	/* Initialize multi-language support */
	Log(LOG_DEBUG) << "Loading Languages...";
	InitLanguages();

	/* Initialize random number generator */
	srand(Config->Seed);

	/* load modules */
	Log() << "Loading modules...";
	for (std::list<Anope::string>::iterator it = Config->ModulesAutoLoad.begin(), it_end = Config->ModulesAutoLoad.end(); it != it_end; ++it)
		ModuleManager::LoadModule(*it, NULL);

	Module *protocol = ModuleManager::FindFirstOf(PROTOCOL);
	if (protocol == NULL)
		throw FatalException("You must load a protocol module!");
	else if (ModuleManager::FindFirstOf(ENCRYPTION) == NULL)
		throw FatalException("You must load at least one encryption module");

	Log() << "Using IRCd protocol " << protocol->name;

	/* Load up databases */
	Log() << "Loading databases...";
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnLoadDatabase, OnLoadDatabase());
	Log() << "Databases loaded";
}

/*************************************************************************/
