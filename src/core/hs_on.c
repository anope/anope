/* HostServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
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
		char *vHost;
		char *vIdent = NULL;
		
		vHost = getvHost(u->nick);
		vIdent = getvIdent(u->nick);
		if (!vHost)
			notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
		else
		{
			if (vIdent)
				notice_lang(s_HostServ, u, HOST_IDENT_ACTIVATED, vIdent, vHost);
			else
				notice_lang(s_HostServ, u, HOST_ACTIVATED, vHost);
			ircdproto->SendVhost(u->nick, vIdent, vHost);
			if (ircd->vhost)
			{
				if (u->vhost)
					delete [] u->vhost;
				u->vhost = sstrdup(vHost);
			}
			if (ircd->vident)
			{
				if (vIdent)
					u->SetVIdent(vIdent);
			}
			set_lastmask(u);
		}
		
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_HostServ, u, HOST_HELP_ON);
		return true;
	}
};

class HSOn : public Module
{
 public:
	HSOn(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSOn());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(s_HostServ, u, HOST_HELP_CMD_ON);
	}
};

MODULE_INIT(HSOn)
