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
	CommandReturn DoNotify(CommandSource &source, const std::vector<Anope::string> &params, MemoInfo *mi)
	{
		User *u = source.u;
		const Anope::string &param = params[1];

		if (param.equals_ci("ON"))
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			source.Reply(MEMO_SET_NOTIFY_ON, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("LOGON"))
		{
			u->Account()->SetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			source.Reply(MEMO_SET_NOTIFY_LOGON, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("NEW"))
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->SetFlag(NI_MEMO_RECEIVE);
			source.Reply(MEMO_SET_NOTIFY_NEW, Config->s_MemoServ.c_str());
		}
		else if (param.equals_ci("MAIL"))
		{
			if (!u->Account()->email.empty())
			{
				u->Account()->SetFlag(NI_MEMO_MAIL);
				source.Reply(MEMO_SET_NOTIFY_MAIL);
			}
			else
				source.Reply(MEMO_SET_NOTIFY_INVALIDMAIL);
		}
		else if (param.equals_ci("NOMAIL"))
		{
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			source.Reply(MEMO_SET_NOTIFY_NOMAIL);
		}
		else if (param.equals_ci("OFF"))
		{
			u->Account()->UnsetFlag(NI_MEMO_SIGNON);
			u->Account()->UnsetFlag(NI_MEMO_RECEIVE);
			u->Account()->UnsetFlag(NI_MEMO_MAIL);
			source.Reply(MEMO_SET_NOTIFY_OFF, Config->s_MemoServ.c_str());
		}
		else
			SyntaxError(source, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoLimit(CommandSource &source, const std::vector<Anope::string> &params, MemoInfo *mi)
	{
		User *u = source.u;

		Anope::string p1 = params[1];
		Anope::string p2 = params.size() > 2 ? params[2] : "";
		Anope::string p3 = params.size() > 3 ? params[3] : "";
		Anope::string user, chan;
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
			if (!(ci = cs_findchan(chan)))
			{
				source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (!is_servadmin && !check_access(u, ci, CA_MEMO))
			{
				source.Reply(ACCESS_DENIED);
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
					source.Reply(NICK_X_NOT_REGISTERED, p1.c_str());
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
				SyntaxError(source, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
				return MOD_CONT;
			}
			if ((!isdigit(p1[0]) && !p1.equals_ci("NONE")) || (!p2.empty() && !p2.equals_ci("HARD")))
			{
				SyntaxError(source, "SET LIMIT", MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
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
			limit = p1.is_pos_number_only() ? convertTo<int32>(p1) : -1;
			if (limit < 0 || limit > 32767)
			{
				source.Reply(MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
			if (p1.equals_ci("NONE"))
				limit = -1;
		}
		else
		{
			if (p1.empty() || !p2.empty() || !isdigit(p1[0]))
			{
				SyntaxError(source, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
				return MOD_CONT;
			}
			if (!chan.empty() && ci->HasFlag(CI_MEMO_HARDMAX))
			{
				source.Reply(MEMO_SET_LIMIT_FORBIDDEN, chan.c_str());
				return MOD_CONT;
			}
			else if (chan.empty() && nc->HasFlag(NI_MEMO_HARDMAX))
			{
				source.Reply(MEMO_SET_YOUR_LIMIT_FORBIDDEN);
				return MOD_CONT;
			}
			limit = p1.is_pos_number_only() ? convertTo<int32>(p1) : -1;
			/* The first character is a digit, but we could still go negative
			 * from overflow... watch out! */
			if (limit < 0 || (Config->MSMaxMemos > 0 && static_cast<unsigned>(limit) > Config->MSMaxMemos))
			{
				if (!chan.empty())
					source.Reply(MEMO_SET_LIMIT_TOO_HIGH, chan.c_str(), Config->MSMaxMemos);
				else
					source.Reply(MEMO_SET_YOUR_LIMIT_TOO_HIGH, Config->MSMaxMemos);
				return MOD_CONT;
			}
			else if (limit > 32767)
			{
				source.Reply(MEMO_SET_LIMIT_OVERFLOW, 32767);
				limit = 32767;
			}
		}
		mi->memomax = limit;
		if (limit > 0)
		{
			if (chan.empty() && nc == u->Account())
				source.Reply(MEMO_SET_YOUR_LIMIT, limit);
			else
				source.Reply(MEMO_SET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str(), limit);
		}
		else if (!limit)
		{
			if (chan.empty() && nc == u->Account())
				source.Reply(MEMO_SET_YOUR_LIMIT_ZERO);
			else
				source.Reply(MEMO_SET_LIMIT_ZERO, !chan.empty() ? chan.c_str() : user.c_str());
		}
		else
		{
			if (chan.empty() && nc == u->Account())
				source.Reply(MEMO_UNSET_YOUR_LIMIT);
			else
				source.Reply(MEMO_UNSET_LIMIT, !chan.empty() ? chan.c_str() : user.c_str());
		}
		return MOD_CONT;
	}
 public:
	CommandMSSet() : Command("SET", 2, 5)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &cmd = params[0];
		MemoInfo *mi = &u->Account()->memos;

		if (readonly)
			source.Reply(MEMO_SET_DISABLED);
		else if (cmd.equals_ci("NOTIFY"))
			return this->DoNotify(source, params, mi);
		else if (cmd.equals_ci("LIMIT"))
			return this->DoLimit(source, params, mi);
		else
		{
			source.Reply(NICK_SET_UNKNOWN_OPTION, cmd.c_str());
			source.Reply(MORE_INFO, Config->s_MemoServ.c_str(), "SET");
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			source.Reply(MEMO_HELP_SET);
		else if (subcommand.equals_ci("NOTIFY"))
			source.Reply(MEMO_HELP_SET_NOTIFY);
		else if (subcommand.equals_ci("LIMIT"))
		{
			User *u = source.u;
			if (u->Account() && u->Account()->IsServicesOper())
				source.Reply(MEMO_SERVADMIN_HELP_SET_LIMIT, Config->MSMaxMemos);
			else
				source.Reply(MEMO_HELP_SET_LIMIT, Config->MSMaxMemos);
		}
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(MEMO_HELP_CMD_SET);
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
