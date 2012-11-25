/* HostServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class HostServCore : public Module
{
 public:
	HostServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	
		BotInfo *bi = BotInfo::Find(Config->HostServ);
		if (!bi)
			throw ModuleException("No bot named " + Config->HostServ);

		Implementation i[] = { I_OnNickIdentify, I_OnNickUpdate, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		
		Service::AddAlias("BotInfo", "HostServ", bi->nick);
	}

	~HostServCore()
	{
		Service::DelAlias("BotInfo", "HostServ");
	}

	void OnNickIdentify(User *u) anope_override
	{
		const NickAlias *na = NickAlias::Find(u->nick);
		if (!na || !na->HasVhost())
			na = NickAlias::Find(u->Account()->display);
		if (!IRCD->CanSetVHost || !na || !na->HasVhost())
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
					u->SendMessage(HostServ, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->GetVhostIdent().c_str(), na->GetVhostHost().c_str());
				else
					u->SendMessage(HostServ, _("Your vhost of \002%s\002 is now activated."), na->GetVhostHost().c_str());
			}
		}
	}

	void OnNickUpdate(User *u) anope_override
	{
		this->OnNickIdentify(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service->nick != Config->HostServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Config->HostServ.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(HostServCore)

