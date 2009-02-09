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

void myMemoServHelp(User *u);

class CommandMSSend : public Command
{
 public:
	CommandMSSend() : Command("SEND", 2, 2)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		const char *name = params[0].c_str();
		const char *text = params[1].c_str();
		int z = 0;
		memo_send(u, name, text, z);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_SEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "SEND", MEMO_SEND_SYNTAX);
	}
};

class MSSend : public Module
{
 public:
	MSSend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSSend(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_SEND);
}

MODULE_INIT("ms_send", MSSend)
