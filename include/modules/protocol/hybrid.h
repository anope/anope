/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "modules/protocol/rfc1459.h"
#include "modules/protocol/ts6.h"

namespace hybrid
{

namespace senders
{

class Akill : public messages::Akill
{
 public:
	using messages::Akill::Akill;

	void Send(User *, XLine *) override;
};

class AkillDel : public messages::AkillDel
{
 public:
	using messages::AkillDel::AkillDel;

	void Send(XLine *) override;
};

class MessageChannel : public messages::MessageChannel
{
 public:
	using messages::MessageChannel::MessageChannel;

	void Send(Channel *) override;
};

class GlobalNotice : public messages::GlobalNotice
{
 public:
	using messages::GlobalNotice::GlobalNotice;

	void Send(const MessageSource &, Server *dest, const Anope::string &msg) override;
};

class GlobalPrivmsg : public messages::GlobalPrivmsg
{
 public:
	using messages::GlobalPrivmsg::GlobalPrivmsg;

	void Send(const MessageSource &, Server *dest, const Anope::string &msg) override;
};

class Invite : public messages::Invite
{
 public:
	using messages::Invite::Invite;

	void Send(const MessageSource &source, Channel *chan, User *user) override;
};

class Join : public messages::Join
{
 public:
	using messages::Join::Join;

	void Send(User *u, Channel *c, const ChannelStatus *status) override;
};

class Login : public messages::Login
{
 public:
	using messages::Login::Login;

	void Send(User *u, NickServ::Nick *na) override;
};

class Logout : public messages::Logout
{
 public:
	using messages::Logout::Logout;

	void Send(User *u) override;
};

class ModeUser : public messages::ModeUser
{
 public:
	using messages::ModeUser::ModeUser;

	void Send(const MessageSource &source, User *user, const Anope::string &modes) override;
};

class NickIntroduction : public messages::NickIntroduction
{
 public:
	using messages::NickIntroduction::NickIntroduction;

	void Send(User *user) override;
};

class MessageServer : public messages::MessageServer
{
 public:
	using messages::MessageServer::MessageServer;

	void Send(Server *server) override;
};

class SGLine : public messages::SGLine
{
 public:
	using messages::SGLine::SGLine;

	void Send(User *, XLine *) override;
};

class SGLineDel : public messages::SGLineDel
{
 public:
	using messages::SGLineDel::SGLineDel;

	void Send(XLine *) override;
};

class SQLine : public messages::SQLine
{
 public:
	using messages::SQLine::SQLine;

	void Send(User *, XLine *) override;
};

class SQLineDel : public messages::SQLineDel
{
 public:
	using messages::SQLineDel::SQLineDel;

	void Send(XLine *) override;
};

class SZLine : public messages::SZLine
{
 public:
	using messages::SZLine::SZLine;

	void Send(User *, XLine *) override;
};

class SZLineDel : public messages::SZLineDel
{
 public:
	using messages::SZLineDel::SZLineDel;

	void Send(XLine *) override;
};

class SVSHold : public messages::SVSHold
{
 public:
	using messages::SVSHold::SVSHold;

	void Send(const Anope::string &, time_t) override;
};

class SVSHoldDel : public messages::SVSHoldDel
{
 public:
	using messages::SVSHoldDel::SVSHoldDel;

	void Send(const Anope::string &) override;
};

class SVSJoin : public messages::SVSJoin
{
 public:
	using messages::SVSJoin::SVSJoin;

	void Send(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &key) override;
};

class SVSNick : public messages::SVSNick
{
 public:
	using messages::SVSNick::SVSNick;

	void Send(User *u, const Anope::string &newnick, time_t ts) override;
};

class SVSPart : public messages::SVSPart
{
 public:
	using messages::SVSPart::SVSPart;

	void Send(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &reason) override;
};

class Topic : public messages::Topic
{
 public:
	using messages::Topic::Topic;

	void Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter) override;
};

class VhostDel : public messages::VhostDel
{
 public:
	using messages::VhostDel::VhostDel;

	void Send(User *u) override;
};

class VhostSet : public messages::VhostSet
{
 public:
	using messages::VhostSet::VhostSet;

	void Send(User *u, const Anope::string &vident, const Anope::string &vhost) override;
};

class Wallops : public messages::Wallops
{
 public:
	using messages::Wallops::Wallops;

	void Send(const MessageSource &source, const Anope::string &msg) override;
};

} // namespace senders

class Proto : public ts6::Proto
{
	ServiceBot *FindIntroduced();

  public:
	Proto(Module *creator);

	void Handshake() override;

	void SendEOB() override;

	bool IsIdentValid(const Anope::string &ident) override;
};

class BMask : public IRCDMessage
{
 public:
	BMask(Module *creator) : IRCDMessage(creator, "BMASK", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class EOB : public IRCDMessage
{
 public:
	EOB(Module *craetor) : IRCDMessage(craetor, "EOB", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Join : public rfc1459::Join
{
 public:
	Join(Module *creator) : rfc1459::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Pong : public IRCDMessage
{
 public:
	Pong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SID : public IRCDMessage
{
 public:
	SID(Module *creator) : IRCDMessage(creator, "SID", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SJoin : public IRCDMessage
{
 public:
	SJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SVSMode : public IRCDMessage
{
 public:
	SVSMode(Module *creator) : IRCDMessage(creator, "SVSMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class TBurst : public IRCDMessage
{
 public:
	TBurst(Module *creator) : IRCDMessage(creator, "TBURST", 5) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class TMode : public IRCDMessage
{
 public:
	TMode(Module *creator) : IRCDMessage(creator, "TMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 10) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class CertFP : public IRCDMessage
{
 public:
	CertFP(Module *creator) : IRCDMessage(creator, "CERTFP", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace hybrid
