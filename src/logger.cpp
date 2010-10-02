/* Logging routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"

void InitLogChannels(ServerConfig *config)
{
	for (unsigned i = 0; i < config->LogInfos.size(); ++i)
	{
		LogInfo *l = config->LogInfos[i];

		if (!ircd->join2msg && !l->Inhabit)
			continue;

		for (std::list<Anope::string>::const_iterator sit = l->Targets.begin(), sit_end = l->Targets.end(); sit != sit_end; ++sit)
		{
			const Anope::string &target = *sit;

			if (target[0] == '#')
			{
				Channel *c = findchan(target);
				bool created = false;
				if (!c)
				{
					c = new Channel(target);
					created = true;
				}
				c->SetFlag(CH_LOGCHAN);
				c->SetFlag(CH_PERSIST);

				for (botinfo_map::const_iterator bit = BotListByNick.begin(), bit_end = BotListByNick.end(); bit != bit_end; ++bit)
				{
					BotInfo *bi = bit->second;

					if (bi->HasFlag(BI_CORE) && !c->FindUser(bi))
					{
						bi->Join(c, created);
					}
				}
			}
		}
	}
}

static Anope::string GetTimeStamp()
{
	char tbuf[256];
	time_t t;

	if (time(&t) < 0)
		throw CoreException("time() failed");
	tm tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
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
#endif
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

LogFile::LogFile(const Anope::string &name) : filename(name), stream(name.c_str(), std::ios_base::out)
{
}

Anope::string LogFile::GetName() const
{
	return this->filename;
}

Log::Log(LogType type, const Anope::string &category, BotInfo *b) : bi(b), Type(type), Category(category)
{
	if (!b)
		b = Global;
	if (b)
		this->Sources.push_back(b->nick);
}

Log::Log(LogType type, User *u, Command *c, ChannelInfo *ci) : Type(type)
{
	if (!u || !c)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (type != LOG_COMMAND && type != LOG_OVERRIDE && type != LOG_ADMIN)
		throw CoreException("This constructor does not support this log type");

	this->bi = c->service ? c->service : Global;
	this->Category = (c->service ? c->service->nick + "/" : "") + c->name;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(u->nick);
	this->Sources.push_back(c->name);
	if (ci)
		this->Sources.push_back(ci->name);

	if (type == LOG_ADMIN)
		buf << "ADMIN: ";
	else if (type == LOG_OVERRIDE)
		buf << "OVERRIDE: ";
	else
		buf << "COMMAND: ";
	buf << u->GetMask() << " used " << c->name << " ";
	if (ci)
		buf << "on " << ci->name << " ";
}

Log::Log(User *u, Channel *c, const Anope::string &category) : Type(LOG_CHANNEL)
{
	if (!c)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	this->bi = whosends(c->ci);
	this->Category = category;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	if (u)
		this->Sources.push_back(u->nick);
	this->Sources.push_back(c->name);

	buf << "CHANNEL: ";
	if (u)
		buf << u->GetMask() << " " << this->Category << " " << c->name << " ";
	else
		buf << this->Category << " " << c->name << " ";
}

Log::Log(User *u, const Anope::string &category) : bi(Global), Type(LOG_USER), Category(category)
{
	if (!u)
		throw CoreException("Invalid pointers passed to Log::Log");
	
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(u->nick);

	buf << "USERS: " << u->GetMask() << " ";
}

Log::Log(Server *s, const Anope::string &category) : bi(OperServ), Type(LOG_SERVER), Category(category)
{
	if (!s)
		throw CoreException("Invalid pointer passed to Log::Log");
	
	if (!this->bi)
		this->bi = Global;
	if (this->bi)
		this->Sources.push_back(this->bi->nick);
	this->Sources.push_back(s->GetName());
	
	buf << "SERVER: " << s->GetName() << " (" << s->GetDescription() << ") ";
}

Log::Log(BotInfo *b, const Anope::string &category) : bi(b), Type(LOG_USER), Category(category)
{
	if (!b)
		b = Global;
	if (this->bi)
		this->Sources.push_back(bi->nick);
}

Log::~Log()
{
	if (nofork && debug && this->Type >= LOG_NORMAL && this->Type <= LOG_DEBUG + debug - 1)
		std::cout << GetTimeStamp() << " Debug: " << this->buf.str() << std::endl;
	else if (this->Type == LOG_TERMINAL)
		std::cout << this->buf.str() << std::endl;
	for (unsigned i = 0; Config && i < Config->LogInfos.size(); ++i)
	{
		LogInfo *l = Config->LogInfos[i];
		l->ProcessMessage(this);
	}
}

LogInfo::LogInfo(int logage, bool inhabit, bool normal, bool rawio, bool ldebug) : LogAge(logage), Inhabit(inhabit), Normal(normal), RawIO(rawio), Debug(ldebug)
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

bool LogInfo::HasType(std::list<Anope::string> &list, const Anope::string &type) const
{
	for (std::list<Anope::string>::iterator it = list.begin(), it_end = list.end(); it != it_end; ++it)
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

std::list<Anope::string> &LogInfo::GetList(LogType type)
{
	static std::list<Anope::string> empty;

	switch (type)
	{
		case LOG_ADMIN:
		case LOG_OVERRIDE:
		case LOG_COMMAND:
			return this->Commands;
		case LOG_SERVER:
			return this->Servers;
		case LOG_CHANNEL:
			return this->Channels;
		case LOG_USER:
			return this->Users;
		default:
			return empty;
	}
}

bool LogInfo::HasType(LogType type)
{
	switch (type)
	{
		case LOG_ADMIN:
		case LOG_OVERRIDE:
		case LOG_COMMAND:
		case LOG_SERVER:
		case LOG_CHANNEL:
		case LOG_USER:
			return !this->GetList(type).empty();
		case LOG_NORMAL:
			return this->Normal;
		case LOG_TERMINAL:
			return true;
		case LOG_RAWIO:
			return this->RawIO;
		case LOG_DEBUG:
			return debug ? true : this->Debug;
		// LOG_DEBUG_[234]
		default:
			break;
	}

	return false;
}

void LogInfo::ProcessMessage(const Log *l)
{
	static time_t lastwarn = Anope::CurTime;

	if (!l)
		throw CoreException("Bad values passed to LogInfo::ProcessMessages");
	
	if (!this->HasType(l->Type))
		return;
	else if (!l->Category.empty() && !this->HasType(this->GetList(l->Type), l->Category))
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

		if (target[0] == '#')
		{
			if (UplinkSock && l->Type <= LOG_NORMAL && Me && Me->IsSynced())
			{
				Channel *c = findchan(target);
				if (!c || !l->bi)
					continue;
				ircdproto->SendPrivmsg(l->bi, c->name, "%s", l->buf.str().c_str());
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
				log->stream << GetTimeStamp() << " " << l->buf.str() << std::endl;
		}
	}
}

