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

void myHostServHelp(User *u);

class CommandHSOff : public Command
{
 public:
	CommandHSOff() : Command("OFF", 0, 0)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na;
		char *vhost;
		char *vident = NULL;
		if ((na = findnick(u->nick)))
		{
			if (na->status & NS_IDENTIFIED)
			{
				vhost = getvHost(u->nick);
				vident = getvIdent(u->nick);
				if (!vhost && !vident)
					notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
				else
					ircdproto->SendVhostDel(u);
			}
			else
				notice_lang(s_HostServ, u, HOST_ID);
		}
		else
			notice_lang(s_HostServ, u, HOST_NOT_REGED);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_HostServ, u, HOST_HELP_OFF);
		return true;
	}
};

class HSOff : public Module
{
 public:
	HSOff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSOff(), MOD_UNIQUE);

		this->SetHostHelp(myHostServHelp);
	}
};

/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User *u)
{
	notice_lang(s_HostServ, u, HOST_HELP_CMD_OFF);
}

MODULE_INIT("hs_off", HSOff)
