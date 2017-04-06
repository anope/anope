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

namespace plexus
{

namespace senders
{

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

class SASL : public messages::SASL
{
 public:
	using messages::SASL::SASL;

	 void Send(const ::SASL::Message &) override;
};

class Topic : public messages::Topic
{
 public:
	using messages::Topic::Topic;

	void Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter) override;
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

class Proto : public ts6::Proto
{
 public:
	Proto(Module *creator);

	void Handshake() override;
};

class Encap : public IRCDMessage
{
	ServiceReference<SASL::Service> sasl;

 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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

