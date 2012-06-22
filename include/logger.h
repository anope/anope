/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "anope.h"
#include "defs.h"

enum LogType
{
	LOG_ADMIN,
	LOG_OVERRIDE,
	LOG_COMMAND,
	LOG_SERVER,
	LOG_CHANNEL,
	LOG_USER,
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

 public:
	std::ofstream stream;

	LogFile(const Anope::string &name);
	Anope::string GetName() const;
};


class CoreExport Log
{
 public:
	const BotInfo *bi;
	const User *u;
	const NickCore *nc;
	Command *c;
	Channel *chan;
	const ChannelInfo *ci;
	Server *s;
	LogType Type;
	Anope::string Category;
	std::list<Anope::string> Sources;

	std::stringstream buf;

	Log(LogType type = LOG_NORMAL, const Anope::string &category = "", const BotInfo *bi = NULL);

	/* LOG_COMMAND/OVERRIDE/ADMIN */
	Log(LogType type, const CommandSource &source, Command *c, const ChannelInfo *ci = NULL);

	/* LOG_CHANNEL */
	Log(const User *u, Channel *c, const Anope::string &category = "");

	/* LOG_USER */
	explicit Log(const User *u, const Anope::string &category = "", const BotInfo *bi = NULL);

	/* LOG_SERVER */
	explicit Log(Server *s, const Anope::string &category = "", const BotInfo *bi = NULL);

	explicit Log(const BotInfo *b, const Anope::string &category = "");

	~Log();

	Anope::string BuildPrefix() const;

	template<typename T> Log &operator<<(T val)
	{
		this->buf << val;
		return *this;
	}
};

class CoreExport LogInfo
{
 public:
	std::list<Anope::string> Targets;
	std::map<Anope::string, LogFile *> Logfiles;
	std::list<Anope::string> Sources;
	int LogAge;
	std::list<Anope::string> Admin;
	std::list<Anope::string> Override;
	std::list<Anope::string> Commands;
	std::list<Anope::string> Servers;
	std::list<Anope::string> Users;
	std::list<Anope::string> Channels;
	std::list<Anope::string> Normal;
	bool RawIO;
	bool Debug;

	LogInfo(int logage, bool rawio, bool debug);

	~LogInfo();

	void AddType(std::list<Anope::string> &list, const Anope::string &type);

	bool HasType(LogType ltype, const Anope::string &type) const;

	void ProcessMessage(const Log *l);
};

#endif // LOGGER_H

