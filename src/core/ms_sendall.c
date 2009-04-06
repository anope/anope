/* MemoServ core functions
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

class CommandMSSendAll : public Command
{
 public:
	CommandMSSendAll() : Command("SENDALL", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		int i, z = 1;
		NickCore *nc;
		const char *text = params[0].c_str();

		if (!u->nc->HasCommand("memoserv/sendall"))
		{
			// XXX: error?
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);
			return MOD_CONT;
		}
		else if (checkDefCon(DEFCON_NO_NEW_MEMOS))
		{
			notice_lang(s_MemoServ, u, OPER_DEFCON_DENIED);
			return MOD_CONT;
		}

		for (i = 0; i < 1024; ++i)
		{
			for (nc = nclists[i]; nc; nc = nc->next)
			{
				if (stricmp(u->nick, nc->display))
					memo_send(u, nc->display, text, z);
			} /* /nc */
		} /* /i */

		notice_lang(s_MemoServ, u, MEMO_MASS_SENT);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_MemoServ, u, MEMO_HELP_SENDALL);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "SENDALL", MEMO_SEND_SYNTAX);
	}
};

class MSSendAll : public Module
{
 public:
	MSSendAll(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSSendAll(), MOD_UNIQUE);
	}
	void MemoServHelp(User *u)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_SENDALL);
	}
};

MODULE_INIT("ms_sendall", MSSendAll)
