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
			this->OnSyntaxError(source, "RESTRICTED");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_RESTRICTED, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET RESTRICTED", CHAN_SET_RESTRICTED_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_RESTRICTED);
	}
};

class CommandCSSASetRestricted : public CommandCSSetRestricted
{
 public:
	CommandCSSASetRestricted() : CommandCSSetRestricted("chanserv/saset/restricted")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_RESTRICTED, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET RESTRICTED", CHAN_SASET_RESTRICTED_SYNTAX);
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
			c->AddSubcommand(this, &commandcssetrestricted);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetrestricted);
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
