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

extern int do_hs_sync(NickCore *nc, char *vIdent, char *hostmask, char *creator, time_t time);

class CommandHSGroup : public Command
{
 public:
	CommandHSGroup() : Command("GROUP", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na = findnick(u->nick);
		if (na && u->Account() == na->nc && na->hostinfo.HasVhost())
		{
			HostServSyncVhosts(na);
			if (!na->hostinfo.GetIdent().empty())
				notice_lang(Config.s_HostServ, u, HOST_IDENT_GROUP, u->Account()->display, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				notice_lang(Config.s_HostServ, u, HOST_GROUP, u->Account()->display, na->hostinfo.GetHost().c_str());
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOT_ASSIGNED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_GROUP);
		return true;
	}
};

class HSGroup : public Module
{
 public:
	HSGroup(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(HostServ, new CommandHSGroup());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_GROUP);
	}
};

MODULE_INIT(HSGroup)
