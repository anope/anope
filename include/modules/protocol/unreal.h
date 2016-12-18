/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

namespace unreal
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

class Kill : public messages::Kill
{
 public:
	using messages::Kill::Kill;

	void Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason) override;

	void Send(const MessageSource &source, User *user, const Anope::string &reason) override;
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

class SASL : public messages::SASL
{
 public:
	using messages::SASL::SASL;

	 void Send(const ::SASL::Message &) override;
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

class SVSLogin : public messages::SVSLogin
{
 public:
	using messages::SVSLogin::SVSLogin;

	void Send(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override;
};

class SVSPart : public messages::SVSPart
{
 public:
	using messages::SVSPart::SVSPart;

	void Send(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &reason) override;
};

class SWhois : public messages::SWhois
{
 public:
	using messages::SWhois::SWhois;

	void Send(const MessageSource &, User *user, const Anope::string &) override;
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

} // namespace senders

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator);

 private:

	void Handshake() override;

	void SendEOB() override;

	bool IsNickValid(const Anope::string &nick) override;

	bool IsChannelValid(const Anope::string &chan) override;

	bool IsExtbanValid(const Anope::string &mask) override;

	bool IsIdentValid(const Anope::string &ident) override;
};

class ChgHost : public IRCDMessage
{
 public:
	ChgHost(Module *creator) : IRCDMessage(creator, "CHGHOST", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChgIdent : public IRCDMessage
{
 public:
	ChgIdent(Module *creator) : IRCDMessage(creator, "CHGIDENT", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChgName : public IRCDMessage
{
 public:
	ChgName(Module *creator) : IRCDMessage(creator, "CHGNAME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class MD : public IRCDMessage
{
 public:
	MD(Module *creator) : IRCDMessage(creator, "MD", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator, const Anope::string &mname) : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class NetInfo : public IRCDMessage
{
 public:
	NetInfo(Module *creator) : IRCDMessage(creator, "NETINFO", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Pong : public IRCDMessage
{
 public:
	Pong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Protoctl : public rfc1459::Capab
{
 public:
	Protoctl(Module *creator) : rfc1459::Capab(creator, "PROTOCTL") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SASL : public IRCDMessage
{
	ServiceReference<::SASL::Service> sasl;

 public:
	SASL(Module *creator) : IRCDMessage(creator, "SASL", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SDesc : public IRCDMessage
{
 public:
	SDesc(Module *creator) : IRCDMessage(creator, "SDESC", 1) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SetHost : public IRCDMessage
{
 public:
	SetHost(Module *creator) : IRCDMessage(creator, "SETHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SetIdent : public IRCDMessage
{
 public:
	SetIdent(Module *creator) : IRCDMessage(creator, "SETIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

struct SetName : IRCDMessage
{
 public:
	SetName(Module *creator) : IRCDMessage(creator, "SETNAME", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

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
	SID(Module *creator) : IRCDMessage(creator, "SID", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SJoin : public IRCDMessage
{
 public:
	SJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Topic : public IRCDMessage
{
 public:
	Topic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 12) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Umode2 : public IRCDMessage
{
 public:
	Umode2(Module *creator) : IRCDMessage(creator, "UMODE2", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace unreal
