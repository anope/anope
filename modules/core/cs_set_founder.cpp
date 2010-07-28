/* ChanServ core functions
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

class CommandCSSetFounder : public Command
{
 public:
	CommandCSSetFounder(const Anope::string &cname, const Anope::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetFounder");

		if (this->permission.empty() && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER)))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(params[1]);
		NickCore *nc, *nc0 = ci->founder;

		if (!na)
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_REGISTERED, params[1].c_str());
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
			return MOD_CONT;
		}

		nc = na->nc;
		if (Config.CSMaxReg && nc->channelcount >= Config.CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SET_FOUNDER_TOO_MANY_CHANS, na->nick.c_str());
			return MOD_CONT;
		}

		Alog() << Config.s_ChanServ << ": Changing founder of " << ci->name << " from " << ci->founder->display << " to " << nc->display << " by " << u->GetMask();

		/* Founder and successor must not be the same group */
		if (nc == ci->successor)
			ci->successor = NULL;

		--nc0->channelcount;
		ci->founder = nc;
		++nc->channelcount;

		notice_lang(Config.s_ChanServ, u, CHAN_FOUNDER_CHANGED, ci->name.c_str(), na->nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_FOUNDER, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_FOUNDER);
	}
};

class CommandCSSASetFounder : public CommandCSSetFounder
{
 public:
	CommandCSSASetFounder(const Anope::string &cname) : CommandCSSetFounder(cname, "chanserv/saset/founder")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_FOUNDER, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetFounder : public Module
{
 public:
	CSSetFounder(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetFounder("FOUNDER"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetFounder("FOUNDER"));
	}

	~CSSetFounder()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("FOUNDER");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("FOUNDER");
	}
};

MODULE_INIT(CSSetFounder)
