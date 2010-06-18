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

class CommandMSSet : public Command
{
 private:
	CommandReturn DoNotify(User *u, const std::vector<ci::string> &params, MemoInfo *mi)
	{
		ci::string param = params[1];

		if (param == "ON")
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_ON, Config.s_MemoServ);
		}
		else if (param == "LOGON")
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_LOGON, Config.s_MemoServ);
		}
		else if (param == "NEW")
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_NEW, Config.s_MemoServ);
		}
		else if (param == "MAIL")
		{
			if (u->Account()->email)
			{
				u->Account()->SetFlag(NI_MEMO_MAIL);
				notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_MAIL);
			}
			else
				notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_INVALIDMAIL);
		}
		else if (param == "NOMAIL")
		{
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_NOMAIL);
		}
		else if (param == "OFF")
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			notice_lang(Config.s_MemoServ, u, MEMO_SET_NOTIFY_OFF, Config.s_MemoServ);
		}
		else
			syntax_error(Config.s_MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoLimit(User *u, const std::vector<ci::string> &params, MemoInfo *mi)
	{
		ci::string p1 = params[1];
		ci::string p2 = params.size() > 2 ? params[2] : "";
		ci::string p3 = params.size() > 3 ? params[3] : "";
		ci::string user, chan;
		int32 limit;
		NickCore *nc = u->Account();
		ChannelInfo *ci = NULL;
		bool is_servadmin = u->Account()->HasPriv("memoserv/set-limit");

		if (p1[0] == '#')
		{
			chan = p1;
			p1 = p2;
			p2 = p3;
			p3 = params.size() > 4 ? params[4] : "";
			if (!(ci = cs_findchan(chan.c_str())))
			{
				notice_lang(Config.s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!is_servadmin && !check_access(u, ci, CA_MEMO))
			{
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		if (is_servadmin)
		{
			if (!p2.empty() && p2 != "HARD" && chan.empty())
			{
				NickAlias *na;
				if (!(na = findnick(p1.c_str())))
				{
					notice_lang(Config.s_MemoServ, u, NICK_X_NOT_REGISTERED, p1.c_str());
					return MOD_CONT;
				}
				user = p1;
				mi = &na->nc->memos;
				nc = na->nc;
				p1 = p2;
				p2 = p3;
			}
			else if (p1.empty())
			{
				syntax_error(Config.s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if ((!isdigit(p1[0]) && p1 != "NONE") || (!p2.empty() && p2 != "HARD"))
			{
				syntax_error(Config.s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if (!chan.empty())
			{
				if (!p2.empty())
					ci->SetFlag(CI_MEMO_HARDMAX);
				else
					ci->UnsetFlag(CI_MEMO_HARDMAX);
			}
			else
			{
				if (!p2.empty())
					nc->SetFlag(NI_MEMO_HARDMAX);
				else
					nc->UnsetFlag(NI_MEMO_HARDMAX);
			}
			limit = atoi(p1.c_str());
			if (limit < 0 || limit > 32767)
			{
				notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
			if (p1 == "NONE")
				limit = -1;
		}
		else
		{
			if (p1.empty() || !p2.empty() || !isdigit(p1[0])) {
				syntax_error(Config.s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
				return MOD_CONT;
			}
			if (!chan.empty() && (ci->HasFlag(CI_MEMO_HARDMAX)))
			{
				notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT_FORBIDDEN, chan.c_str());
				return MOD_CONT;
			}
			else if (chan.empty() && (nc->HasFlag(NI_MEMO_HARDMAX)))
			{
				notice_lang(Config.s_MemoServ, u, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
				return MOD_CONT;
			}
			limit = atoi(p1.c_str());
			/* The first character is a digit, but we could still go negative
			 * from overflow... watch out! */
			if (limit < 0 || (Config.MSMaxMemos > 0 && limit > Config.MSMaxMemos))
			{
				if (!chan.empty())
					notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT_TOO_HIGH, chan.c_str(), Config.MSMaxMemos);
				else
					notice_lang(Config.s_MemoServ, u, MEMO_SET_YOUR_LIMIT_TOO_HIGH, Config.MSMaxMemos);
				return MOD_CONT;
			}
			else if (limit > 32767)
			{
				notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
		}
		mi->memomax = limit;
		if (limit > 0)
		{
			if (chan.empty() && nc == u->Account())
				notice_lang(Config.s_MemoServ, u, MEMO_SET_YOUR_LIMIT, limit);
			else
				notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str(), limit);
		}
		else if (!limit)
		{
			if (chan.empty() && nc == u->Account())
				notice_lang(Config.s_MemoServ, u, MEMO_SET_YOUR_LIMIT_ZERO);
			else
				notice_lang(Config.s_MemoServ, u, MEMO_SET_LIMIT_ZERO, !chan.empty() ? chan.c_str() : user.c_str());
		}
		else
		{
			if (chan.empty() && nc == u->Account())
				notice_lang(Config.s_MemoServ, u, MEMO_UNSET_YOUR_LIMIT);
			else
				notice_lang(Config.s_MemoServ, u, MEMO_UNSET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str());
		}
		return MOD_CONT;
	}
 public:
	CommandMSSet() : Command("SET", 2, 5)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];
		MemoInfo *mi = &u->Account()->memos;

		if (readonly)
		{
			notice_lang(Config.s_MemoServ, u, MEMO_SET_DISABLED);
			return MOD_CONT;
		}
		else if (cmd == "NOTIFY")
			return this->DoNotify(u, params, mi);
		else if (cmd == "LIMIT")
			return this->DoLimit(u, params, mi);
		else
		{
			notice_lang(Config.s_MemoServ, u, MEMO_SET_UNKNOWN_OPTION, cmd.c_str());
			notice_lang(Config.s_MemoServ, u, MORE_INFO, Config.s_MemoServ, "SET");
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config.s_MemoServ, u, MEMO_HELP_SET);
		else if (subcommand == "NOTIFY")
			notice_help(Config.s_MemoServ, u, MEMO_HELP_SET_NOTIFY);
		else if (subcommand == "LIMIT")
		{
			if (u->Account() && u->Account()->IsServicesOper())
				notice_help(Config.s_MemoServ, u, MEMO_SERVADMIN_HELP_SET_LIMIT, Config.MSMaxMemos);
			else
				notice_help(Config.s_MemoServ, u, MEMO_HELP_SET_LIMIT, Config.MSMaxMemos);
		}
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "SET", MEMO_SET_SYNTAX);
	}
};

class MSSet : public Module
{
 public:
	MSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(MEMOSERV, new CommandMSSet());

		ModuleManager::Attach(I_OnMemoServHelp, this);
	}
	void OnMemoServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_SET);
	}
};

MODULE_INIT(MSSet)
