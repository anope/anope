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

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		NickAlias *na;
		char *vHost;
		char *vIdent = NULL;
		if ((na = findnick(u->nick)))
		{
			if (na->status & NS_IDENTIFIED)
			{
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
						u->vhost = sstrdup(vHost);
					if (ircd->vident)
					{
						if (vIdent)
							u->SetVIdent(vIdent);
					}
					set_lastmask(u);
				}
			}
			else
				notice_lang(s_HostServ, u, HOST_ID);
		}
		else
			notice_lang(s_HostServ, u, HOST_NOT_REGED);
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

		this->AddCommand(HOSTSERV, new CommandHSOn(), MOD_UNIQUE);
	}
	void HostServHelp(User *u)
	{
		notice_lang(s_HostServ, u, HOST_HELP_CMD_ON);
	}
};

MODULE_INIT("hs_on", HSOn)
