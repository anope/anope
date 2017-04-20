/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2017 Anope Team <team@anope.org>
 * Copyright (C) 2011-2012, 2014 Alexander Barton <alex@barton.de>
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

namespace ngircd
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

class NickIntroduction : public messages::NickIntroduction
{
 public:
	using messages::NickIntroduction::NickIntroduction;

	void Send(User *user) override;
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

class SVSNick : public messages::SVSNick
{
 public:
	using messages::SVSNick::SVSNick;

	void Send(User *u, const Anope::string &newnick, time_t ts) override;
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

}

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator);

	void Handshake() override;

	Anope::string Format(IRCMessage &message) override;
};

class Numeric005 : public IRCDMessage
{
 public:
	Numeric005(Module *creator) : IRCDMessage(creator, "005", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChanInfo : public IRCDMessage
{
 public:
	ChanInfo(Module *creator) : IRCDMessage(creator, "CHANINFO", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * CHANINFO is used by servers to inform each other about a channel: its
	 * modes, channel key, user limits and its topic. The parameter combination
	 * <key> and <limit> is optional, as well as the <topic> parameter, so that
	 * there are three possible forms of this command:
	 *
	 * CHANINFO <chan> +<modes>
	 * CHANINFO <chan> +<modes> :<topic>
	 * CHANINFO <chan> +<modes> <key> <limit> :<topic>
	 *
	 * The parameter <key> must be ignored if a channel has no key (the parameter
	 * <modes> doesn't list the "k" channel mode). In this case <key> should
	 * contain "*" because the parameter <key> is required by the CHANINFO syntax
	 * and therefore can't be omitted. The parameter <limit> must be ignored when
	 * a channel has no user limit (the parameter <modes> doesn't list the "l"
	 * channel mode). In this case <limit> should be "0".
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params);
};

class Join : public rfc1459::Join
{
 public:
	Join(Module *creator) : rfc1459::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Metadata : public IRCDMessage
{
 public:
	Metadata(Module *creator) : IRCDMessage(creator, "METADATA", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

struct Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class NJoin : public IRCDMessage
{
 public:
	NJoin(Module *creator) : IRCDMessage(creator, "NJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); };

	/*
	 * RFC 2813, 4.2.2: Njoin Message:
	 * The NJOIN message is used between servers only.
	 * It is used when two servers connect to each other to exchange
	 * the list of channel members for each channel.
	 *
	 * Even though the same function can be performed by using a succession
	 * of JOIN, this message SHOULD be used instead as it is more efficient.
	 *
	 * Received: :dev.anope.de NJOIN #test :DukeP2,@DukeP,%test,+test2
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Pong : public IRCDMessage
{
 public:
	Pong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace ngircd
