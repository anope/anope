/* NickServ core functions
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

struct AJoinEntry;

struct AJoinList final
	: Serialize::Checker<std::vector<AJoinEntry *> >
{
	AJoinList(Extensible *) : Serialize::Checker<std::vector<AJoinEntry *> >("AJoinEntry") { }
	~AJoinList();
};

struct AJoinEntry final
	: Serializable
{
	Serialize::Reference<NickCore> owner;
	Anope::string channel;
	Anope::string key;

	AJoinEntry(Extensible *) : Serializable("AJoinEntry") { }

	~AJoinEntry() override
	{
		AJoinList *channels = owner->GetExt<AJoinList>("ajoinlist");
		if (channels)
		{
			std::vector<AJoinEntry *>::iterator it = std::find((*channels)->begin(), (*channels)->end(), this);
			if (it != (*channels)->end())
				(*channels)->erase(it);
		}
	}
};

struct AJoinEntryType final
	: public Serialize::Type
{
	AJoinEntryType()
		: Serialize::Type("AJoinEntry")
	{
	}

	void Serialize(const Serializable *obj, Serialize::Data &data) const override
	{
		const auto *aj = static_cast<const AJoinEntry *>(obj);
		if (!aj->owner)
			return;

		data.Store("ownerid", aj->owner->GetId());
		data.Store("channel", aj->channel);
		data.Store("key", aj->key);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &sd) const override
	{
		Anope::string sowner;
		uint64_t sownerid = 0;

		sd["owner"] >> sowner; // Deprecated 2.0 field
		sd["ownerid"] >> sownerid;

		auto *nc = sownerid ? NickCore::FindId(sownerid) : NickCore::Find(sowner);
		if (nc == NULL)
			return NULL;

		AJoinEntry *aj;
		if (obj)
			aj = anope_dynamic_static_cast<AJoinEntry *>(obj);
		else
		{
			aj = new AJoinEntry(nc);
			aj->owner = nc;
		}

		sd["channel"] >> aj->channel;
		sd["key"] >> aj->key;

		if (!obj)
		{
			AJoinList *channels = nc->Require<AJoinList>("ajoinlist");
			(*channels)->push_back(aj);
		}

		return aj;
	}
};

AJoinList::~AJoinList()
{
	for (const auto *ajoin : *(*this))
		delete ajoin;
}

class CommandNSAJoin final
	: public Command
{
	static void DoList(CommandSource &source, NickCore *nc)
	{
		AJoinList *channels = nc->Require<AJoinList>("ajoinlist");

		if ((*channels)->empty())
			source.Reply(_("%s's auto join list is empty."), nc->display.c_str());
		else
		{
			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Number")).AddColumn(_("Channel")).AddColumn(_("Key"));
			for (unsigned i = 0; i < (*channels)->size(); ++i)
			{
				AJoinEntry *aj = (*channels)->at(i);
				ListFormatter::ListEntry entry;
				entry["Number"] = Anope::ToString(i + 1);
				entry["Channel"] = aj->channel;
				entry["Key"] = aj->key;
				list.AddEntry(entry);
			}

			source.Reply(_("%s's auto join list:"), nc->display.c_str());

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (const auto &reply : replies)
				source.Reply(reply);
		}
	}

	void DoAdd(CommandSource &source, NickCore *nc, const Anope::string &chans, const Anope::string &keys)
	{
		AJoinList *channels = nc->Require<AJoinList>("ajoinlist");

		Anope::string addedchans;
		Anope::string alreadyadded;
		Anope::string invalidkey;
		commasepstream ksep(keys, true);
		commasepstream csep(chans);
		for (Anope::string chan, key; csep.GetToken(chan);)
		{
			ksep.GetToken(key);

			unsigned i = 0;
			for (; i < (*channels)->size(); ++i)
				if ((*channels)->at(i)->channel.equals_ci(chan))
					break;

			if ((*channels)->size() >= Config->GetModule(this->owner).Get<unsigned>("ajoinmax"))
			{
				source.Reply(_("Sorry, the maximum of %d auto join entries has been reached."), Config->GetModule(this->owner).Get<unsigned>("ajoinmax"));
				return;
			}
			else if (i != (*channels)->size())
				alreadyadded += chan + ", ";
			else if (!IRCD->IsChannelValid(chan))
				source.Reply(CHAN_X_INVALID, chan.c_str());
			else
			{
				Channel *c = Channel::Find(chan);
				Anope::string k;
				if (c && c->GetParam("KEY", k) && key != k)
				{
					invalidkey += chan + ", ";
					continue;
				}

				auto *entry = new AJoinEntry(nc);
				entry->owner = nc;
				entry->channel = chan;
				entry->key = key;
				(*channels)->push_back(entry);
				addedchans += chan + ", ";
			}
		}

		if (!alreadyadded.empty())
		{
			alreadyadded = alreadyadded.substr(0, alreadyadded.length() - 2);
			source.Reply(_("%s is already on %s's auto join list."), alreadyadded.c_str(), nc->display.c_str());
		}

		if (!invalidkey.empty())
		{
			invalidkey = invalidkey.substr(0, invalidkey.length() - 2);
			source.Reply(_("%s had an invalid key specified, and was thus ignored."), invalidkey.c_str());
		}

		if (addedchans.empty())
			return;

		addedchans = addedchans.substr(0, addedchans.length() - 2);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD channel " << addedchans << " to " << nc->display;
		source.Reply(_("%s added to %s's auto join list."), addedchans.c_str(), nc->display.c_str());
	}

	void DoDel(CommandSource &source, NickCore *nc, const Anope::string &chans)
	{
		AJoinList *channels = nc->Require<AJoinList>("ajoinlist");
		Anope::string delchans;
		Anope::string notfoundchans;
		commasepstream sep(chans);

		for (Anope::string chan; sep.GetToken(chan);)
		{
			unsigned i = 0;
			for (; i < (*channels)->size(); ++i)
				if ((*channels)->at(i)->channel.equals_ci(chan))
					break;

			if (i == (*channels)->size())
				notfoundchans += chan + ", ";
			else
			{
				delete (*channels)->at(i);
				delchans += chan + ", ";
			}
		}

		if (!notfoundchans.empty())
		{
			notfoundchans = notfoundchans.substr(0, notfoundchans.length() - 2);
			source.Reply(_("%s was not found on %s's auto join list."), notfoundchans.c_str(), nc->display.c_str());
		}

		if (delchans.empty())
			return;

		delchans = delchans.substr(0, delchans.length() - 2);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE channel " << delchans << " from " << nc->display;
		source.Reply(_("%s was removed from %s's auto join list."), delchans.c_str(), nc->display.c_str());

		if ((*channels)->empty())
			nc->Shrink<AJoinList>("ajoinlist");
	}

public:
	CommandNSAJoin(Module *creator) : Command(creator, "nickserv/ajoin", 1, 4)
	{
		this->SetDesc(_("Manage your auto join list"));
		this->SetSyntax(_("ADD [\037nickname\037] \037channel\037 [\037key\037]"));
		this->SetSyntax(_("DEL [\037nickname\037] \037channel\037"));
		this->SetSyntax(_("LIST [\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		Anope::string nick, param, param2;

		if (cmd.equals_ci("LIST"))
			nick = params.size() > 1 ? params[1] : "";
		else
			nick = (params.size() > 2 && IRCD->IsChannelValid(params[2])) ? params[1] : "";

		NickCore *nc;
		if (!nick.empty())
		{
			const NickAlias *na = NickAlias::Find(nick);
			if (na == NULL)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return;
			}
			else if (na->nc != source.GetAccount() && !source.HasCommand("nickserv/ajoin"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			nc = na->nc;
			param = params.size() > 2 ? params[2] : "";
			param2 = params.size() > 3 ? params[3] : "";
		}
		else
		{
			nc = source.nc;
			param = params.size() > 1 ? params[1] : "";
			param2 = params.size() > 2 ? params[2] : "";
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else if (nc->HasExt("NS_SUSPENDED"))
			source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
		else if (param.empty())
			this->OnSyntaxError(source, "");
		else if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, param, param2);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, param);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command manages your auto join list. When you identify "
			"you will automatically join the channels on your auto join list. "
			"Services Operators may provide a nick to modify other users' "
			"auto join lists."
		));
		return true;
	}
};

class NSAJoin final
	: public Module
{
	CommandNSAJoin commandnsajoin;
	ExtensibleItem<AJoinList> ajoinlist;
	AJoinEntryType ajoinentry_type;

public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnsajoin(this)
		, ajoinlist(this, "ajoinlist")
	{
		if (!IRCD || !IRCD->CanSVSJoin)
			throw ModuleException("Your IRCd does not support SVSJOIN");
	}

	void OnUserLogin(User *u) override
	{
		BotInfo *NickServ = Config->GetClient("NickServ");
		if (!NickServ)
			return;

		AJoinList *channels = u->Account()->GetExt<AJoinList>("ajoinlist");
		if (channels == NULL)
			return;

		/* Set +r now, so we can ajoin users into +R channels */
		ModeManager::ProcessModes();

		for (auto *entry : *(*channels))
		{
			Channel *c = Channel::Find(entry->channel);
			ChannelInfo *ci;

			if (c)
				ci = c->ci;
			else
				ci = ChannelInfo::Find(entry->channel);

			bool need_invite = false;
			Anope::string key = entry->key;
			AccessGroup u_access;

			if (ci != NULL)
			{
				if (ci->HasExt("CS_SUSPENDED"))
					continue;
				u_access = ci->AccessFor(u);
			}
			if (c != NULL)
			{
				if (c->FindUser(u) != NULL)
					continue;
				else if (c->HasMode("OPERONLY") && !u->HasMode("OPER"))
					continue;
				else if (c->HasMode("ADMINONLY") && !u->HasMode("ADMIN"))
					continue;
				else if (c->HasMode("SSL") && !u->IsSecurelyConnected())
					continue;
				else if (c->MatchesList(u, "BAN") && !c->MatchesList(u, "EXCEPT"))
					need_invite = true;
				else if (c->HasMode("INVITE") && !c->MatchesList(u, "INVITEOVERRIDE"))
					need_invite = true;

				if (c->HasMode("KEY"))
				{
					Anope::string k;
					if (c->GetParam("KEY", k))
					{
						if (u_access.HasPriv("GETKEY"))
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
						if (auto limit = Anope::TryConvert<unsigned>(l))
						{
							if (c->users.size() >= limit.value())
								need_invite = true;
						}
					}
				}
			}

			if (need_invite && c != NULL)
			{
				if (!u_access.HasPriv("INVITE"))
					continue;
				IRCD->SendInvite(NickServ, c, u);
			}

			IRCD->SendSVSJoin(NickServ, u, entry->channel, key);
		}
	}
};

MODULE_INIT(NSAJoin)
