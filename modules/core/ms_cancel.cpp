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
		this->SetDesc("Cancel last memo you sent");
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
				source.Reply(ischan ? _(CHAN_X_FORBIDDEN) : _(NICK_X_FORBIDDEN), nname.c_str());
			else
				source.Reply(ischan ? _(CHAN_X_NOT_REGISTERED) : _(NICK_X_NOT_REGISTERED), nname.c_str());
		}
		else
		{
			for (int i = mi->memos.size() - 1; i >= 0; --i)
				if (mi->memos[i]->HasFlag(MF_UNREAD) && u->Account()->display.equals_ci(mi->memos[i]->sender) && !mi->memos[i]->HasFlag(MF_NOTIFYS))
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(findnick(nname)->nc, mi, mi->memos[i]));
					mi->Del(mi->memos[i]);
					source.Reply(_("Last memo to \002%s\002 has been cancelled."), nname.c_str());
					return MOD_CONT;
				}

			source.Reply(_("No memo was cancelable."));
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002CANCEL {\037nick\037 | \037channel\037}\002\n"
				" \n"
				"Cancels the last memo you sent to the given nick or channel,\n"
				"provided it has not been read at the time you use the command."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CANCEL", _("CANCEL {\037nick\037 | \037channel\037}"));
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
