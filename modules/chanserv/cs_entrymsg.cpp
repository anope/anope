/* ChanServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/chanserv/entrymsg.h"

struct EntryMsgImpl final
	: EntryMsg
	, Serializable
{
	EntryMsgImpl() : Serializable("EntryMsg")
	{
	}

	EntryMsgImpl(ChannelInfo *c, const Anope::string &cname, const Anope::string &cmessage, time_t ct = Anope::CurTime) : Serializable("EntryMsg")
	{
		this->chan = c->name;
		this->creator = cname;
		this->message = cmessage;
		this->when = ct;
	}

	~EntryMsgImpl() override;
};

struct EntryMsgTypeImpl final
	: Serialize::Type
{
	EntryMsgTypeImpl()
		: Serialize::Type("EntryMsg")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *msg = static_cast<const EntryMsgImpl *>(obj);
		data.Store("ci", msg->chan);
		data.Store("creator", msg->creator);
		data.Store("message", msg->message);
		data.Store("when", msg->when);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
};

struct EntryMessageListImpl final
	: EntryMessageList
{
	EntryMessageListImpl(Extensible *) { }

	EntryMsg *Create() override
	{
		return new EntryMsgImpl();
	}
};

EntryMsgImpl::~EntryMsgImpl()
{
	ChannelInfo *ci = ChannelInfo::Find(this->chan);
	if (!ci)
		return;

	EntryMessageList *messages = ci->GetExt<EntryMessageList>("entrymsg");
	if (!messages)
		return;

	std::vector<EntryMsg *>::iterator it = std::find((*messages)->begin(), (*messages)->end(), this);
	if (it != (*messages)->end())
		(*messages)->erase(it);
}


Serializable *EntryMsgTypeImpl::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	Anope::string sci, screator, smessage;
	time_t swhen;

	data["ci"] >> sci;
	data["creator"] >> screator;
	data["message"] >> smessage;

	ChannelInfo *ci = ChannelInfo::Find(sci);
	if (!ci)
		return NULL;

	if (obj)
	{
		EntryMsgImpl *msg = anope_dynamic_static_cast<EntryMsgImpl *>(obj);
		msg->chan = ci->name;
		data["creator"] >> msg->creator;
		data["message"] >> msg->message;
		data["when"] >> msg->when;
		return msg;
	}

	EntryMessageList *messages = ci->Require<EntryMessageList>("entrymsg");

	data["when"] >> swhen;

	auto *m = new EntryMsgImpl(ci, screator, smessage, swhen);
	(*messages)->push_back(m);
	return m;
}

class CommandEntryMessage final
	: public Command
{
private:
	static void DoList(CommandSource &source, ChannelInfo *ci)
	{
		EntryMessageList *messages = ci->Require<EntryMessageList>("entrymsg");

		if ((*messages)->empty())
		{
			source.Reply(_("Entry message list for \002%s\002 is empty."), ci->name.c_str());
			return;
		}

		source.Reply(_("Entry message list for \002%s\002:"), ci->name.c_str());

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Message"));
		for (unsigned i = 0; i < (*messages)->size(); ++i)
		{
			EntryMsg *msg = (*messages)->at(i);

			ListFormatter::ListEntry entry;
			entry["Number"] = Anope::ToString(i + 1);
			entry["Creator"] = msg->creator;
			entry["Created"] = Anope::strftime(msg->when, NULL, true);
			entry["Message"] = msg->message;
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (const auto &reply : replies)
			source.Reply(reply);

		source.Reply(_("End of entry message list."));
	}

	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		EntryMessageList *messages = ci->Require<EntryMessageList>("entrymsg");

		if ((*messages)->size() >= Config->GetModule(this->owner).Get<unsigned>("maxentries"))
			source.Reply(_("The entry message list for \002%s\002 is full."), ci->name.c_str());
		else
		{
			(*messages)->push_back(new EntryMsgImpl(ci, source.GetNick(), message));
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to add a message";
			source.Reply(_("Entry message added to \002%s\002"), ci->name.c_str());
		}
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		EntryMessageList *messages = ci->Require<EntryMessageList>("entrymsg");

		if (!message.is_pos_number_only())
			source.Reply(("Entry message \002%s\002 not found on channel \002%s\002."), message.c_str(), ci->name.c_str());
		else if ((*messages)->empty())
			source.Reply(_("Entry message list for \002%s\002 is empty."), ci->name.c_str());
		else
		{
			auto i = Anope::Convert<unsigned>(message, 0);
			if (i > 0 && i <= (*messages)->size())
			{
				delete (*messages)->at(i - 1);
				if ((*messages)->empty())
					ci->Shrink<EntryMessageList>("entrymsg");
				Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to remove a message";
				source.Reply(_("Entry message \002%i\002 for \002%s\002 deleted."), i, ci->name.c_str());
			}
			else
			{
				source.Reply(_("Entry message \002%s\002 not found on channel \002%s\002."), message.c_str(), ci->name.c_str());
			}
		}
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		ci->Shrink<EntryMessageList>("entrymsg");

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to remove all messages";
		source.Reply(_("Entry messages for \002%s\002 have been cleared."), ci->name.c_str());
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
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (Anope::ReadOnly && !params[1].equals_ci("LIST"))
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("LIST"))
			this->DoList(source, ci);
		else if (params[1].equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else if (params.size() < 3)
			this->OnSyntaxError(source, "");
		else if (params[1].equals_ci("ADD"))
			this->DoAdd(source, ci, params[2]);
		else if (params[1].equals_ci("DEL"))
			this->DoDel(source, ci, params[2]);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Controls what messages will be sent to users when they join the channel."
				"\n\n"
				"The \002%s\032ADD\002 command adds the given message to "
				"the list of messages shown to users when they join "
				"the channel."
				"\n\n"
				"The \002%s\032DEL\002 command removes the specified message from "
				"the list of messages shown to users when they join "
				"the channel. You can remove a message by specifying its number "
				"which you can get by listing the messages as explained below."
				"\n\n"
				"The \002%s\032LIST\002 command displays a listing of messages "
				"shown to users when they join the channel."
				"\n\n"
				"The \002%s\032CLEAR\002 command clears all entries from "
				"the list of messages shown to users when they join "
				"the channel, effectively disabling entry messages."
				"\n\n"
				"Adding, deleting, or clearing entry messages requires the "
				"SET permission."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());
		return true;
	}
};

class CSEntryMessage final
	: public Module
{
	CommandEntryMessage commandentrymsg;
	ExtensibleItem<EntryMessageListImpl> eml;
	EntryMsgTypeImpl entrymsg_type;

public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandentrymsg(this)
		, eml(this, "entrymsg")
	{
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (u && c && c->ci && u->server->IsSynced())
		{
			EntryMessageList *messages = c->ci->GetExt<EntryMessageList>("entrymsg");
			if (!messages)
				return;

			for (const auto &message : *(*messages))
			{
				if (u->ShouldPrivmsg())
					IRCD->SendContextPrivmsg(c->ci->WhoSends(), u, c, message->message);
				else
					IRCD->SendContextNotice(c->ci->WhoSends(), u, c, message->message);
			}
		}
	}
};

MODULE_INIT(CSEntryMessage)
