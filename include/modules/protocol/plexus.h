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

namespace plexus
{

class Proto : public IRCDProto
{
	ServiceReference<IRCDProto> hybrid; // XXX use moddeps + inheritance here

 public:
	Proto(Module *creator);

	void SendSVSKill(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKill(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, XLine *x) override { hybrid->SendSQLine(u, x); }
	void SendSQLineDel(XLine *x) override { hybrid->SendSQLineDel(x); }
	void SendSGLineDel(XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendSGLine(User *u, XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendAkillDel(XLine *x) override { hybrid->SendAkillDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendServer(Server *server) override { hybrid->SendServer(server); }
	void SendChannel(Channel *c) override { hybrid->SendChannel(c); }
	void SendSVSHold(const Anope::string &nick, time_t t) override { hybrid->SendSVSHold(nick, t); }
	void SendSVSHoldDel(const Anope::string &nick) override { hybrid->SendSVSHoldDel(nick); }

	void SendGlobops(const MessageSource &source, const Anope::string &buf) override;

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override;

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override;

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) override;

	void SendVhostDel(User *u) override;

	void SendConnect() override;

	void SendClientIntroduction(User *u) override;

	void SendMode(const MessageSource &source, User *u, const Anope::string &buf) override;

	void SendLogin(User *u, NickServ::Nick *na) override;

	void SendLogout(User *u) override;

	void SendTopic(const MessageSource &source, Channel *c) override;

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override;

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override;
};

class Encap : public IRCDMessage
{
 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 11) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace plexus

