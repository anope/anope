/* Initialization and related routines.
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "config.h"
#include "users.h"
#include "protocol.h"
#include "bots.h"
#include "xline.h"
#include "socketengine.h"
#include "servers.h"
#include "language.h"

#ifndef _WIN32
#include <sys/wait.h>
#include <sys/stat.h>

#include <cerrno>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#endif
#include <thread>

Anope::string Anope::ConfigDir = "conf", Anope::DataDir = "data", Anope::ModuleDir = "lib", Anope::LocaleDir = "locale", Anope::LogDir = "logs";

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

		CommandLineArguments.emplace_back(option, param);
	}
}

/** Check if an argument was given on startup and its parameter
 * @param name The argument name
 * @param shortname A shorter name, eg --debug and -d
 * @param param A string to put the param, if any, of the argument
 * @return true if name/shortname was found, false if not
 */
static bool GetCommandLineArgument(const Anope::string &name, char shortname, Anope::string &param)
{
	param.clear();

	for (const auto &[argument, value] : CommandLineArguments)
	{
		if (argument.equals_ci(name) || (argument.length() == 1 && argument[0] == shortname))
		{
			param = value;
			return true;
		}
	}

	return false;
}

/** Check if an argument was given on startup
 * @param name The argument name
 * @param shortname A shorter name, eg --debug and -d
 * @return true if name/shortname was found, false if not
 */
static bool GetCommandLineArgument(const Anope::string &name, char shortname = 0)
{
	Anope::string Unused;
	return GetCommandLineArgument(name, shortname, Unused);
}

bool Anope::AtTerm()
{
	return isatty(fileno(stdout)) && isatty(fileno(stdin)) && isatty(fileno(stderr));
}

static void setuidgid();

void Anope::Fork()
{
#ifndef _WIN32
	kill(getppid(), SIGUSR2);

	if (!freopen("/dev/null", "r", stdin))
		Log() << "Unable to redirect stdin to /dev/null: " << Anope::LastError();
	if (!freopen("/dev/null", "w", stdout))
		Log() << "Unable to redirect stdout to /dev/null: " << Anope::LastError();
	if (!freopen("/dev/null", "w", stderr))
		Log() << "Unable to redirect stderr to /dev/null: " << Anope::LastError();

	setpgid(0, 0);

	setuidgid();
#else
	FreeConsole();
#endif
}

void Anope::HandleSignal()
{
	switch (Signal)
	{
		case SIGHUP:
		{
			Anope::SaveDatabases();

			try
			{
				auto *new_config = new Configuration::Conf();
				Configuration::Conf *old = Config;
				Config = new_config;
				Config->Post(old);
				delete old;
			}
			catch (const ConfigException &ex)
			{
				Log() << "Error reloading configuration file: " << ex.GetReason();
			}
			break;
		}
		case SIGTERM:
		case SIGINT:
#ifndef _WIN32
			Log() << "Received " << strsignal(Signal) << " signal (" << Signal << "), exiting.";
			Anope::QuitReason = Anope::string("Services terminating via signal ") + strsignal(Signal) + " (" + Anope::ToString(Signal) + ")";
#else
			Log() << "Received signal " << Signal << ", exiting.";
			Anope::QuitReason = Anope::string("Services terminating via signal ") + Anope::ToString(Signal);
#endif
			Anope::Quitting = true;
			Anope::SaveDatabases();
			break;
	}

	Signal = 0;
}

#ifndef _WIN32
static void parent_signal_handler(int signal)
{
	if (signal == SIGUSR2)
	{
		Anope::Quitting = true;
	}
	else if (signal == SIGCHLD)
	{
		Anope::ReturnValue = -1;
		Anope::Quitting = true;
		int status = 0;
		wait(&status);
		if (WIFEXITED(status))
			Anope::ReturnValue = WEXITSTATUS(status);
	}
}
#endif

static void SignalHandler(int sig)
{
	Anope::Signal = sig;
}

static void InitSignals()
{
	struct sigaction sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sa.sa_handler = SignalHandler;

	sigaction(SIGHUP, &sa, NULL);

	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = SIG_IGN;

#ifndef _WIN32
	sigaction(SIGCHLD, &sa, NULL);
#endif
	sigaction(SIGPIPE, &sa, NULL);
}

/* Remove our PID file.  Done at exit. */

static void remove_pidfile()
{
	remove(Config->GetBlock("serverinfo")->Get<const Anope::string>("pid").c_str());
}

/* Create our PID file and write the PID to it. */

static void write_pidfile()
{
	const auto pidfile = Config->GetBlock("serverinfo")->Get<const Anope::string>("pid");
	std::ofstream stream(pidfile.str());
	if (!stream.is_open())
		throw CoreException("Can not write to PID file " + pidfile);
#ifdef _WIN32
		stream << GetCurrentProcessId() << std::endl;
#else
		stream << getpid() << std::endl;
#endif
	atexit(remove_pidfile);
}

static void setuidgid()
{
#ifndef _WIN32
	Configuration::Block *options = Config->GetBlock("options");
	uid_t uid = -1;
	gid_t gid = -1;

	if (!options->Get<const Anope::string>("user").empty())
	{
		errno = 0;
		struct passwd *u = getpwnam(options->Get<const Anope::string>("user").c_str());
		if (u == NULL)
			Log() << "Unable to setuid to " << options->Get<const Anope::string>("user") << ": " << Anope::LastError();
		else
			uid = u->pw_uid;
	}
	if (!options->Get<const Anope::string>("group").empty())
	{
		errno = 0;
		struct group *g = getgrnam(options->Get<const Anope::string>("group").c_str());
		if (g == NULL)
			Log() << "Unable to setgid to " << options->Get<const Anope::string>("group") << ": " << Anope::LastError();
		else
			gid = g->gr_gid;
	}

	for (const auto &li : Config->LogInfos)
	{
		for (const auto *lf : li.logfiles)
		{
			errno = 0;
			if (chown(lf->filename.c_str(), uid, gid) != 0)
				Log() << "Unable to change the ownership of " << lf->filename << " to " << uid << "/" << gid << ": " << Anope::LastError();
		}
	}

	if (static_cast<int>(gid) != -1)
	{
		if (setgid(gid) == -1)
			Log() << "Unable to setgid to " << options->Get<const Anope::string>("group") << ": " << Anope::LastError();
		else
			Log() << "Successfully set group to " << options->Get<const Anope::string>("group");
	}
	if (static_cast<int>(uid) != -1)
	{
		if (setuid(uid) == -1)
			Log() << "Unable to setuid to " << options->Get<const Anope::string>("user") << ": " << Anope::LastError();
		else
			Log() << "Successfully set user to " << options->Get<const Anope::string>("user");
	}
#endif
}

bool Anope::Init(int ac, char **av)
{
	/* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
	umask(DEFUMASK);
#endif

	Anope::UpdateTime();
	Anope::StartTime = Anope::CurTime;

	Serialize::RegisterTypes();

	/* Parse command line arguments */
	ParseCommandLineArguments(ac, av);

	if (GetCommandLineArgument("version", 'v'))
	{
		Log(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::VersionBuildString();
		Anope::ReturnValue = EXIT_SUCCESS;
		return false;
	}

	if (GetCommandLineArgument("help", 'h'))
	{
		Log(LOG_TERMINAL) << "Anope-" << Anope::Version() << " -- " << Anope::VersionBuildString();
		Log(LOG_TERMINAL) << "Anope IRC Services (https://www.anope.org/)";
		Log(LOG_TERMINAL) << "Usage ./" << Anope::ServicesBin << " [options] ...";
		Log(LOG_TERMINAL) << "-c, --config=filename.conf";
		Log(LOG_TERMINAL) << "    --confdir=conf file directory";
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
		Log(LOG_TERMINAL) << "Further support is available from https://www.anope.org/";
		Log(LOG_TERMINAL) << "Or visit us on IRC at irc.anope.org #anope";
		Anope::ReturnValue = EXIT_SUCCESS;
		return false;
	}

	if (GetCommandLineArgument("nofork", 'n'))
		Anope::NoFork = true;

	if (GetCommandLineArgument("support", 's'))
	{
		Anope::NoFork = Anope::NoThird = true;
		++Anope::Debug;
	}

	if (GetCommandLineArgument("readonly", 'r'))
		Anope::ReadOnly = true;

	if (GetCommandLineArgument("nothird"))
		Anope::NoThird = true;

	if (GetCommandLineArgument("noexpire", 'e'))
		Anope::NoExpire = true;

	if (GetCommandLineArgument("protocoldebug"))
		Anope::ProtocolDebug = true;

	Anope::string arg;
	if (GetCommandLineArgument("debug", 'd', arg))
	{
		if (!arg.empty())
		{
			auto level = Anope::Convert<int>(arg, -1);
			if (level > 0)
				Anope::Debug = level;
			else
				throw CoreException("Invalid option given to --debug");
		}
		else
			++Anope::Debug;
	}

	if (GetCommandLineArgument("config", 'c', arg))
	{
		if (arg.empty())
			throw CoreException("The --config option requires a file name");
		ServicesConf = Configuration::File(arg, false);
	}

	if (GetCommandLineArgument("confdir", 0, arg))
	{
		if (arg.empty())
			throw CoreException("The --confdir option requires a path");
		Anope::ConfigDir = arg;
	}

	if (GetCommandLineArgument("dbdir", 0, arg))
	{
		if (arg.empty())
			throw CoreException("The --dbdir option requires a path");
		Anope::DataDir = arg;
	}

	if (GetCommandLineArgument("localedir", 0, arg))
	{
		if (arg.empty())
			throw CoreException("The --localedir option requires a path");
		Anope::LocaleDir = arg;
	}

	if (GetCommandLineArgument("modulesdir", 0, arg))
	{
		if (arg.empty())
			throw CoreException("The --modulesdir option requires a path");
		Anope::ModuleDir = arg;
	}

	if (GetCommandLineArgument("logdir", 0, arg))
	{
		if (arg.empty())
			throw CoreException("The --logdir option requires a path");
		Anope::LogDir = arg;
	}

	/* Chdir to Anope data directory. */
	if (chdir(Anope::ServicesDir.c_str()) < 0)
	{
		throw CoreException("Unable to chdir to " + Anope::ServicesDir + ": " + Anope::LastError());
	}

	Log(LOG_TERMINAL) << "Anope " << Anope::Version() << ", " << Anope::VersionBuildString();

#ifdef _WIN32
	Log(LOG_TERMINAL) << "Using configuration file " << Anope::ConfigDir << "\\" << ServicesConf.GetName();
#else
	Log(LOG_TERMINAL) << "Using configuration file " << Anope::ConfigDir << "/" << ServicesConf.GetName();

	/* Fork to background */
	if (!Anope::NoFork)
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

			return false;
		}
		else if (i == -1)
		{
			Log() << "Error, unable to fork: " << Anope::LastError();
			Anope::NoFork = true;
		}

		/* Child doesn't need these */
		sigaction(SIGUSR2, &old_sigusr2, NULL);
		sigaction(SIGCHLD, &old_sigchld, NULL);
	}

#endif

	/* Initialize the socket engine. Note that some engines can not survive a fork(), so this must be here. */
	SocketEngine::Init();

	/* Read configuration file; exit if there are problems. */
	try
	{
		Config = new Configuration::Conf();
	}
	catch (const ConfigException &ex)
	{
		Log(LOG_TERMINAL) << ex.GetReason();
		Log(LOG_TERMINAL) << "*** Support resources: Read through the anope.conf self-contained";
		Log(LOG_TERMINAL) << "*** documentation. Read the documentation files found in the 'docs'";
		Log(LOG_TERMINAL) << "*** folder. Visit our portal located at https://www.anope.org/. Join";
		Log(LOG_TERMINAL) << "*** our support channel on /server irc.anope.org channel #anope.";
		throw CoreException("Configuration file failed to validate");
	}

	/* Create me */
	Configuration::Block *block = Config->GetBlock("serverinfo");
	Me = new Server(NULL, block->Get<const Anope::string>("name"), 0, block->Get<const Anope::string>("description"), block->Get<const Anope::string>("id"));
	for (const auto &[_, bi] : *BotListByNick)
	{
		bi->server = Me;
		++Me->users;
	}

	/* Announce ourselves to the logfile. */
	Log() << "Anope " << Anope::Version() << " starting up" << (Anope::Debug || Anope::ReadOnly ? " (options:" : "") << (Anope::Debug ? " debug" : "") << (Anope::ReadOnly ? " readonly" : "") << (Anope::Debug || Anope::ReadOnly ? ")" : "");

	InitSignals();

	/* Initialize multi-language support */
	Language::InitLanguages();

	/* load modules */
	Log() << "Loading modules...";
	for (int i = 0; i < Config->CountBlock("module"); ++i)
		ModuleManager::LoadModule(Config->GetBlock("module", i)->Get<const Anope::string>("name"), NULL);

#ifndef _WIN32
	/* If we're root, issue a warning now */
	if (!getuid() && !getgid())
	{
		/* If we are configured to setuid later, don't issue a warning */
		Configuration::Block *options = Config->GetBlock("options");
		if (options->Get<const Anope::string>("user").empty())
		{
			std::cerr << "WARNING: You are currently running Anope as the root superuser. Anope does not" << std::endl;
			std::cerr << "         require root privileges to run, and it is discouraged that you run Anope" << std::endl;
			std::cerr << "         as the root superuser." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}

	/* We won't background later, so we should setuid now */
	if (Anope::NoFork)
		setuidgid();
#endif

	auto *encryption = ModuleManager::FindFirstOf(ENCRYPTION);
	if (!encryption)
		throw CoreException("You must load an encryption module!");

	auto *protocol = ModuleManager::FindFirstOf(PROTOCOL);
	if (!protocol)
		throw CoreException("You must load a protocol module!");

	/* Write our PID to the PID file. */
	write_pidfile();

	Log() << "Using IRCd protocol " << protocol->name;

	/* Auto assign sid if applicable */
	if (IRCD->RequiresID)
	{
		Anope::string sid = IRCD->SID_Retrieve();
		if (Me->GetSID() == Me->GetName())
			Me->SetSID(sid);
		for (const auto &[_, bi] : *BotListByNick)
			bi->GenerateUID();
	}

	/* Load up databases */
	Log() << "Loading databases...";
	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnLoadDatabase, MOD_RESULT, ());
	static_cast<void>(MOD_RESULT);
	Log() << "Databases loaded";

	FOREACH_MOD(OnPostInit, ());

	for (const auto &[_, ci] : ChannelList)
		ci->Sync();

	Serialize::CheckTypes();
	return true;
}
