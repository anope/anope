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

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(const Anope::string &cpermission = "") : Command("KEEPTOPIC", 2, 2, cpermission)
	{
		this->SetDesc(_("Retain topic when channel is not in use"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetKeepTopic");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "KEEPTOPIC");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 KEEPTOPIC {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002topic retention\002 option for a	\n"
				"channel. When \002topic retention\002 is set, the topic for the\n"
				"channel will be remembered by %s even after the\n"
				"last user leaves the channel, and will be restored the\n"
				"next time the channel is created."), this->name.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET KEEPTOPIC", _("SET \037channel\037 KEEPTOPIC {ON | OFF}"));
	}
};

class CommandCSSASetKeepTopic : public CommandCSSetKeepTopic
{
 public:
	CommandCSSASetKeepTopic() : CommandCSSetKeepTopic("chanserv/saset/keeptopic")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET KEEPTOPIC", _("SASET \002channel\002 KEEPTOPIC {ON | OFF}"));
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
			c->AddSubcommand(this, &commandcssetkeeptopic);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetkeeptopic);
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
