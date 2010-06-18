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

class CommandHSDelAll : public Command
{
 public:
	CommandHSDelAll() : Command("DELALL", 1, 1, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		int i;
		const char *nick = params[0].c_str();
		NickAlias *na;
		NickCore *nc;
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config.s_HostServ, u, NICK_X_FORBIDDEN, nick);
				return MOD_CONT;
			}
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			nc = na->nc;
			for (i = 0; i < nc->aliases.count; ++i)
			{
				na = static_cast<NickAlias *>(nc->aliases.list[i]);
				na->hostinfo.RemoveVhost();
			}
			Alog() << "vHosts for all nicks in group \002" << nc->display << "\002 deleted by oper \002" << u->nick << "\002";
			notice_lang(Config.s_HostServ, u, HOST_DELALL, nc->display);
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOREG, nick);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_DELALL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_HostServ, u, "DELALL", HOST_DELALL_SYNTAX);
	}
};

class HSDelAll : public Module
{
 public:
	HSDelAll(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSDelAll());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_DELALL);
	}
};

MODULE_INIT(HSDelAll)
