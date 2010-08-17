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

class CommandHSDelAll : public Command
{
 public:
	CommandHSDelAll() : Command("DELALL", 1, 1, "hostserv/del")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		NickAlias *na;
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config->s_HostServ, u, NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			NickCore *nc = na->nc;
			for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
			{
				na = *it;
				na->hostinfo.RemoveVhost();
			}
			Alog() << "vHosts for all nicks in group \2" << nc->display << "\2 deleted by oper \2" << u->nick << "\2";
			notice_lang(Config->s_HostServ, u, HOST_DELALL, nc->display.c_str());
		}
		else
			notice_lang(Config->s_HostServ, u, HOST_NOREG, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_HostServ, u, HOST_HELP_DELALL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_HostServ, u, "DELALL", HOST_DELALL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_HostServ, u, HOST_HELP_CMD_DELALL);
	}
};

class HSDelAll : public Module
{
	CommandHSDelAll commandhsdelall;

 public:
	HSDelAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsdelall);
	}
};

MODULE_INIT(HSDelAll)
