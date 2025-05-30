/* Anope -- main source file.
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "timers.h"
#include "config.h"
#include "bots.h"
#include "socketengine.h"
#include "uplink.h"

#ifndef _WIN32
#include <climits>
#else
#include <process.h>
#endif

/* Command-line options: */
unsigned Anope::Debug = 0;
bool Anope::ReadOnly = false, Anope::NoFork = false, Anope::NoThird = false, Anope::NoPID = false, Anope::NoExpire = false, Anope::ProtocolDebug = false;
Anope::string Anope::ServicesDir;
Anope::string Anope::ServicesBin;

int Anope::ReturnValue = 0;
sig_atomic_t Anope::Signal = 0;
bool Anope::Quitting = false;
bool Anope::Restarting = false;
Anope::string Anope::QuitReason;

static Anope::string BinaryDir;       /* Full path to services bin directory */

time_t Anope::StartTime = 0;
time_t Anope::CurTime = 0;
long long Anope::CurTimeNs = 0;

size_t Anope::CurrentUplink = -1;

class UpdateTimer final
	: public Timer
{
public:
	UpdateTimer(time_t timeout)
		: Timer(timeout, true)
	{
	}

	void Tick() override
	{
		Anope::SaveDatabases();
	}
};

class ExpireTimer final
	: public Timer
{
public:
	ExpireTimer(time_t timeout)
		: Timer(timeout, true)
	{
	}

	void Tick() override
	{
		FOREACH_MOD(OnExpireTick, ());
	}
};

void Anope::SaveDatabases()
{
	if (Anope::ReadOnly)
		return;

	Log(LOG_DEBUG) << "Saving databases";
	FOREACH_MOD(OnSaveDatabase, ());
}

/** The following comes from InspIRCd to get the full path of the Anope executable
 */
static Anope::string GetFullProgDir(const Anope::string &argv0)
{
#ifdef _WIN32
	/* Windows has specific API calls to get the EXE path that never fail.
	 * For once, Windows has something of use, compared to the POSIX code
	 * for this, this is positively neato.
	 */
	char buffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		Anope::string fullpath = buffer;
		Anope::string::size_type n = fullpath.rfind("\\");
		Anope::ServicesBin = fullpath.substr(n + 1, fullpath.length());
		return fullpath.substr(0, n);
	}
#else
	// Get the current working directory
	char buffer[PATH_MAX];
	if (getcwd(buffer, PATH_MAX))
	{
		Anope::string remainder = argv0;

		Anope::ServicesBin = remainder;
		Anope::string::size_type n = Anope::ServicesBin.rfind("/");
		Anope::string fullpath;
		if (Anope::ServicesBin[0] == '/')
			fullpath = Anope::ServicesBin.substr(0, n);
		else
			fullpath = Anope::string(buffer) + "/" + Anope::ServicesBin.substr(0, n);
		Anope::ServicesBin = Anope::ServicesBin.substr(n + 1, remainder.length());
		return fullpath;
	}
#endif
	return "/";
}

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
{
	/* String comparisons won't work until we build the case map cache, so do it first */
	Anope::CaseMapRebuild();

	BinaryDir = GetFullProgDir(av[0]);
	if (BinaryDir[BinaryDir.length() - 1] == '.')
		BinaryDir = BinaryDir.substr(0, BinaryDir.length() - 2);

#ifdef _WIN32
	Anope::string::size_type n = BinaryDir.rfind('\\');
#else
	Anope::string::size_type n = BinaryDir.rfind('/');
#endif
	Anope::ServicesDir = BinaryDir.substr(0, n);

#ifdef _WIN32
	/* Clean out the module runtime directory prior to running, just in case files were left behind during a previous run */
	ModuleManager::CleanupRuntimeDirectory();

	OnStartup();
#endif

	try
	{
		/* General initialization first */
		if (!Anope::Init(ac, av))
			return Anope::ReturnValue;
	}
	catch (const CoreException &ex)
	{
		Log(LOG_TERMINAL) << "Error: " << ex.GetReason();
		return EXIT_FAILURE;
	}

	try
	{
		Uplink::Connect();
	}
	catch (const SocketException &ex)
	{
		Log(LOG_TERMINAL) << "Unable to connect to uplink #" << (Anope::CurrentUplink + 1) << " (" << Config->Uplinks[Anope::CurrentUplink].str() << "): " << ex.GetReason();
	}

	/* Set up timers */
	time_t last_check = Anope::CurTime;
	UpdateTimer updateTimer(Config->GetBlock("options").Get<time_t>("updatetimeout", "2m"));
	ExpireTimer expireTimer(Config->GetBlock("options").Get<time_t>("expiretimeout", "30m"));

	/*** Main loop. ***/
	while (!Anope::Quitting)
	{
		Log(LOG_DEBUG_2) << "Top of main loop";

		/* Process timers */
		if (Anope::CurTime - last_check >= Config->TimeoutCheck)
		{
			TimerManager::TickTimers();
			last_check = Anope::CurTime;
		}

		/* Process the socket engine */
		SocketEngine::Process();

		if (Anope::Signal)
			Anope::HandleSignal();
	}

	if (Anope::Restarting)
	{
		FOREACH_MOD(OnRestart, ());
	}
	else
	{
		FOREACH_MOD(OnShutdown, ());
	}

	if (Anope::QuitReason.empty())
		Anope::QuitReason = "Terminating, reason unknown";
	Log() << Anope::QuitReason;

	delete UplinkSock;

	ModuleManager::UnloadAll();
	SocketEngine::Shutdown();
	for (Module *m; (m = ModuleManager::FindFirstOf(PROTOCOL)) != NULL;)
		ModuleManager::UnloadModule(m, NULL);

#ifdef _WIN32
	ModuleManager::CleanupRuntimeDirectory();

	OnShutdown();
#endif

	if (Anope::Restarting)
	{
		auto pidfile = Anope::ExpandData(Config->GetBlock("serverinfo").Get<const Anope::string>("pid"));
		if (!pidfile.empty())
			remove(pidfile.c_str());

		if (chdir(BinaryDir.c_str()) == 0)
		{
			Anope::string sbin = "./" + Anope::ServicesBin;
			av[0] = const_cast<char *>(sbin.c_str());
			execve(Anope::ServicesBin.c_str(), av, envp);
		}
		Log() << "Restart failed: " << strerror(errno);
		Anope::ReturnValue = -1;
	}

	return Anope::ReturnValue;
}
