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

struct EntryMsg : Serializable
{
	Serialize::Reference<ChannelInfo> ci;
	Anope::string creator;
	Anope::string message;
	time_t when;

	EntryMsg(ChannelInfo *c, const Anope::string &cname, const Anope::string &cmessage, time_t ct = Anope::CurTime) : Serializable("EntryMsg")
	{
		this->ci = c;
		this->creator = cname;
		this->message = cmessage;
		this->when = ct;
	}

	void Serialize(Serialize::Data &data) const anope_override
	{
		data["ci"] << this->ci->name;
		data["creator"] << this->creator;
		data["message"] << this->message;
		data.SetType("when", Serialize::Data::DT_INT); data["when"] << this->when;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data);
};

struct EntryMessageList : Serialize::Checker<std::vector<EntryMsg *> >
{
	EntryMessageList(Extensible *) : Serialize::Checker<std::vector<EntryMsg *> >("EntryMsg") { }

	~EntryMessageList()
	{
		for (unsigned i = 0; i < (*this)->size(); ++i)
			delete (*this)->at(i);
	}
};

Serializable* EntryMsg::Unserialize(Serializable *obj, Serialize::Data &data)
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
		EntryMsg *msg = anope_dynamic_static_cast<EntryMsg *>(obj);
		msg->ci = ci;
		data["creator"] >> msg->creator;
		data["message"] >> msg->message;
		data["when"] >> msg->when;
		return msg;
	}

	EntryMessageList *messages = ci->GetExt<EntryMessageList>("entrymsg");
	if (messages == NULL)
		messages = ci->Extend<EntryMessageList>("entrymsg");

	data["when"] >> swhen;

	EntryMsg *m = new EntryMsg(ci, screator, smessage, swhen);
	(*messages)->push_back(m);
	return m;
}

class CommandEntryMessage : public Command
{
 private:
	void DoList(CommandSource &source, ChannelInfo *ci)
	{
		EntryMessageList *messages = ci->GetExt<EntryMessageList>("entrymsg");
		if (messages == NULL)
			messages = ci->Extend<EntryMessageList>("entrymsg");

		if ((*messages)->empty())
		{
			source.Reply(_("Entry message list for \002%s\002 is empty."), ci->name.c_str());
			return;
		}

		source.Reply(_("Entry message list for \002%s\002:"), ci->name.c_str());

		ListFormatter list;
		list.AddColumn("Number").AddColumn("Creator").AddColumn("Created").AddColumn("Message");
		for (unsigned i = 0; i < (*messages)->size(); ++i)
		{
			EntryMsg *msg = (*messages)->at(i);

			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(i + 1);
			entry["Creator"] = msg->creator;
			entry["Created"] = Anope::strftime(msg->when);
			entry["Message"] = msg->message;
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of entry message list."));
	}
		
	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		EntryMessageList *messages = ci->GetExt<EntryMessageList>("entrymsg");
		if (messages == NULL)
			messages = ci->Extend<EntryMessageList>("entrymsg");

		if ((*messages)->size() >= Config->GetModule(this->owner)->Get<unsigned>("maxentries"))
			source.Reply(_("The entry message list for \002%s\002 is full."), ci->name.c_str());
		else
		{
			(*messages)->push_back(new EntryMsg(ci, source.GetNick(), message));
			Log(source.IsFounder(ci) ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to add a message";
			source.Reply(_("Entry message added to \002%s\002"), ci->name.c_str());
		}
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		EntryMessageList *messages = ci->GetExt<EntryMessageList>("entrymsg");
		if (messages == NULL)
			messages = ci->Extend<EntryMessageList>("entrymsg");

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
					delete (*messages)->at(i - 1);
					(*messages)->erase((*messages)->begin() + i - 1);
					if ((*messages)->empty())
						ci->Shrink<EntryMessageList>("entrymsg");
					Log(source.IsFounder(ci) ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to remove a message";
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
		ci->Shrink<EntryMessageList>("entrymsg");

		Log(source.IsFounder(ci) ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to remove all messages";
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
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.IsFounder(ci) || source.HasCommand("chanserv/set"))
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
	Serialize::Type entrymsg_type;
	CommandEntryMessage commandentrymsg;
	ExtensibleItem<EntryMessageList> eml;

 public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR), entrymsg_type("EntryMsg", EntryMsg::Unserialize), commandentrymsg(this), eml(this, "entrymsg")
	{

	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		if (u && c && c->ci && u->server->IsSynced())
		{
			EntryMessageList *messages = c->ci->GetExt<EntryMessageList>("entrymsg");

			if (messages != NULL)
				for (unsigned i = 0; i < (*messages)->size(); ++i)
					u->SendMessage(c->ci->WhoSends(), "[%s] %s", c->ci->name.c_str(), (*messages)->at(i)->message.c_str());
		}
	}
};

MODULE_INIT(CSEntryMessage)
