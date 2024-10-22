/*
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2024 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
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

BotInfo::BotInfo(const Anope::string &nnick, const Anope::string &nuser, const Anope::string &nhost, const Anope::string &nreal, const Anope::string &bmodes)
	: User(nnick, nuser, nhost, "", "", Me, nreal, Anope::CurTime, "", {}, IRCD ? IRCD->UID_Retrieve() : "", NULL)
	, Serializable("BotInfo")
	, channels("ChannelInfo")
	, botmodes(bmodes)
{
	this->lastmsg = this->created = Anope::CurTime;
	this->introduced = false;
	this->oper_only = this->conf = false;

	(*BotListByNick)[this->nick] = this;
	if (!this->uid.empty())
		(*BotListByUID)[this->uid] = this;

	FOREACH_MOD(OnCreateBot, (this));

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		spacesepstream modesep(this->botmodes.empty() ? IRCD->DefaultPseudoclientModes : "+" + this->botmodes);

		Anope::string modechars;
		modesep.GetToken(modechars);

		std::vector<Anope::string> modeparams;
		modesep.GetTokens(modeparams);

		if (!modechars.empty())
			this->SetModesInternal(this, modechars, modeparams);

		XLine x(this->nick, "Reserved for services");
		IRCD->SendSQLine(NULL, &x);
		IRCD->SendClientIntroduction(this);
		this->introduced = true;
	}
}

BotInfo::~BotInfo()
{
	UnsetExtensibles();

	FOREACH_MOD(OnDelBot, (this));

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		IRCD->SendQuit(this, "");
		FOREACH_MOD(OnUserQuit, (this, ""));
		this->introduced = false;
		XLine x(this->nick);
		IRCD->SendSQLineDel(&x);
	}

	for (std::set<ChannelInfo *>::iterator it = this->channels->begin(), it_end = this->channels->end(); it != it_end;)
	{
		ChannelInfo *ci = *it++;
		this->UnAssign(NULL, ci);
	}

	BotListByNick->erase(this->nick);
	if (!this->uid.empty())
		BotListByUID->erase(this->uid);
}

void BotInfo::Serialize(Serialize::Data &data) const
{
	data.Store("nick", this->nick);
	data.Store("user", this->ident);
	data.Store("host", this->host);
	data.Store("realname", this->realname);
	data.Store("created", this->created);
	data.Store("oper_only", this->oper_only);

	Extensible::ExtensibleSerialize(this, this, data);
}

Serializable *BotInfo::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string nick, user, host, realname, flags;

	data["nick"] >> nick;
	data["user"] >> user;
	data["host"] >> host;
	data["realname"] >> realname;

	BotInfo *bi;
	if (obj)
		bi = anope_dynamic_static_cast<BotInfo *>(obj);
	else if (!(bi = BotInfo::Find(nick, true)))
		bi = new BotInfo(nick, user, host, realname);

	data["created"] >> bi->created;
	data["oper_only"] >> bi->oper_only;

	Extensible::ExtensibleUnserialize(bi, bi, data);

	return bi;
}

void BotInfo::GenerateUID()
{
	if (this->introduced)
		throw CoreException("Changing bot UID when it is introduced?");

	if (!this->uid.empty())
	{
		BotListByUID->erase(this->uid);
		UserListByUID.erase(this->uid);
	}

	this->uid = IRCD->UID_Retrieve();
	(*BotListByUID)[this->uid] = this;
	UserListByUID[this->uid] = this;
}

void BotInfo::OnKill()
{
	this->introduced = false;
	this->GenerateUID();
	IRCD->SendClientIntroduction(this);
	this->introduced = true;

	for (const auto &[_, chan] : this->chans)
		IRCD->SendJoin(this, chan->chan, &chan->status);
}

void BotInfo::SetNewNick(const Anope::string &newnick)
{
	UserListByNick.erase(this->nick);
	BotListByNick->erase(this->nick);

	this->nick = newnick;

	UserListByNick[this->nick] = this;
	(*BotListByNick)[this->nick] = this;
}

const std::set<ChannelInfo *> &BotInfo::GetChannels() const
{
	return this->channels;
}

void BotInfo::Assign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnPreBotAssign, MOD_RESULT, (u, ci, this));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->bi)
		ci->bi->UnAssign(u, ci);

	ci->bi = this;
	this->channels->insert(ci);

	FOREACH_MOD(OnBotAssign, (u, ci, this));
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnBotUnAssign, MOD_RESULT, (u, ci));
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
	this->channels->erase(ci);
}

unsigned BotInfo::GetChannelCount() const
{
	return this->channels->size();
}

void BotInfo::Join(Channel *c, ChannelStatus *status)
{
	if (c->FindUser(this) != NULL)
		return;

	c->JoinUser(this, status);
	if (IRCD)
		IRCD->SendJoin(this, c, status);

	FOREACH_MOD(OnJoinChannel, (this, c));
}

void BotInfo::Join(const Anope::string &chname, ChannelStatus *status)
{
	bool c;
	return this->Join(Channel::FindOrCreate(chname, c), status);
}

void BotInfo::Part(Channel *c, const Anope::string &reason)
{
	if (c->FindUser(this) == NULL)
		return;

	FOREACH_MOD(OnPrePartChannel, (this, c));

	IRCD->SendPart(this, c, reason);

	c->DeleteUser(this);

	FOREACH_MOD(OnPartChannel, (this, c, c->name, reason));
}

void BotInfo::OnMessage(User *u, const Anope::string &message, const Anope::map<Anope::string> &tags)
{
	if (this->commands.empty())
		return;

	Anope::string msgid;
	auto iter = tags.find("msgid");
	if (iter != tags.end())
		msgid = iter->second;

	CommandSource source(u->nick, u, u->Account(), u, this, msgid);
	Command::Run(source, message);
}

CommandInfo &BotInfo::SetCommand(const Anope::string &cname, const Anope::string &sname, const Anope::string &permission)
{
	CommandInfo ci;
	ci.name = sname;
	ci.permission = permission;
	this->commands[cname] = ci;
	return this->commands[cname];
}

CommandInfo *BotInfo::GetCommand(const Anope::string &cname)
{
	CommandInfo::map::iterator it = this->commands.find(cname);
	if (it != this->commands.end())
		return &it->second;
	return NULL;
}

Anope::string BotInfo::GetQueryCommand() const
{
	if (Config->ServiceAlias && !this->alias.empty())
		return Anope::printf("/%s", this->alias.c_str());
	return Anope::printf("/msg %s", this->nick.c_str());
}

BotInfo *BotInfo::Find(const Anope::string &nick, bool nick_only)
{
	if (!nick_only && IRCD != NULL && IRCD->RequiresID)
	{
		botinfo_map::iterator it = BotListByUID->find(nick);
		if (it != BotListByUID->end())
		{
			BotInfo *bi = it->second;
			bi->QueueUpdate();
			return bi;
		}

		if (IRCD->AmbiguousID)
			return NULL;
	}

	botinfo_map::iterator it = BotListByNick->find(nick);
	if (it != BotListByNick->end())
	{
		BotInfo *bi = it->second;
		bi->QueueUpdate();
		return bi;
	}

	return NULL;
}
