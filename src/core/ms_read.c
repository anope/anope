/* MemoServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int read_memo_callback(User *u, int num, va_list args);
int read_memo(User *u, int index, MemoInfo *mi, const char *chan);
extern void rsend_notify(User *u, Memo *m, const char *chan);

class CommandMSRead : public Command
{
 public:
	CommandMSRead() : Command("READ", 0, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci;
		ci::string numstr = params.size() ? params[0] : "", chan;
		int num, count;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan.c_str())))
			{
				notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
		{
			mi = &u->nc->memos;
		}
		num = !numstr.empty() ? atoi(numstr.c_str()) : -1;
		if (numstr.empty() || (numstr != "LAST" && numstr != "NEW" && num <= 0))
			this->OnSyntaxError(u);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else {
			int i;

			if (numstr == "NEW")
			{
				int readcount = 0;
				for (i = 0; i < mi->memos.size(); ++i)
				{
					if (mi->memos[i]->HasFlag(MF_UNREAD))
					{
						read_memo(u, i, mi, chan.c_str());
						++readcount;
					}
				}
				if (!readcount)
				{
					if (!chan.empty())
						notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
					else
						notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
				}
			}
			else if (numstr == "LAST")
			{
				for (i = 0; i < mi->memos.size() - 1; ++i);
				read_memo(u, i, mi, chan.c_str());
			}
			else /* number[s] */
			{
				if (!process_numlist(numstr.c_str(), &count, read_memo_callback, u, mi, chan.c_str()))
				{
					if (count == 1)
						notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, num);
					else
						notice_lang(s_MemoServ, u, MEMO_LIST_NOT_FOUND, numstr.c_str());
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_MemoServ, u, MEMO_HELP_READ);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "READ", MEMO_READ_SYNTAX);
	}
};

class MSRead : public Module
{
 public:
	MSRead(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSRead());

		ModuleManager::Attach(I_OnMemoServHelp, this);
	}
	void OnMemoServHelp(User *u)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_READ);
	}
};

/**
 * Read a memo callback function
 * @param u User Struct
 * @param int Index number
 * @param va_list variable arguements
 * @return result of read_memo()
 */
int read_memo_callback(User *u, int num, va_list args)
{
	MemoInfo *mi = va_arg(args, MemoInfo *);
	const char *chan = va_arg(args, const char *);
	int i;

	for (i = 0; i < mi->memos.size(); ++i)
	{
		if (mi->memos[i]->number == num)
			break;
	}
	/* Range check done in read_memo */
	return read_memo(u, i, mi, chan);
}

/**
 * Read a memo
 * @param u User Struct
 * @param int Index number
 * @param mi MemoInfo struct
 * @param chan Channel Name
 * @return 1 on success, 0 if failed
 */
int read_memo(User *u, int index, MemoInfo *mi, const char *chan)
{
	Memo *m;
	char timebuf[64];
	struct tm tm;

	if (index < 0 || index >= mi->memos.size())
		return 0;
	m = mi->memos[index];
	tm = *localtime(&m->time);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
	timebuf[sizeof(timebuf) - 1] = 0;
	if (chan)
		notice_lang(s_MemoServ, u, MEMO_CHAN_HEADER, m->number, m->sender, timebuf, s_MemoServ, chan, m->number);
	else
		notice_lang(s_MemoServ, u, MEMO_HEADER, m->number, m->sender, timebuf, s_MemoServ, m->number);
	notice_lang(s_MemoServ, u, MEMO_TEXT, m->text);
	m->UnsetFlag(MF_UNREAD);

	/* Check if a receipt notification was requested */
	if (m->HasFlag(MF_RECEIPT))
		rsend_notify(u, m, chan);

	return 1;
}

MODULE_INIT(MSRead)
