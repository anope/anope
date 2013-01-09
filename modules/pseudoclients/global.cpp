/* Global core functions
 *
 * (C) 2003-2013 Anope Team
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

class MyGlobalService : public GlobalService
{
	void ServerGlobal(const BotInfo *sender, Server *s, const Anope::string &message)
	{
		if (s != Me && !s->HasFlag(SERVER_JUPED))
			s->Notice(sender, message);
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			this->ServerGlobal(sender, s->GetLinks()[i], message);
	}

 public:
	MyGlobalService(Module *m) : GlobalService(m) { }

	void SendGlobal(const BotInfo *sender, const Anope::string &source, const Anope::string &message) anope_override
	{
		if (Me->GetLinks().empty())
			return;

		Anope::string rmessage;

		if (!source.empty() && !Config->AnonymousGlobal)
			rmessage = "[" + source + "] " + message;
		else
			rmessage = message;

		this->ServerGlobal(sender, Me->GetLinks().front(), rmessage);
	}
};

class GlobalCore : public Module
{
	MyGlobalService myglobalservice;

 public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		myglobalservice(this)
	{
		this->SetAuthor("Anope");

		Global = BotInfo::Find(Config->Global);
		if (!Global)
			throw ModuleException("No bot named " + Config->Global);

		Implementation i[] = { I_OnBotDelete, I_OnRestart, I_OnShutdown, I_OnNewServer, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~GlobalCore()
	{
		Global = NULL;
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		if (bi == Global)
			Global = NULL;
	}

	void OnRestart() anope_override
	{
		if (Config->GlobalOnCycle)
			this->myglobalservice.SendGlobal(Global, "", Config->GlobalOnCycleMessage);
	}
	
	void OnShutdown() anope_override
	{
		if (Config->GlobalOnCycle)
			this->myglobalservice.SendGlobal(Global, "", Config->GlobalOnCycleMessage);
	}

	void OnNewServer(Server *s) anope_override
	{
		if (Config->GlobalOnCycle && !Config->GlobalOnCycleUP.empty())
			s->Notice(Global, Config->GlobalOnCycleUP);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service->nick != Config->Global)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Config->Global.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(GlobalCore)

