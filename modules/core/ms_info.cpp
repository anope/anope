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

class CommandMSInfo : public Command
{
 public:
	CommandMSInfo() : Command("INFO", 0, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		const MemoInfo *mi;
		NickAlias *na = NULL;
		ChannelInfo *ci = NULL;
		Anope::string name = !params.empty() ? params[0] : "";
		int hardmax = 0;

		if (!name.empty() && name[0] != '#' && u->Account()->HasPriv("memoserv/info"))
		{
			na = findnick(name);
			if (!na)
			{
				notice_lang(Config->s_MemoServ, u, NICK_X_NOT_REGISTERED, name.c_str());
				return MOD_CONT;
			}
			else if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config->s_MemoServ, u, NICK_X_FORBIDDEN, name.c_str());
				return MOD_CONT;
			}
			mi = &na->nc->memos;
			hardmax = na->nc->HasFlag(NI_MEMO_HARDMAX) ? 1 : 0;
		}
		else if (!name.empty() && name[0] == '#')
		{
			if (!(ci = cs_findchan(name)))
			{
				notice_lang(Config->s_MemoServ, u, CHAN_X_NOT_REGISTERED, name.c_str());
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(Config->s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
			hardmax = ci->HasFlag(CI_MEMO_HARDMAX) ? 1 : 0;
		}
		else if (!name.empty()) /* It's not a chan and we aren't services admin */
		{
			notice_lang(Config->s_MemoServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else
		{
			mi = &u->Account()->memos;
			hardmax = u->Account()->HasFlag(NI_MEMO_HARDMAX) ? 1 : 0;
		}

		if (!name.empty() && (ci || na->nc != u->Account()))
		{
			if (mi->memos.empty())
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NO_MEMOS, name.c_str());
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->HasFlag(MF_UNREAD))
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMO_UNREAD, name.c_str());
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMO, name.c_str());
			}
			else
			{
				int count = 0, i, end;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
						++count;
				if (count == mi->memos.size())
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMOS_ALL_UNREAD, name.c_str(), count);
				else if (!count)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMOS, name.c_str(), mi->memos.size());
				else if (count == 1)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMOS_ONE_UNREAD, name.c_str(), mi->memos.size());
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_MEMOS_SOME_UNREAD, name.c_str(), mi->memos.size(), count);
			}
			if (!mi->memomax)
			{
				if (hardmax)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name.c_str(), mi->memomax);
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_LIMIT, name.c_str(), mi->memomax);
			}
			else if (mi->memomax > 0)
			{
				if (hardmax)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name.c_str(), mi->memomax);
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_LIMIT, name.c_str(), mi->memomax);
			}
			else
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NO_LIMIT, name.c_str());

			/* I ripped this code out of ircservices 4.4.5, since I didn't want
			   to rewrite the whole thing (it pisses me off). */
			if (na)
			{
				if (na->nc->HasFlag(NI_MEMO_RECEIVE) && na->nc->HasFlag(NI_MEMO_SIGNON))
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NOTIFY_ON, name.c_str());
				else if (na->nc->HasFlag(NI_MEMO_RECEIVE))
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NOTIFY_RECEIVE, name.c_str());
				else if (na->nc->HasFlag(NI_MEMO_SIGNON))
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NOTIFY_SIGNON, name.c_str());
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_X_NOTIFY_OFF, name.c_str());
			}
		}
		else /* !name || (!ci || na->nc == u->Account()) */
		{
			if (mi->memos.empty())
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NO_MEMOS);
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->HasFlag(MF_UNREAD))
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMO_UNREAD);
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMO);
			}
			else
			{
				int count = 0, i, end;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
						++count;
				if (count == mi->memos.size())
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMOS_ALL_UNREAD, count);
				else if (!count)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMOS, mi->memos.size());
				else if (count == 1)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMOS_ONE_UNREAD, mi->memos.size());
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_MEMOS_SOME_UNREAD, mi->memos.size(), count);
			}

			if (!mi->memomax)
			{
				if (!u->Account()->IsServicesOper() && hardmax)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_HARD_LIMIT_ZERO);
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_LIMIT_ZERO);
			}
			else if (mi->memomax > 0)
			{
				if (!u->Account()->IsServicesOper() && hardmax)
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_HARD_LIMIT, mi->memomax);
				else
					notice_lang(Config->s_MemoServ, u, MEMO_INFO_LIMIT, mi->memomax);
			}
			else
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NO_LIMIT);

			/* Ripped too. But differently because of a seg fault (loughs) */
			if (u->Account()->HasFlag(NI_MEMO_RECEIVE) && u->Account()->HasFlag(NI_MEMO_SIGNON))
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NOTIFY_ON);
			else if (u->Account()->HasFlag(NI_MEMO_RECEIVE))
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NOTIFY_RECEIVE);
			else if (u->Account()->HasFlag(NI_MEMO_SIGNON))
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NOTIFY_SIGNON);
			else
				notice_lang(Config->s_MemoServ, u, MEMO_INFO_NOTIFY_OFF);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config->s_MemoServ, u, MEMO_SERVADMIN_HELP_INFO);
		else
			notice_help(Config->s_MemoServ, u, MEMO_HELP_INFO);

		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_MemoServ, u, MEMO_HELP_CMD_INFO);
	}
};

class MSInfo : public Module
{
	CommandMSInfo commandmsinfo;

 public:
	MSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsinfo);
	}
};

MODULE_INIT(MSInfo)
