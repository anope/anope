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

class CommandMSCheck : public Command
{
 public:
	CommandMSCheck() : Command("CHECK", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na = NULL;
		MemoInfo *mi = NULL;
		int i, found = 0;
		const char *recipient = params[0].c_str();
		struct tm *tm;
		char timebuf[64];

		if (!nick_recognized(u))
		{
			notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
			return MOD_CONT;
		}
		else if (!(na = findnick(recipient)))
		{
			notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, recipient);
			return MOD_CONT;
		}

		if ((na->status & NS_FORBIDDEN))
		{
			notice_lang(s_MemoServ, u, NICK_X_FORBIDDEN, recipient);
			return MOD_CONT;
		}

		mi = &na->nc->memos;

		/* Okay, I know this looks strange but we wanna get the LAST memo, so we
			have to loop backwards */

		for (i = mi->memos.size() - 1; i >= 0; --i)
		{
			if (!stricmp(mi->memos[i]->sender, u->na->nc->display))
			{
				found = 1; /* Yes, we've found the memo */

				tm = localtime(&mi->memos[i]->time);
				strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);

				if (mi->memos[i]->flags & MF_UNREAD)
					notice_lang(s_MemoServ, u, MEMO_CHECK_NOT_READ, na->nick, timebuf);
				else
					notice_lang(s_MemoServ, u, MEMO_CHECK_READ, na->nick, timebuf);
				break;
			}
		}

		if (!found)
			notice_lang(s_MemoServ, u, MEMO_CHECK_NO_MEMO, na->nick);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_CHECK);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "CHECK", MEMO_CHECK_SYNTAX);
	}
};

class MSCheck : public Module
{
 public:
	MSCheck(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSCheck(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_CHECK);
}

MODULE_INIT("ms_check", MSCheck)
