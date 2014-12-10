/* Global core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/help.h"
#include "modules/global.h"

class GlobalCore : public Module
	, public Global::GlobalService
	, public EventHook<Event::Restart>
	, public EventHook<Event::Shutdown>
	, public EventHook<Event::NewServer>
	, public EventHook<Event::Help>
{
	Reference<ServiceBot> Global;

	void ServerGlobal(ServiceBot *sender, Server *s, const Anope::string &message)
	{
		if (s != Me && !s->IsJuped())
			s->Notice(sender, message);
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			this->ServerGlobal(sender, s->GetLinks()[i], message);
	}

 public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, GlobalService(this)
		, EventHook<Event::Restart>()
		, EventHook<Event::Shutdown>()
		, EventHook<Event::NewServer>()
		, EventHook<Event::Help>()
	{
	}

	void SendGlobal(ServiceBot *sender, const Anope::string &source, const Anope::string &message) override
	{
		if (Me->GetLinks().empty())
			return;
		if (!sender)
			sender = Global;
		if (!sender)
			return;

		Anope::string rmessage;

		if (!source.empty() && !Config->GetModule("global")->Get<bool>("anonymousglobal"))
			rmessage = "[" + source + "] " + message;
		else
			rmessage = message;

		this->ServerGlobal(sender, Servers::GetUplink(), rmessage);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &glnick = conf->GetModule(this)->Get<Anope::string>("client");

		if (glnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(glnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + glnick);

		Global = bi;
	}

	void OnRestart() override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<Anope::string>("globaloncycledown");
		if (!gl.empty())
			this->SendGlobal(Global, "", gl);
	}

	void OnShutdown() override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<Anope::string>("globaloncycledown");
		if (!gl.empty())
			this->SendGlobal(Global, "", gl);
	}

	void OnNewServer(Server *s) override
	{
		const Anope::string &gl = Config->GetModule(this)->Get<Anope::string>("globaloncycleup");
		if (!gl.empty() && !Me->IsSynced())
			s->Notice(Global, gl);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *Global)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Global->nick.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
	}
};

MODULE_INIT(GlobalCore)

