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

class CommandMSCancel : public Command
{
 public:
	CommandMSCancel() : Command("CANCEL", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		int ischan;
		int isforbid;
		const char *name = params[0].c_str();
		MemoInfo *mi;

		if (!nick_recognized(u))
			notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (!(mi = getmemoinfo(name, &ischan, &isforbid)))
		{
			if (isforbid)
				notice_lang(s_MemoServ, u, ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, name);
			else
				notice_lang(s_MemoServ, u, ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, name);
		}
		else
		{
			int i;

			for (i = mi->memos.size() - 1; i >= 0; --i)
			{
				if ((mi->memos[i]->flags & MF_UNREAD) && !stricmp(mi->memos[i]->sender, u->nc->display) && (!(mi->memos[i]->flags & MF_NOTIFYS)))
				{
					delmemo(mi, mi->memos[i]->number);
					notice_lang(s_MemoServ, u, MEMO_CANCELLED, name);
					return MOD_CONT;
				}
			}

			notice_lang(s_MemoServ, u, MEMO_CANCEL_NONE);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_MemoServ, u, MEMO_HELP_CANCEL);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "CANCEL", MEMO_CANCEL_SYNTAX);
	}
};

class MSCancel : public Module
{
 public:
	MSCancel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSCancel(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_CANCEL);
}

MODULE_INIT("ms_cancel", MSCancel)
