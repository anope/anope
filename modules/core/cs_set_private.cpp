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

class CommandCSSetPrivate : public Command
{
 public:
	CommandCSSetPrivate(const Anope::string &cpermission = "") : Command("PRIVATE", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetPrivate");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PRIVATE);
			source.Reply(CHAN_SET_PRIVATE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PRIVATE);
			source.Reply(CHAN_SET_PRIVATE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PRIVATE, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PRIVATE", CHAN_SET_PRIVATE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_PRIVATE);
	}
};

class CommandCSSASetPrivate : public CommandCSSetPrivate
{
 public:
	CommandCSSASetPrivate() : CommandCSSetPrivate("chanserv/saset/private")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PRIVATE, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PRIVATE", CHAN_SASET_PRIVATE_SYNTAX);
	}
};

class CSSetPrivate : public Module
{
	CommandCSSetPrivate commandcssetprivate;
	CommandCSSASetPrivate commandcssasetprivate;

 public:
	CSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetprivate);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetprivate);
	}

	~CSSetPrivate()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetprivate);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetprivate);
	}
};

MODULE_INIT(CSSetPrivate)
