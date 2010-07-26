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
	ChannelInfo *ci;
	const MemoInfo *mi;
	bool SentHeader;
 public:
	MemoListCallback(User *_u, ChannelInfo *_ci, const MemoInfo *_mi, const Anope::string &list) : NumberList(list, false), u(_u), ci(_ci), mi(_mi), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > mi->memos.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			if (ci)
				notice_lang(Config.s_MemoServ, u, MEMO_LIST_CHAN_MEMOS, ci->name.c_str(), Config.s_MemoServ.c_str(), ci->name.c_str());
			else
				notice_lang(Config.s_MemoServ, u, MEMO_LIST_MEMOS, u->nick.c_str(), Config.s_MemoServ.c_str());

			notice_lang(Config.s_MemoServ, u, MEMO_LIST_HEADER);
		}

		DoList(u, ci, mi, Number - 1);
	}

	static void DoList(User *u, ChannelInfo *ci, const MemoInfo *mi, unsigned index)
	{
		Memo *m = mi->memos[index];
		struct tm tm = *localtime(&m->time);
		char timebuf[64];
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
		timebuf[sizeof(timebuf) - 1] = 0;   /* just in case */
		notice_lang(Config.s_MemoServ, u, MEMO_LIST_FORMAT, (m->HasFlag(MF_UNREAD)) ? '*' : ' ', m->number, m->sender.c_str(), timebuf);
	}
};

class CommandMSList : public Command
{
 public:
	CommandMSList() : Command("LIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string param = !params.empty() ? params[0] : "", chan;
		ChannelInfo *ci;
		const MemoInfo *mi;
		int i, end;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

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
		if (!param.empty() && !isdigit(param[0]) && !param.equals_ci("NEW"))
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
			if (!param.empty() && isdigit(param[0]))
			{
				MemoListCallback list(u, ci, mi, param);
				list.Process();
			}
			else
			{
				if (!param.empty())
				{
					for (i = 0, end = mi->memos.size(); i < end; ++i)
						if (mi->memos[i]->HasFlag(MF_UNREAD))
							break;
					if (i == end)
					{
						if (!chan.empty())
							notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
						else
							notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
						return MOD_CONT;
					}
				}

				bool SentHeader = false;

				for (i = 0, end = mi->memos.size(); i < end; ++i)
				{
					if (!param.empty() && !mi->memos[i]->HasFlag(MF_UNREAD))
						continue;

					if (!SentHeader)
					{
						SentHeader = true;
						if (ci)
							notice_lang(Config.s_MemoServ, u, !param.empty() ? MEMO_LIST_CHAN_NEW_MEMOS : MEMO_LIST_CHAN_MEMOS, ci->name.c_str(), Config.s_MemoServ.c_str(), ci->name.c_str());
						else
							notice_lang(Config.s_MemoServ, u, !param.empty() ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS, u->nick.c_str(), Config.s_MemoServ.c_str());
						notice_lang(Config.s_MemoServ, u, MEMO_LIST_HEADER);
					}

					MemoListCallback::DoList(u, ci, mi, i);
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_LIST);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "LIST", MEMO_LIST_SYNTAX);
	}

	void OnServCommand(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_LIST);
	}
};

class MSList : public Module
{
 public:
	MSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, new CommandMSList());
	}
};

MODULE_INIT(MSList)
