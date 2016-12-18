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
#include "modules/protocol/ratbox.h"

namespace charybdis
{

namespace senders
{

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

class SASLMechanisms : public messages::SASLMechanisms
{
 public:
	using messages::SASLMechanisms::SASLMechanisms;

	void Send(const std::vector<Anope::string> &mechanisms) override;
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

class SVSLogin : public messages::SVSLogin
{
 public:
	using messages::SVSLogin::SVSLogin;

	void Send(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override;
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
	ratbox::Proto ratbox;

 public:
	Proto(Module *creator);

	bool IsIdentValid(const Anope::string &ident) override { return ratbox.IsIdentValid(ident); }

	void Handshake() override;
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
