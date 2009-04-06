/* OperServ core functions
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

class CommandOSNOOP : public Command
{
 public:
	CommandOSNOOP() : Command("NOOP", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();
		const char *server = params[1].c_str();

		if (!stricmp(cmd, "SET"))
		{
			User *u2;
			User *u3 = NULL;
			char reason[NICKMAX + 32];

			/* Remove the O:lines */
			ircdproto->SendSVSNOOP(server, 1);

			snprintf(reason, sizeof(reason), "NOOP command used by %s", u->nick);
			if (WallOSNoOp)
				ircdproto->SendGlobops(s_OperServ, "\2%s\2 used NOOP on \2%s\2", u->nick, server);
			notice_lang(s_OperServ, u, OPER_NOOP_SET, server);

			/* Kill all the IRCops of the server */
			for (u2 = firstuser(); u2; u2 = u3)
			{
				u3 = nextuser();
				if (u2 && is_oper(u2) && u2->server->name && Anope::Match(u2->server->name, server, true))
					kill_user(s_OperServ, u2->nick, reason);
			}
		}
		else if (!stricmp(cmd, "REVOKE"))
		{
			ircdproto->SendSVSNOOP(server, 0);
			notice_lang(s_OperServ, u, OPER_NOOP_REVOKE, server);
		}
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_NOOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "NOOP", OPER_NOOP_SYNTAX);
	}
};

class OSNOOP : public Module
{
 public:
	OSNOOP(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSNOOP(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_NOOP);
	}
};

MODULE_INIT("os_noop", OSNOOP)
