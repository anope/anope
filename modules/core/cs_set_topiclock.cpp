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
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetTopicLock");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_TOPICLOCK);
			source.Reply(CHAN_SET_TOPICLOCK_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_TOPICLOCK);
			source.Reply(CHAN_SET_TOPICLOCK_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "TOPICLOCK");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_TOPICLOCK, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET", CHAN_SET_TOPICLOCK_SYNTAX);;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_TOPICLOCK);
	}
};

class CommandCSSASetTopicLock : public CommandCSSetTopicLock
{
 public:
	CommandCSSASetTopicLock() : CommandCSSetTopicLock("chanserv/saset/topiclock")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_TOPICLOCK, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET", CHAN_SASET_TOPICLOCK_SYNTAX);
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
