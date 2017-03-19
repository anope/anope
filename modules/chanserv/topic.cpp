/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/chanserv/mode.h"
#include "modules/chanserv/info.h"
#include "modules/chanserv/set.h"

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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.GetPermission().empty() && !source.HasOverridePriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			logger.Command(source, _("{source} used {command} on {channel} to enable keeptopic"));

			ci->SetKeepTopic(true);
			source.Reply(_("Topic retention option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			logger.Command(source, _("{source} used {command} on {channel} to disable keeptopic"));

			ci->SetKeepTopic(false);
			source.Reply(_("Topic retention option for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
		{
			this->OnSyntaxError(source, "KEEPTOPIC");
		}
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
	void Lock(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, "topiclock on");
		if (MOD_RESULT == EVENT_STOP)
			return;

		ci->SetTopicLock(true);
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, "topiclock off");
		if (MOD_RESULT == EVENT_STOP)
			return;

		ci->SetTopicLock(false);
		source.Reply(_("Topic lock option for \002{0}\002 is now \002off\002."), ci->GetName());
	}

	void Set(CommandSource &source, ChanServ::Channel *ci, const Anope::string &topic)
	{
		bool has_topiclock = ci->IsTopicLock();
		ci->SetTopicLock(false);
		ci->c->ChangeTopic(source.GetNick(), topic, Anope::CurTime);
		if (has_topiclock)
			ci->SetTopicLock(true);

		if (!topic.empty())
			logger.Command(source, _("{source} used {command} on {channel} to change the topic to: {0}"), topic);
		else
			logger.Command(source, _("{source} used {command} on {channel} to unset the topic"));
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
		{
			new_topic = topic;
		}

		this->Set(source, ci, new_topic);
	}

 public:
	CommandCSTopic(Module *creator) : Command(creator, "chanserv/topic", 2, 3)
	{
		this->SetDesc(_("Manipulate the topic of the specified channel"));
		this->SetSyntax(_("\037channel\037 [SET] [\037topic\037]"));
		this->SetSyntax(_("\037channel\037 APPEND \037topic\037"));
		this->SetSyntax(_("\037channel\037 [UNLOCK|LOCK]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &channel = params[0];
		const Anope::string &subcmd = params[1];

		ChanServ::Channel *ci = ChanServ::Find(channel);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("TOPIC") && !source.HasOverrideCommand("chanserv/topic"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "TOPIC", ci->GetName());
			return;
		}

		if (subcmd.equals_ci("LOCK"))
		{
			this->Lock(source, ci, params);
		}
		else if (subcmd.equals_ci("UNLOCK"))
		{
			this->Unlock(source, ci, params);
		}
		else if (!ci->c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
		}
		else if (subcmd.equals_ci("APPEND") && params.size() > 2)
		{
			this->Append(source, ci, params);
		}
		else
		{
			Anope::string topic;
			if (subcmd.equals_ci("SET"))
			{
				topic = params.size() > 2 ? params[2] : "";
			}
			else
			{
				topic = subcmd;
				if (params.size() > 2)
					topic += " " + params[2];
			}

			this->Set(source, ci, topic);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows manipulating the topic of the specified channel."
		               " The \002SET\002 command changes the topic of the channel to the given topic or unsets the topic if no topic is given."
		               " The \002APPEND\002 command appends the given topic to the existing topic.\n"
		               "\n"
		               "\002LOCK\002 and \002UNLOCK\002 may be used to enable and disable topic lock."
		               " When topic lock is set, the channel topic will be unchangeable by users who do not have the \002TOPIC\002 privilege.\n"
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

	ServiceReference<ModeLocks> mlocks;

 public:
	CSTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChannelSync>(this)
		, EventHook<Event::TopicUpdated>(this)
		, EventHook<Event::ChanInfo>(this)
		, commandcstopic(this)
		, commandcssetkeeptopic(this)
	{

	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci)
		{
			/* Update channel topic */
			if ((c->ci->IsTopicLock() || c->ci->IsKeepTopic()) && c->ci->GetLastTopic() != c->topic)
			{
				ServiceBot *sender = c->ci->WhoSends();
				c->ChangeTopic(!c->ci->GetLastTopicSetter().empty() ? c->ci->GetLastTopicSetter() : (sender ? sender->nick : Me->GetName()),
						c->ci->GetLastTopic(), c->ci->GetLastTopicTime() ? c->ci->GetLastTopicTime() : Anope::CurTime);
			}
		}
	}

	void OnTopicUpdated(User *source, Channel *c, const Anope::string &user, const Anope::string &topic) override
	{
		if (!c->ci)
			return;

		/* We only compare the topics here, not the time or setter. This is because some (old) IRCds do not
		 * allow us to set the topic as someone else, meaning we have to bump the TS and change the setter to us.
		 * This desyncs what is really set with what we have stored, and we end up resetting the topic often when
		 * it is not required
		 */
		if (c->ci->IsTopicLock() && c->ci->GetLastTopic() != c->topic && (!source || !c->ci->AccessFor(source).HasPriv("TOPIC")))
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
		if (ci->IsKeepTopic())
			info.AddOption(_("Topic retention"));
		if (ci->IsTopicLock())
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
