/* HostServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandHSOn : public Command
{
 public:
	CommandHSOn() : Command("ON", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na = findnick(u->nick);
		if (na && u->Account() == na->nc && na->hostinfo.HasVhost())
		{
			if (!na->hostinfo.GetIdent().empty())
				notice_lang(Config.s_HostServ, u, HOST_IDENT_ACTIVATED, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				notice_lang(Config.s_HostServ, u, HOST_ACTIVATED, na->hostinfo.GetHost().c_str());
			ircdproto->SendVhost(u, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			if (ircd->vhost)
			{
				if (u->vhost)
					delete [] u->vhost;
				u->vhost = sstrdup(na->hostinfo.GetHost().c_str());
			}
			if (ircd->vident)
			{
				if (!na->hostinfo.GetIdent().empty())
					u->SetVIdent(na->hostinfo.GetIdent());
			}
			u->UpdateHost();
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOT_ASSIGNED);
		
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_ON);
		return true;
	}
};

class HSOn : public Module
{
 public:
	HSOn(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSOn());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_ON);
	}
};

MODULE_INIT(HSOn)
