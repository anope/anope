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

class MyGlobalService : public GlobalService
{
	void ServerGlobal(const BotInfo *sender, Server *s, const Anope::string &message)
	{
		if (s != Me && !s->IsJuped())
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

		if (!source.empty() && !Config->GetModule("global")->Get<bool>("anonymousglobal"))
			rmessage = "[" + source + "] " + message;
		else
			rmessage = message;

		this->ServerGlobal(sender, Servers::GetUplink(), rmessage);
	}
};

class GlobalCore : public Module
{
	MyGlobalService global;

 public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR),
		global(this)
	{
		Implementation i[] = { I_OnReload, I_OnBotDelete, I_OnRestart, I_OnShutdown, I_OnNewServer, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~GlobalCore()
	{
		Global = NULL;
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		const Anope::string &glnick = conf->GetModule(this)->Get<const Anope::string &>("client");

		if (glnick.empty())
			throw ConfigException(this->name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(glnick, true);
		if (!bi)
			throw ConfigException(this->name + ": no bot named " + glnick);

		Global = bi;
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		if (bi == Global)
			Global = NULL;
	}

	void OnRestart() anope_override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<const Anope::string &>("globaloncycledown");
		if (!gl.empty())
			this->global.SendGlobal(Global, "", gl);
	}
	
	void OnShutdown() anope_override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<const Anope::string &>("globaloncycledown");
		if (!gl.empty())
			this->global.SendGlobal(Global, "", gl);
	}

	void OnNewServer(Server *s) anope_override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<const Anope::string &>("globaloncycleup");
		if (!gl.empty() && !Me->IsSynced())
			s->Notice(Global, gl);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service != Global)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Global->nick.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(GlobalCore)

