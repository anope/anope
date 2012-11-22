/*
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "anope.h"
#include "bots.h"
#include "servers.h"
#include "protocol.h"
#include "xline.h"
#include "regchannel.h"
#include "channels.h"
#include "config.h"
#include "language.h"
#include "serialize.h"

Serialize::Checker<botinfo_map> BotListByNick("BotInfo"), BotListByUID("BotInfo");

static const Anope::string BotFlagString[] = { "BEGIN", "CORE", "PRIVATE", "CONF", "" };
template<> const Anope::string* Flags<BotFlag>::flags_strings = BotFlagString;

static const Anope::string BotServFlagStrings[] = {
	"BEGIN", "DONTKICKOPS", "DONTKICKVOICES", "FANTASY", "GREET", "NOBOT",
	"KICK_BOLDs", "KICK_COLORS", "KICK_REVERSES", "KICK_UNDERLINES", "KICK_BADWORDS", "KICK_CAPS",
	"KICK_FLOOD", "KICK_REPEAT", "KICK_ITALICS", "KICK_AMSGS", "MSG_PRIVMSG", "MSG_NOTICE",
	"MSG_NOTICEOPS", ""
};
template<> const Anope::string* Flags<BotServFlag>::flags_strings = BotServFlagStrings;

BotInfo::BotInfo(const Anope::string &nnick, const Anope::string &nuser, const Anope::string &nhost, const Anope::string &nreal, const Anope::string &bmodes) : User(nnick, nuser, nhost, "", "", Me, nreal, Anope::CurTime, "", Servers::TS6_UID_Retrieve()), Serializable("BotInfo"), botmodes(bmodes)
{
	this->lastmsg = this->created = Anope::CurTime;
	this->introduced = false;

	(*BotListByNick)[this->nick] = this;
	if (!this->uid.empty())
		(*BotListByUID)[this->uid] = this;

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		Anope::string tmodes = !this->botmodes.empty() ? ("+" + this->botmodes) : (IRCD ? IRCD->DefaultPseudoclientModes : "");
		if (!tmodes.empty())
			this->SetModesInternal(tmodes.c_str());

		XLine x(this->nick, "Reserved for services");
		IRCD->SendSQLine(NULL, &x);
		IRCD->SendClientIntroduction(this);
		this->introduced = true;
	}
}

BotInfo::~BotInfo()
{
	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		IRCD->SendQuit(this, "");
		this->introduced = false;
		XLine x(this->nick);
		IRCD->SendSQLineDel(&x);
	}

	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->bi == this)
		{
			ci->QueueUpdate();
			ci->bi = NULL;
		}
	}

	BotListByNick->erase(this->nick);
	if (!this->uid.empty())
		BotListByUID->erase(this->uid);
}

Serialize::Data BotInfo::Serialize() const
{
	Serialize::Data data;

	data["nick"].SetMax(64)/*XXX*/ << this->nick;
	data["user"] << this->ident;
	data["host"] << this->host;
	data["realname"] << this->realname;
	data["created"] << this->created;
	data["flags"] << this->ToString();

	return data;
}

Serializable* BotInfo::Unserialize(Serializable *obj, Serialize::Data &data)
{
	BotInfo *bi;
	if (obj)
		bi = anope_dynamic_static_cast<BotInfo *>(obj);
	else if (!(bi = BotInfo::Find(data["nick"].astr())))
		bi = new BotInfo(data["nick"].astr(), data["user"].astr(), data["host"].astr(), data["realname"].astr());
	data["created"] >> bi->created;
	bi->FromString(data["flags"].astr());
	return bi;
}

void BotInfo::GenerateUID()
{
	if (!this->uid.empty())
		throw CoreException("Bot already has a uid?");
	this->uid = Servers::TS6_UID_Retrieve();
	(*BotListByUID)[this->uid] = this;
	UserListByUID[this->uid] = this;
}

void BotInfo::SetNewNick(const Anope::string &newnick)
{
	UserListByNick.erase(this->nick);
	BotListByNick->erase(this->nick);

	this->nick = newnick;

	UserListByNick[this->nick] = this;
	(*BotListByNick)[this->nick] = this;
}

void BotInfo::RejoinAll()
{
	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		const ChannelInfo *ci = it->second;

		if (ci->bi == this && ci->c && ci->c->users.size() >= Config->BSMinUsers)
			this->Join(ci->c);
	}
}

void BotInfo::Assign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotAssign, OnBotAssign(u, ci, this));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->bi)
		ci->bi->UnAssign(u, ci);
	
	ci->bi = this;
	if (ci->c && ci->c->users.size() >= Config->BSMinUsers)
		this->Join(ci->c, &ModeManager::DefaultBotModes);
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotUnAssign, OnBotUnAssign(u, ci));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->c && ci->c->FindUser(ci->bi))
	{
		if (u)
			ci->bi->Part(ci->c, "UNASSIGN from " + u->nick);
		else
			ci->bi->Part(ci->c);
	}

	ci->bi = NULL;
}

unsigned BotInfo::GetChannelCount() const
{
	unsigned count = 0;
	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		const ChannelInfo *ci = it->second;

		if (ci->bi == this)
			++count;
	}
	return count;
}

void BotInfo::Join(Channel *c, ChannelStatus *status)
{
	if (c->FindUser(this) != NULL)
		return;

	if (Config && IRCD && Config->BSSmartJoin)
	{
		std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> bans = c->GetModeList(CMODE_BAN);

		/* We check for bans */
		for (; bans.first != bans.second; ++bans.first)
		{
			Entry ban(CMODE_BAN, bans.first->second);
			if (ban.Matches(this))
				c->RemoveMode(NULL, CMODE_BAN, ban.GetMask());
		}

		Anope::string Limit;
		unsigned limit = 0;
		if (c->GetParam(CMODE_LIMIT, Limit) && Limit.is_pos_number_only())
			limit = convertTo<unsigned>(Limit);

		/* Should we be invited? */
		if (c->HasMode(CMODE_INVITE) || (limit && c->users.size() >= limit))
			IRCD->SendNotice(this, "@" + c->name, "%s invited %s into the channel.", this->nick.c_str(), this->nick.c_str());

		ModeManager::ProcessModes();
	}

	c->JoinUser(this);
	if (IRCD)
		IRCD->SendJoin(this, c, status);

	FOREACH_MOD(I_OnBotJoin, OnBotJoin(c, this));
}

void BotInfo::Join(const Anope::string &chname, ChannelStatus *status)
{
	Channel *c = Channel::Find(chname);
	return this->Join(c ? c : new Channel(chname), status);
}

void BotInfo::Part(Channel *c, const Anope::string &reason)
{
	if (c->FindUser(this) == NULL)
		return;

	IRCD->SendPart(this, c, "%s", !reason.empty() ? reason.c_str() : "");
	c->DeleteUser(this);
}

void BotInfo::OnMessage(User *u, const Anope::string &message)
{
	if (this->commands.empty())
		return;

	CommandSource source(u->nick, u, u->Account(), u, this);
	RunCommand(source, message);
}

void BotInfo::SetCommand(const Anope::string &cname, const Anope::string &sname, const Anope::string &permission)
{
	CommandInfo ci;
	ci.name = sname;
	ci.permission = permission;
	this->commands[cname] = ci;
}

CommandInfo *BotInfo::GetCommand(const Anope::string &cname)
{
	CommandInfo::map::iterator it = this->commands.find(cname);
	if (it != this->commands.end())
		return &it->second;
	return NULL;
}

BotInfo* BotInfo::Find(const Anope::string &nick, bool nick_only)
{
	BotInfo *bi = NULL;
	if (!nick_only && isdigit(nick[0]) && IRCD->RequiresID)
	{
		botinfo_map::iterator it = BotListByUID->find(nick);
		if (it != BotListByUID->end())
			bi = it->second;
	}
	else
	{
		botinfo_map::iterator it = BotListByNick->find(nick);
		if (it != BotListByNick->end())
			bi = it->second;
	}

	if (bi)
		bi->QueueUpdate();
	return bi;
}

