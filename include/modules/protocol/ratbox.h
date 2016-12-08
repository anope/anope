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

namespace ratbox
{

class Proto : public IRCDProto
{
	ServiceReference<IRCDProto> hybrid; // XXX

	ServiceBot *FindIntroduced();

 public:
	Proto(Module *creator);

	void SendSVSKill(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKill(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSGLine(User *u, XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendSGLineDel(XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendAkillDel(XLine *x) override { hybrid->SendAkillDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override { hybrid->SendJoin(user, c, status); }
	void SendServer(Server *server) override { hybrid->SendServer(server); }
	void SendMode(const MessageSource &source, User *u, const Anope::string &buf) override { hybrid->SendMode(source, u, buf); }
	void SendChannel(Channel *c) override { hybrid->SendChannel(c); }
	bool IsIdentValid(const Anope::string &ident) override { return hybrid->IsIdentValid(ident); }

	void SendGlobops(const MessageSource &source, const Anope::string &buf) override;

	void SendConnect() override;

	void SendClientIntroduction(User *u) override;

	void SendLogin(User *u, NickServ::Nick *na) override;

	void SendLogout(User *u) override;

	void SendTopic(const MessageSource &source, Channel *c) override;

	void SendSQLine(User *, XLine *x) override;

	void SendSQLineDel(XLine *x) override;
};

class Encap : public IRCDMessage
{
 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 3) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	// Debug: Received: :00BAAAAAB ENCAP * LOGIN Adam
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Join : public rfc1459::Join
{
 public:
	Join(Module *creator) : rfc1459::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class TB : public IRCDMessage
{
 public:
	TB(Module *creator) : IRCDMessage(creator, "TB", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 9) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace ratbox
