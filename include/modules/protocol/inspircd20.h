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

namespace inspircd20
{

class Capab : public rfc1459::Capab
{
 public:
	Capab(Module *creator) : rfc1459::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChgHost : public IRCDMessage
{
 public:
	ChgHost(Module *creator) : IRCDMessage(creator, "CHGHOST", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChgIdent : public IRCDMessage
{
 public:
	ChgIdent(Module *creator) : IRCDMessage(creator, "CHGIDENT", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ChgName : public IRCDMessage
{
 public:
	ChgName(Module *creator) : IRCDMessage(creator, "CHGNAME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Encap : public IRCDMessage
{
 public:
	Encap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Endburst : public IRCDMessage
{
 public:
	Endburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FHost : public IRCDMessage
{
 public:
	FHost(Module *creator) : IRCDMessage(creator, "FHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FIdent : public IRCDMessage
{
 public:
	FIdent(Module *creator) : IRCDMessage(creator, "FIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FJoin : public IRCDMessage
{
 public:
	FJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FMode : public IRCDMessage
{
 public:
	FMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class FTopic : public IRCDMessage
{
 public:
	FTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Idle : public IRCDMessage
{
 public:
	Idle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Metadata : public IRCDMessage
{
 public:
	const bool &do_topiclock, &do_mlock;

	Metadata(Module *creator, const bool &handle_topiclock, const bool &handle_mlock)
		: IRCDMessage(creator, "METADATA", 3)
		, do_topiclock(handle_topiclock)
		, do_mlock(handle_mlock)
	{
		SetFlag(IRCDMESSAGE_REQUIRE_SERVER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Mode : public IRCDMessage
{
 public:
	Mode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Nick : public IRCDMessage
{
 public:
	Nick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class OperType : public IRCDMessage
{
 public:
	OperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class RSQuit : public IRCDMessage
{
 public:
	RSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Save : public IRCDMessage
{
	time_t last_collide = 0;

 public:
	Save(Module *creator) : IRCDMessage(creator, "SAVE", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class ServerMessage : public IRCDMessage
{
 public:
	ServerMessage(Module *creator) : IRCDMessage(creator, "SERVER", 5) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class SQuit : public rfc1459::SQuit
{
 public:
	SQuit(Module *creator) : rfc1459::SQuit(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class Time : public IRCDMessage
{
 public:
	Time(Module *creator) : IRCDMessage(creator, "TIME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

class UID : public IRCDMessage
{
	ServiceReference<SASL::Service> sasl;

 public:
	UID(Module *creator) : IRCDMessage(creator, "UID", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override;
};

} // namespace inspircd20

