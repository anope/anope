/* Logging routines.
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
#include "global.h"
#include "operserv.h"
#include "chanserv.h"

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
		strftime(tbuf, sizeof(tbuf) - 1, "[%b %d %H:%M:%S %Y]", &tm);

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

Anope::string LogFile::GetName() const
{
	return this->filename;
}

Log::Log(LogType t, const Anope::string &cat, const BotInfo *b) : bi(b), u(NULL), nc(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), type(t), category(cat)
{
	if (!bi && Config)
		bi = Global;
	if (bi)
		this->sources.push_back(bi->nick);
}

Log::Log(LogType t, CommandSource &source, Command *_c, const ChannelInfo *_ci) : nick(source.GetNick()), u(source.GetUser()), nc(source.nc), c(_c), chan(NULL), ci(_ci), s(NULL), m(NULL), type(t)
{
	if (!c)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (type != LOG_COMMAND && type != LOG_OVERRIDE && type != LOG_ADMIN)
		throw CoreException("This constructor does not support this log type");

	size_t sl = c->name.find('/');
	this->bi = NULL;
	if (sl != Anope::string::npos)
		this->bi = BotInfo::Find(c->name.substr(0, sl));
	if (this->bi == NULL && Config)
		this->bi = Global;
	this->category = c->name;
	if (this->bi)
		this->sources.push_back(this->bi->nick);
	if (u)
		this->sources.push_back(u->nick);
	this->sources.push_back(c->name);
	if (ci)
		this->sources.push_back(ci->name);
}

Log::Log(const User *_u, Channel *ch, const Anope::string &cat) : bi(NULL), u(_u), nc(NULL), c(NULL), chan(ch), ci(chan ? *chan->ci : NULL), s(NULL), m(NULL), type(LOG_CHANNEL), category(cat)
{
	if (!chan)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (Config)
		this->bi = ChanServ;
	if (this->bi)
		this->sources.push_back(this->bi->nick);
	if (u)
		this->sources.push_back(u->nick);
	this->sources.push_back(chan->name);
}

Log::Log(const User *_u, const Anope::string &cat, const BotInfo *_bi) : bi(_bi), u(_u), nc(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), m(NULL), type(LOG_USER), category(cat)
{
	if (!u)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (!this->bi && Config)
		this->bi = Global;
	if (this->bi)
		this->sources.push_back(this->bi->nick);
	this->sources.push_back(u->nick);
}

Log::Log(Server *serv, const Anope::string &cat, const BotInfo *_bi) : bi(_bi), u(NULL), nc(NULL), c(NULL), chan(NULL), ci(NULL), s(serv), m(NULL), type(LOG_SERVER), category(cat)
{
	if (!s)
		throw CoreException("Invalid pointer passed to Log::Log");
	
	if (!this->bi && Config)
		this->bi = OperServ;
	if (!this->bi && Config)
		this->bi = Global;
	if (this->bi)
		this->sources.push_back(this->bi->nick);
	this->sources.push_back(s->GetName());
}

Log::Log(const BotInfo *b, const Anope::string &cat) : bi(b), u(NULL), nc(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), m(NULL), type(LOG_NORMAL), category(cat)
{
	if (!this->bi && Config)
		this->bi = Global;
	if (this->bi)
		this->sources.push_back(bi->nick);
}

Log::Log(Module *mod, const Anope::string &cat) : bi(NULL), u(NULL), nc(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), m(mod), type(LOG_MODULE), category(cat)
{
	if (m)
		this->sources.push_back(m->name);
}

Log::~Log()
{
	if (Anope::NoFork && Anope::Debug && this->type >= LOG_NORMAL && this->type <= LOG_DEBUG + Anope::Debug - 1)
		std::cout << GetTimeStamp() << " Debug: " << this->BuildPrefix() << this->buf.str() << std::endl;
	else if (Anope::NoFork && this->type <= LOG_TERMINAL)
		std::cout << GetTimeStamp() << " " << this->BuildPrefix() << this->buf.str() << std::endl;
	else if (this->type == LOG_TERMINAL)
		std::cout << this->BuildPrefix() << this->buf.str() << std::endl;
	for (unsigned i = 0; Config && i < Config->LogInfos.size(); ++i)
	{
		LogInfo *l = Config->LogInfos[i];
		l->ProcessMessage(this);
	}
	FOREACH_MOD(I_OnLog, OnLog(this));
}

Anope::string Log::BuildPrefix() const
{
	Anope::string buffer;

	switch (this->type)
	{
		case LOG_ADMIN:
		{
			if (!this->c && !(this->u || this->nc))
				break;
			buffer += "ADMIN: ";
			size_t sl = this->c->name.find('/');
			Anope::string cname = sl != Anope::string::npos ? this->c->name.substr(sl + 1) : this->c->name;
			if (this->u)
				buffer += this->u->GetMask() + " used " + cname + " ";
			else if (this->nc)
				buffer += this->nc->display + " used " + cname + " ";
			if (this->ci)
				buffer += "on " + this->ci->name + " ";
			break;
		}
		case LOG_OVERRIDE:
		{
			if (!this->c && !(this->u || this->nc))
				break;
			buffer += "OVERRIDE: ";
			size_t sl = this->c->name.find('/');
			Anope::string cname = sl != Anope::string::npos ? this->c->name.substr(sl + 1) : this->c->name;
			if (this->u)
				buffer += this->u->GetMask() + " used " + cname + " ";
			else if (this->nc)
				buffer += this->nc->display + " used " + cname + " ";
			if (this->ci)
				buffer += "on " + this->ci->name + " ";
			break;
		}
		case LOG_COMMAND:
		{
			if (!this->c)
				break;
			buffer += "COMMAND: ";
			size_t sl = this->c->name.find('/');
			Anope::string cname = sl != Anope::string::npos ? this->c->name.substr(sl + 1) : this->c->name;
			if (this->u)
				buffer += this->u->GetMask() + " used " + cname + " ";
			else if (this->nc)
				buffer += this->nc->display + " used " + cname + " ";
			else
				buffer += this->nick + " used " + cname + " ";
			if (this->ci)
				buffer += "on " + this->ci->name + " ";
			break;
		}
		case LOG_CHANNEL:
		{
			if (!this->chan)
				break;
			buffer += "CHANNEL: ";
			if (this->u)
				buffer += this->u->GetMask() + " " + this->category + " " + this->chan->name + " ";
			else
				buffer += this->category + " " + this->chan->name + " ";
			break;
		}
		case LOG_USER:
		{
			if (this->u)
				buffer += "USERS: " + this->u->GetMask() + " ";
			break;
		}
		case LOG_SERVER:
		{
			if (this->s)
				buffer += "SERVER: " + this->s->GetName() + " (" + this->s->GetDescription() + ") ";
			break;
		}
		case LOG_MODULE:
		{
			if (this->m)
				buffer += this->m->name.upper() + ": ";
			break;
		}
		default:
			break;
	}

	return buffer;
}

LogInfo::LogInfo(int la, bool rio, bool ldebug) : log_age(la), raw_io(rio), debug(ldebug)
{
}

LogInfo::~LogInfo()
{
	for (std::map<Anope::string, LogFile *>::iterator it = this->logfiles.begin(), it_end = this->logfiles.end(); it != it_end; ++it)
	{
		LogFile *f = it->second;

		if (f && f->stream.is_open())
			f->stream.close();
		delete f;
	}
	this->logfiles.clear();
}

void LogInfo::AddType(std::list<Anope::string> &list, const Anope::string &type)
{
	for (std::list<Anope::string>::iterator it = list.begin(), it_end = list.end(); it != it_end; ++it)
	{
		if (Anope::Match(type, *it))
		{
			Log() << "Log: Type " << type << " is already covered by " << *it;
			return;
		}
	}

	list.push_back(type);
}

bool LogInfo::HasType(LogType ltype, const Anope::string &type) const
{
	const std::list<Anope::string> *list = NULL;
	switch (ltype)
	{
		case LOG_ADMIN:
			list = &this->admin;
			break;
		case LOG_OVERRIDE:
			list = &this->override;
			break;
		case LOG_COMMAND:
			list = &this->commands;
			break;
		case LOG_SERVER:
			list = &this->servers;
			break;
		case LOG_CHANNEL:
			list = &this->channels;
			break;
		case LOG_USER:
			list = &this->users;
			break;
		case LOG_TERMINAL:
			return true;
		case LOG_RAWIO:
			return debug ? true : this->raw_io;
		case LOG_DEBUG:
			return debug ? true : this->debug;
		case LOG_DEBUG_2:
		case LOG_DEBUG_3:
		case LOG_DEBUG_4:
			break;
		case LOG_MODULE:
		case LOG_NORMAL:
		default:
			list = &this->normal;
			break;
	}

	if (list == NULL)
		return false;

	for (std::list<Anope::string>::const_iterator it = list->begin(), it_end = list->end(); it != it_end; ++it)
	{
		Anope::string cat = *it;
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

void LogInfo::ProcessMessage(const Log *l)
{
	static time_t lastwarn = Anope::CurTime;

	if (!l)
		throw CoreException("Bad values passed to LogInfo::ProcessMessages");
	else if (!this->HasType(l->type, l->category))
		return;
	
	if (!this->sources.empty())
	{
		bool log = false;
		for (std::list<Anope::string>::const_iterator it = this->sources.begin(), it_end = this->sources.end(); it != it_end; ++it)
		{
			if (std::find(l->sources.begin(), l->sources.end(), *it) != l->sources.end())
			{
				log = true;
				break;
			}
		}
		if (!log)
			return;
	}

	for (std::list<Anope::string>::iterator it = this->targets.begin(), it_end = this->targets.end(); it != it_end; ++it)
	{
		const Anope::string &target = *it;
		Anope::string buffer = l->BuildPrefix() + l->buf.str();

		if (target[0] == '#')
		{
			if (UplinkSock && l->type <= LOG_NORMAL && Me && Me->IsSynced())
			{
				Channel *c = Channel::Find(target);
				if (!c || !l->bi)
					continue;
				IRCD->SendPrivmsg(l->bi, c->name, "%s", buffer.c_str());
			}
		}
		else if (target == "globops")
		{
			if (UplinkSock && l->bi && l->type <= LOG_NORMAL && Me && Me->IsSynced())
			{
				IRCD->SendGlobops(l->bi, "%s", buffer.c_str());
			}
		}
		else
		{
			LogFile *log = NULL;
			std::map<Anope::string, LogFile *>::iterator lit = this->logfiles.find(target);
			if (lit != this->logfiles.end())
			{
				log = lit->second;
				if (log && log->GetName() != CreateLogName(target))
				{
					delete log;
					this->logfiles.erase(lit);
					log = new LogFile(CreateLogName(target));
					this->logfiles[target] = log;

					if (this->log_age)
					{
						Anope::string oldlog = CreateLogName(target, Anope::CurTime - 86400 * this->log_age);
						if (IsFile(oldlog))
						{
							unlink(oldlog.c_str());
							Log(LOG_DEBUG) << "Deleted old logfile " << oldlog;
						}
					}
				}
				if (!log || !log->stream.is_open())
				{
					if (log && lastwarn + 300 < Anope::CurTime)
					{
						lastwarn = Anope::CurTime;
						Log() << "Unable to open logfile " << log->GetName();
					}
					delete log;
					this->logfiles.erase(lit);
					log = NULL;
				}
			}
			else if (lit == this->logfiles.end())
			{
				log = new LogFile(CreateLogName(target));

				if (!log->stream.is_open())
				{
					if (lastwarn + 300 < Anope::CurTime)
					{
						lastwarn = Anope::CurTime;
						Log() << "Unable to open logfile " << log->GetName();
					}
					delete log;
					log = NULL;
				}
				else
					this->logfiles[target] = log;
			}

			if (log)
				log->stream << GetTimeStamp() << " " << buffer << std::endl;
		}
	}
}

