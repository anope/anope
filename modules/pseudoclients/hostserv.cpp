/* HostServ core functions
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
		HostInfo *ho = NULL;
		NickAlias *na = findnick(u->nick);
		if (na && na->hostinfo.HasVhost())
			ho = &na->hostinfo;
		else
		{
			na = findnick(u->Account()->display);
			if (na && na->hostinfo.HasVhost())
				ho = &na->hostinfo;
		}
		if (ho == NULL)
			return;

		if (u->vhost.empty() || !u->vhost.equals_cs(na->hostinfo.GetHost()) || (!na->hostinfo.GetIdent().empty() && !u->GetVIdent().equals_cs(na->hostinfo.GetIdent())))
		{
			ircdproto->SendVhost(u, na->hostinfo.GetIdent(), na->hostinfo.GetHost());
			if (ircd->vhost)
			{
				u->vhost = na->hostinfo.GetHost();
				u->UpdateHost();
			}
			if (ircd->vident && !na->hostinfo.GetIdent().empty())
				u->SetVIdent(na->hostinfo.GetIdent());

			BotInfo *bi = findbot(Config->HostServ);
			if (bi)
			{
				if (!na->hostinfo.GetIdent().empty())
					u->SendMessage(bi, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
				else
					u->SendMessage(bi, _("Your vhost of \002%s\002 is now activated."), na->hostinfo.GetHost().c_str());
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

