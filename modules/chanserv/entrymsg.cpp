/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
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
#include "modules/chanserv/entrymsg.h"

class EntryMsgImpl : public EntryMsg
{
	friend class EntryMsgType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> creator, message;
	Serialize::Storage<time_t> when;

 public:
	using EntryMsg::EntryMsg;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *ci) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &c) override;

	Anope::string GetMessage() override;
	void SetMessage(const Anope::string &m) override;

	time_t GetWhen() override;
	void SetWhen(const time_t &t) override;
};

class EntryMsgType : public Serialize::Type<EntryMsgImpl>
{
 public:
	Serialize::ObjectField<EntryMsgImpl, ChanServ::Channel *> chan;
	Serialize::Field<EntryMsgImpl, Anope::string> creator, message;
	Serialize::Field<EntryMsgImpl, time_t> when;

	EntryMsgType(Module *me) : Serialize::Type<EntryMsgImpl>(me)
		, chan(this, "chan", &EntryMsgImpl::channel, true)
		, creator(this, "creator", &EntryMsgImpl::creator)
		, message(this, "message", &EntryMsgImpl::message)
		, when(this, "when", &EntryMsgImpl::when)
	{
	}
};

ChanServ::Channel *EntryMsgImpl::GetChannel()
{
	return Get(&EntryMsgType::chan);
}

void EntryMsgImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&EntryMsgType::chan, ci);
}

Anope::string EntryMsgImpl::GetCreator()
{
	return Get(&EntryMsgType::creator);
}

void EntryMsgImpl::SetCreator(const Anope::string &c)
{
	Set(&EntryMsgType::creator, c);
}

Anope::string EntryMsgImpl::GetMessage()
{
	return Get(&EntryMsgType::message);
}

void EntryMsgImpl::SetMessage(const Anope::string &m)
{
	Set(&EntryMsgType::message, m);
}

time_t EntryMsgImpl::GetWhen()
{
	return Get(&EntryMsgType::when);
}

void EntryMsgImpl::SetWhen(const time_t &t)
{
	Set(&EntryMsgType::when, t);
}

class CommandEntryMessage : public Command
{
 private:
	void DoList(CommandSource &source, ChanServ::Channel *ci)
	{
		std::vector<EntryMsg *> messages = ci->GetRefs<EntryMsg *>();

		if (messages.empty())
		{
			source.Reply(_("Entry message list for \002{0}\002 is empty."), ci->GetName());
			return;
		}

		source.Reply(_("Entry message list for \002{0}\002:"), ci->GetName());

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Message"));
		for (unsigned i = 0; i < messages.size(); ++i)
		{
			EntryMsg *msg = messages[i];

			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(i + 1);
			entry["Creator"] = msg->GetCreator();
			entry["Created"] = Anope::strftime(msg->GetWhen(), NULL, true);
			entry["Message"] = msg->GetMessage();
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of entry message list."));
	}

	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const Anope::string &message)
	{
		std::vector<EntryMsg *> messages = ci->GetRefs<EntryMsg *>();

		if (messages.size() >= Config->GetModule(this->GetOwner())->Get<unsigned>("maxentries"))
		{
			source.Reply(_("The entry message list for \002{0}\002 is full."), ci->GetName());
			return;
		}

		EntryMsg *msg = Serialize::New<EntryMsg *>();
		msg->SetChannel(ci);
		msg->SetCreator(source.GetNick());
		msg->SetMessage(message);

		logger.Command(source, ci, _("{source} used {command} on {channel} to add a message"));

		source.Reply(_("Entry message added to \002{0}\002"), ci->GetName());
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const Anope::string &message)
	{
		std::vector<EntryMsg *> messages = ci->GetRefs<EntryMsg *>();

		if (!message.is_pos_number_only())
		{
			source.Reply(("Entry message \002{0}\002 not found on channel \002{1}\002."), message, ci->GetName());
			return;
		}

		if (messages.empty())
		{
			source.Reply(_("Entry message list for \002{0}\002 is empty."), ci->GetName());
			return;
		}

		try
		{
			unsigned i = convertTo<unsigned>(message);
			if (i > 0 && i <= messages.size())
			{
				messages[i - 1]->Delete();

				logger.Command(source, ci, _("{source} used {command} on {channel} to remove a message"));

				source.Reply(_("Entry message \002{0}\002 for \002{1]\002 deleted."), i, ci->GetName());
			}
			else
				throw ConvertException();
		}
		catch (const ConvertException &)
		{
			source.Reply(_("Entry message \002{0}\002 not found on channel \002{1}\002."), message, ci->GetName());
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		for (EntryMsg *e : ci->GetRefs<EntryMsg *>())
			e->Delete();

		logger.Command(source, ci, _("{source} used {command} on {channel} to remove all messages"));

		source.Reply(_("Entry messages for \002{0}\002 have been cleared."), ci->GetName());
	}

 public:
	CommandEntryMessage(Module *creator) : Command(creator, "chanserv/entrymsg", 2, 3)
	{
		this->SetDesc(_("Manage the channel's entry messages"));
		this->SetSyntax(_("\037channel\037 ADD \037message\037"));
		this->SetSyntax(_("\037channel\037 DEL \037num\037"));
		this->SetSyntax(_("\037channel\037 LIST"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (Anope::ReadOnly && !params[1].equals_ci("LIST"))
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SET") && !source.HasOverridePriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		if (params[1].equals_ci("LIST"))
			this->DoList(source, ci);
		else if (params[1].equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else if (params.size() < 3)
			this->OnSyntaxError(source);
		else if (params[1].equals_ci("ADD"))
			this->DoAdd(source, ci, params[2]);
		else if (params[1].equals_ci("DEL"))
			this->DoDel(source, ci, params[2]);
		else
			this->OnSyntaxError(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Controls what messages will be sent to users when they join \037channel\037.\n"
		               "\n"
		               "The \002{0} ADD\002 command adds the given message to the list of messages to be shown to users when they join the channel.\n"
		               "\n"
		               "The \002{0} DEL\002 command removes the given message from the entry message list.\n"
		               " You can remove the message by specifying its number which you can get by listing the messages as explained below.\n"
		               "\n"
		               "The \002{0} LIST\002 command displays the list of messages on the entry message list.\n"
		               "\n"
		               "The \002{0} CLEAR\002 command clears the entry message list.\n"
		               "\n"
		               "Use of this command requires the \002SET\002 privilege on \037channel\037."),
		               source.GetCommand());
		return true;
	}
};

class CSEntryMessage : public Module
	, public EventHook<Event::JoinChannel>
{
	CommandEntryMessage commandentrymsg;
	EntryMsgType entrymsg_type;

 public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::JoinChannel>(this)
		, commandentrymsg(this)
		, entrymsg_type(this)
	{
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		if (ci && u->server->IsSynced())
			for (EntryMsg *msg : ci->GetRefs<EntryMsg *>())
				u->SendMessage(ci->WhoSends(), "[{0}] {1}", ci->GetName(), msg->GetMessage());
	}
};

MODULE_INIT(CSEntryMessage)
