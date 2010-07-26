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

class CommandHSDel : public Command
{
 public:
	CommandHSDel() : Command("DEL", 1, 1, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na;
		Anope::string nick = params[0];
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config.s_HostServ, u, NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}
			Alog() << "vHost for user \2" << nick << "\2 deleted by oper \2" << u->nick << "\2";
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			na->hostinfo.RemoveVhost();
			notice_lang(Config.s_HostServ, u, HOST_DEL, nick.c_str());
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOREG, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_HostServ, u, "DEL", HOST_DEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_DEL);
	}
};

class HSDel : public Module
{
 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, new CommandHSDel());
	}
};

MODULE_INIT(HSDel)
