// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/memoserv/service.h"

static void rsend_notify(CommandSource &source, MemoInfo *mi, Memo *m, const Anope::string &targ)
{
	/* Only send receipt if memos are allowed */
	if (MemoServ::service && !Anope::ReadOnly)
	{
		/* Get nick alias for sender */
		const NickAlias *na = NickAlias::Find(m->sender);

		if (!na)
			return;

		/* Get nick core for sender */
		const NickCore *nc = na->nc;

		if (!nc)
			return;

		/* Text of the memo varies if the recipient was a
		   nick or channel */
		Anope::string text = Anope::Format(Language::Translate(na->nc, _("\002[auto-memo]\002 The memo you sent to %s has been viewed.")), targ.c_str());

		/* Send notification */
		MemoServ::service->Send(source.GetNick(), m->sender, text, true);

		/* Notify recipient of the memo that a notification has
		   been sent to the sender */
		source.Reply(_("A notification memo has been sent to %s informing them you have read their memo."),
			nc->display.c_str());
	}

	/* Remove receipt flag from the original memo */
	m->receipt = false;
}

class MemoListCallback final
	: public NumberList
{
	CommandSource &source;
	MemoInfo *mi;
	const ChannelInfo *ci;
	bool found;
public:
	MemoListCallback(CommandSource &_source, MemoInfo *_mi, const ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), source(_source), mi(_mi), ci(_ci)
	{
		found = false;
	}

	~MemoListCallback() override
	{
		if (!found)
			source.Reply(_("No memos to display."));
	}

	void HandleNumber(unsigned number) override
	{
		if (!number || number > mi->memos->size())
			return;

		MemoListCallback::DoRead(source, mi, ci, number - 1);
		found = true;
	}

	static void DoRead(CommandSource &source, MemoInfo *mi, const ChannelInfo *ci, unsigned index)
	{
		Memo *m = mi->GetMemo(index);
		if (!m)
			return;

		if (ci)
			source.Reply(_("Memo %d from %s (%s)."), index + 1, m->sender.c_str(), Anope::strftime(m->time, source.GetAccount()).c_str());
		else
			source.Reply(_("Memo %d from %s (%s)."), index + 1, m->sender.c_str(), Anope::strftime(m->time, source.GetAccount()).c_str());

		BotInfo *bi;
		Anope::string cmd;
		if (Command::FindCommandFromService("memoserv/del", bi, cmd))
		{
			if (ci)
				source.Reply(_("To delete, type: \002%s %s %d\002"), bi->GetQueryCommand({}, cmd).c_str(), ci->name.c_str(), index + 1);
			else
				source.Reply(_("To delete, type: \002%s %d\002"), bi->GetQueryCommand({}, cmd).c_str(), index + 1);
		}

		source.Reply("%s", m->text.c_str());
		m->unread = false;

		/* Check if a receipt notification was requested */
		if (m->receipt)
			rsend_notify(source, mi, m, ci ? ci->name : source.GetNick());
	}
};

class CommandMSRead final
	: public Command
{
public:
	CommandMSRead(Module *creator) : Command(creator, "memoserv/read", 1, 2)
	{
		this->SetDesc(_("Read a memo or memos"));
		this->SetSyntax(_("[\037channel\037] {\037num\037 | \037list\037 | LAST | NEW | ALL}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		Anope::string numstr = params[0], chan;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			ci = ChannelInfo::Find(chan);
			if (!ci)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
				return;
			}
			else if (!source.AccessFor(ci).HasPriv("MEMO"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			mi = &ci->memos;
		}
		else
			mi = &source.nc->memos;

		if (numstr.empty() || (!numstr.equals_ci("LAST") && !numstr.equals_ci("NEW") && !numstr.equals_ci("ALL") && numstr.find_first_not_of("0123456789.,-") != Anope::string::npos))
			this->OnSyntaxError(source, numstr);
		else if (mi->memos->empty())
		{
			if (!chan.empty())
				source.Reply(MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				source.Reply(MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			int i, end;

			if (numstr.equals_ci("NEW"))
			{
				int readcount = 0;
				for (i = 0, end = mi->memos->size(); i < end; ++i)
					if (mi->GetMemo(i)->unread)
					{
						MemoListCallback::DoRead(source, mi, ci, i);
						++readcount;
					}
				if (!readcount)
				{
					if (!chan.empty())
						source.Reply(MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
					else
						source.Reply(MEMO_HAVE_NO_NEW_MEMOS);
				}
			}
			else if (numstr.equals_ci("LAST"))
			{
				for (i = 0, end = mi->memos->size() - 1; i < end; ++i);
				MemoListCallback::DoRead(source, mi, ci, i);
			}
			else if (numstr.equals_ci("ALL"))
			{
				for (i = 0, end = mi->memos->size(); i < end; ++i)
				{
					MemoListCallback::DoRead(source, mi, ci, i);
				}
			}
			else /* number[s] */
			{
				MemoListCallback list(source, mi, ci, numstr);
				list.Process();
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sends you the text of the memos specified. If LAST is "
			"given, sends you the memo you most recently received. If "
			"NEW is given, sends you all of your new memos. If ALL is "
			"given, sends you all of your memos. Otherwise, sends you "
			"memo number \037num\037. You can also give a list of numbers."
		));

		ExampleWrapper examples;
		examples.AddEntry("ALL", _(
			"Displays all of your memos."
		));
		examples.AddEntry("LAST", _(
			"Displays your last memo."
		));
		examples.AddEntry("NEW", _(
			"Displays any new memos."
		));
		examples.AddEntry("2-5,7-9", _(
			"Displays memos numbered 2 through 5 and 7 through 9."
		));
		examples.SendTo(source);

		return true;
	}
};

class MSRead final
	: public Module
{
	CommandMSRead commandmsread;

public:
	MSRead(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmsread(this)
	{

	}
};

MODULE_INIT(MSRead)
