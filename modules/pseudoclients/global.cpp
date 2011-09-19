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

class MyGlobalService : public GlobalService
{
	void ServerGlobal(Server *s, const Anope::string &message)
	{
		if (s != Me && !s->HasFlag(SERVER_JUPED))
			notice_server(Config->Global, s, "%s", message.c_str());
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			this->ServerGlobal(s->GetLinks()[i], message);
	}

 public:
	MyGlobalService(Module *m) : GlobalService(m) { }

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
	MyGlobalService myglobalservice;

 public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		myglobalservice(this)
	{
		this->SetAuthor("Anope");

		BotInfo *Global = findbot(Config->Global);
		if (Global == NULL)
			throw ModuleException("No bot named " + Config->Global);

		Implementation i[] = { I_OnRestart, I_OnShutdown, I_OnNewServer, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnRestart()
	{
		if (Config->GlobalOnCycle)
			global->SendGlobal(findbot(Config->Global), "", Config->GlobalOnCycleMessage);
	}
	
	void OnShutdown()
	{
		if (Config->GlobalOnCycle)
			global->SendGlobal(findbot(Config->Global), "", Config->GlobalOnCycleMessage);
	}

	void OnNewServer(Server *s)
	{
		if (Config->GlobalOnCycle && !Config->GlobalOnCycleUP.empty())
			notice_server(Config->Global, s, "%s", Config->GlobalOnCycleUP.c_str());
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->Global)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:\n"), Config->Global.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(GlobalCore)

