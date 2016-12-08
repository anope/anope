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

namespace bahamut
{

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator);

	void SendMode(const MessageSource &source, Channel *dest, const Anope::string &buf) override;

	void SendMode(const MessageSource &source, User *u, const Anope::string &buf) override;

	void SendGlobalNotice(ServiceBot *bi, Server *dest, const Anope::string &msg) override;

	void SendGlobalPrivmsg(ServiceBot *bi, Server *dest, const Anope::string &msg) override;

	void SendSVSHold(const Anope::string &nick, time_t time) override;

	void SendSVSHoldDel(const Anope::string &nick) override;

	void SendSQLine(User *, XLine *x) override;

	void SendSGLineDel(XLine *x) override;

	void SendSZLineDel(XLine *x) override;

	void SendSZLine(User *, XLine *x) override;

	void SendSVSNOOP(Server *server, bool set) override;

	void SendSGLine(User *, XLine *x) override;

	void SendAkillDel(XLine *x) override;

	void SendTopic(const MessageSource &source, Channel *c) override;

	void SendSQLineDel(XLine *x) override;

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override;

	void SendAkill(User *u, XLine *x) override;

	void SendSVSKill(const MessageSource &source, User *user, const Anope::string &buf) override;

	void SendBOB() override;

	void SendEOB() override;

	void SendClientIntroduction(User *u) override;

	void SendServer(Server *server) override;

	void SendConnect() override;

	void SendChannel(Channel *c) override;

	void SendLogin(User *u, NickServ::Nick *) override;

	void SendLogout(User *u) override;
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
