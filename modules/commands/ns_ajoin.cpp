/* NickServ core functions
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

struct AJoinEntry;

struct AJoinList : Serialize::Checker<std::vector<AJoinEntry *> >, ExtensibleItem
{
	AJoinList() : Serialize::Checker<std::vector<AJoinEntry *> >("AJoinEntry") { }
	~AJoinList();
};

struct AJoinEntry : Serializable
{
	Serialize::Reference<NickCore> owner;
	Anope::string channel;
	Anope::string key;

	AJoinEntry() : Serializable("AJoinEntry") { }

	void Serialize(Serialize::Data &sd) const anope_override
	{
		if (!this->owner)
			return;

		sd["owner"] << this->owner->display;
		sd["channel"] << this->channel;
		sd["key"] << this->key;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &sd)
	{
		Anope::string sowner;

		sd["owner"] >> sowner;

		NickCore *nc = NickCore::Find(sowner);
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

AJoinList::~AJoinList()
{
	for (unsigned i = 0; i < (*this)->size(); ++i)
		delete (*this)->at(i);
}

class CommandNSAJoin : public Command
{
	void DoList(CommandSource &source, NickCore *nc)
	{
		AJoinList *channels = nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			nc->Extend("ns_ajoin_channels", channels);
		}

		if ((*channels)->empty())
			source.Reply(_("%s's auto join list is empty."), nc->display.c_str());
		else
		{
			ListFormatter list;
			list.AddColumn("Number").AddColumn("Channel").AddColumn("Key");
			for (unsigned i = 0; i < (*channels)->size(); ++i)
			{
				AJoinEntry *aj = (*channels)->at(i);
				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Channel"] = aj->channel;
				entry["Key"] = aj->key;
				list.AddEntry(entry);
			}

			source.Reply(_("%s's auto join list:"), nc->display.c_str());

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoAdd(CommandSource &source, NickCore *nc, const Anope::string &chan, const Anope::string &key)
	{
		AJoinList *channels = nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			nc->Extend("ns_ajoin_channels", channels);
		}

		unsigned i = 0;
		for (; i < (*channels)->size(); ++i)
			if ((*channels)->at(i)->channel.equals_ci(chan))
				break;

		if (*source.nc == nc && (*channels)->size() >= Config->GetModule(this->owner)->Get<unsigned>("ajoinmax"))
			source.Reply(_("Your auto join list is full."));
		else if (i != (*channels)->size())
			source.Reply(_("%s is already on %s's auto join list."), chan.c_str(), nc->display.c_str());
		else if (IRCD->IsChannelValid(chan) == false)
 			source.Reply(CHAN_X_INVALID, chan.c_str());
		else
		{
			AJoinEntry *entry = new AJoinEntry();
			entry->owner = nc;
			entry->channel = chan;
			entry->key = key;
			(*channels)->push_back(entry);
			source.Reply(_("Added %s to %s's auto join list."), chan.c_str(), nc->display.c_str());
		}
	}

	void DoDel(CommandSource &source, NickCore *nc, const Anope::string &chan)
	{
		AJoinList *channels = nc->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			nc->Extend("ns_ajoin_channels", channels);
		}

		unsigned i = 0;
		for (; i < (*channels)->size(); ++i)
			if ((*channels)->at(i)->channel.equals_ci(chan))
				break;
		
		if (i == (*channels)->size())
			source.Reply(_("%s was not found on %s's auto join list."), chan.c_str(), nc->display.c_str());
		else
		{
			delete (*channels)->at(i);
			(*channels)->erase((*channels)->begin() + i);
			source.Reply(_("%s was removed from %s's auto join list."), chan.c_str(), nc->display.c_str());
		}
	}

 public:
	CommandNSAJoin(Module *creator) : Command(creator, "nickserv/ajoin", 1, 3)
	{
		this->SetDesc(_("Manage your auto join list"));
		this->SetSyntax(_("ADD [\037user\037] \037channel\037 [\037key\037]"));
		this->SetSyntax(_("DEL [\037user\037] \037channel\037"));
		this->SetSyntax(_("LIST [\037user\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickCore *nc = source.GetAccount();
		Anope::string param, param2;

		if (params.size() > 1 && source.IsServicesOper() && IRCD->IsNickValid(params[1]))
		{
			NickAlias *na = NickAlias::Find(params[1]);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
				return;
			}

			nc = na->nc;
			param = params.size() > 2 ? params[2] : "";
			param2 = params.size() > 3 ? params[3] : "";
		}
		else
		{
			param = params.size() > 1 ? params[1] : "";
			param2 = params.size() > 2 ? params[2] : "";
		}

		if (params[0].equals_ci("LIST"))
			this->DoList(source, nc);
		else if (param.empty())
			this->OnSyntaxError(source, "");
		else if (params[0].equals_ci("ADD"))
			this->DoAdd(source, nc, param, param2);
		else if (params[0].equals_ci("DEL"))
			this->DoDel(source, nc, param);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command manages your auto join list. When you identify\n"
				"you will automatically join the channels on your auto join list.\n"
				"Services Operators may provide a nick to modify other users'\n"
				"auto join lists."));
		return true;
	}
};

class NSAJoin : public Module
{
	Serialize::Type ajoinentry_type;
	CommandNSAJoin commandnsajoin;

 public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		ajoinentry_type("AJoinEntry", AJoinEntry::Unserialize), commandnsajoin(this)
	{

		if (!IRCD->CanSVSJoin)
			throw ModuleException("Your IRCd does not support SVSJOIN");

		Implementation i[] = { I_OnNickIdentify };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnNickIdentify(User *u) anope_override
	{
		BotInfo *NickServ = Config->GetClient("NickServ");
		if (!NickServ)
			return;

		AJoinList *channels = u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList();
			u->Account()->Extend("ns_ajoin_channels", channels);
		}

		for (unsigned i = 0; i < (*channels)->size(); ++i)
		{
			AJoinEntry *entry = (*channels)->at(i);
			Channel *c = Channel::Find(entry->channel);
			ChannelInfo *ci;

			if (c)
				ci = c->ci;
			else
				ci = ChannelInfo::Find(entry->channel);

			bool need_invite = false;
			Anope::string key = entry->key;
			
			if (ci != NULL)
			{
				if (ci->HasExt("SUSPENDED"))
					continue;
			}
			if (c != NULL)
			{
				if (c->FindUser(u) != NULL)
					continue;
				else if (c->HasMode("OPERONLY") && !u->HasMode("OPER"))
					continue;
				else if (c->HasMode("ADMINONLY") && !u->HasMode("ADMIN"))
					continue;
				else if (c->HasMode("SSL") && !u->HasMode("SSL"))
					continue;
				else if (c->MatchesList(u, "BAN") == true && c->MatchesList(u, "EXCEPT") == false)
					need_invite = true;
				else if (c->HasMode("INVITE") && c->MatchesList(u, "INVITEOVERRIDE") == false)
					need_invite = true;
					
				if (c->HasMode("KEY"))
				{
					Anope::string k;
					if (c->GetParam("KEY", k))
					{
						if (ci->AccessFor(u).HasPriv("GETKEY"))
							key = k;
						else if (key != k)
							need_invite = true;
					}
				}
				if (c->HasMode("LIMIT"))
				{
					Anope::string l;
					if (c->GetParam("LIMIT", l))
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
				IRCD->SendInvite(NickServ, c, u);
			}

			IRCD->SendSVSJoin(NickServ, u, entry->channel, key);
		}
	}
};

MODULE_INIT(NSAJoin)
