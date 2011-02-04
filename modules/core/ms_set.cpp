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

class CommandMSSet : public Command
{
 private:
	CommandReturn DoNotify(User *u, const std::vector<Anope::string> &params, MemoInfo *mi)
	{
		Anope::string param = params[1];

		if (param.equals_ci("ON"))
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			u->SendMessage(MemoServ, MEMO_SET_NOTIFY_ON, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("LOGON"))
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			u->SendMessage(MemoServ, MEMO_SET_NOTIFY_LOGON, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("NEW"))
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			u->SendMessage(MemoServ, MEMO_SET_NOTIFY_NEW, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("MAIL"))
		{
			if (!u->Account()->email.empty())
			{
				u->Account()->SetFlag(NI_MEMO_MAIL);
				u->SendMessage(MemoServ, MEMO_SET_NOTIFY_MAIL);
			}
			else
				u->SendMessage(MemoServ, MEMO_SET_NOTIFY_INVALIDMAIL);
		}
		else if (param.equals_ci("NOMAIL"))
		{
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			u->SendMessage(MemoServ, MEMO_SET_NOTIFY_NOMAIL);
		}
		else if (param.equals_ci("OFF"))
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			u->SendMessage(MemoServ, MEMO_SET_NOTIFY_OFF, Config->s_MemoServ.c_str());
		}
		else
			SyntaxError(MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoLimit(User *u, const std::vector<Anope::string> &params, MemoInfo *mi)
	{
		Anope::string p1 = params[1];
		Anope::string p2 = params.size() > 2 ? params[2] : "";
		Anope::string p3 = params.size() > 3 ? params[3] : "";
		Anope::string user, chan;
		int16 limit;
		NickCore *nc = u->Account();
		ChannelInfo *ci = NULL;
		bool is_servadmin = u->Account()->HasPriv("memoserv/set-limit");

		if (p1[0] == '#')
		{
			chan = p1;
			p1 = p2;
			p2 = p3;
			p3 = params.size() > 4 ? params[4] : "";
			if (!(ci = cs_findchan(chan)))
			{
				u->SendMessage(MemoServ, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!is_servadmin && !check_access(u, ci, CA_MEMO))
			{
				u->SendMessage(MemoServ, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		if (is_servadmin)
		{
			if (!p2.empty() && !p2.equals_ci("HARD") && chan.empty())
			{
				NickAlias *na;
				if (!(na = findnick(p1)))
				{
					u->SendMessage(MemoServ, NICK_X_NOT_REGISTERED, p1.c_str());
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
				SyntaxError(MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if ((!p1.is_pos_number_only() && !p1.equals_ci("NONE")) || (!p2.empty() && !p2.equals_ci("HARD")))
			{
				SyntaxError(MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
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
			limit = -1;
			try
			{
				limit = convertTo<int16>(p1);
			}
			catch (const CoreException &) { }
		}
		else
		{
			if (p1.empty() || !p2.empty() || !isdigit(p1[0]))
			{
				SyntaxError(MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
				return MOD_CONT;
			}
			if (!chan.empty() && ci->HasFlag(CI_MEMO_HARDMAX))
			{
				u->SendMessage(MemoServ, MEMO_SET_LIMIT_FORBIDDEN, chan.c_str());
				return MOD_CONT;
			}
			else if (chan.empty() && nc->HasFlag(NI_MEMO_HARDMAX))
			{
				u->SendMessage(MemoServ, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
				return MOD_CONT;
			}
			limit = -1;
			try
			{
				limit = convertTo<int16>(p1);
			}
			catch (const CoreException &) { }
			/* The first character is a digit, but we could still go negative
			 * from overflow... watch out! */
			if (limit < 0 || (Config->MSMaxMemos > 0 && static_cast<unsigned>(limit) > Config->MSMaxMemos))
			{
				if (!chan.empty())
					u->SendMessage(MemoServ, MEMO_SET_LIMIT_TOO_HIGH, chan.c_str(), Config->MSMaxMemos);
				else
					u->SendMessage(MemoServ, MEMO_SET_YOUR_LIMIT_TOO_HIGH, Config->MSMaxMemos);
				return MOD_CONT;
			}
		}
		mi->memomax = limit;
		if (limit > 0)
		{
			if (chan.empty() && nc == u->Account())
				u->SendMessage(MemoServ, MEMO_SET_YOUR_LIMIT, limit);
			else
				u->SendMessage(MemoServ, MEMO_SET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str(), limit);
		}
		else if (!limit)
		{
			if (chan.empty() && nc == u->Account())
				u->SendMessage(MemoServ, MEMO_SET_YOUR_LIMIT_ZERO);
			else
				u->SendMessage(MemoServ, MEMO_SET_LIMIT_ZERO, !chan.empty() ? chan.c_str() : user.c_str());
		}
		else
		{
			if (chan.empty() && nc == u->Account())
				u->SendMessage(MemoServ, MEMO_UNSET_YOUR_LIMIT);
			else
				u->SendMessage(MemoServ, MEMO_UNSET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str());
		}
		return MOD_CONT;
	}
 public:
	CommandMSSet() : Command("SET", 2, 5)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];
		MemoInfo *mi = &u->Account()->memos;

		if (readonly)
		{
			u->SendMessage(MemoServ, MEMO_SET_DISABLED);
			return MOD_CONT;
		}
		else if (cmd.equals_ci("NOTIFY"))
			return this->DoNotify(u, params, mi);
		else if (cmd.equals_ci("LIMIT"))
			return this->DoLimit(u, params, mi);
		else
		{
			u->SendMessage(MemoServ, NICK_SET_UNKNOWN_OPTION, cmd.c_str());
			u->SendMessage(MemoServ, MORE_INFO, Config->s_MemoServ.c_str(), "SET");
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			u->SendMessage(MemoServ, MEMO_HELP_SET);
		else if (subcommand.equals_ci("NOTIFY"))
			u->SendMessage(MemoServ, MEMO_HELP_SET_NOTIFY);
		else if (subcommand.equals_ci("LIMIT"))
		{
			if (u->Account() && u->Account()->IsServicesOper())
				u->SendMessage(MemoServ, MEMO_SERVADMIN_HELP_SET_LIMIT, Config->MSMaxMemos);
			else
				u->SendMessage(MemoServ, MEMO_HELP_SET_LIMIT, Config->MSMaxMemos);
		}
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_SET);
	}
};

class MSSet : public Module
{
	CommandMSSet commandmsset;

 public:
	MSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsset);
	}
};

MODULE_INIT(MSSet)
