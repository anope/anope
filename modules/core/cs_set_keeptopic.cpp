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

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(const Anope::string &cpermission = "") : Command("KEEPTOPIC", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetKeepTopic");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_KEEPTOPIC);
			source.Reply(CHAN_SET_KEEPTOPIC_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			source.Reply(CHAN_SET_KEEPTOPIC_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "KEEPTOPIC");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_KEEPTOPIC, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET KEEPTOPIC", CHAN_SET_KEEPTOPIC_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_KEEPTOPIC);
	}
};

class CommandCSSASetKeepTopic : public CommandCSSetKeepTopic
{
 public:
	CommandCSSASetKeepTopic() : CommandCSSetKeepTopic("chanserv/saset/keeptopic")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_KEEPTOPIC, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET KEEPTOPIC", CHAN_SASET_KEEPTOPIC_SYNTAX);
	}
};

class CSSetKeepTopic : public Module
{
	CommandCSSetKeepTopic commandcssetkeeptopic;
	CommandCSSASetKeepTopic commandcssasetkeeptopic;

 public:
	CSSetKeepTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(&commandcssetkeeptopic);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetkeeptopic);
	}

	~CSSetKeepTopic()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetkeeptopic);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetkeeptopic);
	}
};

MODULE_INIT(CSSetKeepTopic)
