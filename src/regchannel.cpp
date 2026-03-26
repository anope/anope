// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "services.h"
#include "modules.h"
#include "regchannel.h"
#include "account.h"
#include "access.h"
#include "channels.h"
#include "config.h"
#include "bots.h"
#include "servers.h"

Serialize::Checker<registered_channel_map> RegisteredChannelList(CHANNELINFO_TYPE);

ChannelInfo::ChannelInfo(const Anope::string &chname)
	: Serializable(CHANNELINFO_TYPE)
	, access(CHANACCESS_TYPE)
	, name(chname)
	, registered(Anope::CurTime)
	, last_used(Anope::CurTime)
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChannelInfo constructor");

	this->c = Channel::Find(chname);
	if (this->c)
		this->c->ci = this;

	if (!RegisteredChannelList->insert_or_assign(this->name, this).second)
		Log(LOG_DEBUG) << "Duplicate channel " << this->name << " in registered channel table?";

	FOREACH_MOD(OnCreateChan, (this));
}

ChannelInfo::ChannelInfo(const ChannelInfo &ci)
	: Serializable(CHANNELINFO_TYPE)
	, access(CHANACCESS_TYPE)
{
	*this = ci;

	if (this->founder)
		++this->founder->channelcount;

	this->access->clear();

	FOREACH_MOD(OnCreateChan, (this));
}

ChannelInfo::~ChannelInfo()
{
	FOREACH_MOD(OnDelChan, (this));

	UnsetExtensibles();

	Log(LOG_DEBUG) << "Deleting channel " << this->name;

	if (this->c)
	{
		if (this->bi && this->c->FindUser(this->bi))
			this->bi->Part(this->c);

		/* Parting the service bot can cause the channel to go away */

		if (this->c)
		{
			if (this->c && this->c->CheckDelete())
				this->c->QueueForDeletion();

			this->c = NULL;
		}
	}

	RegisteredChannelList->erase(this->name);

	this->SetFounder(NULL);
	this->SetSuccessor(NULL);

	this->ClearAccess();

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			delete this->memos.GetMemo(i);
		this->memos.memos->clear();
	}
}

ChannelInfo::Type::Type()
	: Serialize::Type(CHANNELINFO_TYPE)
{
}

void ChannelInfo::Type::Serialize(Serializable *obj, Serialize::Data &data) const
{
	const auto *ci = static_cast<const ChannelInfo *>(obj);

	data.Store("name", ci->name);
	if (ci->founder)
		data.Store("founderid", ci->founder->GetId());
	if (ci->successor)
		data.Store("successorid", ci->successor->GetId());
	data.Store("description", ci->desc);
	data.Store("registered", ci->registered);
	data.Store("last_used", ci->last_used);
	data.Store("last_topic", ci->last_topic);
	data.Store("last_topic_setter", ci->last_topic_setter);
	data.Store("last_topic_time", ci->last_topic_time);
	data.Store("bantype", ci->bantype);
	{
		Anope::string levels_buffer;
		for (const auto &[name, level] : ci->levels)
			levels_buffer += name + " " + Anope::ToString(level) + " ";
		data.Store("levels", levels_buffer);
	}
	if (ci->bi)
		data.Store("bi", ci->bi->nick);
	data.Store("banexpire", ci->banexpire);
	data.Store("memomax", ci->memos.memomax);

	std::ostringstream oss;
	for (const auto &ignore : ci->memos.ignores)
		oss << ignore << " ";
	data.Store("memoignores", oss.str());

	Extensible::ExtensibleSerialize(ci, ci, data);
}

Serializable *ChannelInfo::Type::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	ChannelInfo *ci;
	if (obj)
		ci = anope_dynamic_static_cast<ChannelInfo *>(obj);
	else
		ci = new ChannelInfo(data.Load("name"));

	const auto sfounderid = data.Load<uint64_t>("founderid");
	ci->SetFounder(sfounderid ? NickCore::FindId(sfounderid) : NickCore::Find(data.Load("founder")));

	const auto ssuccessorid = data.Load<uint64_t>("successorid");
	ci->SetSuccessor(ssuccessorid ? NickCore::FindId(ssuccessorid) : NickCore::Find(data.Load("successor")));

	ci->desc = data.Load("description");
	ci->registered = data.Load<time_t>("registered", data.Load<time_t>("time_registered"));
	ci->last_used = data.Load<time_t>("last_used");
	ci->last_topic = data.Load("last_topic");
	ci->last_topic_setter = data.Load("last_topic_setter");
	ci->last_topic_time = data.Load<time_t>("last_topic_time");
	ci->bantype = data.Load<int16_t>("bantype");
	{
		std::vector<Anope::string> v;
		spacesepstream(data.Load("levels")).GetTokens(v);
		for (unsigned i = 0; i + 1 < v.size(); i += 2)
		{
			// Begin 2.0 compatibility.
			if (v[i] == "FANTASIA")
				v[i] = "FANTASY";
			// End 2.0 compatibility.

			if (auto level = Anope::TryConvert<int16_t>(v[i + 1]))
				ci->levels[v[i]] = level.value();
		}
	}

	auto *bi = BotInfo::Find(data.Load("bi"), true);
	if (*ci->bi != bi)
	{
		if (bi)
			bi->Assign(NULL, ci);
		else if (ci->bi)
			ci->bi->UnAssign(NULL, ci);
	}

	ci->banexpire = data.Load<time_t>("banexpire");
	ci->memos.memomax = data.Load<int16_t>("memomax");
	{
		Anope::string buf;
		buf = data.Load("memoignores");
		spacesepstream sep(buf);
		ci->memos.ignores.clear();
		while (sep.GetToken(buf))
			ci->memos.ignores.insert(buf);
	}

	Extensible::ExtensibleUnserialize(ci, ci, data);

	// Begin 1.9 compatibility.
	if (data.Load<bool>("extensible:PRIVATE"))
		ci->Extend<bool>("CS_PRIVATE");
	if (data.Load<bool>("extensible:NO_EXPIRE"))
		ci->Extend<bool>("CS_NO_EXPIRE");
	if (data.Load<bool>("extensible:FANTASY"))
		ci->Extend<bool>("BS_FANTASY");
	if (data.Load<bool>("extensible:GREET"))
		ci->Extend<bool>("BS_GREET");
	if (data.Load<bool>("extensible:PEACE"))
		ci->Extend<bool>("PEACE");
	if (data.Load<bool>("extensible:SECUREFOUNDER"))
		ci->Extend<bool>("SECUREFOUNDER");
	if (data.Load<bool>("extensible:RESTRICTED"))
		ci->Extend<bool>("RESTRICTED");
	if (data.Load<bool>("extensible:KEEPTOPIC"))
		ci->Extend<bool>("KEEPTOPIC");
	if (data.Load<bool>("extensible:SIGNKICK"))
		ci->Extend<bool>("SIGNKICK");
	if (data.Load<bool>("extensible:SIGNKICK_LEVEL"))
		ci->Extend<bool>("SIGNKICK_LEVEL");
	// End 1.9 compatibility.

	// Begin 2.0 compatibility.
	if (!ci->registered)
		ci->registered = ci->last_used ? ci->last_used : Anope::CurTime; // Probably corrupt database.
	// End 2.0 compatibility.

	return ci;
}


void ChannelInfo::SetFounder(NickCore *nc)
{
	if (this->founder)
	{
		--this->founder->channelcount;
		this->founder->RemoveChannelReference(this);
	}

	this->founder = nc;

	if (this->founder)
	{
		++this->founder->channelcount;
		this->founder->AddChannelReference(this);
	}
}

NickCore *ChannelInfo::GetFounder() const
{
	return this->founder;
}

void ChannelInfo::SetSuccessor(NickCore *nc)
{
	if (this->successor)
		this->successor->RemoveChannelReference(this);
	this->successor = nc;
	if (this->successor)
		this->successor->AddChannelReference(this);
}

NickCore *ChannelInfo::GetSuccessor() const
{
	return this->successor;
}

BotInfo *ChannelInfo::WhoSends() const
{
	if (this->bi)
		return this->bi;

	BotInfo *ChanServ = Config->GetClient("ChanServ");
	if (ChanServ)
		return ChanServ;

	if (!BotListByNick->empty())
		return BotListByNick->begin()->second;

	return NULL;
}

void ChannelInfo::AddAccess(ChanAccess *taccess)
{
	this->access->push_back(taccess);
}

ChanAccess *ChannelInfo::GetAccess(unsigned index) const
{
	if (this->access->empty() || index >= this->access->size())
		return NULL;

	ChanAccess *acc = (*this->access)[index];
	acc->QueueUpdate();
	return acc;
}

static void FindMatchesRecurse(ChannelInfo *ci, const User *u, const NickCore *account, unsigned int depth, std::vector<ChanAccess::Path> &paths, ChanAccess::Path &path)
{
	if (depth > ChanAccess::MAX_DEPTH)
		return;

	for (unsigned int i = 0; i < ci->GetAccessCount(); ++i)
	{
		ChanAccess *a = ci->GetAccess(i);
		ChannelInfo *next = NULL;

		if (a->Matches(u, account, next))
		{
			ChanAccess::Path next_path = path;
			next_path.push_back(a);

			paths.push_back(next_path);
		}
		else if (next)
		{
			ChanAccess::Path next_path = path;
			next_path.push_back(a);

			FindMatchesRecurse(next, u, account, depth + 1, paths, next_path);
		}
	}
}

static void FindMatches(AccessGroup &group, ChannelInfo *ci, const User *u, const NickCore *account)
{
	ChanAccess::Path path;
	FindMatchesRecurse(ci, u, account, 0, group.paths, path);
}

AccessGroup ChannelInfo::AccessFor(const User *u, bool updateLastUsed)
{
	AccessGroup group;

	if (u == NULL)
		return group;

	group.super_admin = u->super_admin;
	group.founder = IsFounder(u, this);
	group.ci = this;
	group.nc = u->Account();

	FindMatches(group, this, u, u->Account());

	if (group.founder || !group.paths.empty())
	{
		if (updateLastUsed)
			this->last_used = Anope::CurTime;

		for (auto &p : group.paths)
		{
			for (auto *ca : p)
				ca->last_seen = Anope::CurTime;
		}
	}

	return group;
}

AccessGroup ChannelInfo::AccessFor(const NickCore *nc, bool updateLastUsed)
{
	AccessGroup group;

	group.founder = (this->founder && this->founder == nc);
	group.ci = this;
	group.nc = nc;

	FindMatches(group, this, NULL, nc);

	if (group.founder || !group.paths.empty())
		if (updateLastUsed)
			this->last_used = Anope::CurTime;

	/* don't update access last seen here, this isn't the user requesting access */

	return group;
}

unsigned ChannelInfo::GetAccessCount() const
{
	return this->access->size();
}

static unsigned int GetDeepAccessCount(const ChannelInfo *ci, std::set<const ChannelInfo *> &seen, unsigned int depth)
{
	if (depth > ChanAccess::MAX_DEPTH || seen.count(ci))
		return 0;
	seen.insert(ci);

	unsigned int total = 0;

	for (unsigned int i = 0; i < ci->GetAccessCount(); ++i)
	{
		ChanAccess::Path path;
		ChanAccess *a = ci->GetAccess(i);
		ChannelInfo *next = NULL;

		a->Matches(NULL, NULL, next);
		++total;

		if (next)
			total += GetDeepAccessCount(ci, seen, depth + 1);
	}

	return total;
}

unsigned ChannelInfo::GetDeepAccessCount() const
{
	std::set<const ChannelInfo *> seen;
	return ::GetDeepAccessCount(this, seen, 0);
}

ChanAccess *ChannelInfo::EraseAccess(unsigned index)
{
	if (this->access->empty() || index >= this->access->size())
		return NULL;

	ChanAccess *ca = this->access->at(index);
	this->access->erase(this->access->begin() + index);
	return ca;
}

void ChannelInfo::ClearAccess()
{
	for (unsigned i = this->access->size(); i > 0; --i)
		delete this->GetAccess(i - 1);
}

const Anope::map<int16_t> &ChannelInfo::GetLevelEntries()
{
	return this->levels;
}

int16_t ChannelInfo::GetLevel(const Anope::string &priv) const
{
	if (PrivilegeManager::FindPrivilege(priv) == NULL)
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return ACCESS_INVALID;
	}

	auto it = this->levels.find(priv);
	if (it == this->levels.end())
		return 0;
	return it->second;
}

void ChannelInfo::SetLevel(const Anope::string &priv, int16_t level)
{
	if (PrivilegeManager::FindPrivilege(priv) == NULL)
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return;
	}

	this->levels[priv] = level;
}

void ChannelInfo::RemoveLevel(const Anope::string &priv)
{
	this->levels.erase(priv);
}

void ChannelInfo::ClearLevels()
{
	this->levels.clear();
}

Anope::string ChannelInfo::GetIdealBan(User *u) const
{
	switch (this->bantype)
	{
		case 0:
			return "*!" + u->GetVIdent() + "@" + u->GetDisplayedHost();
		case 1:
			if (u->GetVIdent()[0] == '~')
				return "*!*" + u->GetVIdent() + "@" + u->GetDisplayedHost();
			else
				return "*!" + u->GetVIdent() + "@" + u->GetDisplayedHost();
		case 3:
			return "*!" + u->Mask();
		case 2:
		default:
			return "*!*@" + u->GetDisplayedHost();
	}
}

ChannelInfo *ChannelInfo::Find(const Anope::string &name)
{
	auto it = RegisteredChannelList->find(name);
	if (it != RegisteredChannelList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

bool IsFounder(const User *user, const ChannelInfo *ci)
{
	if (!user || !ci)
		return false;

	if (user->super_admin)
		return true;

	if (user->Account() && user->Account() == ci->GetFounder())
		return true;

	return false;
}


void ChannelInfo::AddChannelReference(const Anope::string &what)
{
	++references[what];
}

void ChannelInfo::RemoveChannelReference(const Anope::string &what)
{
	int &i = references[what];
	if (--i <= 0)
		references.erase(what);
}

void ChannelInfo::GetChannelReferences(std::deque<Anope::string> &chans)
{
	chans.clear();
	for (auto &[chan, _] : references)
		chans.push_back(chan);
}
