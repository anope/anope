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

template<typename T>
class MessageSender : public Service
{
 public:
	static constexpr const char *NAME = "messagesender";

	MessageSender(Module *owner) : Service(owner, NAME, T::NAME) { }
};

namespace messages
{

class Akill : public MessageSender<Akill>
{
 public:
	static constexpr const char *NAME = "akill";

	using MessageSender::MessageSender;

	/** Sets an akill. This is a recursive function that can be called multiple times
	 * for the same xline, but for different users, if the xline is not one that can be
	 * enforced by the IRCd, such as a nick/user/host/realname combination ban.
	 * @param u The user affected by the akill, if known
	 * @param x The akill
	 */
	virtual void Send(User *, XLine *) anope_abstract;
};

class AkillDel : public MessageSender<AkillDel>
{
 public:
	static constexpr const char *NAME = "akilldel";

	using MessageSender::MessageSender;

	virtual void Send(XLine *) anope_abstract;
};

class MessageChannel : public MessageSender<MessageChannel>
{
 public:
	static constexpr const char *NAME = "channel";

	using MessageSender::MessageSender;

	/** Send a channel creation message to the uplink.
	 * On most IRCds this is a SJOIN with no nicks
	 */
	virtual void Send(Channel *) anope_abstract;
};

class GlobalNotice : public MessageSender<GlobalNotice>
{
 public:
	static constexpr const char *NAME = "globalnotice";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &, Server *dest, const Anope::string &msg) anope_abstract;
};

class GlobalPrivmsg : public MessageSender<GlobalPrivmsg>
{
 public:
	static constexpr const char *NAME = "globalprivmsg";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &, Server *dest, const Anope::string &msg) anope_abstract;
};

class Invite : public MessageSender<Invite>
{
 public:
	static constexpr const char *NAME = "invite";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &source, Channel *c, User *u) anope_abstract;
};

class Join : public MessageSender<Join>
{
 public:
	static constexpr const char *NAME = "join";

	using MessageSender::MessageSender;

	/** Joins one of our users to a channel.
	 * @param u The user to join
	 * @param c The channel to join the user to
	 * @param status The status to set on the user after joining. This may or may not already internally
	 * be set on the user. This may include the modes in the join, but will usually place them on the mode
	 * stacker to be set "soon".
	 */
	virtual void Send(User *u, Channel *c, const ChannelStatus *status) anope_abstract;
};

class Kick : public MessageSender<Kick>
{
 public:
	static constexpr const char *NAME = "kick";

	using MessageSender::MessageSender;

	/** Kick a user from a channel
	 * @param source Who is doing the kick
	 * @param channel Channel
	 * @param user Target of the kick
	 * @param reason Kick reason
	 */
	virtual void Send(const MessageSource &source, Channel *channel, User *user, const Anope::string &reason) anope_abstract;
};

class Kill : public MessageSender<Kill>
{
 public:
	static constexpr const char *NAME = "svskill";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason) anope_abstract;

	/** Kills a user
	 * @param source Who is doing the kill
	 * @param user The target to be killed
	 * @param reason Kill reason
	 */
	virtual void Send(const MessageSource &source, User *user, const Anope::string &reason) anope_abstract;
};

class Login : public MessageSender<Login>
{
 public:
	static constexpr const char *NAME = "login";

	using MessageSender::MessageSender;

	virtual void Send(User *u, NickServ::Nick *na) anope_abstract;
};

class Logout : public MessageSender<Logout>
{
 public:
	static constexpr const char *NAME = "logout";

	using MessageSender::MessageSender;

	virtual void Send(User *u) anope_abstract;
};

class ModeChannel : public MessageSender<ModeChannel>
{
 public:
	static constexpr const char *NAME = "modechannel";

	using MessageSender::MessageSender;

	/** Changes the mode on a channel
	 * @param source Who is changing the mode
	 * @param channel Channel
	 * @param modes Modes eg +nt
	 */
	virtual void Send(const MessageSource &source, Channel *channel, const Anope::string &modes) anope_abstract;
};

class ModeUser : public MessageSender<ModeUser>
{
 public:
	static constexpr const char *NAME = "modeuser";

	using MessageSender::MessageSender;

	/** Changes the mode on a user
	 * @param source Who is changing the mode
	 * @param user user
	 * @param modes Modes eg +i
	 */
	virtual void Send(const MessageSource &source, User *user, const Anope::string &modes) anope_abstract;
};

class NickChange : public MessageSender<NickChange>
{
 public:
	static constexpr const char *NAME = "nickchange";

	using MessageSender::MessageSender;

	/** Sends a nick change of one of our clients.
	 */
	virtual void Send(User *u, const Anope::string &newnick, time_t ts) anope_abstract;
};

class NickIntroduction : public MessageSender<NickIntroduction>
{
 public:
	static constexpr const char *NAME = "nickintroduction";

	using MessageSender::MessageSender;

	/** Introduces a client to the rest of the network
	 * @param user The client to introduce
	 */
	virtual void Send(User *user) anope_abstract;
};

class NOOP : public MessageSender<NOOP>
{
 public:
	static constexpr const char *NAME = "noop";

	using MessageSender::MessageSender;

	/** Sets the server in NOOP mode. If NOOP mode is enabled, no users
	 * will be able to oper on the server.
	 * @param s The server
	 * @param mode Whether to turn NOOP on or off
	 */
	virtual void Send(Server *s, bool mode) anope_abstract;
};

class Notice : public MessageSender<Notice>
{
 public:
	static constexpr const char *NAME = "notice";

	using MessageSender::MessageSender;

	/** Send a notice to a target
	 * @param source Source of the notice
	 * @param dest Destination user/channel
	 * @param msg Message to send
	 */
	virtual void Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg) anope_abstract;
};

class Part : public MessageSender<Part>
{
 public:
	static constexpr const char *NAME = "part";

	using MessageSender::MessageSender;

	virtual void Send(User *, Channel *chan, const Anope::string &reason) anope_abstract;
};

class Ping : public MessageSender<Ping>
{
 public:
	static constexpr const char *NAME = "ping";

	using MessageSender::MessageSender;

	virtual void Send(const Anope::string &servname, const Anope::string &who) anope_abstract;
};

class Pong : public MessageSender<Pong>
{
 public:
	static constexpr const char *NAME = "pong";

	using MessageSender::MessageSender;

	/**
	 * Send a PONG reply to a received PING.
	 * servname should be left empty to send a one param reply.
	 * @param servname Daemon or client that is responding to the PING.
	 * @param who Origin of the PING and destination of the PONG message.
	 **/
	virtual void Send(const Anope::string &servname, const Anope::string &who) anope_abstract;
};

class Privmsg : public MessageSender<Privmsg>
{
 public:
	static constexpr const char *NAME = "privmsg";

	using MessageSender::MessageSender;

	/** Send a privmsg to a target
	 * @param source Source of the privmsg
	 * @param dest Destination user/channel
	 * @param msg Message to send
	 */
	virtual void Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg) anope_abstract;
};

class Quit : public MessageSender<Quit>
{
 public:
	static constexpr const char *NAME = "quit";

	using MessageSender::MessageSender;

	virtual void Send(User *user, const Anope::string &reason) anope_abstract;
};

class MessageServer : public MessageSender<MessageServer>
{
 public:
	static constexpr const char *NAME = "server";

	using MessageSender::MessageSender;

	/** Introduces a server to the uplink
	 */
	virtual void Send(Server *s) anope_abstract;
};

class SASL : public MessageSender<SASL>
{
 public:
	static constexpr const char *NAME = "sasl";

	using MessageSender::MessageSender;

	virtual void Send(const ::SASL::Message &) anope_abstract;
};

class SASLMechanisms : public MessageSender<SASLMechanisms>
{
 public:
	static constexpr const char *NAME = "saslmechanisms";

	using MessageSender::MessageSender;

	virtual void Send(const std::vector<Anope::string> &mechanisms) anope_abstract;
};

// realname ban
class SGLine : public MessageSender<SGLine>
{
 public:
	static constexpr const char *NAME = "sgline";

	using MessageSender::MessageSender;

	virtual void Send(User *, XLine *) anope_abstract;
};

class SGLineDel : public MessageSender<SGLineDel>
{
 public:
	static constexpr const char *NAME = "sglinedel";

	using MessageSender::MessageSender;

	virtual void Send(XLine *) anope_abstract;
};

// Nick ban (and sometimes channel)
class SQLine : public MessageSender<SQLine>
{
 public:
	static constexpr const char *NAME = "sqline";

	using MessageSender::MessageSender;

	virtual void Send(User *, XLine *) anope_abstract;
};

class SQLineDel : public MessageSender<SQLineDel>
{
 public:
	static constexpr const char *NAME = "sqlinedel";

	using MessageSender::MessageSender;

	virtual void Send(XLine *) anope_abstract;
};

// IP ban
class SZLine : public MessageSender<SZLine>
{
 public:
	static constexpr const char *NAME = "szline";

	using MessageSender::MessageSender;

	virtual void Send(User *, XLine *) anope_abstract;
};

class SZLineDel : public MessageSender<SZLineDel>
{
 public:
	static constexpr const char *NAME = "szlinedel";

	using MessageSender::MessageSender;

	virtual void Send(XLine *) anope_abstract;
};

class SQuit : public MessageSender<SQuit>
{
 public:
	static constexpr const char *NAME = "squit";

	using MessageSender::MessageSender;

	virtual void Send(Server *s, const Anope::string &message) anope_abstract;
};

class SVSHold : public MessageSender<SVSHold>
{
 public:
	static constexpr const char *NAME = "svshold";

	using MessageSender::MessageSender;

	virtual void Send(const Anope::string &, time_t) anope_abstract;
};

class SVSHoldDel : public MessageSender<SVSHoldDel>
{
 public:
	static constexpr const char *NAME = "svsholddel";

	using MessageSender::MessageSender;

	virtual void Send(const Anope::string &) anope_abstract;
};

class SVSJoin : public MessageSender<SVSJoin>
{
 public:
	static constexpr const char *NAME = "svsjoin";

	using MessageSender::MessageSender;

	/** Force joins a user that isn't ours to a channel.
	 * @param bi The source of the message
	 * @param u The user to join
	 * @param chan The channel to join the user to
	 * @param key Channel key
	 */
	virtual void Send(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &key) anope_abstract;
};

class SVSNick : public MessageSender<SVSNick>
{
 public:
	static constexpr const char *NAME = "svsnick";

	using MessageSender::MessageSender;

	virtual void Send(User *u, const Anope::string &newnick, time_t ts) anope_abstract;
};

class SVSPart : public MessageSender<SVSPart>
{
 public:
	static constexpr const char *NAME = "svspart";

	using MessageSender::MessageSender;

	/** Force parts a user that isn't ours from a channel.
	 * @param source The source of the message
	 * @param u The user to part
	 * @param chan The channel to part the user from
	 * @param param part reason, some IRCds don't support this
	 */
	virtual void Send(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &reason) anope_abstract;
};

class SVSLogin : public MessageSender<SVSLogin>
{
 public:
	static constexpr const char *NAME = "svslogin";

	using MessageSender::MessageSender;

	virtual void Send(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) anope_abstract;
};

class SWhois : public MessageSender<SWhois>
{
 public:
	static constexpr const char *NAME = "swhois";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &, User *user, const Anope::string &) anope_abstract;
};

class Topic : public MessageSender<Topic>
{
 public:
	static constexpr const char *NAME = "topic";

	using MessageSender::MessageSender;

	/** Sets the topic on a channel
	 * @param source The bot to set the topic from
	 * @param chan The channel to set the topic on
	 * @param topic The new topic
	 * @param topic_ts The desired time set for the new topic
	 * @param topic_setter Who set the topic
	 */
	virtual void Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter) anope_abstract;
};

class VhostDel : public MessageSender<VhostDel>
{
 public:
	static constexpr const char *NAME = "vhostdel";

	using MessageSender::MessageSender;

	virtual void Send(User *u) anope_abstract;
};

class VhostSet : public MessageSender<VhostSet>
{
 public:
	static constexpr const char *NAME = "vhostset";

	using MessageSender::MessageSender;

	/** Sets a vhost on a user.
	 * @param u The user
	 * @param vident The ident to set
	 * @param vhost The vhost to set
	 */
	virtual void Send(User *u, const Anope::string &vident, const Anope::string &vhost) anope_abstract;
};

class Wallops : public MessageSender<Wallops>
{
 public:
	static constexpr const char *NAME = "wallops";

	using MessageSender::MessageSender;

	virtual void Send(const MessageSource &source, const Anope::string &msg) anope_abstract;
};

} // namespace messages
