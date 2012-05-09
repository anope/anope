/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "../commands/os_session.h"
#include <unistd.h>

Anope::string DatabaseFile;
Anope::string BackupFile;
std::stringstream db_buffer;

struct ExtensibleString : Anope::string, ExtensibleItem
{
	ExtensibleString(const Anope::string &s) : Anope::string(s) { }
};

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
			nc->greet = params[0];
		else if (key.equals_ci("ACCESS"))
			nc->AddAccess(params[0]);
		else if (key.equals_ci("CERT"))
			nc->AddCert(params[0]);
		else if (key.equals_ci("FLAGS"))
			nc->FromVector(params);
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			m->time = params[0].is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
			m->sender = params[1];
			for (unsigned j = 2; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->SetFlag(MF_UNREAD);
				else if (params[j].equals_ci("RECEIPT"))
					m->SetFlag(MF_RECEIPT);
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
		na->FromVector(params);
	else if (key.equals_ci("VHOST"))
		na->SetVhost(params.size() > 3 ? params[3] : "", params[2], params[0], params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0);
	return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(BotInfo *bi, const Anope::string &key, const std::vector<Anope::string> &params)
{
	if (key.equals_ci("FLAGS"))
		bi->FromVector(params);

	return EVENT_CONTINUE;
}

EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
{
	try
	{
		if (key.equals_ci("BANTYPE"))
			ci->bantype = params[0].is_pos_number_only() ? convertTo<int16_t>(params[0]) : Config->CSDefBantype;
		else if (key.equals_ci("MEMOMAX"))
			ci->memos.memomax = params[0].is_pos_number_only() ? convertTo<int16_t>(params[0]) : -1;
		else if (key.equals_ci("FOUNDER"))
			ci->SetFounder(findcore(params[0]));
		else if (key.equals_ci("SUCCESSOR"))
			ci->successor = findcore(params[0]);
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
			ci->FromVector(params);
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
			ci->Extend("suspend_by", new ExtensibleString(params[0]));
			ci->Extend("suspend_reason", new ExtensibleString(params[1]));
		}
		else if (key.equals_ci("ACCESS")) // Older access system, from Anope 1.9.4.
		{
			service_reference<AccessProvider> provider("AccessProvider", "access/access");
			if (!provider)
				throw DatabaseException("Old access entry for nonexistant provider");

			ChanAccess *access = const_cast<ChanAccess *>(provider->Create());
			access->ci = ci;
			access->mask = params[0];
			access->Unserialize(params[1]);
			access->last_seen = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
			access->creator = params[3];
			access->created = Anope::CurTime;

			ci->AddAccess(access);
		}
		else if (key.equals_ci("ACCESS2"))
		{
			service_reference<AccessProvider> provider("AccessProvider", params[0]);
			if (!provider)
				throw DatabaseException("Access entry for nonexistant provider " + params[0]);

			ChanAccess *access = const_cast<ChanAccess *>(provider->Create());
			access->ci = ci;
			access->mask = params[1];
			access->Unserialize(params[2]);
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
				nc = findcore(params[2]);
				if (!nc)
					throw DatabaseException("Akick for nonexistant core " + params[2] + " on " + ci->name);
			}

			AutoKick *ak;
			if (Nick)
				ak = ci->AddAkick(params[3], nc, params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);
			else
				ak = ci->AddAkick(params[3], params[2], params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);
			if (Nick)
				ak->SetFlag(AK_ISNICK);

		}
		else if (key.equals_ci("LOG"))
		{
			LogSetting *l = new LogSetting();

			l->ci = ci;
			l->service_name = params[0];
			l->command_service = params[1];
			l->command_name = params[2];
			l->method = params[3];
			l->creator = params[4];
			l->created = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : Anope::CurTime;
			l->extra = params.size() > 6 ? params[6] : "";

			ci->log_settings->push_back(l);
		}
		else if (key.equals_ci("MLOCK"))
		{
			bool set = params[0] == "1" ? true : false;
			Anope::string mode_name = params[1];
			Anope::string setter = params[2];
			time_t mcreated = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : Anope::CurTime;
			Anope::string param = params.size() > 4 ? params[4] : "";
			for (size_t i = CMODE_BEGIN + 1; i < CMODE_END; ++i)
				if (ChannelModeNameStrings[i] == mode_name)
				{
					ChannelModeName n = static_cast<ChannelModeName>(i);
					ci->mode_locks->insert(std::make_pair(n, new ModeLock(ci, set, n, param, setter, mcreated)));
					break;
				}
		}
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			m->time = params[0].is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
			m->sender = params[1];
			for (unsigned j = 2; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->SetFlag(MF_UNREAD);
				else if (params[j].equals_ci("RECEIPT"))
					m->SetFlag(MF_RECEIPT);
			}
			m->text = params[params.size() - 1];
			ci->memos.memos->push_back(m);
		}
		else if (key.equals_ci("MIG"))
			ci->memos.ignores.push_back(params[0].ci_str());
		else if (key.equals_ci("BI"))
		{
			if (params[0].equals_ci("NAME"))
				ci->bi = findbot(params[1]);
			else if (params[0].equals_ci("FLAGS"))
				ci->botflags.FromVector(params);
			else if (params[0].equals_ci("TTB"))
			{
				for (unsigned j = 1, end = params.size(); j < end; j += 2)
				{
					if (params[j].equals_ci("BOLDS"))
						ci->ttb[0] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("COLORS"))
						ci->ttb[1] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REVERSES"))
						ci->ttb[2] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("UNDERLINES"))
						ci->ttb[3] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("BADWORDS"))
						ci->ttb[4] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("CAPS"))
						ci->ttb[5] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("FLOOD"))
						ci->ttb[6] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REPEAT"))
						ci->ttb[7] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("ITALICS"))
						ci->ttb[8] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
					else if (params[j].equals_ci("AMSGS"))
						ci->ttb[9] = params[j + 1].is_pos_number_only() ? convertTo<int16_t>(params[j + 1]) : 0;
				}
			}
			else if (params[0].equals_ci("CAPSMIN"))
				ci->capsmin = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (params[0].equals_ci("CAPSPERCENT"))
				ci->capspercent = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (params[0].equals_ci("FLOODLINES"))
				ci->floodlines = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (params[0].equals_ci("FLOODSECS"))
				ci->floodsecs = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (params[0].equals_ci("REPEATTIMES"))
				ci->repeattimes = params[1].is_pos_number_only() ? convertTo<int16_t>(params[1]) : 0;
			else if (params[0].equals_ci("BADWORD"))
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
				ci->AddBadWord(params[2], Type);
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
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		if (MOD_RESULT == EVENT_STOP)
			continue;*/
		OnDatabaseRead(params);

		if (!params.empty())
		{
			if (params[0].equals_ci("NC"))
			{
				nc = findcore(params[1]);
				Type = MD_NC;
			}
			else if (params[0].equals_ci("NA"))
			{
				na = findnick(params[2]);
				Type = MD_NA;
			}
			else if (params[0].equals_ci("BI"))
			{
				bi = findbot(params[1]);
				Type = MD_BI;
			}
			else if (params[0].equals_ci("CH"))
			{
				ci = cs_findchan(params[1]);
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
	/* Clear default flags */
	nc->ClearFlags();

	nc->pass = params.size() > 1 ? params[1] : "";

	Log(LOG_DEBUG_2) << "[db_plain]: Loaded NickCore " << nc->display;
}

static void LoadNickAlias(const std::vector<Anope::string> &params)
{
	NickCore *nc = findcore(params[0]);
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
	BotInfo *bi = findbot(params[0]);
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
	/* CLear default mlock */
	ci->ClearMLock();
	/* Remove default channel flags */
	ci->ClearFlags();
	ci->botflags.ClearFlags();

	ci->time_registered = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;

	ci->last_used = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;

	Log(LOG_DEBUG_2) << "[db_plain]: loaded channel " << ci->name;
}

static void LoadOperInfo(const std::vector<Anope::string> &params)
{
	if (params[0].equals_ci("STATS"))
	{
		maxusercnt = params[1].is_pos_number_only() ? convertTo<uint32_t>(params[1]) : 0;
		maxusertime = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
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
				x->Created = seton;
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
	DBPlain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnLoadDatabase, I_OnSaveDatabase };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();

		LastDay = 0;
	}

	~DBPlain()
	{
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
			Anope::string newname = BackupFile + "." + stringify(tm->tm_year) + stringify(tm->tm_mon) + stringify(tm->tm_mday);

			/* Backup already exists */
			if (IsFile(newname))
				return;

			Log(LOG_DEBUG) << "db_plain: Attemping to rename " << DatabaseFile << " to " << newname;
			if (rename(DatabaseFile.c_str(), newname.c_str()))
			{
				Log() << "Unable to back up database!";

				if (!Config->NoBackupOkay)
					quitting = true;

				return;
			}

			Backups.push_back(newname);

			unsigned KeepBackups = Config->KeepBackups;
			if (KeepBackups && Backups.size() > KeepBackups)
			{
				unlink(Backups.front().c_str());
				Backups.pop_front();
			}
		}
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		DatabaseFile = db_dir + "/" + config.ReadValue("db_plain", "database", "anope.db", 0);
		DatabaseFile = db_dir + "/backups/" + config.ReadValue("db_plain", "database", "anope.db", 0);
	}

	EventReturn OnLoadDatabase() anope_override
	{
		ReadDatabase();

		/* No need to ever reload this again, although this should never be trigged again */
		ModuleManager::Detach(I_OnLoadDatabase, this);

		return EVENT_STOP;
	}


	EventReturn OnSaveDatabase() anope_override
	{
		BackupDatabase();

		db_buffer << "VER 2" << endl;

		for (nickcore_map::const_iterator nit = NickCoreList->begin(), nit_end = NickCoreList->end(); nit != nit_end; ++nit)
		{
			const NickCore *nc = nit->second;

			db_buffer << "NC " << nc->display << " " << nc->pass << endl;

			db_buffer << "MD MEMOMAX " << nc->memos.memomax << endl;

			if (!nc->language.empty())
				db_buffer << "MD LANGUAGE " << nc->language << endl;
			if (!nc->email.empty())
				db_buffer << "MD EMAIL " << nc->email << endl;
			if (!nc->greet.empty())
				db_buffer << "MD GREET :" << nc->greet << endl;

			if (!nc->access.empty())
			{
				for (std::vector<Anope::string>::const_iterator it = nc->access.begin(), it_end = nc->access.end(); it != it_end; ++it)
					db_buffer << "MD ACCESS " << *it << endl;
			}
			if (!nc->cert.empty())
			{
				for (std::vector<Anope::string>::const_iterator it = nc->cert.begin(), it_end = nc->cert.end(); it != it_end; ++it)
					db_buffer << "MD CERT " << *it << endl;
			}
			if (nc->FlagCount())
				db_buffer << "MD FLAGS " << nc->ToString() << endl;
			const MemoInfo *mi = &nc->memos;
			for (unsigned k = 0, end = mi->memos->size(); k < end; ++k)
			{
				const Memo *m = mi->GetMemo(k);
				db_buffer << "MD MI " << m->time << " " << m->sender;
				if (m->HasFlag(MF_UNREAD))
					db_buffer << " UNREAD";
				if (m->HasFlag(MF_RECEIPT))
					db_buffer << " RECEIPT";
				db_buffer << " :" << m->text << endl;
			}
			for (unsigned k = 0, end = mi->ignores.size(); k < end; ++k)
				db_buffer << "MD MIG " << Anope::string(mi->ignores[k]) << endl;
			//FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, nc));
		}

		for (nickalias_map::const_iterator it = NickAliasList->begin(), it_end = NickAliasList->end(); it != it_end; ++it)
		{
			const NickAlias *na = it->second;

			db_buffer << "NA " << na->nc->display << " " << na->nick << " " << na->time_registered << " " << na->last_seen << endl;
			if (!na->last_usermask.empty())
				db_buffer << "MD LAST_USERMASK " << na->last_usermask << endl;
			if (!na->last_realhost.empty())
				db_buffer << "MD LAST_REALHOST " << na->last_realhost << endl;
			if (!na->last_realname.empty())
				db_buffer << "MD LAST_REALNAME :" << na->last_realname << endl;
			if (!na->last_quit.empty())
				db_buffer << "MD LAST_QUIT :" << na->last_quit << endl;
			if (na->FlagCount())
				db_buffer << "MD FLAGS " << na->ToString() << endl;
			if (na->HasVhost())
				db_buffer << "MD VHOST " << na->GetVhostCreator() << " " << na->GetVhostCreated() << " " << na->GetVhostHost() << " :" << na->GetVhostIdent() << endl;

			//FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, na));
		}

		for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			if (bi->HasFlag(BI_CONF))
				continue;

			db_buffer << "BI " << bi->nick << " " << bi->GetIdent() << " " << bi->host << " " << bi->created << " " << bi->GetChannelCount() << " :" << bi->realname << endl;
			if (bi->FlagCount())
				db_buffer << "MD FLAGS " << bi->ToString() << endl;
		}

		for (registered_channel_map::const_iterator cit = RegisteredChannelList->begin(), cit_end = RegisteredChannelList->end(); cit != cit_end; ++cit)
		{
			const ChannelInfo *ci = cit->second;

			db_buffer << "CH " << ci->name << " " << ci->time_registered << " " << ci->last_used << endl;
			db_buffer << "MD BANTYPE " << ci->bantype << endl;
			db_buffer << "MD MEMOMAX " << ci->memos.memomax << endl;
			if (ci->GetFounder())
				db_buffer << "MD FOUNDER " << ci->GetFounder()->display << endl;
			if (ci->successor)
				db_buffer << "MD SUCCESSOR " << ci->successor->display << endl;
			if (!ci->desc.empty())
				db_buffer << "MD DESC :" << ci->desc << endl;
			if (!ci->last_topic.empty())
				db_buffer << "MD TOPIC " << ci->last_topic_setter << " " << ci->last_topic_time << " :" << ci->last_topic << endl;
			db_buffer << "MD LEVELS";
			const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
			for (unsigned i = 0; i < privs.size(); ++i)
			{
				const Privilege &p = privs[i];
				db_buffer << p.name << " " << ci->GetLevel(p.name);
			}
			db_buffer << endl;
			if (ci->FlagCount())
				db_buffer << "MD FLAGS " << ci->ToString() << endl;
			if (ci->HasFlag(CI_SUSPENDED))
			{
				Anope::string *by = ci->GetExt<ExtensibleString *>("suspend_by"), *reason = ci->GetExt<ExtensibleString *>("suspend_reason");
				if (by && reason)
					db_buffer << "MD SUSPEND " << *by << " :" << *reason << endl;
			}
			for (unsigned k = 0, end = ci->GetAccessCount(); k < end; ++k)
			{
				const ChanAccess *access = ci->GetAccess(k);
				db_buffer << "MD ACCESS2 " << access->provider->name << " " << access->mask << " " << access->Serialize() << " " << access->last_seen << " " << access->creator << " " << access->created << endl;
			}
			for (unsigned k = 0, end = ci->GetAkickCount(); k < end; ++k)
			{
				db_buffer << "MD AKICK 0 " << (ci->GetAkick(k)->HasFlag(AK_ISNICK) ? "NICK " : "MASK ") <<
					(ci->GetAkick(k)->HasFlag(AK_ISNICK) ? ci->GetAkick(k)->nc->display : ci->GetAkick(k)->mask) << " " << ci->GetAkick(k)->creator << " " << ci->GetAkick(k)->addtime << " " << ci->last_used << " :";
				if (!ci->GetAkick(k)->reason.empty())
					db_buffer << ci->GetAkick(k)->reason;
				db_buffer << endl;
			}
			for (unsigned k = 0, end = ci->log_settings->size(); k < end; ++k)
			{
				const LogSetting &l = *ci->log_settings->at(k);

				db_buffer << "MD LOG " << l.service_name << " " << l.command_service << " " << l.command_name << " " << l.method << " " << l.creator << " " << l.created << " " << l.extra << endl;
			}
			for (ChannelInfo::ModeList::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
			{
				const ModeLock &ml = *it->second;
				ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
				if (cm != NULL)
					db_buffer << "MD MLOCK " << (ml.set ? 1 : 0) << " " << cm->NameAsString() << " " << ml.setter << " " << ml.created << " " << ml.param << endl;
			}
			const MemoInfo *memos = &ci->memos;
			for (unsigned k = 0, end = memos->memos->size(); k < end; ++k)
			{
				const Memo *m = memos->GetMemo(k);
				db_buffer << "MD MI " << m->time << " " << m->sender;
				if (m->HasFlag(MF_UNREAD))
					db_buffer << " UNREAD";
				if (m->HasFlag(MF_RECEIPT))
					db_buffer << " RECEIPT";
				db_buffer << " :" << m->text << endl;
			}
			for (unsigned k = 0, end = memos->ignores.size(); k < end; ++k)
				db_buffer << "MD MIG " << Anope::string(memos->ignores[k]) << endl;
			if (ci->bi)
				db_buffer << "MD BI NAME " << ci->bi->nick << endl;
			if (ci->botflags.FlagCount())
				db_buffer << "MD BI FLAGS " << ci->botflags.ToString() << endl;
			db_buffer << "MD BI TTB BOLDS " << ci->ttb[0] << " COLORS " << ci->ttb[1] << " REVERSES " << ci->ttb[2] << " UNDERLINES " << ci->ttb[3] << " BADWORDS " << ci->ttb[4] << " CAPS " << ci->ttb[5] << " FLOOD " << ci->ttb[6] << " REPEAT " << ci->ttb[7] << " ITALICS " << ci->ttb[8] << " AMSGS " << ci->ttb[9] << endl;
			if (ci->capsmin)
				db_buffer << "MD BI CAPSMIN " << ci->capsmin << endl;
			if (ci->capspercent)
				db_buffer << "MD BI CAPSPERCENT " << ci->capspercent << endl;
			if (ci->floodlines)
				db_buffer << "MD BI FLOODLINES " << ci->floodlines << endl;
			if (ci->floodsecs)
				db_buffer << "MD BI FLOODSECS " << ci->floodsecs << endl;
			if (ci->repeattimes)
				db_buffer << "MD BI REPEATTIMES " << ci->repeattimes << endl;
			for (unsigned k = 0, end = ci->GetBadWordCount(); k < end; ++k)
				db_buffer << "MD BI BADWORD " << (ci->GetBadWord(k)->type == BW_ANY ? "ANY " : "") << (ci->GetBadWord(k)->type == BW_SINGLE ? "SINGLE " : "") << (ci->GetBadWord(k)->type == BW_START ? "START " : "") <<
					(ci->GetBadWord(k)->type == BW_END ? "END " : "") << ":" << ci->GetBadWord(k)->word << endl;

			//FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, ci));
		}

		db_buffer << "OS STATS " << maxusercnt << " " << maxusertime << endl;

		for (std::list<XLineManager *>::iterator it = XLineManager::XLineManagers.begin(), it_end = XLineManager::XLineManagers.end(); it != it_end; ++it)
		{
			XLineManager *xl = *it;
			for (unsigned i = 0, end = xl->GetCount(); i < end; ++i)
			{
				const XLine *x = xl->GetEntry(i);
				db_buffer << "OS SXLINE " << xl->Type() << " " << x->GetUser() << " " << x->GetHost() << " " << x->By << " " << x->Created << " " << x->Expires << " :" << x->Reason << endl;
			}
		}

		if (session_service)
			for (SessionService::ExceptionVector::iterator it = session_service->GetExceptions().begin(); it != session_service->GetExceptions().end(); ++it)
			{
				Exception *e = *it;
				db_buffer << "OS EXCEPTION " << e->mask << " " << e->limit << " " << e->who << " " << e->time << " " << e->expires << " " << e->reason << endl;
			}

		//FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		std::fstream db;
		db.open(DatabaseFile.c_str(), std::ios_base::out | std::ios_base::trunc);

		if (!db.is_open())
		{
			ircdproto->SendGlobops(NULL, "Unable to open %s for writing!", DatabaseFile.c_str());
			return EVENT_CONTINUE;
		}

		db << db_buffer.str();
		db_buffer.str("");

		db.close();

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(DBPlain)
