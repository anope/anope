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
#include "memoserv.h"

class CommandMSCancel : public Command
{
 public:
	CommandMSCancel(Module *creator) : Command(creator, "memoserv/cancel", 1, 1)
	{
		this->SetDesc(_("Cancel the last memo you sent"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nname = params[0];

		bool ischan;
		MemoInfo *mi = memoserv->GetMemoInfo(nname, ischan);

		if (mi == NULL)
			source.Reply(ischan ? CHAN_X_NOT_REGISTERED : _(NICK_X_NOT_REGISTERED), nname.c_str());
		else
		{
			for (int i = mi->memos.size() - 1; i >= 0; --i)
				if (mi->memos[i]->HasFlag(MF_UNREAD) && u->Account()->display.equals_ci(mi->memos[i]->sender))
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(findnick(nname)->nc, mi, mi->memos[i]));
					mi->Del(mi->memos[i]);
					source.Reply(_("Last memo to \002%s\002 has been cancelled."), nname.c_str());
					return;
				}

			source.Reply(_("No memo was cancelable."));
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Cancels the last memo you sent to the given nick or channel,\n"
				"provided it has not been read at the time you use the command."));
		return true;
	}
};

class MSCancel : public Module
{
	CommandMSCancel commandmscancel;

 public:
	MSCancel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmscancel(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmscancel);
	}
};

MODULE_INIT(MSCancel)
