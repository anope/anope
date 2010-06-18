/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
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

int list_memo_callback(User *u, int num, va_list args);
int list_memo(User *u, int index, MemoInfo *mi, int *sent_header, int newi, const char *chan);

class CommandMSList : public Command
{
 public:
	CommandMSList() : Command("LIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string param = params.size() ? params[0] : "", chan;
		ChannelInfo *ci;
		MemoInfo *mi;
		int i;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan.c_str())))
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
		{
			mi = &u->Account()->memos;
		}
		if (!param.empty() && !isdigit(param[0]) && param != "NEW")
			this->OnSyntaxError(u, param);
		else if (!mi->memos.size())
		{
			if (!chan.empty())
				notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			int sent_header = 0;
			if (!param.empty() && isdigit(param[0]))
				process_numlist(param.c_str(), NULL, list_memo_callback, u, mi, &sent_header, chan.empty() ? NULL : chan.c_str());
			else
			{
				if (!param.empty())
				{
					for (i = 0; i < mi->memos.size(); ++i)
					{
						if (mi->memos[i]->HasFlag(MF_UNREAD))
							break;
					}
					if (i == mi->memos.size())
					{
						if (!chan.empty())
							notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
						else
							notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
						return MOD_CONT;
					}
				}
				for (i = 0; i < mi->memos.size(); ++i)
				{
					if (!param.empty() && !(mi->memos[i]->HasFlag(MF_UNREAD)))
						continue;
					list_memo(u, i, mi, &sent_header, !param.empty(), chan.empty() ? NULL : chan.c_str());
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_LIST);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "LIST", MEMO_LIST_SYNTAX);
	}
};

class MSList : public Module
{
 public:
	MSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSList());

		ModuleManager::Attach(I_OnMemoServHelp, this);
	}
	void OnMemoServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_LIST);
	}
};

/**
 * list memno callback function
 * @param u User Struct
 * @param int Memo number
 * @param va_list List of arguements
 * @return result form list_memo()
 */
int list_memo_callback(User *u, int num, va_list args)
{
	MemoInfo *mi = va_arg(args, MemoInfo *);
	int *sent_header = va_arg(args, int *);
	const char *chan = va_arg(args, const char *);
	int i;

	for (i = 0; i < mi->memos.size(); ++i)
	{
		if (mi->memos[i]->number == num)
			break;
	}
	/* Range checking done by list_memo() */
	return list_memo(u, i, mi, sent_header, 0, chan);
}

/**
 * Display a single memo entry, possibly printing the header first.
 * @param u User Struct
 * @param int Memo index
 * @param mi MemoInfo Struct
 * @param send_header If we are to send the headers
 * @param newi If we are listing new memos
 * @param chan Channel name
 * @return MOD_CONT
 */
int list_memo(User *u, int index, MemoInfo *mi, int *sent_header, int newi, const char *chan)
{
	Memo *m;
	char timebuf[64];
	struct tm tm;

	if (index < 0 || index >= mi->memos.size())
		return 0;
	if (!*sent_header)
	{
		if (chan)
			notice_lang(Config.s_MemoServ, u, newi ? MEMO_LIST_CHAN_NEW_MEMOS : MEMO_LIST_CHAN_MEMOS, chan, Config.s_MemoServ, chan);
		else
			notice_lang(Config.s_MemoServ, u, newi ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS, u->nick.c_str(), Config.s_MemoServ);
		notice_lang(Config.s_MemoServ, u, MEMO_LIST_HEADER);
		*sent_header = 1;
	}
	m = mi->memos[index];
	tm = *localtime(&m->time);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
	timebuf[sizeof(timebuf) - 1] = 0;   /* just in case */
	notice_lang(Config.s_MemoServ, u, MEMO_LIST_FORMAT, (m->HasFlag(MF_UNREAD)) ? '*' : ' ', m->number, m->sender.c_str(), timebuf);
	return 1;
}

MODULE_INIT(MSList)
