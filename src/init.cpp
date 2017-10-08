/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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
#include "event.h"
#include "modules.h"

#ifndef _WIN32
#include <sys/wait.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#endif

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

		CommandLineArguments.push_back(std::make_pair(option, param));
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

	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

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
			try
			{
				Configuration::Conf *new_config = new Configuration::Conf();
				Configuration::Conf *old = Config;
				Config = new_config;
				Config->Post(old);
				delete old;
			}
			catch (const ConfigException &ex)
			{
				Anope::Logger.Log("Error reloading configuration file: {0}", ex.GetReason());
			}
			break;
		}
		case SIGTERM:
		case SIGINT:
#ifndef _WIN32
			Anope::Logger.Log(_("Received {0} signal ({1}), exiting"), strsignal(Signal), Signal);
			Anope::QuitReason = Anope::string("Services terminating via signal ") + strsignal(Signal) + " (" + stringify(Signal) + ")";
#else
			Anope::Logger.Log(_("Received signal {0}, exiting"), Signal);
			Anope::QuitReason = "Services terminating via signal " + stringify(Signal);
#endif
			Anope::Quitting = true;
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
	remove(Config->GetBlock("serverinfo")->Get<Anope::string>("pid").c_str());
}

/* Create our PID file and write the PID to it. */

static void write_pidfile()
{
	FILE *pidfile = fopen(Config->GetBlock("serverinfo")->Get<Anope::string>("pid").c_str(), "w");
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
		throw CoreException("Can not write to PID file " + Config->GetBlock("serverinfo")->Get<Anope::string>("pid"));
}

static void setuidgid()
{
#ifndef _WIN32
	Configuration::Block *options = Config->GetBlock("options");
	uid_t uid = -1;
	gid_t gid = -1;

	if (!options->Get<Anope::string>("user").empty())
	{
		errno = 0;
		struct passwd *u = getpwnam(options->Get<Anope::string>("user").c_str());
		if (u == NULL)
			Anope::Logger.Log(_("Unable to setuid to {0}: {1}"), options->Get<Anope::string>("user"), Anope::LastError());
		else
			uid = u->pw_uid;
	}
	if (!options->Get<Anope::string>("group").empty())
	{
		errno = 0;
		struct group *g = getgrnam(options->Get<Anope::string>("group").c_str());
		if (g == NULL)
			Anope::Logger.Log(_("Unable to setgid to {0}: {1}"), options->Get<Anope::string>("group"), Anope::LastError());
		else
			gid = g->gr_gid;
	}

	for (unsigned i = 0; i < Config->LogInfos.size(); ++i)
	{
		LogInfo& li = Config->LogInfos[i];

		for (unsigned j = 0; j < li.logfiles.size(); ++j)
		{
			LogFile* lf = li.logfiles[j];

			chown(lf->filename.c_str(), uid, gid);
		}
	}

	if (static_cast<int>(gid) != -1)
	{
		if (setgid(gid) == -1)
			Anope::Logger.Log(_("Unable to setgid to {0}: {1}"), options->Get<Anope::string>("group"), Anope::LastError());
		else
			Anope::Logger.Log(_("Successfully set group to {0"), options->Get<Anope::string>("group"));
	}
	if (static_cast<int>(uid) != -1)
	{
		if (setuid(uid) == -1)
			Anope::Logger.Log(_("Unable to setuid to {0}: {1}"), options->Get<Anope::string>("user"), Anope::LastError());
		else
			Anope::Logger.Log(_("Successfully set user to {0}"), options->Get<Anope::string>("user"));
	}
#endif
}

void Anope::Init(int ac, char **av)
{
	/* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
	umask(DEFUMASK);
#endif

	/* Parse command line arguments */
	ParseCommandLineArguments(ac, av);

	if (GetCommandLineArgument("version", 'v'))
	{
		Anope::Logger.Terminal(_("Anope-{0} -- Built: {1} -- Flags: {2}"), Anope::Version(), Anope::VersionBuildTime(), Anope::VersionFlags());
		exit(EXIT_SUCCESS);
	}

	if (GetCommandLineArgument("help", 'h'))
	{
		Anope::Logger.Terminal(_("Anope-{0}"), Anope::Version());
		Anope::Logger.Terminal(_("Anope IRC Services (https://anope.org)"));
		Anope::Logger.Terminal(_("Usage ./{0} [options] ..."), Anope::ServicesBin);
		Anope::Logger.Terminal(_("-c, --config=filename.conf"));
		Anope::Logger.Terminal(_("    --confdir=conf file direcory"));
		Anope::Logger.Terminal(_("    --dbdir=database directory"));
		Anope::Logger.Terminal(_("-d, --debug[=level]"));
		Anope::Logger.Terminal(_("-h, --help"));
		Anope::Logger.Terminal(_("    --localedir=locale directory"));
		Anope::Logger.Terminal(_("    --logdir=logs directory"));
		Anope::Logger.Terminal(_("    --modulesdir=modules directory"));
		Anope::Logger.Terminal(_("-e, --noexpire"));
		Anope::Logger.Terminal(_("-n, --nofork"));
		Anope::Logger.Terminal(_("    --nothird"));
		Anope::Logger.Terminal(_("    --protocoldebug"));
		Anope::Logger.Terminal(_("-r, --readonly"));
		Anope::Logger.Terminal(_("-s, --support"));
		Anope::Logger.Terminal(_("-v, --version"));
		Anope::Logger.Terminal("");
		Anope::Logger.Terminal(_("Further support is available from https://anope.org"));
		Anope::Logger.Terminal(_("Or visit us on IRC at irc.anope.org #anope"));
		exit(EXIT_SUCCESS);
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
			int level = arg.is_number_only() ? convertTo<int>(arg) : -1;
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

	/* Chdir to Services data directory. */
	if (chdir(Anope::ServicesDir.c_str()) < 0)
	{
		throw CoreException("Unable to chdir to " + Anope::ServicesDir + ": " + Anope::LastError());
	}

	Anope::Logger.Terminal("Anope-{0} -- Built: {1} -- Flags: {2}", Anope::Version(), Anope::VersionBuildTime(), Anope::VersionFlags());

#ifdef _WIN32
	if (!SupportedWindowsVersion())
		throw CoreException(GetWindowsVersion() + " is not a supported version of Windows");
#else
	/* If we're root, issue a warning now */
	if (!getuid() && !getgid())
	{
		/* If we are configured to setuid later, don't issue a warning */
		Configuration::Block *options = Config->GetBlock("options");
		if (options->Get<Anope::string>("user").empty())
		{
			std::cerr << "WARNING: You are currently running Anope as the root superuser. Anope does not" << std::endl;
			std::cerr << "         require root privileges to run, and it is discouraged that you run Anope" << std::endl;
			std::cerr << "         as the root superuser." << std::endl;
			sleep(3);
		}
	}
#endif

#ifdef _WIN32
	Anope::Logger.Terminal("Using configuration file {0}\\{1}", Anope::ConfigDir, ServicesConf.GetName());
#else
	Anope::Logger.Terminal("Using configuration file {0}/{1}", Anope::ConfigDir, ServicesConf.GetName());

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

			exit(Anope::ReturnValue);
		}
		else if (i == -1)
		{
			Anope::Logger.Terminal("Error, unable to fork: {0}", Anope::LastError());
			Anope::NoFork = true;
		}

		/* Child doesn't need these */
		sigaction(SIGUSR2, &old_sigusr2, NULL);
		sigaction(SIGCHLD, &old_sigchld, NULL);
	}

#endif

	/* Initialize the socket engine. Note that some engines can not survive a fork(), so this must be here. */
	SocketEngine::Init();

	ServiceManager::Init();
	EventManager::Init();

	new BotInfoType();
	new XLineType(nullptr);
	new OperBlockType();

	/* Read configuration file; exit if there are problems. */
	try
	{
		Config = new Configuration::Conf();
	}
	catch (const ConfigException &ex)
	{
		Anope::Logger.Terminal(ex.GetReason());
		Anope::Logger.Terminal("*** Support resources: Read through the anope.conf self-contained");
		Anope::Logger.Terminal("*** documentation. Read the documentation files found in the 'docs'");
		Anope::Logger.Terminal("*** folder. Visit our portal located at https://anope.org/. Join");
		Anope::Logger.Terminal("*** our support channel on irc.anope.org #anope.");
		throw CoreException("Configuration file failed to validate");
	}

	/* Create me */
	Configuration::Block *block = Config->GetBlock("serverinfo");
	Me = new Server(NULL, block->Get<Anope::string>("name"), 0, block->Get<Anope::string>("description"), block->Get<Anope::string>("id"));
	for (std::pair<Anope::string, User *> p : UserListByNick)
	{
		User *u = p.second;
		if (u->type != UserType::BOT)
			continue;

		ServiceBot *bi = anope_dynamic_static_cast<ServiceBot *>(u);
		bi->server = Me;
		++Me->users;
	}

	/* Announce ourselves to the logfile. */
	if (Anope::Debug || Anope::ReadOnly)
		Anope::Logger.Log("Anope {0} starting up ({1})",
			Anope::Version(),
			Anope::Format("options:{0}{1}", Anope::Debug ? " debug" : "", Anope::ReadOnly ? " readonly" : ""));
	else
		Anope::Logger.Log("Anope {0} starting up", Anope::Version());

	InitSignals();

	/* Initialize multi-language support */
	Language::InitLanguages();

	/* Initialize random number generator */
	block = Config->GetBlock("options");
	srand(block->Get<unsigned>("seed") ^ time(NULL));

	ModeManager::Apply(nullptr);

	/* load modules */
	Anope::Logger.Log("Loading modules...");
	for (int i = 0; i < Config->CountBlock("module"); ++i)
	{
		Configuration::Block *config = Config->GetBlock("module", i);

		ModuleReturn modret = ModuleManager::LoadModule(config->Get<Anope::string>("name"), NULL);

		if (modret == ModuleReturn::NOEXIST)
		{
			throw CoreException("Module " + config->Get<Anope::string>("name") + " does not exist!");
		}
	}

	Config->LoadOpers();
	Config->ApplyBots();

#ifndef _WIN32
	/* We won't background later, so we should setuid now */
	if (Anope::NoFork)
		setuidgid();
#endif

	Module *protocol = ModuleManager::FindFirstOf(PROTOCOL);
	if (protocol == NULL)
		throw CoreException("You must load a protocol module!");

	/* Write our PID to the PID file. */
	write_pidfile();

	Anope::Logger.Log("Using IRCd protocol {0}", protocol->GetName());

	/* Auto assign sid if applicable */
	if (IRCD->RequiresID)
	{
		Anope::string sid = IRCD->SID_Retrieve();
		if (Me->GetSID() == Me->GetName())
			Me->SetSID(sid);
		for (std::pair<Anope::string, User *> p : UserListByNick)
		{
			User *u = p.second;
			if (u->type != UserType::BOT)
				continue;

			ServiceBot *bi = anope_dynamic_static_cast<ServiceBot *>(u);
			bi->GenerateUID();
		}
	}

	/* Load up databases */
	Anope::Logger.Log("Loading databases...");
	EventManager::Get()->Dispatch(&Event::LoadDatabase::OnLoadDatabase);;
	Anope::Logger.Log("Databases loaded");

	EventManager::Get()->Dispatch(&Event::PostInit::OnPostInit);

	for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
		it->second->Sync();
}

