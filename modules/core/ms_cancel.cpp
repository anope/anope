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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		bool ischan, isforbid;
		Anope::string nname = params[0];
		MemoInfo *mi;

		if (!(mi = getmemoinfo(nname, ischan, isforbid)))
		{
			if (isforbid)
				u->SendMessage(MemoServ, ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, nname.c_str());
			else
				u->SendMessage(MemoServ, ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nname.c_str());
		}
		else
		{
			int i;

			for (i = mi->memos.size() - 1; i >= 0; --i)
				if (mi->memos[i]->HasFlag(MF_UNREAD) && u->Account()->display.equals_ci(mi->memos[i]->sender) && !mi->memos[i]->HasFlag(MF_NOTIFYS))
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(findnick(nname)->nc, mi, mi->memos[i]));
					mi->Del(mi->memos[i]);
					u->SendMessage(MemoServ, MEMO_CANCELLED, nname.c_str());
					return MOD_CONT;
				}

			u->SendMessage(MemoServ, MEMO_CANCEL_NONE);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CANCEL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "CANCEL", MEMO_CANCEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_CANCEL);
	}
};

class MSCancel : public Module
{
	CommandMSCancel commandmscancel;

 public:
	MSCancel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmscancel);
	}
};

MODULE_INIT(MSCancel)
