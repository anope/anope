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

class CommandCSSetTopicLock : public Command
{
 public:
	CommandCSSetTopicLock(const Anope::string &cname, const Anope::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_TOPICLOCK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_TOPICLOCK_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_TOPICLOCK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_TOPICLOCK_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "TOPICLOCK");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_TOPICLOCK, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_TOPICLOCK_SYNTAX);;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_TOPICLOCK);
	}
};

class CommandCSSASetTopicLock : public CommandCSSetTopicLock
{
 public:
	CommandCSSASetTopicLock(const Anope::string &cname) : CommandCSSetTopicLock(cname, "chanserv/saset/topiclock")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_TOPICLOCK, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_TOPICLOCK_SYNTAX);
	}
};

class CSSetTopicLock : public Module
{
 public:
	CSSetTopicLock(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetTopicLock("TOPICLOCK"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetTopicLock("TOPICLOCK"));
	}

	~CSSetTopicLock()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("TOPICLOCK");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("TOPICLOCK");
	}
};

MODULE_INIT(CSSetTopicLock)
