/*
 *
 * (C) 2003-2021 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "anope.h"
#include "defs.h"

enum LogType
{
	/* Used whenever an administrator uses an administrative command */
	LOG_ADMIN,
	/* Used whenever an administrator overrides something, such as adding
	 * access to a channel where they don't have permission to.
	 */
	LOG_OVERRIDE,
	/* Any other command usage */
	LOG_COMMAND,
	LOG_SERVER,
	LOG_CHANNEL,
	LOG_USER,
	LOG_MODULE,
	LOG_NORMAL,
	LOG_TERMINAL,
	LOG_RAWIO,
	LOG_DEBUG,
	LOG_DEBUG_2,
	LOG_DEBUG_3,
	LOG_DEBUG_4
};

struct LogFile
{
	Anope::string filename;
	std::ofstream stream;

	LogFile(const Anope::string &name);
	~LogFile();
	const Anope::string &GetName() const;
};

/* Represents a single log message */
class CoreExport Log
{
 public:
	/* Bot that should log this message */
	BotInfo *bi = nullptr;
	/* For commands, the user executing the command, but might not always exist */
	User *u = nullptr;
	/* For commands, the account executing the command, but will not always exist */
	NickCore *nc = nullptr;
	/* For commands, the command being executed */
	Command *c = nullptr;
	/* For commands, the command source */
	CommandSource *source = nullptr;
	/* Used for LOG_CHANNEL */
	Channel *chan = nullptr;
	/* For commands, the channel the command was executed on, will not always exist */
	const ChannelInfo *ci = nullptr;
	/* For LOG_SERVER */
	Server *s = nullptr;
	/* For LOG_MODULE */
	Module *m = nullptr;
	LogType type;
	Anope::string category;

	std::stringstream buf;

	Log(LogType type = LOG_NORMAL, const Anope::string &category = "", BotInfo *bi = NULL);

	/* LOG_COMMAND/OVERRIDE/ADMIN */
	Log(LogType type, CommandSource &source, Command *c, ChannelInfo *ci = NULL);

	/* LOG_CHANNEL */
	Log(User *u, Channel *c, const Anope::string &category = "");

	/* LOG_USER */
	Log(User *u, const Anope::string &category = "", BotInfo *bi = NULL);

	/* LOG_SERVER */
	Log(Server *s, const Anope::string &category = "", BotInfo *bi = NULL);

	Log(BotInfo *b, const Anope::string &category = "");

	Log(Module *m, const Anope::string &category = "", BotInfo *bi = NULL);

	~Log();

 private:
	Anope::string FormatSource() const;
	Anope::string FormatCommand() const;

 public:
	Anope::string BuildPrefix() const;

	template<typename T> Log &operator<<(T val)
	{
		this->buf << val;
		return *this;
	}
};

/* Configured in the configuration file, actually does the message logging */
class CoreExport LogInfo
{
 public:
	BotInfo *bot = nullptr;
	std::vector<Anope::string> targets;
	std::vector<LogFile *> logfiles;
	int last_day = 0;
	std::vector<Anope::string> sources;
	int log_age;
	std::vector<Anope::string> admin;
	std::vector<Anope::string> override;
	std::vector<Anope::string> commands;
	std::vector<Anope::string> servers;
	std::vector<Anope::string> users;
	std::vector<Anope::string> channels;
	std::vector<Anope::string> normal;
	bool raw_io;
	bool debug;

	LogInfo(int logage, bool rawio, bool debug);

	~LogInfo();

	void OpenLogFiles();

	bool HasType(LogType ltype, const Anope::string &type) const;

	/* Logs the message l if configured to */
	void ProcessMessage(const Log *l);
};
