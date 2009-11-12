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

class CommandHSOff : public Command
{
 public:
	CommandHSOff() : Command("OFF", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		char *vhost;
		char *vident = NULL;
		
		vhost = getvHost(u->nick);
		vident = getvIdent(u->nick);
		if (!vhost && !vident)
			notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
		else
		{
			ircdproto->SendVhostDel(u);
			notice_lang(s_HostServ, u, HOST_OFF);
		}
		
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_HostServ, u, HOST_HELP_OFF);
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

		this->AddCommand(HOSTSERV, new CommandHSOff());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(s_HostServ, u, HOST_HELP_CMD_OFF);
	}
};

MODULE_INIT(HSOff)
