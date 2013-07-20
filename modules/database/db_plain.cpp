/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/os_session.h"
#include "modules/cs_suspend.h"
#include "modules/bs_kick.h"
#include "modules/cs_log.h"
#include "modules/cs_mode.h"
#include "modules/bs_badwords.h"

Anope::string DatabaseFile;
Anope::string BackupFile;
std::stringstream db_buffer;

class DatabaseException : public CoreException
{
 public:
	DatabaseException(const Anope::string &reason = "") : CoreException(reason) { }

	virtual ~DatabaseException() throw() { }
};

/** Enum used for what METADATA type we are reading
 */
enum MDType
{
	MD_NONE,
	MD_NC,
	MD_NA,
	MD_BI,
	MD_CH
};

static void LoadNickCore(const std::vector<Anope::string> &params);
static void LoadNickAlias(const std::vector<Anope::string> &params);
static void LoadBotInfo(const std::vector<Anope::string> &params);
static void LoadChanInfo(const std::vector<Anope::string> &params);
static void LoadOperInfo(const std::vector<Anope::string> &params);

EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
{
	Anope::string key = params[0];
	std::vector<Anope::string> otherparams = params;
	otherparams.erase(otherparams.begin());

	if (key.equals_ci("NC"))
		LoadNickCore(otherparams);
	else if (key.equals_ci("NA"))
		LoadNickAlias(otherparams);
	else if (key.equals_ci("BI"))
		LoadBotInfo(otherparams);
	else if (key.equals_ci("CH"))
		LoadChanInfo(otherparams);
	else if (key.equals_ci("OS"))
		LoadOperInfo(otherparams);

		return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params)
{
	try
	{
		if (key.equals_ci("LANGUAGE"))
			nc->language = params[0];
		else if (key.equals_ci("MEMOMAX"))
			nc->memos.memomax = params[0].is_pos_number_only() ? convertTo<int16_t>(params[0]) : -1;
		else if (key.equals_ci("EMAIL"))
			nc->email = params[0];
		else if (key.equals_ci("GREET"))
			nc->Extend<Anope::string>("greet", params[0]);
		else if (key.equals_ci("ACCESS"))
			nc->AddAccess(params[0]);
		else if (key.equals_ci("FLAGS"))
		{
			for (unsigned i = 0; i < params.size(); ++i)
				nc->Extend<bool>(params[i]);
		}
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			m->time = params[0].is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
			m->sender = params[1];
			for (unsigned j = 2; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->unread = true;
				else if (params[j].equals_ci("RECEIPT"))
					m->receipt = true;
			}
			m->text = params[params.size() - 1];
			nc->memos.memos->push_back(m);
		}
		else if (key.equals_ci("MIG"))
			nc->memos.ignores.push_back(params[0].ci_str());
	}
	catch (const ConvertException &ex)
	{
		throw DatabaseException(ex.GetReason());
	}
	return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(NickAlias *na, const Anope::string &key, const std::vector<Anope::string> &params)
{
	if (key.equals_ci("LAST_USERMASK"))
		na->last_usermask = params[0];
	else if (key.equals_ci("LAST_REALHOST"))
		na->last_realhost = params[0];
	else if (key.equals_ci("LAST_REALNAME"))
		na->last_realname = params[0];
	else if (key.equals_ci("LAST_QUIT"))
		na->last_quit = params[0];
	else if (key.equals_ci("FLAGS"))
		for (unsigned i = 0; i < params.size(); ++i)
			na->Extend<bool>(params[i]);
	else if (key.equals_ci("VHOST"))
		na->SetVhost(params.size() > 3 ? params[3] : "", params[2], params[0], params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0);
	return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(BotInfo *bi, const Anope::string &key, const std::vector<Anope::string> &params)
{
	return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
{
	KickerData *kd = ci->GetExt<KickerData>("kickerdata");
	try
	{
		if (key.equals_ci("BANTYPE"))
			ci->bantype = params[0].is_pos_number_only() ? convertTo<int16_t>(params[0]) : 2;
		else if (key.equals_ci("MEMOMAX"))
			ci->memos.memomax = params[0].is_pos_number_only() ? convertTo<int16_t>(params[0]) : -1;
		else if (key.equals_ci("FOUNDER"))
			ci->SetFounder(NickCore::Find(params[0]));
		else if (key.equals_ci("SUCCESSOR"))
			ci->SetSuccessor(NickCore::Find(params[0]));
		else if (key.equals_ci("LEVELS"))
		{
			for (unsigned j = 0, end = params.size(); j < end; j += 2)
			{
				Privilege *p = PrivilegeManager::FindPrivilege(params[j]);
				if (p == NULL)
					continue;
				ci->SetLevel(p->name, params[j + 1].is_number_only() ? convertTo<int16_t>(params[j + 1]) : 0);
			}
		}
		else if (key.equals_ci("FLAGS"))
			for (unsigned i = 0; i < params.size(); ++i)
				ci->Extend<bool>(params[i]);
		else if (key.equals_ci("DESC"))
			ci->desc = params[0];
		else if (key.equals_ci("TOPIC"))
		{
			ci->last_topic_setter = params[0];
			ci->last_topic_time = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
			ci->last_topic = params[2];
		}
		else if (key.equals_ci("SUSPEND"))
		{
			CSSuspendInfo *si = ci->Extend<CSSuspendInfo>("suspend");
			si->by = params[0];
			si->reason = params[1];
		}
		else if (key.equals_ci("ACCESS")) // Older access system, from Anope 1.9.4.
		{
			ServiceReference<AccessProvider> provider("AccessProvider", "access/access");
			if (!provider)
				throw DatabaseException("Old access entry for nonexistant provider");

			ChanAccess *access = const_cast<ChanAccess *>(provider->Create());
			access->ci = ci;
			access->mask = params[0];
			access->AccessUnserialize(params[1]);
			access->last_seen = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
			access->creator = params[3];
			access->created = Anope::CurTime;

			ci->AddAccess(access);
		}
		else if (key.equals_ci("ACCESS2"))
		{
			ServiceReference<AccessProvider> provider("AccessProvider", params[0]);
			if (!provider)
				throw DatabaseException("Access entry for nonexistant provider " + params[0]);

			ChanAccess *access = const_cast<ChanAccess *>(provider->Create());
			access->ci = ci;
			access->mask = params[1];
			access->AccessUnserialize(params[2]);
			access->last_seen = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
			access->creator = params[4];
			access->created = params.size() > 5 && params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : Anope::CurTime;

			ci->AddAccess(access);
		}
		else if (key.equals_ci("AKICK"))
		{
			// 0 is the old stuck
			bool Nick = params[1].equals_ci("NICK");
			NickCore *nc = NULL;
			if (Nick)
			{
				nc = NickCore::Find(params[2]);
				if (!nc)
					throw DatabaseException("Akick for nonexistant core " + params[2] + " on " + ci->name);
			}

			AutoKick *ak;
			if (Nick)
				ak = ci->AddAkick(params[3], nc, params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);
			else
				ak = ci->AddAkick(params[3], params[2], params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);

		}
		else if (key.equals_ci("LOG"))
		{
			LogSettings *ls = ci->Require<LogSettings>("logsettings");
			if (ls)
			{
				LogSetting *l = ls->Create();

				l->chan = ci->name;
				l->service_name = params[0];
				l->command_service = params[1];
				l->command_name = params[2];
				l->method = params[3];
				l->creator = params[4];
				l->created = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : Anope::CurTime;
				l->extra = params.size() > 6 ? params[6] : "";

				(*ls)->push_back(l);
			}
		}
		else if (key.equals_ci("MLOCK"))
		{
			bool set = params[0] == "1" ? true : false;
			Anope::string mode_name = params[1].substr(6);
			Anope::string setter = params[2];
			time_t mcreated = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : Anope::CurTime;
			Anope::string param = params.size() > 4 ? params[4] : "";
			ModeLocks *ml = ci->Require<ModeLocks>("modelocks");
			if (ml)
				ml->SetMLock(ModeManager::FindChannelModeByName(mode_name), set, param, setter, mcreated);
		}
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			m->time = params[0].is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
			m->sender = params[1];
			for (unsigned j = 2; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->unread = true;
				else if (params[j].equals_ci("RECEIPT"))
					m->receipt = true;
			}
			m->text = params[params.size() - 1];
			ci->memos.memos->push_back(m);
		}
		else if (key.equals_ci("MIG"))
			ci->memos.ignores.push_back(params[0].ci_str());
		else if (key.equals_ci("BI"))
		{
			if (params[0].equals_ci("NAME"))
				ci->bi = BotInfo::Find(params[1]);
			else if (params[0].equals_ci("FLAGS"))
				for (unsigned i = 0; i < params.size(); ++i)
					ci->Extend<bool>(params[i]);
			else if (params[0].equals_ci("TTB"))
			{
				for (unsigned j = 1, end = params.size(); j < end && kd; j += 2)
				{
					if (params[j].equals_ci("BOLDS"))
						kd->ttb[0] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("COLORS"))
						kd->ttb[1] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REVERSES"))
						kd->ttb[2] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("UNDERLINES"))
						kd->ttb[3] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("BADWORDS"))
						kd->ttb[4] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("CAPS"))
						kd->ttb[5] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("FLOOD"))
						kd->ttb[6] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REPEAT"))
						kd->ttb[7] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("ITALICS"))
						kd->ttb[8] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("AMSGS"))
						kd->ttb[9] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
				}
			}
			else if (kd && params[0].equals_ci("CAPSMIN"))
				kd->capsmin = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (kd && params[0].equals_ci("CAPSPERCENT"))
				kd->capspercent = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (kd && params[0].equals_ci("FLOODLINES"))
				kd->floodlines = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (kd && params[0].equals_ci("FLOODSECS"))
				kd->floodsecs = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (kd && params[0].equals_ci("REPEATTIMES"))
				kd->repeattimes = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (kd && params[0].equals_ci("BADWORD"))
			{
				BadWordType Type;
				if (params[1].equals_ci("SINGLE"))
					Type = BW_SINGLE;
				else if (params[1].equals_ci("START"))
					Type = BW_START;
				else if (params[1].equals_ci("END"))
					Type = BW_END;
				else
					Type = BW_ANY;
				BadWords *bw = ci->Require<BadWords>("badwords");
				if (bw)
					bw->AddBadWord(params[2], Type);
			}
		}
	}
	catch (const ConvertException &ex)
	{
		throw DatabaseException(ex.GetReason());
	}
	return EVENT_CONTINUE;
}

/* Character for newlines, in my research I have concluded it is significantly faster
 * to use \n instead of std::endl because std::endl also flushes the buffer, which
 * is not necessary here
 */
static const char endl = '\n';

/** Read from the database and call the events
 * @param m If specified it only calls the read events for the specific module
 */
static void ReadDatabase(Module *m = NULL)
{
	//EventReturn MOD_RESULT;
	MDType Type = MD_NONE;

	std::fstream db;
	db.open(DatabaseFile.c_str(), std::ios_base::in);

	if (!db.is_open())
	{
		Log() << "Unable to open " << DatabaseFile << " for reading!";
		return;
	}

	NickCore *nc = NULL;
	NickAlias *na = NULL;
	BotInfo *bi = NULL;
	ChannelInfo *ci = NULL;

	Anope::string buf;
	while (std::getline(db, buf.str()))
	{
		if (buf.empty())
			continue;

		spacesepstream sep(buf);
		std::vector<Anope::string> params;
		while (sep.GetToken(buf))
		{
			if (buf[0] == ':')
			{
				buf.erase(buf.begin());
				if (!buf.empty() && !sep.StreamEnd())
					params.push_back(buf + " " + sep.GetRemaining());
				else if (!sep.StreamEnd())
					params.push_back(sep.GetRemaining());
				else if (!buf.empty())
					params.push_back(buf);
				break;
			}
			else
				params.push_back(buf);
		}

		/*if (m)
			MOD_RESULT = m->OnDatabaseRead(params);
		else
			FOREACH_RESULT(OnDatabaseRead, MOD_RESULT, (params));
		if (MOD_RESULT == EVENT_STOP)
			continue;*/
		OnDatabaseRead(params);

		if (!params.empty())
		{
			if (params[0].equals_ci("NC"))
			{
				nc = NickCore::Find(params[1]);
				Type = MD_NC;
			}
			else if (params[0].equals_ci("NA"))
			{
				na = NickAlias::Find(params[2]);
				Type = MD_NA;
			}
			else if (params[0].equals_ci("BI"))
			{
				bi = BotInfo::Find(params[1]);
				Type = MD_BI;
			}
			else if (params[0].equals_ci("CH"))
			{
				ci = ChannelInfo::Find(params[1]);
				Type = MD_CH;
			}
			else if (params[0].equals_ci("MD"))
			{
				Anope::string key = params[1];
				params.erase(params.begin());
				params.erase(params.begin());

				if (Type == MD_NC && nc)
				{
					try
					{
						OnDatabaseReadMetadata(nc, key, params);
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
					}
				}
				else if (Type == MD_NA && na)
				{
					try
					{
						OnDatabaseReadMetadata(na, key, params);
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
					}
				}
				else if (Type == MD_BI && bi)
				{
					try
					{
						OnDatabaseReadMetadata(bi, key, params);
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
					}
				}
				else if (Type == MD_CH && ci)
				{
					try
					{
						OnDatabaseReadMetadata(ci, key, params);
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
					}
				}
			}
		}
	}

	db.close();
}

static void LoadNickCore(const std::vector<Anope::string> &params)
{
	NickCore *nc = new NickCore(params[0]);

	nc->pass = params.size() > 1 ? params[1] : "";

	Log(LOG_DEBUG_2) << "[db_plain]: Loaded NickCore " << nc->display;
}

static void LoadNickAlias(const std::vector<Anope::string> &params)
{
	NickCore *nc = NickCore::Find(params[0]);
	if (!nc)
	{
		Log() << "[db_plain]: Unable to find core " << params[0];
		return;
	}

	NickAlias *na = new NickAlias(params[1], nc);

	na->time_registered = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;

	na->last_seen = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;

	Log(LOG_DEBUG_2) << "[db_plain}: Loaded nickalias for " << na->nick;
}

static void LoadBotInfo(const std::vector<Anope::string> &params)
{
	BotInfo *bi = BotInfo::Find(params[0]);
	if (!bi)
		bi = new BotInfo(params[0], params[1], params[2]);

	bi->created = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
	//bi->chancount = params[4].is_pos_number_only() ? convertTo<uint32_t>(params[4]) : 0;
	bi->realname = params[5];

	Log(LOG_DEBUG_2) << "[db_plain]: Loaded botinfo for " << bi->nick;
}

static void LoadChanInfo(const std::vector<Anope::string> &params)
{
	ChannelInfo *ci = new ChannelInfo(params[0]);

	ci->time_registered = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;

	ci->last_used = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;

	Log(LOG_DEBUG_2) << "[db_plain]: loaded channel " << ci->name;
}

static void LoadOperInfo(const std::vector<Anope::string> &params)
{
	if (params[0].equals_ci("STATS"))
	{
		MaxUserCount = params[1].is_pos_number_only() ? convertTo<uint32_t>(params[1]) : 0;
		MaxUserTime = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
	}
	else if (params[0].equals_ci("SXLINE"))
	{
		char type = params[1][0];
		for (std::list<XLineManager *>::iterator it = XLineManager::XLineManagers.begin(), it_end = XLineManager::XLineManagers.end(); it != it_end; ++it)
		{
			XLineManager *xl = *it;
			if (xl->Type() == type)
			{
				Anope::string mask = params[2] + "@" + params[3];
				Anope::string by = params[4];
				time_t seton = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0;
				time_t expires = params[6].is_pos_number_only() ? convertTo<time_t>(params[6]) : 0;
				Anope::string reason = params[7];

				XLine *x = new XLine(mask, by, expires, reason, XLineManager::GenerateUID());
				x->created = seton;
				xl->AddXLine(x);
				break;
			}
		}
	}
	else if (params[0].equals_ci("EXCEPTION") && session_service)
	{
		Exception *exception = new Exception();
		exception->mask = params[1];
		exception->limit = params[2].is_pos_number_only() ? convertTo<int>(params[2]) : 1;
		exception->who = params[3];
		exception->time = params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0;
		exception->expires = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0;
		exception->reason = params[6];
		session_service->AddException(exception);
	}
}

void Write(const Anope::string &buf)
{
	db_buffer << buf << endl;
}

void WriteMetadata(const Anope::string &key, const Anope::string &data)
{
	Write("MD " + key + " " + data);
}

class DBPlain : public Module
{
	/* Day the last backup was on */
	int LastDay;
	/* Backup file names */
	std::list<Anope::string> Backups;
 public:
	DBPlain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
	{

		LastDay = 0;
	}

	void BackupDatabase()
	{
		/* Do not backup a database that doesn't exist */
		if (!IsFile(DatabaseFile))
			return;

		time_t now = Anope::CurTime;
		tm *tm = localtime(&now);

		if (tm->tm_mday != LastDay)
		{
			LastDay = tm->tm_mday;
			Anope::string newname = BackupFile + "-" + stringify(tm->tm_year + 1900) + Anope::printf("-%02i-", tm->tm_mon + 1) + Anope::printf("%02i", tm->tm_mday);

			/* Backup already exists */
			if (IsFile(newname))
				return;

			Log(LOG_DEBUG) << "db_plain: Attempting to rename " << DatabaseFile << " to " << newname;
			if (rename(DatabaseFile.c_str(), newname.c_str()))
			{
				Log() << "Unable to back up database!";

				if (!Config->GetModule(this)->Get<bool>("nobackupok"))
					Anope::Quitting = true;

				return;
			}

			Backups.push_back(newname);

			unsigned KeepBackups = Config->GetModule(this)->Get<unsigned>("keepbackups");
			if (KeepBackups && Backups.size() > KeepBackups)
			{
				unlink(Backups.front().c_str());
				Backups.pop_front();
			}
		}
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		DatabaseFile = Anope::DataDir + "/" + conf->GetModule(this)->Get<const Anope::string>("database");
		if (DatabaseFile.empty())
			DatabaseFile = "anope.db";
		BackupFile = Anope::DataDir + "/backups/" + DatabaseFile;
	}

	EventReturn OnLoadDatabase() anope_override
	{
		ReadDatabase();

		return EVENT_STOP;
	}
};

MODULE_INIT(DBPlain)
