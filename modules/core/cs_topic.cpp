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

class CommandCSTopic : public Command
{
 public:
	CommandCSTopic() : Command("TOPIC", 1, 2)
	{
		this->SetDesc("Manipulate the topic of the specified channel");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
			source.Reply(_(CHAN_X_NOT_IN_USE), ci->name.c_str());
		else if (!check_access(u, ci, CA_TOPIC) && !u->Account()->HasCommand("chanserv/topic"))
			source.Reply(_(ACCESS_DENIED));
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
		source.Reply(_("Syntax: \002TOPIC \037channel\037 [\037topic\037]\002\n"
				" \n"
				"Causes %s to set the channel topic to the one\n"
				"specified. If \002topic\002 is not given, then an empty topic\n"
				"is set. This command is most useful in conjunction\n"
				"with \002SET TOPICLOCK\002. See \002%R%s HELP SET TOPICLOCK\002\n"
				"for more information.\n"
				" \n"
				"By default, limited to those with founder access on the\n"
				"channel."), ChanServ->nick.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "TOPIC", _("TOPIC \037channel\037 [\037topic\037]"));
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
