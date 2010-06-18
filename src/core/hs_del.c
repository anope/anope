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

class CommandHSDel : public Command
{
 public:
	CommandHSDel() : Command("DEL", 1, 1, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na;
		const char *nick = params[0].c_str();
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config.s_HostServ, u, NICK_X_FORBIDDEN, nick);
				return MOD_CONT;
			}
			Alog() << "vHost for user \002" << nick << "\002 deleted by oper \002" << u->nick << "\002";
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			na->hostinfo.RemoveVhost();
			notice_lang(Config.s_HostServ, u, HOST_DEL, nick);
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOREG, nick);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_HostServ, u, "DEL", HOST_DEL_SYNTAX);
	}
};

class HSDel : public Module
{
 public:
	HSDel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSDel());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_DEL);
	}
};

MODULE_INIT(HSDel)
