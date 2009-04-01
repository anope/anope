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

class CommandMSStaff : public Command
{
 public:
	CommandMSStaff() : Command("STAFF", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickCore *nc;
		int i, z = 0;
		const char *text = params[0].c_str();

		if (!u->nc->HasCommand("memoserv/staff"))
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
				if (nc->IsServicesOper())
					memo_send(u, nc->display, text, z);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_oper(u))
			return false;

		notice_help(s_MemoServ, u, MEMO_HELP_STAFF);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "STAFF", MEMO_STAFF_SYNTAX);
	}
};

class MSStaff : public Module
{
 public:
	MSStaff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSStaff(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	if (is_services_oper(u))
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_STAFF);
}

MODULE_INIT("ms_staff", MSStaff)
