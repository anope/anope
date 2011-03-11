/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
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
		this->SetDesc(_("Set the founder of a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetFounder");

		if (this->permission.empty() && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER)))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		NickAlias *na = findnick(params[1]);
		NickCore *nc, *nc0 = ci->founder;

		if (!na)
		{
			source.Reply(_(NICK_X_NOT_REGISTERED), params[1].c_str());
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
			return MOD_CONT;
		}

		nc = na->nc;
		if (Config->CSMaxReg && nc->channelcount >= Config->CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002%s\002 has too many channels registered."), na->nick.c_str());
			return MOD_CONT;
		}

		Log(!this->permission.empty() ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << "to change the founder to " << nc->display;

		/* Founder and successor must not be the same group */
		if (nc == ci->successor)
			ci->successor = NULL;

		--nc0->channelcount;
		ci->founder = nc;
		++nc->channelcount;

		source.Reply(_("Founder of %s changed to \002%s\002."), ci->name.c_str(), na->nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 FOUNDER \037nick\037\002\n"
				" \n"
				"Changes the founder of a channel. The new nickname must\n"
				"be a registered one."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", _(CHAN_SET_SYNTAX));
	}
};

class CommandCSSASetFounder : public CommandCSSetFounder
{
 public:
	CommandCSSASetFounder() : CommandCSSetFounder("chanserv/saset/founder")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", _(CHAN_SASET_SYNTAX));
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
			c->AddSubcommand(this, &commandcssetfounder);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetfounder);
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
