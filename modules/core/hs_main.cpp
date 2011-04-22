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
#include "hostserv.h"

static BotInfo *HostServ = NULL;

class MyHostServService : public HostServService
{
 public:
	MyHostServService(Module *m) : HostServService(m) { }

	BotInfo *Bot()
	{
		return HostServ;
	}

	void Sync(NickAlias *na)
	{
		if (!na || !na->hostinfo.HasVhost())
			return;
	
		for (std::list<NickAlias *>::iterator it = na->nc->aliases.begin(), it_end = na->nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *nick = *it;
			nick->hostinfo.SetVhost(na->hostinfo.GetIdent(), na->hostinfo.GetHost(), na->hostinfo.GetCreator());
		}
	}
};

class HostServCore : public Module
{
	MyHostServService myhostserv;

 public:
	HostServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), myhostserv(this)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!ircd->vhost)
			throw ModuleException("Your IRCd does not suppor vhosts");

		ModuleManager::RegisterService(&this->myhostserv);
		
		HostServ = new BotInfo(Config->s_HostServ, Config->ServiceUser, Config->ServiceHost, Config->desc_HostServ);
		HostServ->SetFlag(BI_CORE);

		Implementation i[] = { I_OnNickIdentify, I_OnNickUpdate };
		ModuleManager::Attach(i, this, 2);

		spacesepstream coreModules(Config->HostCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~HostServCore()
	{
		spacesepstream coreModules(Config->HostCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete HostServ;
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

			if (!na->hostinfo.GetIdent().empty())
				u->SendMessage(HostServ, _("Your vhost of \002%s\002@\002%s\002 is now activated."), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				u->SendMessage(HostServ, _("Your vhost of \002%s\002 is now activated."), na->hostinfo.GetHost().c_str());
		}
	}

	void OnNickUpdate(User *u)
	{
		this->OnNickIdentify(u);
	}
};

MODULE_INIT(HostServCore)

