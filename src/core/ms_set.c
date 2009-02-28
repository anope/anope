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

void myMemoServHelp(User *u);

class CommandMSSet : public Command
{
 private:
	CommandReturn DoNotify(User *u, std::vector<std::string> &params, MemoInfo *mi)
	{
		const char *param = params[1].c_str();

		if (!stricmp(param, "ON"))
		{
			u->nc->flags |= NI_MEMO_SIGNON | NI_MEMO_RECEIVE;
			notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_ON, s_MemoServ);
		}
		else if (!stricmp(param, "LOGON"))
		{
			u->nc->flags |= NI_MEMO_SIGNON;
			u->nc->flags &= ~NI_MEMO_RECEIVE;
			notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_LOGON, s_MemoServ);
		}
		else if (!stricmp(param, "NEW"))
		{
			u->nc->flags &= ~NI_MEMO_SIGNON;
			u->nc->flags |= NI_MEMO_RECEIVE;
			notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NEW, s_MemoServ);
		}
		else if (!stricmp(param, "MAIL"))
		{
			if (u->nc->email)
			{
				u->nc->flags |= NI_MEMO_MAIL;
				notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_MAIL);
			}
			else
				notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_INVALIDMAIL);
		}
		else if (!stricmp(param, "NOMAIL"))
		{
			u->nc->flags &= ~NI_MEMO_MAIL;
			notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NOMAIL);
		}
		else if (!stricmp(param, "OFF"))
		{
			u->nc->flags &= ~(NI_MEMO_SIGNON | NI_MEMO_RECEIVE);
			notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_OFF, s_MemoServ);
		}
		else
			syntax_error(s_MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoLimit(User *u, std::vector<std::string> &params, MemoInfo *mi)
	{
		const char *p1 = params[1].c_str();
		const char *p2 = params.size() > 2 ? params[2].c_str() : NULL;
		const char *p3 = params.size() > 3 ? params[3].c_str() : NULL;
		const char *user = NULL, *chan = NULL;
		int32 limit;
		NickCore *nc = u->nc;
		ChannelInfo *ci = NULL;
		bool is_servadmin = u->nc->HasPriv("memoserv/set-limit");

		if (*p1 == '#')
		{
			chan = p1;
			p1 = p2;
			p2 = p3;
			p3 = params.size() > 4 ? params[4].c_str() : NULL;
			if (!(ci = cs_findchan(chan)))
			{
				notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
				return MOD_CONT;
			}
			else if (ci->flags & CI_FORBIDDEN)
			{
				notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
				return MOD_CONT;
			}
			else if (!is_servadmin && !check_access(u, ci, CA_MEMO))
			{
				notice_lang(s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		if (is_servadmin)
		{
			if (p2 && stricmp(p2, "HARD") && !chan)
			{
				NickAlias *na;
				if (!(na = findnick(p1)))
				{
					notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, p1);
					return MOD_CONT;
				}
				user = p1;
				mi = &na->nc->memos;
				nc = na->nc;
				p1 = p2;
				p2 = p3;
			}
			else if (!p1)
			{
				syntax_error(s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if ((!isdigit(*p1) && stricmp(p1, "NONE")) || (p2 && stricmp(p2, "HARD")))
			{
				syntax_error(s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if (chan)
			{
				if (p2)
					ci->flags |= CI_MEMO_HARDMAX;
				else
					ci->flags &= ~CI_MEMO_HARDMAX;
			}
			else
			{
				if (p2)
					nc->flags |= NI_MEMO_HARDMAX;
				else
					nc->flags &= ~NI_MEMO_HARDMAX;
			}
			limit = atoi(p1);
			if (limit < 0 || limit > 32767)
			{
				notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
			if (stricmp(p1, "NONE") == 0)
				limit = -1;
		}
		else
		{
			if (!p1 || p2 || !isdigit(*p1)) {
				syntax_error(s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
				return MOD_CONT;
			}
			if (chan && (ci->flags & CI_MEMO_HARDMAX))
			{
				notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_FORBIDDEN, chan);
				return MOD_CONT;
			}
			else if (!chan && (nc->flags & NI_MEMO_HARDMAX)) {
				notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
				return MOD_CONT;
			}
			limit = atoi(p1);
			/* The first character is a digit, but we could still go negative
			 * from overflow... watch out! */
			if (limit < 0 || (MSMaxMemos > 0 && limit > MSMaxMemos))
			{
				if (chan)
					notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_TOO_HIGH, chan, MSMaxMemos);
				else
					notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_TOO_HIGH, MSMaxMemos);
				return MOD_CONT;
			}
			else if (limit > 32767)
			{
				notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
		}
		mi->memomax = limit;
		if (limit > 0)
		{
			if (!chan && nc == u->nc)
				notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT, limit);
			else
				notice_lang(s_MemoServ, u, MEMO_SET_LIMIT, chan ? chan : user, limit);
		}
		else if (!limit)
		{
			if (!chan && nc == u->nc)
				notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_ZERO);
			else
				notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_ZERO, chan ? chan : user);
		}
		else
		{
			if (!chan && nc == u->nc)
				notice_lang(s_MemoServ, u, MEMO_UNSET_YOUR_LIMIT);
			else
				notice_lang(s_MemoServ, u, MEMO_UNSET_LIMIT,
							chan ? chan : user);
		}
		return MOD_CONT;
	}
 public:
	CommandMSSet() : Command("SET", 2, 5)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();
		MemoInfo *mi = &u->nc->memos;

		if (readonly)
		{
			notice_lang(s_MemoServ, u, MEMO_SET_DISABLED);
			return MOD_CONT;
		}
		if (!nick_identified(u))
			notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (!stricmp(cmd, "NOTIFY"))
			return this->DoNotify(u, params, mi);
		else if (!stricmp(cmd, "LIMIT")) {
			return this->DoLimit(u, params, mi);
		}
		else
		{
			notice_lang(s_MemoServ, u, MEMO_SET_UNKNOWN_OPTION, cmd);
			notice_lang(s_MemoServ, u, MORE_INFO, s_MemoServ, "SET");
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(s_MemoServ, u, MEMO_HELP_SET);
		else if (subcommand == "NOTIFY")
			notice_help(s_MemoServ, u, MEMO_HELP_SET_NOTIFY);
		else if (subcommand == "LIMIT")
		{
			if (is_services_oper(u))
				notice_help(s_MemoServ, u, MEMO_SERVADMIN_HELP_SET_LIMIT, MSMaxMemos);
			else
				notice_help(s_MemoServ, u, MEMO_HELP_SET_LIMIT, MSMaxMemos);
		}

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "SET", MEMO_SET_SYNTAX);
	}
};

class MSSet : public Module
{
 public:
	MSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(MEMOSERV, new CommandMSSet(), MOD_UNIQUE);

		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_SET);
}

MODULE_INIT("ms_set", MSSet)
