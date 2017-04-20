/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

namespace bahamut
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

class Kill : public messages::Kill
{
 public:
	using messages::Kill::Kill;

	void Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason) override;

	void Send(const MessageSource &source, User *user, const Anope::string &reason) override;
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

class NOOP : public messages::NOOP
{
 public:
	static constexpr const char *NAME = "noop";

	using messages::NOOP::NOOP;

	void Send(Server *s, bool mode) override;
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

class SVSNick : public messages::SVSNick
{
 public:
	using messages::SVSNick::SVSNick;

	void Send(User *u, const Anope::string &newnick, time_t ts) override;
};

class Topic : public messages::Topic
{
 public:
	using messages::Topic::Topic;

	void Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter) override;
};

class Wallops : public messages::Wallops
{
 public:
	using messages::Wallops::Wallops;

	void Send(const MessageSource &source, const Anope::string &msg) override;
};

} // namespace senders

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator);

	void SendBOB() override;

	void SendEOB() override;

	void Handshake() override;
};

class Burst : public IRCDMessage
{
 public:
	Burst(Module *creator) : IRCDMessage(creator, "BURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator, const Anope::string &sname) : IRCDMessage(creator, sname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SJoin : public IRCDMessage
{
 public:
	SJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Topic : public IRCDMessage
{
 public:
	Topic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace bahamut
