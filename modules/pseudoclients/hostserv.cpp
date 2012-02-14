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

		if (!ircd || !ircd->vhost)
			throw ModuleException("Your IRCd does not suppor vhosts");
	
		BotInfo *HostServ = findbot(Config->HostServ);
		if (HostServ == NULL)
			throw ModuleException("No bot named " + Config->HostServ);

		Implementation i[] = { I_OnNickIdentify, I_OnNickUpdate, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnNickIdentify(User *u)
	{
		NickAlias *na = findnick(u->nick);
		if (!na || !na->HasVhost())
			na = findnick(u->Account()->display);
		if (!na)
			return;

		if (u->vhost.empty() || !u->vhost.equals_cs(na->GetVhostHost()) || (!na->GetVhostIdent().empty() && !u->GetVIdent().equals_cs(na->GetVhostIdent())))
		{
			ircdproto->SendVhost(u, na->GetVhostIdent(), na->GetVhostHost());
			if (ircd->vhost)
			{
				u->vhost = na->GetVhostHost();
				u->UpdateHost();
			}
			if (ircd->vident && !na->GetVhostIdent().empty())
				u->SetVIdent(na->GetVhostIdent());

			BotInfo *bi = findbot(Config->HostServ);
			if (bi)
			{
				if (!na->GetVhostIdent().empty())
					u->SendMessage(bi, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->GetVhostIdent().c_str(), na->GetVhostHost().c_str());
				else
					u->SendMessage(bi, _("Your vhost of \002%s\002 is now activated."), na->GetVhostHost().c_str());
			}
		}
	}

	void OnNickUpdate(User *u)
	{
		this->OnNickIdentify(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->HostServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:\n"), Config->HostServ.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(HostServCore)

