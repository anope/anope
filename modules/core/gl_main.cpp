/* Global core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "global.h"

static BotInfo *Global = NULL;

class MyGlobalService : public GlobalService
{
	 void ServerGlobal(Server *s, const Anope::string &message)
	 {
	 	if (s != Me && !s->HasFlag(SERVER_JUPED))
			notice_server(Config->s_Global, s, "%s", message.c_str());
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			this->ServerGlobal(s->GetLinks()[i], message);
	 }

 public:
	MyGlobalService(Module *m) : GlobalService(m) { }

	BotInfo *Bot()
	{
		return Global;
	}

	void SendGlobal(BotInfo *sender, const Anope::string &source, const Anope::string &message)
	{
		if (Me->GetLinks().empty())
			return;

		Anope::string rmessage;

		if (!source.empty() && !Config->AnonymousGlobal)
			rmessage = "[" + source + "] " + message;
		else
			rmessage = message;

		this->ServerGlobal(Me->GetLinks().front(), rmessage);
	}
};

class GlobalCore : public Module
{
	MyGlobalService myglobal;

 public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), myglobal(this)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Implementation i[] = { I_OnPreRestart, I_OnPreShutdown, I_OnNewServer };
		ModuleManager::Attach(i, this, 3);

		ModuleManager::RegisterService(&this->myglobal);
		
		Global = new BotInfo(Config->s_Global, Config->ServiceUser, Config->ServiceHost, Config->desc_Global);
		Global->SetFlag(BI_CORE);

		spacesepstream coreModules(Config->GlobalCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~GlobalCore()
	{
		spacesepstream coreModules(Config->GlobalCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete Global;
	}

	void OnPreRestart()
	{
		if (Config->GlobalOnCycle)
			global->SendGlobal(global->Bot(), "", Config->GlobalOnCycleMessage);
	}
	
	void OnPreShutdown()
	{
		if (Config->GlobalOnCycle)
			global->SendGlobal(global->Bot(), "", Config->GlobalOnCycleMessage);
	}

	void OnNewServer(Server *s)
	{
		if (Config->GlobalOnCycle && !Config->GlobalOnCycleUP.empty())
			notice_server(Config->s_Global, s, "%s", Config->GlobalOnCycleUP.c_str());
	}
};

MODULE_INIT(GlobalCore)

