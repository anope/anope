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

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator);

 private:
	void SendSVSNOOP(Server *server, bool set) override;

	void SendAkillDel(XLine *x) override;

	void SendTopic(const MessageSource &source, Channel *c) override;

	void SendGlobalNotice(ServiceBot *bi, Server *dest, const Anope::string &msg) override;

	void SendGlobalPrivmsg(ServiceBot *bi, Server *dest, const Anope::string &msg) override;

	void SendVhostDel(User *u) override;

	void SendAkill(User *u, XLine *x) override;

	void SendSVSKill(const MessageSource &source, User *user, const Anope::string &buf) override;

	void SendMode(const MessageSource &source, User *u, const Anope::string &buf) override;

	void SendClientIntroduction(User *u) override;

	void SendServer(Server *server) override;

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override;

	void SendSQLineDel(XLine *x) override;

	void SendSQLine(User *, XLine *x) override;

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) override;

	void SendConnect() override;

	void SendSVSHold(const Anope::string &nick, time_t t) override;

	void SendSVSHoldDel(const Anope::string &nick) override;

	void SendSGLineDel(XLine *x) override;

	void SendSZLineDel(XLine *x) override;

	void SendSZLine(User *, XLine *x) override;

	void SendSGLine(User *, XLine *x) override;

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override;

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override;

	void SendSWhois(const MessageSource &source, const Anope::string &who, const Anope::string &mask) override;

	void SendEOB() override;

	bool IsNickValid(const Anope::string &nick) override;

	bool IsChannelValid(const Anope::string &chan) override;

	bool IsExtbanValid(const Anope::string &mask) override;

	void SendLogin(User *u, NickServ::Nick *na) override;

	void SendLogout(User *u) override;

	void SendChannel(Channel *c) override;

	void SendSASLMessage(const ::SASL::Message &message) override;

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override;

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
