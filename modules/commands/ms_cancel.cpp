/* MemoServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/memoserv.h"

class CommandMSCancel : public Command
{
 public:
	CommandMSCancel(Module *creator) : Command(creator, "memoserv/cancel", 1, 1)
	{
		this->SetDesc(_("Cancel the last memo you sent"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly || !MemoServ::service)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nname = params[0];

		bool ischan, isregistered;
		MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(name, ischan, isregistered, false);

		if (!isregistered)
			source.Reply(ischan ? CHAN_X_NOT_REGISTERED : _(NICK_X_NOT_REGISTERED), nname.c_str());
		else
		{
			ChanServ::Channel *ci = NULL;
			NickServ::Nick *na = NULL;
			if (ischan)
				ci = ChanServ::Find(nname);
			else
				na = NickServ::FindNick(nname);
			if (mi)
				for (int i = mi->memos->size() - 1; i >= 0; --i)
					if (mi->GetMemo(i)->unread && source.nc->display.equals_ci(mi->GetMemo(i)->sender))
					{
						if (MemoServ::Event::OnMemoDel)
							MemoServ::Event::OnMemoDel(&MemoServ::Event::MemoDel::OnMemoDel, ischan ? ci->name : na->nc->display, mi, mi->GetMemo(i));
						mi->Del(i);
						source.Reply(_("Last memo to \002%s\002 has been cancelled."), nname.c_str());
						return;
					}

			source.Reply(_("No memo was cancelable."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
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
	MSCancel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmscancel(this)
	{
	}
};

MODULE_INIT(MSCancel)
