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
	void DoList(User *u, ChannelInfo *ci)
	{
		std::vector<EntryMsg> messages;
		if (ci->GetExtRegular("cs_entrymsg", messages))
		{
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_HEADER, ci->name.c_str());
			for (unsigned i = 0; i < messages.size(); ++i)
				u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_ENTRY, i + 1, messages[i].message.c_str(), messages[i].creator.c_str(), do_strftime(messages[i].when).c_str());
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_END);
		}
		else
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_EMPTY, ci->name.c_str());
	}
		
	void DoAdd(User *u, ChannelInfo *ci, const Anope::string &message)
	{
		std::vector<EntryMsg> messages;
		ci->GetExtRegular("cs_entrymsg", messages);

		if (EntryMsg::MaxEntries && messages.size() >= EntryMsg::MaxEntries)
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_FULL, ci->name.c_str());
		else
		{
			messages.push_back(EntryMsg(u->nick, message));
			ci->Extend("cs_entrymsg", new ExtensibleItemRegular<std::vector<EntryMsg> >(messages));
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_ADDED, ci->name.c_str());
		}
	}

	void DoDel(User *u, ChannelInfo *ci, const Anope::string &message)
	{
		std::vector<EntryMsg> messages;
		if (!message.is_pos_number_only())
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_NOT_FOUND, message.c_str(), ci->name.c_str());
		else if (ci->GetExtRegular("cs_entrymsg", messages))
		{
			unsigned i = convertTo<unsigned>(message);
			if (i > 0 && i <= messages.size())
			{
				messages.erase(messages.begin() + i - 1);
				ci->Extend("cs_entrymsg", new ExtensibleItemRegular<std::vector<EntryMsg> >(messages));
				u->SendMessage(ChanServ, CHAN_ENTRYMSG_DELETED, i, ci->name.c_str());
			}
			else
				u->SendMessage(ChanServ, CHAN_ENTRYMSG_NOT_FOUND, message.c_str(), ci->name.c_str());
		}
		else
			u->SendMessage(ChanServ, CHAN_ENTRYMSG_LIST_EMPTY, ci->name.c_str());
	}

	void DoClear(User *u, ChannelInfo *ci)
	{
		ci->Shrink("cs_entrymsg");
		u->SendMessage(ChanServ, CHAN_ENTRYMSG_CLEARED, ci->name.c_str());
	}

 public:
	CommandEntryMessage(const Anope::string &cname) : Command(cname, 2, 3)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (ci && (IsFounder(u, ci) || u->Account()->HasCommand("chanserv/entrymsg")))
		{
			bool success = true;
			if (params[1].equals_ci("LIST"))
				this->DoList(u, ci);
			else if (params[1].equals_ci("CLEAR"))
				this->DoClear(u, ci);
			else if (params.size() < 3)
			{
				success = false;
				this->OnSyntaxError(source, "");
			}
			else if (params[1].equals_ci("ADD"))
				this->DoAdd(u, ci, params[2]);
			else if (params[1].equals_ci("DEL"))
				this->DoDel(u, ci, params[2]);
			else
			{
				success = false;
				this->OnSyntaxError(source, "");
			}
			if (success)
				Log(IsFounder(u, ci) ? LOG_COMMAND : LOG_OVERRIDE, u, this, ci) << params[1];
		}
		else
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
		}

		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "ENTRYMSG", CHAN_ENTRYMSG_SYNTAX);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_ENTRYMSG);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_ENTRYMSG);
	}
};

class CSEntryMessage : public Module
{
	CommandEntryMessage commandentrymsg;

 public:
	CSEntryMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), commandentrymsg("ENTRYMSG")
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
			
		Implementation i[] = { I_OnJoinChannel, I_OnReload, I_OnDatabaseReadMetadata, I_OnDatabaseWriteMetadata }; 
		ModuleManager::Attach(i, this, 4);
			
		this->AddCommand(ChanServ, &commandentrymsg);

		this->OnReload(false);
	}

	void OnJoinChannel(User *u, Channel *c)
	{
		if (u && c && c->ci && u->server->IsSynced())
		{
			std::vector<EntryMsg> messages;

			if (c->ci->GetExtRegular("cs_entrymsg", messages))
				for (unsigned i = 0; i < messages.size(); ++i)
					u->SendMessage(whosends(c->ci), "[%s] %s", c->ci->name.c_str(), messages[i].message.c_str());
		}
	}
		
	void OnReload(bool)
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
