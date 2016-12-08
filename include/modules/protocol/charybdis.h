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

namespace charybdis
{

class Proto : public IRCDProto
{
	ServiceReference<IRCDProto> ratbox; // XXX

 public:
	Proto(Module *creator);

	void SendSVSKill(const MessageSource &source, User *targ, const Anope::string &reason) override { ratbox->SendSVSKill(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, Server *dest, const Anope::string &msg) override { ratbox->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, Server *dest, const Anope::string &msg) override { ratbox->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobops(const MessageSource &source, const Anope::string &buf) override { ratbox->SendGlobops(source, buf); }
	void SendSGLine(User *u, XLine *x) override { ratbox->SendSGLine(u, x); }
	void SendSGLineDel(XLine *x) override { ratbox->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { ratbox->SendAkill(u, x); }
	void SendAkillDel(XLine *x) override { ratbox->SendAkillDel(x); }
	void SendSQLine(User *u, XLine *x) override { ratbox->SendSQLine(u, x); }
	void SendSQLineDel(XLine *x) override { ratbox->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override { ratbox->SendJoin(user, c, status); }
	void SendServer(Server *server) override { ratbox->SendServer(server); }
	void SendChannel(Channel *c) override { ratbox->SendChannel(c); }
	void SendTopic(const MessageSource &source, Channel *c) override { ratbox->SendTopic(source, c); }
	bool IsIdentValid(const Anope::string &ident) override { return ratbox->IsIdentValid(ident); }
	void SendLogin(User *u, NickServ::Nick *na) override { ratbox->SendLogin(u, na); }
	void SendLogout(User *u) override { ratbox->SendLogout(u); }

	void SendConnect() override;

	void SendClientIntroduction(User *u) override;

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override;

	void SendSVSHold(const Anope::string &nick, time_t delay) override;

	void SendSVSHoldDel(const Anope::string &nick) override;

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) override;

	void SendVhostDel(User *u) override;

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override;

	void SendSASLMessage(const SASL::Message &message) override;

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override;
};

class Encap : public IRCDMessage
{
	ServiceReference<SASL::Service> sasl;

 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT);}

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class EUID : public IRCDMessage
{
 public:
	EUID(Module *creator) : IRCDMessage(creator, "EUID", 11) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { }

	// SERVER dev.anope.de 1 :charybdis test server
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Pass : public IRCDMessage
{
 public:
	Pass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace charybdis
