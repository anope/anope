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

class CommandMSInfo : public Command
{
 public:
	CommandMSInfo() : Command("INFO", 0, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		MemoInfo *mi;
		NickAlias *na = NULL;
		ChannelInfo *ci = NULL;
		const char *name = params.size() ? params[0].c_str() : NULL;
		int hardmax = 0;

		if (name && *name != '#' && u->nc->HasPriv("memoserv/info"))
		{
			na = findnick(name);
			if (!na)
			{
				notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, name);
				return MOD_CONT;
			}
			else if (na->status & NS_FORBIDDEN)
			{
				notice_lang(s_MemoServ, u, NICK_X_FORBIDDEN, name);
				return MOD_CONT;
			}
			mi = &na->nc->memos;
			hardmax = na->nc->flags & NI_MEMO_HARDMAX ? 1 : 0;
		}
		else if (name && *name == '#')
		{
			ci = cs_findchan(name);
			if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
			hardmax = ci->flags & CI_MEMO_HARDMAX ? 1 : 0;
		}
		else if (name) /* It's not a chan and we aren't services admin */
		{
			notice_lang(s_MemoServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else 
		{
			mi = &u->nc->memos;
			hardmax = u->nc->flags & NI_MEMO_HARDMAX ? 1 : 0;
		}

		if (name && (ci || na->nc != u->nc))
		{
			if (mi->memos.empty())
				notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_MEMOS, name);
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->flags & MF_UNREAD)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO_UNREAD, name);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO, name);
			}
			else
			{
				int count = 0, i;
				for (i = 0; i < mi->memos.size(); ++i)
				{
					if (mi->memos[i]->flags & MF_UNREAD)
						++count;
				}
				if (count == mi->memos.size())
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ALL_UNREAD, name, count);
				else if (!count)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS, name, mi->memos.size());
				else if (count == 1)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ONE_UNREAD, name, mi->memos.size());
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_SOME_UNREAD, name, mi->memos.size(), count);
			}
			if (!mi->memomax)
			{
				if (hardmax)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name, mi->memomax);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_X_LIMIT, name, mi->memomax);
			}
			else if (mi->memomax > 0)
			{
				if (hardmax)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name, mi->memomax);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_X_LIMIT, name, mi->memomax);
			}
			else
				notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_LIMIT, name);

			/* I ripped this code out of ircservices 4.4.5, since I didn't want
			   to rewrite the whole thing (it pisses me off). */
			if (na)
			{
				if ((na->nc->flags & NI_MEMO_RECEIVE) && (na->nc->flags & NI_MEMO_SIGNON))
					notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_ON, name);
				else if (na->nc->flags & NI_MEMO_RECEIVE)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_RECEIVE, name);
				else if (na->nc->flags & NI_MEMO_SIGNON)
					notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_SIGNON, name);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_OFF, name);
			}
		}
		else /* !name || (!ci || na->nc == u->nc) */
		{
			if (mi->memos.empty())
				notice_lang(s_MemoServ, u, MEMO_INFO_NO_MEMOS);
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->flags & MF_UNREAD)
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMO_UNREAD);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMO);
			}
			else
			{
				int count = 0, i;
				for (i = 0; i < mi->memos.size(); ++i)
				{
					if (mi->memos[i]->flags & MF_UNREAD)
						++count;
				}
				if (count == mi->memos.size())
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ALL_UNREAD, count);
				else if (!count)
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS, mi->memos.size());
				else if (count == 1)
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ONE_UNREAD, mi->memos.size());
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_SOME_UNREAD, mi->memos.size(), count);
			}

			if (!mi->memomax)
			{
				if (!u->nc->IsServicesOper() && hardmax)
					notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT_ZERO);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT_ZERO);
			}
			else if (mi->memomax > 0)
			{
				if (!u->nc->IsServicesOper() && hardmax)
					notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT, mi->memomax);
				else
					notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT, mi->memomax);
			}
			else
				notice_lang(s_MemoServ, u, MEMO_INFO_NO_LIMIT);

			/* Ripped too. But differently because of a seg fault (loughs) */
			if ((u->nc->flags & NI_MEMO_RECEIVE) && (u->nc->flags & NI_MEMO_SIGNON))
				notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_ON);
			else if (u->nc->flags & NI_MEMO_RECEIVE)
				notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_RECEIVE);
			else if (u->nc->flags & NI_MEMO_SIGNON)
				notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_SIGNON);
			else
				notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_OFF);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_MemoServ, u, MEMO_SERVADMIN_HELP_INFO);
		else
			notice_help(s_MemoServ, u, MEMO_HELP_INFO);

		return true;
	}
};

class MSInfo : public Module
{
 public:
	MSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSInfo(), MOD_UNIQUE);
	}
	void MemoServHelp(User *u)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_INFO);
	}
};

MODULE_INIT("ms_info", MSInfo)
