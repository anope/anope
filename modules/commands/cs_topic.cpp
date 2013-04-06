/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSTopic : public Command
{
	void Lock(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, "topiclock on"));
		if (MOD_RESULT == EVENT_STOP)
			return;

		ci->ExtendMetadata("TOPICLOCK");
		source.Reply(_("Topic lock option for %s is now \002on\002."), ci->name.c_str());
	}

	void Unlock(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, "topiclock off"));
		if (MOD_RESULT == EVENT_STOP)
			return;

		ci->Shrink("TOPICLOCK");
		source.Reply(_("Topic lock option for %s is now \002off\002."), ci->name.c_str());
	}

	void Set(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params.size() > 2 ? params[2] : "";

		bool has_topiclock = ci->HasExt("TOPICLOCK");
		ci->Shrink("TOPICLOCK");
		ci->c->ChangeTopic(source.GetNick(), topic, Anope::CurTime);
		if (has_topiclock)
			ci->ExtendMetadata("TOPICLOCK");
	
		bool override = !source.AccessFor(ci).HasPriv("TOPIC");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << (!topic.empty() ? "to change the topic to: " : "to unset the topic") << (!topic.empty() ? topic : "");
	}

	void Append(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params[2];

		Anope::string new_topic;
		if (!ci->c->topic.empty())
		{
			new_topic = ci->c->topic + " " + topic;
			ci->last_topic.clear();
		}
		else
			new_topic = topic;

		std::vector<Anope::string> new_params;
		new_params.push_back("SET");
		new_params.push_back(ci->name);
		new_params.push_back(new_topic);

		this->Set(source, ci, new_params);
	}

 public:
	CommandCSTopic(Module *creator) : Command(creator, "chanserv/topic", 2, 3)
	{
		this->SetDesc(_("Manipulate the topic of the specified channel"));
		this->SetSyntax(_("\037channel\037 SET [\037topic\037]"));
		this->SetSyntax(_("\037channel\037 APPEND \037topic\037"));
		this->SetSyntax(_("\037channel\037 [UNLOCK|LOCK]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &subcmd = params[1];

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (!source.AccessFor(ci).HasPriv("TOPIC") && !source.HasCommand("chanserv/topic"))
			source.Reply(ACCESS_DENIED);
		else if (subcmd.equals_ci("LOCK"))
			this->Lock(source, ci, params);
		else if (subcmd.equals_ci("UNLOCK"))
			this->Unlock(source, ci, params);
		else if (!ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (subcmd.equals_ci("SET"))
			this->Set(source, ci, params);
		else if (subcmd.equals_ci("APPEND") && params.size() > 2)
			this->Append(source, ci, params);
		else
			this->SendSyntax(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows manipulating the topic of the specified channel.\n"
				"The \002SET\002 command changes the topic of the channel to the given topic\n"
				"or unsets the topic if no topic is given. The \002APPEND\002 command appends\n"
				"the given topic to the existing topic.\n"
				" \n"
				"\002LOCK\002 and \002UNLOCK\002 may be used to enable and disable topic lock. When\n"
				"topic lock is set, the channel topic will be unchangeable except via this command."));
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
