/* Logging routines.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"

static Anope::string GetTimeStamp()
{
	char tbuf[256];
	time_t t;

	if (time(&t) < 0)
		throw CoreException("time() failed");
	tm tm = *localtime(&t);
	if (debug)
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

static Anope::string GetLogDate(time_t t = Anope::CurTime)
{
	char timestamp[32];

	time(&t);
	tm *tm = localtime(&t);
	
	strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);

	return timestamp;
}

static inline Anope::string CreateLogName(const Anope::string &file, time_t t = Anope::CurTime)
{
	return "logs/" + file + "." + GetLogDate(t);
}

LogFile::LogFile(const Anope::string &name) : filename(name), stream(name.c_str(), std::ios_base::out | std::ios_base::app)
{
}

Anope::string LogFile::GetName() const
{
	return this->filename;
}

Log::Log(LogType type, const Anope::string &category, BotInfo *b) : bi(b), u(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), Type(type), Category(category)
{
	if (!bi)
		bi = Config ? findbot(Config->Global) : NULL;
	if (bi)
		this->Sources.push_back(bi->nick);
}

Log::Log(LogType type, User *_u, Command *_c, ChannelInfo *_ci) : u(_u), c(_c), chan(NULL), ci(_ci), s(NULL), Type(type)
{
	if (!u || !c)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (type != LOG_COMMAND && type != LOG_OVERRIDE && type != LOG_ADMIN)
		throw CoreException("This constructor does not support this log type");

	size_t sl = c->name.find('/');
	this->bi = NULL;
	if (sl != Anope::string::npos)
		this->bi = findbot(c->name.substr(0, sl));
	if (this->bi == NULL)
		this->bi = Config ? findbot(Config->Global) : NULL;
	this->Category = c->name;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(u->nick);
	this->Sources.push_back(c->name);
	if (ci)
		this->Sources.push_back(ci->name);
}

Log::Log(User *_u, Channel *ch, const Anope::string &category) : u(_u), c(NULL), chan(ch), ci(chan ? chan->ci : NULL), s(NULL), Type(LOG_CHANNEL)
{
	if (!chan)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	this->bi = Config ? findbot(Config->ChanServ) : NULL;
	this->Category = category;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	if (u)
		this->Sources.push_back(u->nick);
	this->Sources.push_back(chan->name);
}

Log::Log(User *_u, const Anope::string &category) : bi(NULL), u(_u), c(NULL), chan(NULL), ci(NULL), s(NULL), Type(LOG_USER), Category(category)
{
	if (!u)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	this->bi = Config ? findbot(Config->Global) : NULL;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(u->nick);
}

Log::Log(Server *serv, const Anope::string &category) : bi(NULL), u(NULL), c(NULL), chan(NULL), ci(NULL), s(serv), Type(LOG_SERVER), Category(category)
{
	if (!s)
		throw CoreException("Invalid pointer passed to Log::Log");
	
	this->bi = Config ? findbot(Config->OperServ) : NULL;
	if (!this->bi)
		this->bi = Config ? findbot(Config->Global) : NULL;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(s->GetName());
}

Log::Log(BotInfo *b, const Anope::string &category) : bi(b), u(NULL), c(NULL), chan(NULL), ci(NULL), s(NULL), Type(LOG_NORMAL), Category(category)
{
	if (!this->bi)
		this->bi = Config ? findbot(Config->Global) : NULL;
	if (this->bi)
		this->Sources.push_back(bi->nick);
}

Log::~Log()
{
	if (nofork && debug && this->Type >= LOG_NORMAL && this->Type <= LOG_DEBUG + debug - 1)
		std::cout << GetTimeStamp() << " Debug: " << this->BuildPrefix() << this->buf.str() << std::endl;
	else if (nofork && this->Type <= LOG_TERMINAL)
		std::cout << GetTimeStamp() << " " << this->BuildPrefix() << this->buf.str() << std::endl;
	else if (this->Type == LOG_TERMINAL)
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

	switch (this->Type)
	{
		case LOG_ADMIN:
		{
			if (!this->c)
				break;
			buffer += "ADMIN: ";
			size_t sl = this->c->name.find('/');
			Anope::string cname = sl != Anope::string::npos ? this->c->name.substr(sl + 1) : this->c->name;
			buffer += this->u->GetMask() + " used " + cname + " ";
			if (this->ci)
				buffer += "on " + this->ci->name + " ";
			break;
		}
		case LOG_OVERRIDE:
		{
			if (!this->c)
				break;
			buffer += "OVERRIDE: ";
			size_t sl = this->c->name.find('/');
			Anope::string cname = sl != Anope::string::npos ? this->c->name.substr(sl + 1) : this->c->name;
			buffer += this->u->GetMask() + " used " + cname + " ";
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
			buffer += this->u->GetMask() + " used " + cname + " ";
			if (this->ci)
				buffer += "on " + this->ci->name + " ";
			break;
		}
		case LOG_CHANNEL:
		{
			buffer += "CHANNEL: ";
			if (this->u)
				buffer += this->u->GetMask() + " " + this->Category + " " + this->chan->name + " ";
			else
				buffer += this->Category + " " + this->chan->name + " ";
			break;
		}
		case LOG_USER:
		{
			buffer += "USERS: " + this->u->GetMask() + " ";
			break;
		}
		case LOG_SERVER:
		{
			buffer += "SERVER: " + this->s->GetName() + " (" + this->s->GetDescription() + ") ";
			break;
		}
		default:
			break;
	}

	return buffer;
}

LogInfo::LogInfo(int logage, bool rawio, bool ldebug) : LogAge(logage), RawIO(rawio), Debug(ldebug)
{
}

LogInfo::~LogInfo()
{
	for (std::map<Anope::string, LogFile *>::iterator it = this->Logfiles.begin(), it_end = this->Logfiles.end(); it != it_end; ++it)
	{
		LogFile *f = it->second;

		if (f && f->stream.is_open())
			f->stream.close();
		delete f;
	}
	this->Logfiles.clear();
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
			list = &this->Admin;
			break;
		case LOG_OVERRIDE:
			list = &this->Override;
			break;
		case LOG_COMMAND:
			list = &this->Commands;
			break;
		case LOG_SERVER:
			list = &this->Servers;
			break;
		case LOG_CHANNEL:
			list = &this->Channels;
			break;
		case LOG_USER:
			list = &this->Users;
			break;
		case LOG_NORMAL:
			list = &this->Normal;
			break;
		case LOG_TERMINAL:
			return true;
		case LOG_RAWIO:
			return debug ? true : this->RawIO;
		case LOG_DEBUG:
			return debug ? true : this->Debug;
		default:
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
	else if (!this->HasType(l->Type, l->Category))
		return;
	
	if (!this->Sources.empty())
	{
		bool log = false;
		for (std::list<Anope::string>::const_iterator it = this->Sources.begin(), it_end = this->Sources.end(); it != it_end; ++it)
		{
			if (std::find(l->Sources.begin(), l->Sources.end(), *it) != l->Sources.end())
			{
				log = true;
				break;
			}
		}
		if (!log)
			return;
	}

	for (std::list<Anope::string>::iterator it = this->Targets.begin(), it_end = this->Targets.end(); it != it_end; ++it)
	{
		const Anope::string &target = *it;
		Anope::string buffer = l->BuildPrefix() + l->buf.str();

		if (target[0] == '#')
		{
			if (UplinkSock && l->Type <= LOG_NORMAL && Me && Me->IsSynced())
			{
				Channel *c = findchan(target);
				if (!c || !l->bi)
					continue;
				ircdproto->SendPrivmsg(l->bi, c->name, "%s", buffer.c_str());
			}
		}
		else if (target == "globops")
		{
			if (UplinkSock && l->bi && l->Type <= LOG_NORMAL && Me && Me->IsSynced())
			{
				ircdproto->SendGlobops(l->bi, "%s", buffer.c_str());
			}
		}
		else
		{
			LogFile *log = NULL;
			std::map<Anope::string, LogFile *>::iterator lit = this->Logfiles.find(target);
			if (lit != this->Logfiles.end())
			{
				log = lit->second;
				if (log && log->GetName() != CreateLogName(target))
				{
					delete log;
					this->Logfiles.erase(lit);
					log = new LogFile(CreateLogName(target));
					this->Logfiles[target] = log;

					if (this->LogAge)
					{
						Anope::string oldlog = CreateLogName(target, Anope::CurTime - 86400 * this->LogAge);
						if (IsFile(oldlog))
						{
							DeleteFile(oldlog.c_str());
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
					this->Logfiles.erase(lit);
					log = NULL;
				}
			}
			else if (lit == this->Logfiles.end())
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
					this->Logfiles[target] = log;
			}

			if (log)
				log->stream << GetTimeStamp() << " " << buffer << std::endl;
		}
	}
}

