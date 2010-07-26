/* HostServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSGroup : public Command
{
 public:
	CommandHSGroup() : Command("GROUP", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(u->nick);
		if (na && u->Account() == na->nc && na->hostinfo.HasVhost())
		{
			HostServSyncVhosts(na);
			if (!na->hostinfo.GetIdent().empty())
				notice_lang(Config.s_HostServ, u, HOST_IDENT_GROUP, u->Account()->display.c_str(), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				notice_lang(Config.s_HostServ, u, HOST_GROUP, u->Account()->display.c_str(), na->hostinfo.GetHost().c_str());
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOT_ASSIGNED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_GROUP);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_GROUP);
	}
};

class HSGroup : public Module
{
 public:
	HSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, new CommandHSGroup());
	}
};

MODULE_INIT(HSGroup)
