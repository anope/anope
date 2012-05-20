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

struct EntryMsg : Serializable
{
	serialize_obj<ChannelInfo> ci;
	Anope::string creator;
	Anope::string message;
	time_t when;

	EntryMsg(ChannelInfo *c, const Anope::string &cname, const Anope::string &cmessage, time_t ct = Anope::CurTime)
	{
	
		this->ci = c;
		this->creator = cname;
		this->message = cmessage;
		this->when = ct;
	}

	const Anope::string serialize_name() const anope_override
	{
		return "EntryMsg";
	}

	Serialize::Data serialize() const anope_override
	{
		Serialize::Data data;

		data["ci"] << this->ci->name;
		data["creator"] << this->creator;
		data["message"] << this->message;
		data["when"].setType(Serialize::DT_INT) << this->when;

		return data;
	}

	static Serializable* unserialize(Serializable *obj, Serialize::Data &data);
};

static unsigned MaxEntries = 0;

struct EntryMessageList : serialize_checker<std::vector<EntryMsg> >, ExtensibleItem
{
	EntryMessageList() : serialize_checker<std::vector<EntryMsg> >("EntryMsg") { }
};

Serializable* EntryMsg::unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (!ci)
		return NULL;

	if (obj)
	{
		EntryMsg *msg = anope_dynamic_static_cast<EntryMsg *>(obj);
		msg->ci = ci;
		data["creator"] >> msg->creator;
		data["message"] >> msg->message;
		data["when"] >> msg->when;
		return msg;
	}

	EntryMessageList *messages = ci->GetExt<EntryMessageList *>("cs_entrymsg");
	if (messages == NULL)
	{
		messages = new EntryMessageList();
		ci->Extend("cs_entrymsg", messages);
	}

	(*messages)->push_back(EntryMsg(ci, data["creator"].astr(), data["message"].astr()));
	return &(*messages)->back();
}

class CommandEntryMessage : public Command
{
 private:
	void DoList(CommandSource &source, ChannelInfo *ci)
	{
		EntryMessageList *messages = ci->GetExt<EntryMessageList *>("cs_entrymsg");
		if (messages == NULL)
		{
			messages = new EntryMessageList();
			ci->Extend("cs_entrymsg", messages);
		}

		if ((*messages)->empty())
		{
			source.Reply(_("Entry message list for \002%s\002 is empty."), ci->name.c_str());
			return;
		}

		source.Reply(_("Entry message list for \002%s\002:"), ci->name.c_str());

		ListFormatter list;
		list.addColumn("Number").addColumn("Creator").addColumn("Created").addColumn("Message");
		for (unsigned i = 0; i < (*messages)->size(); ++i)
		{
			EntryMsg &msg = (*messages)->at(i);

			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(i + 1);
			entry["Creator"] = msg.creator;
			entry["Created"] = do_strftime(msg.when);
			entry["Message"] = msg.message;
			list.addEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of entry message list."));
	}
		
	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		User *u = source.u;

		EntryMessageList *messages = ci->GetExt<EntryMessageList *>("cs_entrymsg");
		if (messages == NULL)
		{
			messages = new EntryMessageList();
			ci->Extend("cs_entrymsg", messages);
		}

		if (MaxEntries && (*messages)->size() >= MaxEntries)
			source.Reply(_("The entry message list for \002%s\002 is full."), ci->name.c_str());
		else
		{
			(*messages)->push_back(EntryMsg(ci, source.u->nick, message));
			Log(IsFounder(u, ci) ? LOG_COMMAND : LOG_OVERRIDE, u, this, ci) << "to add a message";
			source.Reply(_("Entry message added to \002%s\002"), ci->name.c_str());
		}
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		User *u = source.u;

		EntryMessageList *messages = ci->GetExt<EntryMessageList *>("cs_entrymsg");
		if (messages == NULL)
		{
			messages = new EntryMessageList();
			ci->Extend("cs_entrymsg", messages);
		}

		if (!message.is_pos_number_only())
			source.Reply(("Entry message \002%s\002 not found on channel \002%s\002."), message.c_str(), ci->name.c_str());
		else if ((*messages)->empty())
			source.Reply(_("Entry message list for \002%s\002 is empty."), ci->name.c_str());
		else
		{
			try
			{
				unsigned i = convertTo<unsigned>(message);
				if (i > 0 && i <= (*messages)->size())
				{
					(*messages)->erase((*messages)->begin() + i - 1);
					if ((*messages)->empty())
						ci->Shrink("cs_entrymsg");
					Log(IsFounder(u, ci) ? LOG_COMMAND : LOG_OVERRIDE, u, this, ci) << "to remove a message";
					source.Reply(_("Entry message \002%i\002 for \002%s\002 deleted."), i, ci->name.c_str());
				}
				else
					throw ConvertException();
			}
			catch (const ConvertException &)
			{
				source.Reply(_("Entry message \002%s\002 not found on channel \002%s\002."), message.c_str(), ci->name.c_str());
			}
		}
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;

		ci->Shrink("cs_entrymsg");
		Log(IsFounder(u, ci) ? LOG_COMMAND : LOG_OVERRIDE, u, this, ci) << "to remove all messages";
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (IsFounder(u, ci) || u->HasCommand("chanserv/set"))
		{
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
		else
			source.Reply(ACCESS_DENIED);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Controls what messages will be sent to users when they join the channel."));
		source.Reply(" ");
		source.Reply(_("The \002ENTRYMSG ADD\002 command adds the given message to\n"
				"the list of messages to be shown to users when they join\n"
				"the channel."));
		source.Reply(" ");
		source.Reply(_("The \002ENTRYMSG DEL\002 command removes the given message from\n"
				"the list of messages to be shown to users when they join\n"
				"the channel. You can remove the message by specifying its number\n"
				"which you can get by listing the messages as explained below."));
		source.Reply(" ");
		source.Reply(_("The \002ENTRYMSG LIST\002 command displays a listing of messages\n"
				"to be shown to users when they join the channel."));
		source.Reply(" ");
		source.Reply(_("The \002ENTRYMSG CLEAR\002 command clears all entries from\n"
				"the list of messages to be shown to users when they join\n"
				"the channel, effectively disabling entry messages."));
		return true;
	}
};

class CSEntryMessage : public Module
{
	SerializeType entrymsg_type;
	CommandEntryMessage commandentrymsg;

 public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), entrymsg_type("EntryMsg", EntryMsg::unserialize), commandentrymsg(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnJoinChannel };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		if (u && c && c->ci && u->server->IsSynced())
		{
			EntryMessageList *messages = c->ci->GetExt<EntryMessageList *>("cs_entrymsg");

			if (messages != NULL)
				for (unsigned i = 0; i < (*messages)->size(); ++i)
					u->SendMessage(c->ci->WhoSends(), "[%s] %s", c->ci->name.c_str(), (*messages)->at(i).message.c_str());
		}
	}
		
	void OnReload() anope_override
	{
		ConfigReader config;
		MaxEntries = config.ReadInteger("cs_entrymsg", "maxentries", "5", 0, true);
	}
};

MODULE_INIT(CSEntryMessage)
