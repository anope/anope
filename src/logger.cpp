/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
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

#include "services.h"
#include "modules.h"
#include "commands.h"
#include "channels.h"
#include "users.h"
#include "logger.h"
#include "config.h"
#include "bots.h"
#include "servers.h"
#include "uplink.h"
#include "protocol.h"
#include "event.h"
#include "anope.h"
#include "modules/nickserv.h"
#include "modules/chanserv.h"

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

static Anope::string GetTimeStamp()
{
	char tbuf[256];
	time_t t;

	if (time(&t) < 0)
		t = Anope::CurTime;

	tm tm = *localtime(&t);
	if (Anope::Debug)
	{
		char *s;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		strftime(tbuf, sizeof(tbuf) - 1, "[%b %d %H:%M:%S", &tm);
		s = tbuf + strlen(tbuf);
		s += snprintf(s, sizeof(tbuf) - (s - tbuf), ".%06d", static_cast<int>(tv.tv_usec));
		strftime(s, sizeof(tbuf) - (s - tbuf) - 1, " %Y]", &tm);
	}
	else
	{
		strftime(tbuf, sizeof(tbuf) - 1, "[%b %d %H:%M:%S %Y]", &tm);
	}

	return tbuf;
}

static inline Anope::string CreateLogName(const Anope::string &file, time_t t = Anope::CurTime)
{
	char timestamp[32];
	tm *tm = localtime(&t);
	strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);

	return Anope::LogDir + "/" + file + "." + timestamp;
}

LogFile::LogFile(const Anope::string &name) : filename(name), stream(name.c_str(), std::ios_base::out | std::ios_base::app)
{
}

LogFile::~LogFile()
{
	this->stream.close();
}

const Anope::string &LogFile::GetName() const
{
	return this->filename;
}

LogInfo::LogInfo(int la, bool rio, bool ldebug) : log_age(la), raw_io(rio), debug(ldebug)
{
}

LogInfo::~LogInfo()
{
	for (unsigned i = 0; i < this->logfiles.size(); ++i)
		delete this->logfiles[i];
	this->logfiles.clear();
}

bool LogInfo::HasType(LogType ltype, LogLevel level, const Anope::string &type) const
{
	switch (level)
	{
		case LogLevel::TERMINAL:
			return true;
		case LogLevel::RAWIO:
			return (Anope::Debug || this->debug) ? true : this->raw_io;
		case LogLevel::DEBUG:
			return Anope::Debug ? true : this->debug;
		case LogLevel::DEBUG_2:
		case LogLevel::DEBUG_3:
			return false;
		case LogLevel::NORMAL:
			break;
	}

	const std::vector<Anope::string> *list = NULL;
	switch (ltype)
	{
		case LogType::ADMIN:
			list = &this->admin;
			break;
		case LogType::OVERRIDE:
			list = &this->override;
			break;
		case LogType::COMMAND:
			list = &this->commands;
			break;
		case LogType::SERVER:
			list = &this->servers;
			break;
		case LogType::CHANNEL:
			list = &this->channels;
			break;
		case LogType::USER:
			list = &this->users;
			break;
		case LogType::MODULE:
		case LogType::NORMAL:
		default:
			list = &this->normal;
			break;
	}

	if (list == NULL)
		return false;

	for (unsigned int i = 0; i < list->size(); ++i)
	{
		Anope::string cat = list->at(i);
		bool inverse = false;
		if (cat[0] == '~')
		{
			cat.erase(cat.begin());
			inverse = true;
		}
		if (Anope::Match(type, cat))
		{
			return !inverse;
		}
	}

	return false;
}

void LogInfo::OpenLogFiles()
{
	for (unsigned i = 0; i < this->logfiles.size(); ++i)
		delete this->logfiles[i];
	this->logfiles.clear();

	for (unsigned i = 0; i < this->targets.size(); ++i)
	{
		const Anope::string &target = this->targets[i];

		if (target.empty() || target[0] == '#' || target == "opers" || target.find(":") != Anope::string::npos)
			continue;

		LogFile *lf = new LogFile(CreateLogName(target));
		if (!lf->stream.is_open())
		{
			Anope::Logger.Log("Unable to open logfile {0}", lf->GetName());
			delete lf;
		}
		else
			this->logfiles.push_back(lf);
	}
}

void LogInfo::ProcessMessage(const Logger *l, const Anope::string &message)
{
	if (!this->sources.empty())
	{
		bool log = false;
		for (unsigned i = 0; i < this->sources.size() && !log; ++i)
		{
			const Anope::string &src = this->sources[i];

			if (l->GetBot() && src == l->GetBot()->nick)
				log = true;
			else if (l->GetUser() && src == l->GetUser()->nick)
				log = true;
			else if (l->GetAccount() && src == l->GetAccount()->GetDisplay())
				log = true;
			else if (l->GetCi() && src == l->GetCi()->GetName())
				log = true;
			else if (l->GetModule() && src == l->GetModule()->name)
				log = true;
			else if (l->GetServer() && src == l->GetServer()->GetName())
				log = true;
		}
		if (!log)
			return;
	}

	const Anope::string &buffer = l->BuildPrefix() + message;

	if (l->GetLevel() <= LogLevel::NORMAL)
	{
		EventManager::Get()->Dispatch(&Event::LogMessage::OnLogMessage, this, l, buffer);
	}

	for (unsigned i = 0; i < this->targets.size(); ++i)
	{
		const Anope::string &target = this->targets[i];

		if (!target.empty() && target[0] == '#')
		{
			if (UplinkSock && l->type == LogType::NORMAL && Me && Me->IsSynced())
			{
				Channel *c = Channel::Find(target);
				if (!c)
					continue;

				User *bi = l->GetBot();
				if (!bi)
					bi = this->bot;
				if (!bi)
				{
					ChanServ::Channel *ci = c->GetChannel();
					bi = ci->WhoSends();
				}
				if (bi)
					IRCD->SendPrivmsg(bi, c->name, buffer);
			}
		}
		else if (target == "opers")
		{
			if (UplinkSock && l->GetBot() && l->type == LogType::NORMAL && Me && Me->IsSynced())
			{
				IRCD->SendWallops(l->GetBot(), buffer);
			}
		}
	}

	tm *tm = localtime(&Anope::CurTime);
	if (tm->tm_mday != this->last_day)
	{
		this->last_day = tm->tm_mday;
		this->OpenLogFiles();

		if (this->log_age)
			for (unsigned i = 0; i < this->targets.size(); ++i)
			{
				const Anope::string &target = this->targets[i];

				if (target.empty() || target[0] == '#' || target == "opers" || target.find(":") != Anope::string::npos)
					continue;

				Anope::string oldlog = CreateLogName(target, Anope::CurTime - 86400 * this->log_age);
				if (IsFile(oldlog))
				{
					unlink(oldlog.c_str());
					Anope::Logger.Debug("Deleted old logfile {0}", oldlog);
				}
			}
	}

	for (unsigned i = 0; i < this->logfiles.size(); ++i)
	{
		LogFile *lf = this->logfiles[i];
		lf->stream << GetTimeStamp() << " " << buffer << std::endl;
	}
}

void Logger::InsertVariables(FormatInfo &fi, bool full)
{
	if (user != nullptr)
		fi.Add("user"_kw = user->GetMask());

	if (ci != nullptr)
		fi.Add("channel"_kw = ci->GetName());
	else if (channel != nullptr)
		fi.Add("channel"_kw = channel->GetName());

	fi.Add("source"_kw = this->FormatSource(full));

	if (source != nullptr && !source->GetCommand().empty())
		fi.Add("command"_kw = source->GetCommand());
	else if (command != nullptr)
		fi.Add("command"_kw = command->GetName());
}

void Logger::CheckOverride()
{
	if (type == LogType::COMMAND && source != nullptr && source->IsOverride())
		type = LogType::OVERRIDE;
}

Anope::string Logger::FormatSource(bool full) const
{
	if (user)
	{
		const Anope::string &umask = full ? user->GetMask() : user->GetDisplayedMask();

		if (account)
			return umask + " (" + account->GetDisplay() + ")";
		else
			return umask;
	}
	else if (account)
		return account->GetDisplay();
	else if (source)
		return source->GetNick();
	return "";
}

Anope::string Logger::BuildPrefix() const
{
	switch (this->type)
	{
		case LogType::ADMIN:
			return "ADMIN: ";
		case LogType::OVERRIDE:
			return "OVERRIDE: ";
		case LogType::COMMAND:
			return "COMMAND: ";
		case LogType::SERVER:
			return "SERVER: ";
		case LogType::CHANNEL:
			return "CHANNEL: ";
		case LogType::USER:
			return "USER: ";
		case LogType::MODULE:
			return this->module != nullptr ? (this->module->name.upper() + ": ") : "";
		case LogType::NORMAL:
			break;
	}

	return "";
}

void Logger::LogMessage()
{
	if (Anope::NoFork && Anope::Debug && level >= LogLevel::NORMAL && static_cast<int>(level) <= static_cast<int>(LogLevel::DEBUG) + Anope::Debug - 1)
		std::cout << GetTimeStamp() << " Debug: " << this->BuildPrefix() << full_message << std::endl;
	else if (Anope::NoFork && level <= LogLevel::TERMINAL)
		std::cout << GetTimeStamp() << " " << this->BuildPrefix() << full_message << std::endl;
	else if (level == LogLevel::TERMINAL)
		std::cout << this->BuildPrefix() << full_message << std::endl;

	if (level <= LogLevel::NORMAL)
	{
		EventManager *em = EventManager::Get();
		if (em != nullptr)
			em->Dispatch(&Event::Log::OnLog, this);
	}

	if (Config != nullptr)
		for (LogInfo &info : Config->LogInfos)
			if (info.HasType(this->type, this->level, this->category))
				info.ProcessMessage(this, full_message);
}

LogType Logger::GetType() const
{
	return type;
}

LogLevel Logger::GetLevel() const
{
	return level;
}

Module *Logger::GetModule() const
{
	return module;
}

Command *Logger::GetCommand() const
{
	return command;
}

ServiceBot *Logger::GetBot() const
{
	return bot;
}

Server *Logger::GetServer() const
{
	return server;
}

User *Logger::GetUser() const
{
	return user;
}

void Logger::SetUser(class User *u)
{
	user = u;
}

NickServ::Account *Logger::GetAccount() const
{
	return account;
}

void Logger::SetAccount(NickServ::Account *acc)
{
	account = acc;
}

Channel *Logger::GetChannel() const
{
	return channel;
}

void Logger::SetChannel(class Channel *c)
{
	channel = c;
}

ChanServ::Channel *Logger::GetCi() const
{
	return ci;
}

void Logger::SetCi(ChanServ::Channel *c)
{
	ci = c;
}

CommandSource *Logger::GetSource() const
{
	return source;
}

void Logger::SetSource(CommandSource *s)
{
	source = s;
	if (s != nullptr)
	{
		SetUser(s->GetUser());
		SetAccount(s->GetAccount());
		SetChannel(s->c);
	}
}

const Anope::string &Logger::GetUnformattedMessage() const
{
	return raw_message;
}

const Anope::string &Logger::GetMessage() const
{
	return full_message;
}

const Anope::string &Logger::GetMaskedMessage() const
{
	return masked_message;
}

Logger Logger::Category(const Anope::string &c) const
{
	Logger l = *this;
	l.category = c;
	return l;
}

Logger Logger::User(class User *u) const
{
	Logger l = *this;
	l.user = u;
	return l;
}

Logger Logger::Channel(class Channel *c) const
{
	Logger l = *this;
	l.channel = c;
	return l;
}

Logger Logger::Channel(ChanServ::Channel *c) const
{
	Logger l = *this;
	l.ci = c;
	return l;
}

Logger Logger::Source(CommandSource *s) const
{
	Logger l = *this;
	l.source = s;
	return l;
}

Logger Logger::Bot(ServiceBot *b) const
{
	Logger l = *this;
	l.bot = b;
	return l;
}

Logger Logger::Bot(const Anope::string &botname) const
{
	return Bot(Config ? Config->GetClient(botname) : nullptr);
}

