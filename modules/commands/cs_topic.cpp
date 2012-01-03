/* ChanServ core functions
 *
 * (C) 2003-2012 Anope Team
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
	CommandCSTopic(Module *creator) : Command(creator, "chanserv/topic", 1, 2)
	{
		this->SetDesc(_("Manipulate the topic of the specified channel"));
		this->SetSyntax(_("\037channel\037 [\037topic\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params.size() > 1 ? params[1] : "";

		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!ci->AccessFor(u).HasPriv("TOPIC") && !u->HasCommand("chanserv/topic"))
			source.Reply(ACCESS_DENIED);
		else
		{
			bool has_topiclock = ci->HasFlag(CI_TOPICLOCK);
			ci->UnsetFlag(CI_TOPICLOCK);
			ci->c->ChangeTopic(u->nick, topic, Anope::CurTime);
			if (has_topiclock)
				ci->SetFlag(CI_TOPICLOCK);
	
			bool override = !ci->AccessFor(u).HasPriv("TOPIC");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to change the topic to " << (!topic.empty() ? topic : "No topic");
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Causes %s to set the channel topic to the one\n"
				"specified. If \002topic\002 is not given, then an empty topic\n"
				"is set. This command is most useful in conjunction\n"
				"with topic lock.\n"
				"By default, limited to those with founder access on the\n"
				"channel."), source.owner->nick.c_str());
		return true;
	}
};

class CSTopic : public Module
{
	CommandCSTopic commandcstopic;

 public:
	CSTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcstopic(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSTopic)
