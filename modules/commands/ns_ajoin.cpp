/* NickServ core functions
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

struct AJoinEntry;

struct AJoinList : serialize_checker<std::vector<AJoinEntry *> >, ExtensibleItem
{
	AJoinList() : serialize_checker<std::vector<AJoinEntry *> >("AJoinEntry") { }
};

struct AJoinEntry : Serializable
{
	serialize_obj<NickCore> owner;
	Anope::string channel;
	Anope::string key;

	const Anope::string serialize_name() const anope_override
	{
		return "AJoinEntry";
	}

	Serialize::Data serialize() const anope_override
	{
		Serialize::Data sd;

		if (!this->owner)
			return sd;

		sd["owner"] << this->owner->display;
		sd["channel"] << this->channel;
		sd["key"] << this->key;

		return sd;
	}

	static Serializable* unserialize(Serializable *obj, Serialize::Data &sd)
	{
		NickCore *nc = findcore(sd["owner"].astr());
		if (nc == NULL)
			return NULL;

		AJoinEntry *aj;
		if (obj)
			aj = anope_dynamic_static_cast<AJoinEntry *>(obj);
		else
		{
			aj = new AJoinEntry();
			aj->owner = nc;
		}

		sd["channel"] >> aj->channel;
		sd["key"] >> aj->key;

		if (!obj)
		{
			AJoinList *channels = nc->GetExt<AJoinList *>("ns_ajoin_channels");
			if (channels == NULL)
			{
				channels = new AJoinList();
				nc->Extend("ns_ajoin_channels", channels);
			}
			(*channels)->push_back(aj);
		}

		return aj;
	}
};

class CommandNSAJoin : public Command
{
	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			source.nc->Extend("ns_ajoin_channels", channels);
		}

		if ((*channels)->empty())
			source.Reply(_("Your auto join list is empty."));
		else
		{
			ListFormatter list;
			list.addColumn("Number").addColumn("Channel").addColumn("Key");
			for (unsigned i = 0; i < (*channels)->size(); ++i)
			{
				AJoinEntry *aj = (*channels)->at(i);
				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Channel"] = aj->channel;
				entry["Key"] = aj->key;
				list.addEntry(entry);
			}

			source.Reply(_("Your auto join list:"));

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			source.nc->Extend("ns_ajoin_channels", channels);
		}

		unsigned i = 0;
		for (; i < (*channels)->size(); ++i)
			if ((*channels)->at(i)->channel.equals_ci(params[1]))
				break;

		if ((*channels)->size() >= Config->AJoinMax)
			source.Reply(_("Your auto join list is full."));
		else if (i != (*channels)->size())
			source.Reply(_("%s is already on your auto join list."), params[1].c_str());
		else if (ircdproto->IsChannelValid(params[1]) == false)
 			source.Reply(CHAN_X_INVALID, params[1].c_str());
		else
		{
			AJoinEntry *entry = new AJoinEntry();
			entry->owner = source.nc;
			entry->channel = params[1];
			entry->key = params.size() > 2 ? params[2] : "";
			(*channels)->push_back(entry);
			source.Reply(_("Added %s to your auto join list."), params[1].c_str());
		}
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			source.nc->Extend("ns_ajoin_channels", channels);
		}

		unsigned i = 0;
		for (; i < (*channels)->size(); ++i)
			if ((*channels)->at(i)->channel.equals_ci(params[1]))
				break;
		
		if (i == (*channels)->size())
			source.Reply(_("%s was not found on your auto join list."), params[1].c_str());
		else
		{
			(*channels)->at(i)->destroy();
			(*channels)->erase((*channels)->begin() + i);
			source.Reply(_("%s was removed from your auto join list."), params[1].c_str());
		}
	}

 public:
	CommandNSAJoin(Module *creator) : Command(creator, "nickserv/ajoin", 1, 3)
	{
		this->SetDesc(_("Manage your auto join list"));
		this->SetSyntax(_("{ADD | DEL | LIST} [\037channel\037] [\037key\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0].equals_ci("LIST"))
			this->DoList(source, params);
		else if (params.size() < 2)
			this->OnSyntaxError(source, "");
		else if (params[0].equals_ci("ADD"))
			this->DoAdd(source, params);
		else if (params[0].equals_ci("DEL"))
			this->DoDel(source, params);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command manages your auto join list. When you identify\n"
				"you will automatically join the channels on your auto join list"));
		return true;
	}
};

class NSAJoin : public Module
{
	SerializeType ajoinentry_type;
	CommandNSAJoin commandnsajoin;

 public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		ajoinentry_type("AJoinEntry", AJoinEntry::unserialize), commandnsajoin(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickIdentify };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnNickIdentify(User *u) anope_override
	{
		AJoinList *channels = u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			u->Account()->Extend("ns_ajoin_channels", channels);
		}

		const BotInfo *bi = findbot(Config->NickServ);
		if (bi == NULL)
			return;

		for (unsigned i = 0; i < (*channels)->size(); ++i)
		{
			AJoinEntry *entry = (*channels)->at(i);
			Channel *c = findchan(entry->channel);
			ChannelInfo *ci;

			if (c)
				ci = c->ci;
			else
				ci = cs_findchan(entry->channel);

			bool need_invite = false;
			Anope::string key = entry->key;
			
			if (ci != NULL)
			{
				if (ci->HasFlag(CI_SUSPENDED))
					continue;
			}
			if (c != NULL)
			{
				if (c->FindUser(u) != NULL)
					continue;
				else if (c->HasMode(CMODE_OPERONLY) && !u->HasMode(UMODE_OPER))
					continue;
				else if (c->HasMode(CMODE_ADMINONLY) && !u->HasMode(UMODE_ADMIN))
					continue;
				else if (c->HasMode(CMODE_SSL) && !u->HasMode(UMODE_SSL))
					continue;
				else if (matches_list(c, u, CMODE_BAN) == true && matches_list(c, u, CMODE_EXCEPT) == false)
					need_invite = true;
				else if (c->HasMode(CMODE_INVITE) && matches_list(c, u, CMODE_INVITEOVERRIDE) == false)
					need_invite = true;
					
				if (c->HasMode(CMODE_KEY))
				{
					Anope::string k;
					if (c->GetParam(CMODE_KEY, k))
					{
						if (ci->AccessFor(u).HasPriv("GETKEY"))
							key = k;
						else if (key != k)
							need_invite = true;
					}
				}
				if (c->HasMode(CMODE_LIMIT))
				{
					Anope::string l;
					if (c->GetParam(CMODE_LIMIT, l))
					{
						try
						{
							unsigned limit = convertTo<unsigned>(l);
							if (c->users.size() >= limit)
								need_invite = true;
						}
						catch (const ConvertException &) { }
					}
				}
			}

			if (need_invite && c != NULL)
			{
				if (!ci->AccessFor(u).HasPriv("INVITE"))
					continue;
				ircdproto->SendInvite(bi, c, u);
			}

			ircdproto->SendSVSJoin(bi, u->nick, entry->channel, key);
		}
	}
};

MODULE_INIT(NSAJoin)
