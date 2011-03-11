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

class CommandCSSetTopicLock : public Command
{
 public:
	CommandCSSetTopicLock(const Anope::string &cpermission = "") : Command("TOPICLOCK", 2, 2, cpermission)
	{
		this->SetDesc(_("Topic can only be changed with TOPIC"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetTopicLock");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_TOPICLOCK);
			source.Reply(_("Topic lock option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_TOPICLOCK);
			source.Reply(_("Topic lock option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "TOPICLOCK");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 TOPICLOCK {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002topic lock\002 option for a channel.\n"
				"When \002topic lock\002 is set, %s will not allow the\n"
				"channel topic to be changed except via the \002TOPIC\002\n"
				"command."), this->name.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET", _("SET \037channel\037 TOPICLOCK {ON | OFF}"));;
	}
};

class CommandCSSASetTopicLock : public CommandCSSetTopicLock
{
 public:
	CommandCSSASetTopicLock() : CommandCSSetTopicLock("chanserv/saset/topiclock")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET", _("SASET \002channel\002 TOPICLOCK {ON | OFF}"));
	}
};

class CSSetTopicLock : public Module
{
	CommandCSSetTopicLock commandcssettopiclock;
	CommandCSSASetTopicLock commandcssasettopiclock;

 public:
	CSSetTopicLock(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssettopiclock);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasettopiclock);
	}

	~CSSetTopicLock()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssettopiclock);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasettopiclock);
	}
};

MODULE_INIT(CSSetTopicLock)
