/* HostServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class HostServCore final
	: public Module
{
	Reference<BotInfo> HostServ;
public:
	HostServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
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

		const NickAlias *na = NickAlias::Find(u->nick);
		if (!na || na->nc != u->Account() || !na->HasVHost())
			na = u->AccountNick();
		if (!na || !na->HasVHost())
			return;

		if (u->vhost.empty() || !u->vhost.equals_cs(na->GetVHostHost()) || (!na->GetVHostIdent().empty() && !u->GetVIdent().equals_cs(na->GetVHostIdent())))
		{
			IRCD->SendVHost(u, na->GetVHostIdent(), na->GetVHostHost());

			u->vhost = na->GetVHostHost();
			u->UpdateHost();

			if (IRCD->CanSetVIdent && !na->GetVHostIdent().empty())
				u->SetVIdent(na->GetVHostIdent());

			if (HostServ)
			{
				u->SendMessage(HostServ, _("Your vhost of \002%s\002 is now activated."),
					na->GetVHostMask().c_str());
			}
		}
	}

	void OnNickDrop(CommandSource &source, NickAlias *na) override
	{
		if (na->HasVHost())
		{
			FOREACH_MOD(OnDeleteVHost, (na));
			na->RemoveVHost();
		}
	}

	void OnNickUpdate(User *u) override
	{
		this->OnUserLogin(u);
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (mname == "OPER" && Config->GetModule(this)->Get<bool>("activate_on_deoper", "yes"))
			this->OnUserLogin(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *HostServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), HostServ->nick.c_str());
		return EVENT_CONTINUE;
	}

	void OnSetVHost(NickAlias *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->nick);

			if (u && u->Account() == na->nc)
			{
				IRCD->SendVHost(u, na->GetVHostIdent(), na->GetVHostHost());

				u->vhost = na->GetVHostHost();
				u->UpdateHost();

				if (IRCD->CanSetVIdent && !na->GetVHostIdent().empty())
					u->SetVIdent(na->GetVHostIdent());

				if (HostServ)
				{
					u->SendMessage(HostServ, _("Your vhost of \002%s\002 is now activated."),
						na->GetVHostMask().c_str());
				}
			}
		}
	}

	void OnDeleteVHost(NickAlias *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->nick);

			if (u && u->Account() == na->nc)
				IRCD->SendVHostDel(u);
		}
	}
};

MODULE_INIT(HostServCore)
