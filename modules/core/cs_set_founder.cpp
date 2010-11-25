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
	CommandCSSetFounder(const Anope::string &cpermission = "") : Command("FOUNDER", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetFounder");

		if (this->permission.empty() && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER)))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(params[1]);
		NickCore *nc, *nc0 = ci->founder;

		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
			return MOD_CONT;
		}

		nc = na->nc;
		if (Config->CSMaxReg && nc->channelcount >= Config->CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(CHAN_SET_FOUNDER_TOO_MANY_CHANS, na->nick.c_str());
			return MOD_CONT;
		}

		Log(!this->permission.empty() ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << "to change the founder to " << nc->display;

		/* Founder and successor must not be the same group */
		if (nc == ci->successor)
			ci->successor = NULL;

		--nc0->channelcount;
		ci->founder = nc;
		++nc->channelcount;

		u->SendMessage(ChanServ, CHAN_FOUNDER_CHANGED, ci->name.c_str(), na->nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_FOUNDER, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		SyntaxError(ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SET_FOUNDER);
	}
};

class CommandCSSASetFounder : public CommandCSSetFounder
{
 public:
	CommandCSSASetFounder() : CommandCSSetFounder("chanserv/saset/founder")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_FOUNDER, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		SyntaxError(ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetFounder : public Module
{
	CommandCSSetFounder commandcssetfounder;
	CommandCSSASetFounder commandcssasetfounder;

 public:
	CSSetFounder(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(&commandcssetfounder);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetfounder);
	}

	~CSSetFounder()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetfounder);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetfounder);
	}
};

MODULE_INIT(CSSetFounder)
