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

#include "modules/protocol/ts6.h"

namespace inspircd20
{

class Proto;

namespace senders
{

class Akill : public messages::Akill
{
	Proto *proto = nullptr;

 public:
	Akill(Module *creator, Proto *p) : messages::Akill(creator),
		proto(p)
	{
	}

	void Send(User *, XLine *) override;
};

class AkillDel : public messages::AkillDel
{
	Proto *proto = nullptr;

 public:
	AkillDel(Module *creator, Proto *p) : messages::AkillDel(creator),
		proto(p)
	{
	}

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

class ModeChannel : public messages::ModeChannel
{
 public:
	using messages::ModeChannel::ModeChannel;

	void Send(const MessageSource &source, Channel *channel, const Anope::string &modes) override;
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

class SASL : public messages::SASL
{
 public:
	using messages::SASL::SASL;

	 void Send(const ::SASL::Message &) override;
};

class SASLMechanisms : public messages::SASLMechanisms
{
 public:
	using messages::SASLMechanisms::SASLMechanisms;

	void Send(const std::vector<Anope::string> &mechanisms) override;
};

class SQLine : public messages::SQLine
{
	Proto *proto = nullptr;

 public:
	SQLine(Module *creator, Proto *p) : messages::SQLine(creator),
		proto(p)
	{
	}

	void Send(User *, XLine *) override;
};

class SQLineDel : public messages::SQLineDel
{
	Proto *proto = nullptr;

 public:
	SQLineDel(Module *creator, Proto *p) : messages::SQLineDel(creator),
		proto(p)
	{
	}

	void Send(XLine *) override;
};

class SQuit : public messages::SQuit
{
 public:
	using messages::SQuit::SQuit;

	void Send(Server *s, const Anope::string &message) override;
};

class SZLine : public messages::SZLine
{
	Proto *proto = nullptr;

 public:
	SZLine(Module *creator, Proto *p) : messages::SZLine(creator),
		proto(p)
	{
	}

	void Send(User *, XLine *) override;
};

class SZLineDel : public messages::SZLineDel
{
	Proto *proto = nullptr;

 public:
	SZLineDel(Module *creator, Proto *p) : messages::SZLineDel(creator),
		proto(p)
	{
	}

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
	Proto *proto = nullptr;

 public:
	VhostDel(Module *creator, Proto *p) : messages::VhostDel(creator),
		proto(p)
	{
	}

	void Send(User *u) override;
};

class VhostSet : public messages::VhostSet
{
	Proto *proto = nullptr;

 public:
	VhostSet(Module *creator, Proto *p) : messages::VhostSet(creator),
		proto(p)
	{
	}

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
 public:
	Proto(Module *creator);

	void SendChgIdentInternal(const Anope::string &nick, const Anope::string &vIdent);

	void SendChgHostInternal(const Anope::string &nick, const Anope::string &vhost);

	void SendAddLine(const Anope::string &xtype, const Anope::string &mask, time_t duration, const Anope::string &addedby, const Anope::string &reason);

	void SendDelLine(const Anope::string &xtype, const Anope::string &mask);

	void Handshake() override;

	void SendNumeric(int numeric, User *dest, IRCMessage &message) override;

	void SendBOB() override;

	void SendEOB() override;

	bool IsExtbanValid(const Anope::string &mask) override;

	bool IsIdentValid(const Anope::string &ident) override;
};

class Capab : public rfc1459::Capab
{
 public:
	Capab(Module *creator) : rfc1459::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
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

class Encap : public IRCDMessage
{
 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Endburst : public IRCDMessage
{
 public:
	Endburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FHost : public IRCDMessage
{
 public:
	FHost(Module *creator) : IRCDMessage(creator, "FHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FIdent : public IRCDMessage
{
 public:
	FIdent(Module *creator) : IRCDMessage(creator, "FIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FJoin : public IRCDMessage
{
 public:
	FJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FMode : public IRCDMessage
{
 public:
	FMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FTopic : public IRCDMessage
{
 public:
	FTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Idle : public IRCDMessage
{
 public:
	Idle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Metadata : public IRCDMessage
{
 public:
	const bool &do_topiclock, &do_mlock;

	Metadata(Module *creator, const bool &handle_topiclock, const bool &handle_mlock)
		: IRCDMessage(creator, "METADATA", 3)
		, do_topiclock(handle_topiclock)
		, do_mlock(handle_mlock)
	{
		SetFlag(IRCDMESSAGE_REQUIRE_SERVER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class OperType : public IRCDMessage
{
 public:
	OperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class RSQuit : public IRCDMessage
{
 public:
	RSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Save : public IRCDMessage
{
	time_t last_collide = 0;

 public:
	Save(Module *creator) : IRCDMessage(creator, "SAVE", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 5) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SQuit : public rfc1459::SQuit
{
 public:
	using rfc1459::SQuit::SQuit;

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Time : public IRCDMessage
{
 public:
	Time(Module *creator) : IRCDMessage(creator, "TIME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
	ServiceReference<SASL::Service> sasl;

 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace inspircd20

