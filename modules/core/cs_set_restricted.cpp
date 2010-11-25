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

class CommandCSSetRestricted : public Command
{
 public:
	CommandCSSetRestricted(const Anope::string &cpermission = "") : Command("RESTRICTED", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetRestricted");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] < 0)
				ci->levels[CA_NOJOIN] = 0;
			source.Reply(CHAN_SET_RESTRICTED_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] >= 0)
				ci->levels[CA_NOJOIN] = -2;
			source.Reply(CHAN_SET_RESTRICTED_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "RESTRICTED");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_RESTRICTED, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SET RESTRICTED", CHAN_SET_RESTRICTED_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SET_RESTRICTED);
	}
};

class CommandCSSASetRestricted : public CommandCSSetRestricted
{
 public:
	CommandCSSASetRestricted() : CommandCSSetRestricted("chanserv/saset/restricted")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_RESTRICTED, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SASET RESTRICTED", CHAN_SASET_RESTRICTED_SYNTAX);
	}
};

class CSSetRestricted : public Module
{
	CommandCSSetRestricted commandcssetrestricted;
	CommandCSSASetRestricted commandcssasetrestricted;

 public:
	CSSetRestricted(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(&commandcssetrestricted);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetrestricted);
	}

	~CSSetRestricted()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetrestricted);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetrestricted);
	}
};

MODULE_INIT(CSSetRestricted)
