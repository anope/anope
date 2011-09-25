/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"
#include "oper.h"

Anope::insensitive_map<BotInfo *> BotListByNick;
Anope::map<BotInfo *> BotListByUID;

BotInfo::BotInfo(const Anope::string &nnick, const Anope::string &nuser, const Anope::string &nhost, const Anope::string &nreal, const Anope::string &bmodes) : User(nnick, nuser, nhost, ts6_uid_retrieve()), Flags<BotFlag, BI_END>(BotFlagString), botmodes(bmodes)
{
	this->realname = nreal;
	this->server = Me;

	this->chancount = 0;
	this->lastmsg = this->created = Anope::CurTime;
	this->introduced = false;

	BotListByNick[this->nick] = this;
	if (!this->uid.empty())
		BotListByUID[this->uid] = this;

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		Anope::string tmodes = !this->botmodes.empty() ? ("+" + this->botmodes) : (ircd ? ircd->pseudoclient_mode : "");
		if (!tmodes.empty())
			this->SetModesInternal(tmodes.c_str());

		ircdproto->SendClientIntroduction(this);
		this->introduced = true;
		XLine x(this->nick, "Reserved for services");
		ircdproto->SendSQLine(NULL, &x);
	}
}

BotInfo::~BotInfo()
{
	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		ircdproto->SendQuit(this, "");
		this->introduced = false;
		XLine x(this->nick);
		ircdproto->SendSQLineDel(&x);
	}

	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->bi == this)
			ci->bi = NULL;
	}

	BotListByNick.erase(this->nick);
	if (!this->uid.empty())
		BotListByUID.erase(this->uid);
}

SerializableBase::serialized_data BotInfo::serialize()
{
	SerializableBase::serialized_data data;

	data["nick"] << this->nick;
	data["user"] << this->ident;
	data["host"] << this->host;
	data["realname"] << this->realname;
	data["created"] << this->created;
	data["chancount"] << this->chancount;
	data["flags"] << this->ToString();

	return data;
}

void BotInfo::unserialize(SerializableBase::serialized_data &data)
{
	BotInfo *bi = findbot(data["nick"].astr());
	if (bi == NULL)
		bi = new BotInfo(data["nick"].astr(), data["user"].astr(), data["host"].astr(), data["realname"].astr());
	data["created"] >> bi->created;
	data["chancount"] >> bi->chancount;
	bi->FromString(data["flags"].astr());
}

void BotInfo::GenerateUID()
{
	if (!this->uid.empty())
		throw CoreException("Bot already has a uid?");
	this->uid = ts6_uid_retrieve();
	BotListByUID[this->uid] = this;
	UserListByUID[this->uid] = this;
}

void BotInfo::SetNewNick(const Anope::string &newnick)
{
	UserListByNick.erase(this->nick);
	BotListByNick.erase(this->nick);

	this->nick = newnick;

	UserListByNick[this->nick] = this;
	BotListByNick[this->nick] = this;
}

void BotInfo::RejoinAll()
{
	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

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
	
	++this->chancount;

	ci->bi = this;
	if (ci->c && ci->c->users.size() >= Config->BSMinUsers)
		this->Join(ci->c, &Config->BotModeList);
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

	--this->chancount;

	ci->bi = NULL;
}

void BotInfo::Join(Channel *c, ChannelStatus *status)
{
	if (c->FindUser(this) != NULL)
		return;

	if (Config && ircdproto && Config->BSSmartJoin)
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
			ircdproto->SendNotice(this, "@" + c->name, "%s invited %s into the channel.", this->nick.c_str(), this->nick.c_str());

		ModeManager::ProcessModes();
	}

	c->JoinUser(this);
	if (ircdproto)
		ircdproto->SendJoin(this, c, status);

	FOREACH_MOD(I_OnBotJoin, OnBotJoin(c, this));
}

void BotInfo::Join(const Anope::string &chname, ChannelStatus *status)
{
	Channel *c = findchan(chname);
	return this->Join(c ? c : new Channel(chname), status);
}

void BotInfo::Part(Channel *c, const Anope::string &reason)
{
	if (c->FindUser(this) == NULL)
		return;

	ircdproto->SendPart(this, c, "%s", !reason.empty() ? reason.c_str() : "");
	c->DeleteUser(this);
}

void BotInfo::OnMessage(User *u, const Anope::string &message)
{
	if (this->commands.empty())
		return;

	std::vector<Anope::string> params = BuildStringVector(message);
	bool has_help = this->commands.find("HELP") != this->commands.end();

	BotInfo::command_map::iterator it = this->commands.end();
	unsigned count = 0;
	for (unsigned max = params.size(); it == this->commands.end() && max > 0; --max)
	{
		Anope::string full_command;
		for (unsigned i = 0; i < max; ++i)
			full_command += " " + params[i];
		full_command.erase(full_command.begin());

		++count;
		it = this->commands.find(full_command);
	}

	if (it == this->commands.end())
	{
		if (has_help)
			u->SendMessage(this, _("Unknown command \002%s\002. \"%s%s HELP\" for help."), message.c_str(), Config->UseStrictPrivMsgString.c_str(), this->nick.c_str());
		else
			u->SendMessage(this, _("Unknown command \002%s\002."), message.c_str());
		return;
	}

	CommandInfo &info = it->second;
	service_reference<Command> c(info.name);
	if (!c)
	{
		if (has_help)
			u->SendMessage(this, _("Unknown command \002%s\002. \"%s%s HELP\" for help."), message.c_str(), Config->UseStrictPrivMsgString.c_str(), this->nick.c_str());
		else
			u->SendMessage(this, _("Unknown command \002%s\002."), message.c_str());
		Log(this) << "Command " << it->first << " exists on me, but its service " << info.name << " was not found!";
		return;
	}

	// Command requires registered users only
	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !u->IsIdentified())
	{
		u->SendMessage(this, NICK_IDENTIFY_REQUIRED);
		Log(LOG_NORMAL, "access_denied", this) << "Access denied for unregistered user " << u->GetMask() << " with command " << c->name;
		return;
	}

	for (unsigned i = 0, j = params.size() - (count - 1); i < j; ++i)
		params.erase(params.begin());

	while (c->MaxParams > 0 && params.size() > c->MaxParams)
	{
		params[c->MaxParams - 1] += " " + params[c->MaxParams];
		params.erase(params.begin() + c->MaxParams);
	}

	CommandSource source;
	source.u = u;
	source.c = NULL;
	source.owner = this;
	source.service = this;
	source.command = it->first;
	source.permission = info.permission;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(source, c, params));
	if (MOD_RESULT == EVENT_STOP)
		return;


	if (params.size() < c->MinParams)
	{
		c->OnSyntaxError(source, !params.empty() ? params[params.size() - 1] : "");
		return;
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!info.permission.empty() && !u->HasCommand(info.permission))
	{
		u->SendMessage(this, ACCESS_DENIED);
		Log(LOG_NORMAL, "access_denied", this) << "Access denied for user " << u->GetMask() << " with command " << c->name;
		return;
	}

	dynamic_reference<User> user_reference(u);
	c->Execute(source, params);
	if (user_reference)
	{
		FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, c, params));
	}
}

/** Link a command name to a command in services
 * @param cname The command name
 * @param sname The service name
 * @param permission Permission required to execute the command, if any
 */
void BotInfo::SetCommand(const Anope::string &cname, const Anope::string &sname, const Anope::string &permission)
{
	CommandInfo ci;
	ci.name = sname;
	ci.permission = permission;
	this->commands[cname] = ci;
}

/** Get command info for a command
 * @param cname The command name
 * @return A struct containing service name and permission
 */
CommandInfo *BotInfo::GetCommand(const Anope::string &cname)
{
	command_map::iterator it = this->commands.find(cname);
	if (it != this->commands.end())
		return &it->second;
	return NULL;
}

