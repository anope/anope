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

class CommandMSHelp : public Command
{
 public:
	CommandMSHelp() : Command("HELP", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		mod_help_cmd(s_MemoServ, u, MEMOSERV, params.size() > 0 ? params[0].c_str() : NULL);
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_MemoServ, u, MEMO_HELP_HEADER);
		moduleDisplayHelp(3, u);
		notice_help(s_MemoServ, u, MEMO_HELP_FOOTER, s_ChanServ);
	}
};

class MSHelp : public Module
{
 public:
	MSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSHelp(), MOD_UNIQUE);
	}
};

MODULE_INIT("ms_help", MSHelp)
