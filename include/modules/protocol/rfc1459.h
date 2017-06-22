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

namespace rfc1459
{

namespace senders
{

class GlobalNotice : public messages::GlobalNotice
{
 public:
	using messages::GlobalNotice::GlobalNotice;

	void Send(const MessageSource &, Server *dest, const Anope::string &msg) override;
};

class GlobalPrivmsg : public messages::GlobalPrivmsg
{
 public:
	using messages::GlobalPrivmsg::GlobalPrivmsg;

	void Send(const MessageSource &, Server *dest, const Anope::string &msg) override;
};

class Invite : public messages::Invite
{
 public:
	using messages::Invite::Invite;

	void Send(const MessageSource &source, Channel *chan, User *user) override;
};

class Join : public messages::Join
{
 public:
	using messages::Join::Join;

	void Send(User *u, Channel *c, const ChannelStatus *status) override;
};

class Kick : public messages::Kick
{
 public:
	using messages::Kick::Kick;

	void Send(const MessageSource &source, Channel *chan, User *user, const Anope::string &reason) override;
};

class Kill : public messages::Kill
{
 public:
	using messages::Kill::Kill;

	void Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason) override;

	void Send(const MessageSource &source, User *user, const Anope::string &reason) override;
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

class NickChange : public messages::NickChange
{
 public:
	using messages::NickChange::NickChange;

	void Send(User *u, const Anope::string &newnick, time_t ts) override;
};

class Notice : public messages::Notice
{
 public:
	using messages::Notice::Notice;

	void Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg) override;
};

class Part : public messages::Part
{
 public:
	using messages::Part::Part;

	void Send(User *, Channel *chan, const Anope::string &reason) override;
};

class Ping : public messages::Ping
{
 public:
	using messages::Ping::Ping;

	void Send(const Anope::string &servname, const Anope::string &who) override;
};

class Pong : public messages::Pong
{
 public:
	using messages::Pong::Pong;

	void Send(const Anope::string &servname, const Anope::string &who) override;
};

class Privmsg : public messages::Privmsg
{
 public:
	using messages::Privmsg::Privmsg;

	void Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg) override;
};

class Quit : public messages::Quit
{
 public:
	using messages::Quit::Quit;

	void Send(User *user, const Anope::string &reason) override;
};

class MessageServer : public messages::MessageServer
{
 public:
	using messages::MessageServer::MessageServer;

	void Send(Server *server) override;
};

class SQuit : public messages::SQuit
{
 public:
	using messages::SQuit::SQuit;

	void Send(Server *s, const Anope::string &message) override;
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

	void Send(const MessageSource &, const Anope::string &msg) override;
};

} // namespace senders

class Away : public IRCDMessage
{
 public:
	Away(Module *creator, const Anope::string &mname = "AWAY") : IRCDMessage(creator, mname, 0) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Capab : public IRCDMessage
{
 public:
	Capab(Module *creator, const Anope::string &mname = "CAPAB") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Error : public IRCDMessage
{
 public:
	Error(Module *creator, const Anope::string &mname = "ERROR") : IRCDMessage(creator, mname, 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Invite : public IRCDMessage
{
 public:
	Invite(Module *creator, const Anope::string &mname = "INVITE") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Join : public IRCDMessage
{
 public:
	Join(Module *creator, const Anope::string &mname = "JOIN") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;

	typedef std::pair<ChannelStatus, User *> SJoinUser;

	/** Handle a SJOIN.
	 * @param source The source of the SJOIN
	 * @param chan The channel the users are joining to
	 * @param ts The TS for the channel
	 * @param modes The modes sent with the SJOIN, if any
	 * @param users The users and their status, if any
	 */
	static void SJoin(MessageSource &source, const Anope::string &chan, time_t ts, const Anope::string &modes, const std::list<SJoinUser> &users);
};

class Kick : public IRCDMessage
{
 public:
	Kick(Module *creator, const Anope::string &mname = "KICK") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Kill : public IRCDMessage
{
 public:
	Kill(Module *creator, const Anope::string &mname = "KILL") : IRCDMessage(creator, mname, 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator, const Anope::string &mname = "MODE") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class MOTD : public IRCDMessage
{
 public:
	MOTD(Module *creator, const Anope::string &mname = "MOTD") : IRCDMessage(creator, mname, 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Notice : public IRCDMessage
{
 public:
	Notice(Module *creator, const Anope::string &mname = "NOTICE") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Part : public IRCDMessage
{
 public:
	Part(Module *creator, const Anope::string &mname = "PART") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Ping : public IRCDMessage
{
 public:
	Ping(Module *creator, const Anope::string &mname = "PING") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Pong : public IRCDMessage
{
 public:
	Pong(Module *creator, const Anope::string &mname = "PONG") : IRCDMessage(creator, mname) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Privmsg : public IRCDMessage
{
 public:
	Privmsg(Module *creator, const Anope::string &mname = "PRIVMSG") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Quit : public IRCDMessage
{
 public:
	Quit(Module *creator, const Anope::string &mname = "QUIT") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SQuit : public IRCDMessage
{
 public:
	SQuit(Module *creator, const Anope::string &mname = "SQUIT") : IRCDMessage(creator, mname, 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Stats : public IRCDMessage
{
 public:
	Stats(Module *creator, const Anope::string &mname = "STATS") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Time : public IRCDMessage
{
 public:
	Time(Module *creator, const Anope::string &mname = "TIME") : IRCDMessage(creator, mname, 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Topic : public IRCDMessage
{
 public:
	Topic(Module *creator, const Anope::string &mname = "TOPIC") : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Version : public IRCDMessage
{
 public:
	Version(Module *creator, const Anope::string &mname = "VERSION") : IRCDMessage(creator, mname, 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Whois : public IRCDMessage
{
 public:
	Whois(Module *creator, const Anope::string &mname = "WHOIS") : IRCDMessage(creator, mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace rfc1459
