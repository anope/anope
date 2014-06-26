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

static void rsend_notify(CommandSource &source, MemoServ::MemoInfo *mi, MemoServ::Memo *m, const Anope::string &targ)
{
	/* Only send receipt if memos are allowed */
	if (MemoServ::service && !Anope::ReadOnly)
	{
		/* Get nick alias for sender */
		const NickServ::Nick *na = NickServ::FindNick(m->sender);

		if (!na)
			return;

		/* Get nick core for sender */
		const NickServ::Account *nc = na->nc;

		if (!nc)
			return;

		/* Text of the memo varies if the recepient was a
		   nick or channel */
		Anope::string text = Anope::printf(Language::Translate(na->nc, _("\002[auto-memo]\002 The memo you sent to \002%s\002 has been viewed.")), targ.c_str());

		/* Send notification */
		MemoServ::service->Send(source.GetNick(), m->sender, text, true);

		/* Notify recepient of the memo that a notification has
		   been sent to the sender */
		source.Reply(_("A notification memo has been sent to \002{0}\002 informing him/her you have read his/her memo."), nc->display);
	}

	/* Remove receipt flag from the original memo */
	m->receipt = false;
}

class CommandMSRead : public Command
{
	static void DoRead(CommandSource &source, MemoServ::MemoInfo *mi, const ChanServ::Channel *ci, unsigned index)
	{
		MemoServ::Memo *m = mi->GetMemo(index);
		if (!m)
			return;

		if (ci)
			source.Reply(_("Memo \002{0}\002 from \002{1}\002 (\002{2}\002)."), index + 1, m->sender, Anope::strftime(m->time, source.GetAccount()));
		else
			source.Reply(_("Memo \002{0}\002 from \002{1}\002 (\002{2}\002)."), index + 1, m->sender, Anope::strftime(m->time, source.GetAccount()));

		BotInfo *bi;
		Anope::string cmd;
		if (Command::FindCommandFromService("memoserv/del", bi, cmd))
		{
			if (ci)
				source.Reply(_("To delete, use \002{0}{1} {2} {3} {4}\002"), Config->StrictPrivmsg, bi->nick, cmd, ci->name, index + 1);
			else
				source.Reply(_("To delete, use \002{0}{1} {2} {3}\002"), Config->StrictPrivmsg, bi->nick, cmd, index + 1);
		}

		source.Reply(m->text);
		m->unread = false;

		/* Check if a receipt notification was requested */
		if (m->receipt)
			rsend_notify(source, mi, m, ci ? ci->name : source.GetNick());
	}

 public:
	CommandMSRead(Module *creator) : Command(creator, "memoserv/read", 1, 2)
	{
		this->SetDesc(_("Read a memo or memos"));
		this->SetSyntax(_("[\037channel\037] {\037num\037 | \037list\037 | LAST | NEW}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		MemoServ::MemoInfo *mi;
		ChanServ::Channel *ci = NULL;
		Anope::string numstr = params[0], chan;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			ci = ChanServ::Find(chan);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
				return;
			}

			if (!source.AccessFor(ci).HasPriv("MEMO"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->name);
				return;
			}

			mi = ci->memos;
		}
		else
			mi = source.nc->memos;

		if (numstr.empty() || (!numstr.equals_ci("LAST") && !numstr.equals_ci("NEW") && !numstr.is_number_only()))
		{
			this->OnSyntaxError(source, numstr);
			return;
		}

		if (mi->memos->empty())
		{
			if (!chan.empty())
				source.Reply(_("\002{0}\002 has no memos."), chan);
			else
				source.Reply(_("You have no memos."));
			return;
		}

		int i, end;

		if (numstr.equals_ci("NEW"))
		{
			int readcount = 0;
			for (i = 0, end = mi->memos->size(); i < end; ++i)
				if (mi->GetMemo(i)->unread)
				{
					DoRead(source, mi, ci, i);
					++readcount;
				}
			if (!readcount)
			{
				if (!chan.empty())
					source.Reply(_("\002{0}\002 has no new memos."), chan);
				else
					source.Reply(_("You have no new memos."));
			}
		}
		else if (numstr.equals_ci("LAST"))
		{
			for (i = 0, end = mi->memos->size() - 1; i < end; ++i);
			DoRead(source, mi, ci, i);
		}
		else /* number[s] */
		{
			NumberList(numstr, false,
				[&](unsigned int number)
				{
					if (!number || number > mi->memos->size())
						return;

					DoRead(source, mi, ci, number - 1);
				},
				[]{});
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sends you the text of the memos specified."
		               " If LAST is given, sends you the memo you most recently received."
		               " If NEW is given, sends you all of your new memos."
		               " Otherwise, sends you memo number \037num\037."
		               " You can also give a list of numbers, as in the example:\n"
		               "\n"
		               "Example:\n"
		               "\n"
		               "         {0} 2-5,7-9\n"
		               "         Displays memos numbered 2 through 5 and 7 through 9."),
		               source.command);
		return true;
	}
};

class MSRead : public Module
{
	CommandMSRead commandmsread;

 public:
	MSRead(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsread(this)
	{

	}
};

MODULE_INIT(MSRead)
