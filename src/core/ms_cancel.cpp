/* MemoServ core functions
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

void myMemoServHelp(User *u);

class CommandMSCancel : public Command
{
 public:
	CommandMSCancel() : Command("CANCEL", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		int ischan;
		int isforbid;
		const char *name = params[0].c_str();
		MemoInfo *mi;

		if (!u->IsRecognized())
			notice_lang(Config.s_MemoServ, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ);
		else if (!(mi = getmemoinfo(name, &ischan, &isforbid)))
		{
			if (isforbid)
				notice_lang(Config.s_MemoServ, u, ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, name);
			else
				notice_lang(Config.s_MemoServ, u, ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, name);
		}
		else
		{
			int i;

			for (i = mi->memos.size() - 1; i >= 0; --i)
				if ((mi->memos[i]->HasFlag(MF_UNREAD)) && !stricmp(mi->memos[i]->sender.c_str(), u->Account()->display) && !mi->memos[i]->HasFlag(MF_NOTIFYS))
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(findnick(name)->nc, mi, mi->memos[i]->number));
					delmemo(mi, mi->memos[i]->number);
					notice_lang(Config.s_MemoServ, u, MEMO_CANCELLED, name);
					return MOD_CONT;
				}

			notice_lang(Config.s_MemoServ, u, MEMO_CANCEL_NONE);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_CANCEL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "CANCEL", MEMO_CANCEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_CANCEL);
	}
};

class MSCancel : public Module
{
 public:
	MSCancel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->AddCommand(MemoServ, new CommandMSCancel());
	}
};

MODULE_INIT(MSCancel)
