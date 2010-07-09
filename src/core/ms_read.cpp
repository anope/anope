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
	MemoListCallback(User *_u, MemoInfo *_mi, const std::string &numlist) : NumberList(numlist, false), u(_u), mi(_mi)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > mi->memos.size())
			return;

		MemoListCallback::DoRead(u, mi, NULL, Number - 1);
	}

	static void DoRead(User *u, MemoInfo *mi, ChannelInfo *ci, unsigned index)
	{
		Memo *m = mi->memos[index];
		struct tm tm = *localtime(&m->time);
		char timebuf[64];
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
		timebuf[sizeof(timebuf) - 1] = 0;
		if (ci)
			notice_lang(Config.s_MemoServ, u, MEMO_CHAN_HEADER, m->number, m->sender.c_str(), timebuf, Config.s_MemoServ, ci->name.c_str(), m->number);
		else
			notice_lang(Config.s_MemoServ, u, MEMO_HEADER, m->number, m->sender.c_str(), timebuf, Config.s_MemoServ, m->number);
		notice_lang(Config.s_MemoServ, u, MEMO_TEXT, m->text);
		m->UnsetFlag(MF_UNREAD);

		/* Check if a receipt notification was requested */
		if (m->HasFlag(MF_RECEIPT))
			rsend_notify(u, m, ci ? ci->name.c_str() : NULL);
	}
};

class CommandMSRead : public Command
{
 public:
	CommandMSRead() : Command("READ", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		ci::string numstr = params.size() ? params[0] : "", chan;
		int num;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				notice_lang(Config.s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
			mi = &u->Account()->memos;
		num = !numstr.empty() ? atoi(numstr.c_str()) : -1;
		if (numstr.empty() || (numstr != "LAST" && numstr != "NEW" && num <= 0))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else {
			int i, end;

			if (numstr == "NEW")
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
						notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
					else
						notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
				}
			}
			else if (numstr == "LAST")
			{
				for (i = 0, end = mi->memos.size() - 1; i < end; ++i);
				MemoListCallback::DoRead(u, mi, ci, i);
			}
			else /* number[s] */
			{
				MemoListCallback list(u, mi, numstr.c_str());
				list.Process();
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_READ);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "READ", MEMO_READ_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_READ);
	}
};

class MSRead : public Module
{
 public:
	MSRead(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, new CommandMSRead());
	}
};

MODULE_INIT(MSRead)
