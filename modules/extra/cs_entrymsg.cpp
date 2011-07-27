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

struct EntryMsg
{
	static unsigned MaxEntries;

	EntryMsg(const Anope::string &cname, const Anope::string &cmessage, time_t ct = Anope::CurTime)
	{
		this->creator = cname;
		this->message = cmessage;
		this->when = ct;
	}

	Anope::string creator;
	Anope::string message;
	time_t when;
};
unsigned EntryMsg::MaxEntries = 0;

class CommandEntryMessage : public Command
{
 private:
	void DoList(CommandSource &source, ChannelInfo *ci)
	{
		std::vector<EntryMsg> messages;
		if (ci->GetExtRegular("cs_entrymsg", messages))
		{
			source.Reply(_("Entry message list for \2%s\2:"), ci->name.c_str());
			for (unsigned i = 0; i < messages.size(); ++i)
				source.Reply(CHAN_LIST_ENTRY, i + 1, messages[i].message.c_str(), messages[i].creator.c_str(), do_strftime(messages[i].when).c_str());
			source.Reply(_("End of entry message list."));
		}
		else
			source.Reply(_("Entry message list for \2%s\2 is empty."), ci->name.c_str());
	}
		
	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		std::vector<EntryMsg> messages;
		ci->GetExtRegular("cs_entrymsg", messages);

		if (EntryMsg::MaxEntries && messages.size() >= EntryMsg::MaxEntries)
			source.Reply(_("The entry message list for \2%s\2 is full."), ci->name.c_str());
		else
		{
			messages.push_back(EntryMsg(source.u->nick, message));
			ci->Extend("cs_entrymsg", new ExtensibleItemRegular<std::vector<EntryMsg> >(messages));
			source.Reply(_("Entry message added to \2%s\2"), ci->name.c_str());
		}
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const Anope::string &message)
	{
		std::vector<EntryMsg> messages;
		if (!message.is_pos_number_only())
			source.Reply(("Entry message \002%s\002 not found on channel \002%s\002."), message.c_str(), ci->name.c_str());
		else if (ci->GetExtRegular("cs_entrymsg", messages))
		{
			try
			{
				unsigned i = convertTo<unsigned>(message);
				if (i > 0 && i <= messages.size())
				{
					messages.erase(messages.begin() + i - 1);
					if (!messages.empty())
						ci->Extend("cs_entrymsg", new ExtensibleItemRegular<std::vector<EntryMsg> >(messages));
					else
						ci->Shrink("cs_entrymsg");
					source.Reply(_("Entry message \2%i\2 for \2%s\2 deleted."), i, ci->name.c_str());
				}
				else
					throw ConvertException();
			}
			catch (const ConvertException &)
			{
				source.Reply(_("Entry message \2%s\2 not found on channel \2%s\2."), message.c_str(), ci->name.c_str());
			}
		}
		else
			source.Reply(_("Entry message list for \2%s\2 is empty."), ci->name.c_str());
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		ci->Shrink("cs_entrymsg");
		source.Reply(_("Entry messages for \2%s\2 have been cleared."), ci->name.c_str());
	}

 public:
	CommandEntryMessage(Module *creator) : Command(creator, "chanserv/entrymsg", 2, 3)
	{
		this->SetDesc(_("Manage the channel's entry messages"));
		this->SetSyntax(_("\037channel\037 {ADD|DEL|LIST|CLEAR} [\037message\037|\037num\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (IsFounder(u, ci) || u->HasCommand("chanserv/entrymsg"))
		{
			bool success = true;
			if (params[1].equals_ci("LIST"))
				this->DoList(source, ci);
			else if (params[1].equals_ci("CLEAR"))
				this->DoClear(source, ci);
			else if (params.size() < 3)
			{
				success = false;
				this->OnSyntaxError(source, "");
			}
			else if (params[1].equals_ci("ADD"))
				this->DoAdd(source, ci, params[2]);
			else if (params[1].equals_ci("DEL"))
				this->DoDel(source, ci, params[2]);
			else
			{
				success = false;
				this->OnSyntaxError(source, "");
			}
			if (success)
				Log(IsFounder(u, ci) ? LOG_COMMAND : LOG_OVERRIDE, u, this, ci) << params[1];
		}
		else
			source.Reply(ACCESS_DENIED);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Controls what messages will be sent to users when they join the channel."));
		return true;
	}
};

class CSEntryMessage : public Module
{
	CommandEntryMessage commandentrymsg;

 public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED), commandentrymsg(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnJoinChannel, I_OnReload, I_OnDatabaseReadMetadata, I_OnDatabaseWriteMetadata }; 
		ModuleManager::Attach(i, this, 4);
			
		ModuleManager::RegisterService(&commandentrymsg);

		this->OnReload();
	}

	void OnJoinChannel(User *u, Channel *c)
	{
		if (u && c && c->ci && u->server->IsSynced())
		{
			std::vector<EntryMsg> messages;

			if (c->ci->GetExtRegular("cs_entrymsg", messages))
				for (unsigned i = 0; i < messages.size(); ++i)
					u->SendMessage(c->ci->WhoSends(), "[%s] %s", c->ci->name.c_str(), messages[i].message.c_str());
		}
	}
		
	void OnReload()
	{
		ConfigReader config;
		EntryMsg::MaxEntries = config.ReadInteger("cs_entrymsg", "maxentries", "5", 0, true);
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.find("CS_ENTRYMSG_") == 0 && params.size() > 2)
		{
			Anope::string creator = params[0];
			time_t t = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
			Anope::string message = params[2];
			for (unsigned j = 3; j < params.size(); ++j)
				message += " " + params[j];

			std::vector<EntryMsg> messages;
			ci->GetExtRegular("cs_entrymsg", messages);
			messages.push_back(EntryMsg(creator, message, t));
			ci->Extend("cs_entrymsg", new ExtensibleItemRegular<std::vector<EntryMsg> >(messages));

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), ChannelInfo *ci)
	{
		std::vector<EntryMsg> messages;
		if (ci->GetExtRegular("cs_entrymsg", messages))
			for (unsigned i = 0; i < messages.size(); ++i)
				WriteMetadata("CS_ENTRYMSG_" + stringify(i), messages[i].creator + " " + stringify(messages[i].when) + " " + messages[i].message);
	}
};

MODULE_INIT(CSEntryMessage)
