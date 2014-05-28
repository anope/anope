/* HostServ core functions
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
#include "modules/ns_update.h"
#include "modules/hs_del.h"
#include "modules/help.h"

class HostServCore : public Module
	, public EventHook<Event::UserLogin>
	, public EventHook<Event::NickUpdate>
	, public EventHook<Event::Help>
	, public EventHook<Event::SetVhost>
	, public EventHook<Event::DeleteVhost>
{
	Reference<BotInfo> HostServ;

 public:
	HostServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, EventHook<Event::UserLogin>("OnUserLogin")
		, EventHook<Event::NickUpdate>("OnNickUpdate")
		, EventHook<Event::Help>("OnHelp")
		, EventHook<Event::SetVhost>("OnSetVhost")
		, EventHook<Event::DeleteVhost>("OnDeleteVhost")
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &hsnick = conf->GetModule(this)->Get<const Anope::string>("client");

		if (hsnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(hsnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + hsnick);

		HostServ = bi;
	}

	void OnUserLogin(User *u) override
	{
		if (!IRCD->CanSetVHost)
			return;

		const NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (!na || na->nc != u->Account() || !na->HasVhost())
			na = NickServ::FindNick(u->Account()->display);
		if (!na || !na->HasVhost())
			return;

		if (u->vhost.empty() || !u->vhost.equals_cs(na->GetVhostHost()) || (!na->GetVhostIdent().empty() && !u->GetVIdent().equals_cs(na->GetVhostIdent())))
		{
			IRCD->SendVhost(u, na->GetVhostIdent(), na->GetVhostHost());

			u->vhost = na->GetVhostHost();
			u->UpdateHost();

			if (IRCD->CanSetVIdent && !na->GetVhostIdent().empty())
				u->SetVIdent(na->GetVhostIdent());

			if (HostServ)
			{
				if (!na->GetVhostIdent().empty())
					u->SendMessage(*HostServ, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->GetVhostIdent().c_str(), na->GetVhostHost().c_str());
				else
					u->SendMessage(*HostServ, _("Your vhost of \002%s\002 is now activated."), na->GetVhostHost().c_str());
			}
		}
	}

	void OnNickUpdate(User *u) override
	{
		this->OnUserLogin(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *HostServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), HostServ->nick.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
	}

	void OnSetVhost(NickServ::Nick *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->nick);

			if (u && u->Account() == na->nc)
			{
				IRCD->SendVhost(u, na->GetVhostIdent(), na->GetVhostHost());

				u->vhost = na->GetVhostHost();
				u->UpdateHost();

				if (IRCD->CanSetVIdent && !na->GetVhostIdent().empty())
					u->SetVIdent(na->GetVhostIdent());

				if (HostServ)
				{
					if (!na->GetVhostIdent().empty())
						u->SendMessage(*HostServ, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->GetVhostIdent().c_str(), na->GetVhostHost().c_str());
					else
						u->SendMessage(*HostServ, _("Your vhost of \002%s\002 is now activated."), na->GetVhostHost().c_str());
				}
			}
		}
	}

	void OnDeleteVhost(NickServ::Nick *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->nick);

			if (u && u->Account() == na->nc)
				IRCD->SendVhostDel(u);
		}
	}
};

MODULE_INIT(HostServCore)

