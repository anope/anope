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

class CommandCSTopic : public Command
{
 public:
	CommandCSTopic() : Command("TOPIC", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!check_access(u, ci, CA_TOPIC) && !u->Account()->HasCommand("chanserv/topic"))
			source.Reply(ACCESS_DENIED);
		else
		{
			bool has_topiclock = ci->HasFlag(CI_TOPICLOCK);
			ci->UnsetFlag(CI_TOPICLOCK);
			c->ChangeTopic(u->nick, topic, Anope::CurTime);
			if (has_topiclock)
				ci->SetFlag(CI_TOPICLOCK);
	
			bool override = !check_access(u, ci, CA_TOPIC);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to change the topic to " << (!topic.empty() ? topic : "No topic");
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_TOPIC);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "TOPIC", CHAN_TOPIC_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_TOPIC);
	}
};

class CSTopic : public Module
{
	CommandCSTopic commandcstopic;

 public:
	CSTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcstopic);
	}
};

MODULE_INIT(CSTopic)
