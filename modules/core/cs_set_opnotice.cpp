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

class CommandCSSetOpNotice : public Command
{
 public:
	CommandCSSetOpNotice(const Anope::string &cpermission = "") : Command("OPNOTICE", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetOpNotice");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_OPNOTICE);
			source.Reply(CHAN_SET_OPNOTICE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_OPNOTICE);
			source.Reply(CHAN_SET_OPNOTICE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "OPNOTICE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_OPNOTICE, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET OPNOTICE", CHAN_SET_OPNOTICE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_OPNOTICE);
	}
};

class CommandCSSASetOpNotice : public CommandCSSetOpNotice
{
 public:
	CommandCSSASetOpNotice() : CommandCSSetOpNotice("chanserv/saset/opnotice")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_OPNOTICE, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET OPNOTICE", CHAN_SASET_OPNOTICE_SYNTAX);
	}
};

class CSSetOpNotice : public Module
{
	CommandCSSetOpNotice commandcssetopnotice;
	CommandCSSASetOpNotice commandcssasetopnotice;

 public:
	CSSetOpNotice(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetopnotice);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetopnotice);
	}

	~CSSetOpNotice()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetopnotice);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetopnotice);
	}
};

MODULE_INIT(CSSetOpNotice)
