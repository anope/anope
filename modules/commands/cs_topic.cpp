/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"
#include "modules/cs_info.h"
#include "modules/cs_set.h"

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(Module *creator, const Anope::string &cname = "chanserv/set/keeptopic") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Retain topic when channel is not in use"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable keeptopic";
			ci->SetS<bool>("KEEPTOPIC", true);
			source.Reply(_("Topic retention option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable keeptopic";
			ci->UnsetS<bool>("KEEPTOPIC");
			source.Reply(_("Topic retention option for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
			this->OnSyntaxError(source, "KEEPTOPIC");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the \002topic retention\002 option for a \037channel\037."
		               " When \002topic retention\002 is set, the topic for the channel will be remembered by {0} even after the last user leaves the channel, and will be restored the next time the channel is created."),
		               source.service->nick);
		return true;
	}
};

class CommandCSTopic : public Command
{
	ExtensibleRef<bool> topiclock;

	void Lock(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, "topiclock on");
		if (MOD_RESULT == EVENT_STOP)
			return;

		topiclock->Set(ci, true);
		source.Reply(_("Topic lock option for \002{0}\002 is now \002on\002."), ci->GetName());
	}

	void Unlock(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, "topiclock off");
		if (MOD_RESULT == EVENT_STOP)
			return;

		topiclock->Unset(ci);
		source.Reply(_("Topic lock option for \002{0}\002 is now \002off\002."), ci->GetName());
	}

	void Set(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params.size() > 2 ? params[2] : "";

		bool has_topiclock = topiclock->HasExt(ci);
		topiclock->Unset(ci);
		ci->c->ChangeTopic(source.GetNick(), topic, Anope::CurTime);
		if (has_topiclock)
			topiclock->Set(ci, true);

		bool override = !source.AccessFor(ci).HasPriv("TOPIC");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << (!topic.empty() ? "to change the topic to: " : "to unset the topic") << (!topic.empty() ? topic : "");
	}

	void Append(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &topic = params[2];

		Anope::string new_topic;
		if (!ci->c->topic.empty())
		{
			new_topic = ci->c->topic + " " + topic;
			ci->SetLastTopic("");
		}
		else
			new_topic = topic;

		std::vector<Anope::string> new_params;
		new_params.push_back("SET");
		new_params.push_back(ci->GetName());
		new_params.push_back(new_topic);

		this->Set(source, ci, new_params);
	}

 public:
	CommandCSTopic(Module *creator) : Command(creator, "chanserv/topic", 2, 3),
		topiclock("TOPICLOCK")
	{
		this->SetDesc(_("Manipulate the topic of the specified channel"));
		this->SetSyntax(_("\037channel\037 SET [\037topic\037]"));
		this->SetSyntax(_("\037channel\037 APPEND \037topic\037"));
		this->SetSyntax(_("\037channel\037 [UNLOCK|LOCK]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &channel = params[0];
		const Anope::string &subcmd = params[1];

		ChanServ::Channel *ci = ChanServ::Find(channel);
		if (ci == NULL)
			source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
		else if (!source.AccessFor(ci).HasPriv("TOPIC") && !source.HasCommand("chanserv/topic"))
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "TOPIC", ci->GetName());
		else if (subcmd.equals_ci("LOCK"))
			this->Lock(source, ci, params);
		else if (subcmd.equals_ci("UNLOCK"))
			this->Unlock(source, ci, params);
		else if (!ci->c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
		else if (subcmd.equals_ci("SET"))
			this->Set(source, ci, params);
		else if (subcmd.equals_ci("APPEND") && params.size() > 2)
			this->Append(source, ci, params);
		else
			this->SendSyntax(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows manipulating the topic of the specified channel."
		               " The \002SET\002 command changes the topic of the channel to the given topic or unsets the topic if no topic is given."
		               " The \002APPEND\002 command appends the given topic to the existing topic.\n"
		               "\n"
		               "\002LOCK\002 and \002UNLOCK\002 may be used to enable and disable topic lock."
		               " When topic lock is set, the channel topic will be unchangeable except via this command.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "TOPIC");
		return true;
	}
};

class CSTopic : public Module
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::TopicUpdated>
	, public EventHook<Event::ChanInfo>
{
	CommandCSTopic commandcstopic;
	CommandCSSetKeepTopic commandcssetkeeptopic;

	/*Serializable*/ExtensibleItem<bool> topiclock, keeptopic;

 public:
	CSTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcstopic(this)
		, commandcssetkeeptopic(this)
		, topiclock(this, "TOPICLOCK")
		, keeptopic(this, "KEEPTOPIC")
	{

	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci)
		{
			/* Update channel topic */
			if ((topiclock.HasExt(c->ci) || keeptopic.HasExt(c->ci)) && c->ci->GetLastTopic() != c->topic)
			{
				c->ChangeTopic(!c->ci->GetLastTopicSetter().empty() ? c->ci->GetLastTopicSetter() : c->ci->WhoSends()->nick, c->ci->GetLastTopic(), c->ci->GetLastTopicTime() ? c->ci->GetLastTopicTime() : Anope::CurTime);
			}
		}
	}

	void OnTopicUpdated(Channel *c, const Anope::string &user, const Anope::string &topic) override
	{
		if (!c->ci)
			return;

		/* We only compare the topics here, not the time or setter. This is because some (old) IRCds do not
		 * allow us to set the topic as someone else, meaning we have to bump the TS and change the setter to us.
		 * This desyncs what is really set with what we have stored, and we end up resetting the topic often when
		 * it is not required
		 */
		if (topiclock.HasExt(c->ci) && c->ci->GetLastTopic() != c->topic)
		{
			c->ChangeTopic(c->ci->GetLastTopicSetter(), c->ci->GetLastTopic(), c->ci->GetLastTopicTime());
		}
		else
		{
			c->ci->SetLastTopic(c->topic);
			c->ci->SetLastTopicSetter(c->topic_setter);
			c->ci->SetLastTopicTime(c->topic_ts);
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_all) override
	{
		if (keeptopic.HasExt(ci))
			info.AddOption(_("Topic retention"));
		if (topiclock.HasExt(ci))
			info.AddOption(_("Topic lock"));

		ModeLock *secret = mlocks ? mlocks->GetMLock(ci, "SECRET") : nullptr;
		if (!ci->GetLastTopic().empty() && (show_all || ((!secret || secret->GetSet() == false) && (!ci->c || !ci->c->HasMode("SECRET")))))
		{
			info[_("Last topic")] = ci->GetLastTopic();
			info[_("Topic set by")] = ci->GetLastTopicSetter();
		}
	}
};

MODULE_INIT(CSTopic)
