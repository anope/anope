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

class MemoListCallback : public NumberList
{
	User *u;
	MemoInfo *mi;
 public:
	MemoListCallback(User *_u, MemoInfo *_mi, const Anope::string &numlist) : NumberList(numlist, false), u(_u), mi(_mi)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > mi->memos.size())
			return;

		MemoListCallback::DoRead(u, mi, NULL, Number - 1);
	}

	static void DoRead(User *u, MemoInfo *mi, ChannelInfo *ci, unsigned index)
	{
		Memo *m = mi->memos[index];
		if (ci)
			u->SendMessage(MemoServ, MEMO_CHAN_HEADER, index + 1, m->sender.c_str(), do_strftime(m->time).c_str(), Config->s_MemoServ.c_str(), ci->name.c_str(), index + 1);
		else
			u->SendMessage(MemoServ, MEMO_HEADER, index + 1, m->sender.c_str(), do_strftime(m->time).c_str(), Config->s_MemoServ.c_str(), index + 1);
		u->SendMessage(MemoServ, MEMO_TEXT, m->text.c_str());
		m->UnsetFlag(MF_UNREAD);

		/* Check if a receipt notification was requested */
		if (m->HasFlag(MF_RECEIPT))
			rsend_notify(u, m, ci ? ci->name : "");
	}
};

class CommandMSRead : public Command
{
 public:
	CommandMSRead() : Command("READ", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		Anope::string numstr = params[0], chan;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				u->SendMessage(MemoServ, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				u->SendMessage(MemoServ, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
			mi = &u->Account()->memos;

		if (numstr.empty() || (!numstr.equals_ci("LAST") && !numstr.equals_ci("NEW") && !numstr.is_number_only()))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				u->SendMessage(MemoServ, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				u->SendMessage(MemoServ, MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			int i, end;

			if (numstr.equals_ci("NEW"))
			{
				int readcount = 0;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
					{
						MemoListCallback::DoRead(u, mi, ci, i);
						++readcount;
					}
				if (!readcount)
				{
					if (!chan.empty())
						u->SendMessage(MemoServ, MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
					else
						u->SendMessage(MemoServ, MEMO_HAVE_NO_NEW_MEMOS);
				}
			}
			else if (numstr.equals_ci("LAST"))
			{
				for (i = 0, end = mi->memos.size() - 1; i < end; ++i);
				MemoListCallback::DoRead(u, mi, ci, i);
			}
			else /* number[s] */
			{
				MemoListCallback list(u, mi, numstr);
				list.Process();
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(MemoServ, MEMO_HELP_READ);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "READ", MEMO_READ_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_READ);
	}
};

class MSRead : public Module
{
	CommandMSRead commandmsread;

 public:
	MSRead(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsread);
	}
};

MODULE_INIT(MSRead)
