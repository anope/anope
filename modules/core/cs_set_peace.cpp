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

class CommandCSSetPeace : public Command
{
 public:
	CommandCSSetPeace(const Anope::string &cpermission = "") : Command("PEACE", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetPeace");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PEACE);
			source.Reply(CHAN_SET_PEACE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PEACE);
			source.Reply(CHAN_SET_PEACE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PEACE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PEACE, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PEACE", CHAN_SET_PEACE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_PEACE, "SET");
	}
};

class CommandCSSASetPeace : public CommandCSSetPeace
{
 public:
	CommandCSSASetPeace() : CommandCSSetPeace("chanserv/saset/peace")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PEACE, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PEACE", CHAN_SASET_PEACE_SYNTAX);
	}
};

class CSSetPeace : public Module
{
	CommandCSSetPeace commandcssetpeace;
	CommandCSSASetPeace commandcssasetpeace;

 public:
	CSSetPeace(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetpeace);
	}

	~CSSetPeace()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetpeace);
	}
};

MODULE_INIT(CSSetPeace)
