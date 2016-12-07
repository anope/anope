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
