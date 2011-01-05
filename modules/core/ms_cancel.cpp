/* MemoServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nname = params[0];

		bool ischan, isforbid;
		MemoInfo *mi;

		if (!(mi = getmemoinfo(nname, ischan, isforbid)))
		{
			if (isforbid)
				source.Reply(ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, nname.c_str());
			else
				source.Reply(ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nname.c_str());
		}
		else
		{
			for (int i = mi->memos.size() - 1; i >= 0; --i)
				if (mi->memos[i]->HasFlag(MF_UNREAD) && u->Account()->display.equals_ci(mi->memos[i]->sender) && !mi->memos[i]->HasFlag(MF_NOTIFYS))
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(findnick(nname)->nc, mi, mi->memos[i]));
					mi->Del(mi->memos[i]);
					source.Reply(MEMO_CANCELLED, nname.c_str());
					return MOD_CONT;
				}

			source.Reply(MEMO_CANCEL_NONE);
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(MEMO_HELP_CANCEL);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CANCEL", MEMO_CANCEL_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(MEMO_HELP_CMD_CANCEL);
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
