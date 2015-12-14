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
		this->SetSyntax(_("{\037user\037 | \037channel\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &nname = params[0];

		bool ischan, isregistered;
		MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(name, ischan, isregistered, false);

		if (!isregistered)
		{
			if (ischan)
				source.Reply(_("Channel \002{0}\002 isn't registered."), nname);
			else
				source.Reply(_("\002{0}\002 isn't registered."), nname);
		}
		else
		{
			ChanServ::Channel *ci = NULL;
			NickServ::Nick *na = NULL;
			if (ischan)
				ci = ChanServ::Find(nname);
			else
				na = NickServ::FindNick(nname);
			if (mi)
			{
				auto memos = mi->GetMemos();
				for (int i = memos.size() - 1; i >= 0; --i)
				{
					MemoServ::Memo *m = memos[i];
					if (m->GetUnread() && source.nc->GetDisplay().equals_ci(m->GetSender()))
					{
						if (MemoServ::Event::OnMemoDel)
							MemoServ::Event::OnMemoDel(&MemoServ::Event::MemoDel::OnMemoDel, ischan ? ci->GetName() : na->GetAccount()->GetDisplay(), mi, m);
						mi->Del(i);
						source.Reply(_("Your last memo to \002{0}\002 has been cancelled."), nname);
						return;
					}
				}
			}

			source.Reply(_("No memo to \002{0}\002 was cancelable."), nname);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Cancels the last memo you sent to \037user\037, provided it has not yet been read."));
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
