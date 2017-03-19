/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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

#include "anope.h"
#include "defs.h"
#include "language.h"

enum class LogType
{
	NORMAL,
	/* Used whenever an administrator uses an administrative comand */
	ADMIN,
	/* Used whenever an administrator overides something, such as adding
	 * access to a channel where they don't have permission to.
	 */
	OVERRIDE,
	/* Any other command usage */
	COMMAND,
	SERVER,
	CHANNEL,
	USER,
	MODULE
};

enum class LogLevel
{
	NORMAL,
	TERMINAL,
	RAWIO,
	DEBUG,
	DEBUG_2,
	DEBUG_3
};

struct LogFile
{
	Anope::string filename;
	std::ofstream stream;

	LogFile(const Anope::string &name);
	~LogFile();
	const Anope::string &GetName() const;
};

/* Configured in the configuration file, actually does the message logging */
class CoreExport LogInfo
{
 public:
	ServiceBot *bot = nullptr;
	std::vector<Anope::string> targets;
	std::vector<LogFile *> logfiles;
	int last_day = 0;
	std::vector<Anope::string> sources;
	int log_age = 0;
	std::vector<Anope::string> admin;
	std::vector<Anope::string> override;
	std::vector<Anope::string> commands;
	std::vector<Anope::string> servers;
	std::vector<Anope::string> users;
	std::vector<Anope::string> channels;
	std::vector<Anope::string> normal;
	bool raw_io = false;
	bool debug = false;

	LogInfo(int logage, bool rawio, bool debug);

	~LogInfo();

	void OpenLogFiles();

	bool HasType(LogType ltype, LogLevel level, const Anope::string &type) const;

	void ProcessMessage(const Logger *l, const Anope::string &message);
};

class Logger
{
	friend class LogInfo;

	LogType type = LogType::NORMAL;
	LogLevel level = LogLevel::NORMAL;

	/* Object logger is attached to */
	Module *module = nullptr;
	Command *command = nullptr;
	ServiceBot *bot = nullptr;
	Server *server = nullptr;

	/* Logger category */
	Anope::string category;
	/* Non formatted message */
	Anope::string raw_message;

	/* Sources */
	User *user = nullptr;
	NickServ::Account *account = nullptr;
	Channel *channel = nullptr;
	ChanServ::Channel *ci = nullptr;
	CommandSource *source = nullptr;

	Anope::string FormatSource() const;
	Anope::string BuildPrefix() const;
	void LogMessage(const Anope::string &message);
	void InsertVariables(FormatInfo &fi);
	void CheckOverride();

	template<typename... Args>
	Anope::string Format(const Anope::string &message, Args&&... args)
	{
		FormatInfo fi(message, sizeof...(Args));
		fi.AddArgs(std::forward<Args>(args)...);
		InsertVariables(fi);
		fi.Format();
		return fi.GetFormat();
	}

 public:
	Logger() = default;
	Logger(Module *m) : type(LogType::MODULE), module(m) { }
	Logger(Command *c) : type(LogType::COMMAND), command(c) { }
	Logger(ServiceBot *b) : bot(b) { }
	Logger(Channel *c) : type(LogType::CHANNEL), channel(c) { }
	Logger(User *u) : type(LogType::USER), user(u) { }
	Logger(Server *s) : type(LogType::SERVER), server(s) { }

	LogType GetType() const;
	LogLevel GetLevel() const;

	Module *GetModule() const;
	Command *GetCommand() const;
	ServiceBot *GetBot() const;
	Server *GetServer() const;

	User *GetUser() const;
	void SetUser(User *);

	NickServ::Account *GetAccount() const;
	void SetAccount(NickServ::Account *);

	Channel *GetChannel() const;
	void SetChannel(Channel *);

	ChanServ::Channel *GetCi() const;
	void SetCi(ChanServ::Channel *);

	CommandSource *GetSource() const;
	void SetSource(CommandSource *);

	Logger Category(const Anope::string &c) const;
	Logger User(class User *u) const;
	Logger Channel(class Channel *c) const;
	Logger Channel(ChanServ::Channel *c) const;
	Logger Source(CommandSource *s) const;
	Logger Bot(ServiceBot *bot) const;
	Logger Bot(const Anope::string &name) const;

	template<typename... Args> void Log(LogLevel lev, const Anope::string &message, Args&&... args)
	{
		Logger l = *this;
		l.raw_message = message;
		l.level = lev;

		Anope::string translated = Language::Translate(message);
		l.LogMessage(l.Format(translated, std::forward<Args>(args)...));
	}

	template<typename... Args> void Command(LogType ltype, CommandSource &csource, ChanServ::Channel *chan, const Anope::string &message, Args&&... args)
	{
		Logger l = *this;
		l.type = ltype;
		l.SetSource(&csource);
		l.SetCi(chan);

		// Override if the source is marked override
		l.CheckOverride();

		Anope::string translated = Language::Translate(message);
		l.LogMessage(l.Format(translated, std::forward<Args>(args)...));
	}

	template<typename... Args> void Command(LogType ltype, CommandSource &csource, const Anope::string &message, Args&&... args)
	{
		Command(ltype, csource, nullptr, message, std::forward<Args>(args)...);
	}

	// LOG_COMMAND

	template<typename... Args> void Command(CommandSource &csource, ChanServ::Channel *chan, const Anope::string &message, Args&&... args)
	{
		Command(LogType::COMMAND, csource, chan, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Command(CommandSource &csource, const Anope::string &message, Args&&... args)
	{
		Command(LogType::COMMAND, csource, nullptr, message, std::forward<Args>(args)...);
	}

	// LOG_ADMIN

	template<typename... Args> void Admin(CommandSource &csource, ChanServ::Channel *chan, const Anope::string &message, Args&&... args)
	{
		Command(LogType::ADMIN, csource, chan, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Admin(CommandSource &csource, const Anope::string &message, Args&&... args)
	{
		Command(LogType::ADMIN, csource, nullptr, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Log(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::NORMAL, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Terminal(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::TERMINAL, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void RawIO(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::RAWIO, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Debug(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::DEBUG, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Debug2(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::DEBUG_2, message, std::forward<Args>(args)...);
	}

	template<typename... Args> void Debug3(const Anope::string &message, Args&&... args)
	{
		Log(LogLevel::DEBUG_3, message, std::forward<Args>(args)...);
	}
};

