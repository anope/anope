/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

std::fstream db;
Anope::string DatabaseFile;

/** Enum used for what METADATA type we are reading
 */
enum MDType
{
	MD_NONE,
	MD_NC,
	MD_NA,
	MD_NR,
	MD_BI,
	MD_CH
};

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
	EventReturn MOD_RESULT;
	MDType Type = MD_NONE;

	db.clear();
	db.open(DatabaseFile.c_str(), std::ios_base::in);

	if (!db.is_open())
	{
		Log() << "Unable to open " << DatabaseFile << " for reading!";
		return;
	}

	NickCore *nc = NULL;
	NickAlias *na = NULL;
	NickRequest *nr = NULL;
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

		if (m)
			MOD_RESULT = m->OnDatabaseRead(params);
		else
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		if (MOD_RESULT == EVENT_STOP)
			continue;

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
			else if (params[0].equals_ci("NR"))
			{
				nr = findrequestnick(params[1]);
				Type = MD_NR;
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
						if (m)
							m->OnDatabaseReadMetadata(nc, key, params);
						else
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nc, key, params));
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
						if (m)
							m->OnDatabaseReadMetadata(na, key, params);
						else
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, key, params));
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
					}
				}
				else if (Type == MD_NR && nr)
				{
					try
					{
						if (m)
							m->OnDatabaseReadMetadata(nr, key, params);
						else
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nr, key, params));
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
						if (m)
							m->OnDatabaseReadMetadata(bi, key, params);
						else
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(bi, key, params));
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
						if (m)
							m->OnDatabaseReadMetadata(ci, key, params);
						else
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, key, params));
					}
					catch (const DatabaseException &ex)
					{
						Log() << "[db_plain]: " << ex.GetReason();
						if (!ci->founder)
						{
							delete ci;
							ci = NULL;
						}
					}
				}
			}
		}
	}

	db.close();
}

struct ChannelFlagInfo
{
	Anope::string Name;
	ChannelInfoFlag Flag;
};

struct ChannelLevel
{
	Anope::string Name;
	int Level;
};

struct BotFlagInfo
{
	Anope::string Name;
	BotServFlag Flag;
};

struct NickCoreFlagInfo
{
	Anope::string Name;
	NickCoreFlag Flag;
};

ChannelFlagInfo ChannelInfoFlags[] = {
	{"KEEPTOPIC", CI_KEEPTOPIC},
	{"SECUREOPS", CI_SECUREOPS},
	{"PRIVATE", CI_PRIVATE},
	{"TOPICLOCK", CI_TOPICLOCK},
	{"RESTRICTED", CI_RESTRICTED},
	{"PEACE", CI_PEACE},
	{"SECURE", CI_SECURE},
	{"FORBIDDEN", CI_FORBIDDEN},
	{"NO_EXPIRE", CI_NO_EXPIRE},
	{"MEMO_HARDMAX", CI_MEMO_HARDMAX},
	{"OPNOTICE", CI_OPNOTICE},
	{"SECUREFOUNDER", CI_SECUREFOUNDER},
	{"SIGNKICK", CI_SIGNKICK},
	{"SIGNKICK_LEVEL", CI_SIGNKICK_LEVEL},
	{"XOP", CI_XOP},
	{"SUSPENDED", CI_SUSPENDED},
	{"PERSIST", CI_PERSIST},
	{"", static_cast<ChannelInfoFlag>(-1)}
};

ChannelLevel ChannelLevels[] = {
	{"INVITE", CA_INVITE},
	{"AKICK", CA_AKICK},
	{"SET", CA_SET},
	{"UNBAN", CA_UNBAN},
	{"AUTOOP", CA_AUTOOP},
	{"AUTODEOP", CA_AUTODEOP},
	{"AUTOVOICE", CA_AUTOVOICE},
	{"OPDEOP", CA_OPDEOP},
	{"ACCESS_LIST", CA_ACCESS_LIST},
	{"CLEAR", CA_CLEAR},
	{"NOJOIN", CA_NOJOIN},
	{"ACCESS_CHANGE", CA_ACCESS_CHANGE},
	{"MEMO", CA_MEMO},
	{"ASSIGN", CA_ASSIGN},
	{"BADWORDS", CA_BADWORDS},
	{"NOKICK", CA_NOKICK},
	{"FANTASIA", CA_FANTASIA},
	{"SAY", CA_SAY},
	{"GREET", CA_GREET},
	{"VOICEME", CA_VOICEME},
	{"VOICE", CA_VOICE},
	{"GETKEY", CA_GETKEY},
	{"AUTOHALFOP", CA_AUTOHALFOP},
	{"AUTOPROTECT", CA_AUTOPROTECT},
	{"OPDEOPME", CA_OPDEOPME},
	{"HALFOPME", CA_HALFOPME},
	{"HALFOP", CA_HALFOP},
	{"PROTECTME", CA_PROTECTME},
	{"PROTECT", CA_PROTECT},
	{"KICKME", CA_KICKME},
	{"KICK", CA_KICK},
	{"SIGNKICK", CA_SIGNKICK},
	{"BANME", CA_BANME},
	{"BAN", CA_BAN},
	{"TOPIC", CA_TOPIC},
	{"INFO", CA_INFO},
	{"AUTOOWNER", CA_AUTOOWNER},
	{"OWNER", CA_OWNER},
	{"OWNERME", CA_OWNERME},
	{"", -1}
};

BotFlagInfo BotFlags[] = {
	{"DONTKICKOPS", BS_DONTKICKOPS},
	{"DONTKICKVOICES", BS_DONTKICKVOICES},
	{"FANTASY", BS_FANTASY},
	{"SYMBIOSIS", BS_SYMBIOSIS},
	{"GREET", BS_GREET},
	{"NOBOT", BS_NOBOT},
	{"KICK_BOLDS", BS_KICK_BOLDS},
	{"KICK_COLORS", BS_KICK_COLORS},
	{"KICK_REVERSES", BS_KICK_REVERSES},
	{"KICK_UNDERLINES", BS_KICK_UNDERLINES},
	{"KICK_BADWORDS", BS_KICK_BADWORDS},
	{"KICK_CAPS", BS_KICK_CAPS},
	{"KICK_FLOOD", BS_KICK_FLOOD},
	{"KICK_REPEAT", BS_KICK_REPEAT},
	{"KICK_ITALICS", BS_KICK_ITALICS},
	{"", static_cast<BotServFlag>(-1)}
};

NickCoreFlagInfo NickCoreFlags[] = {
	{"KILLPROTECT", NI_KILLPROTECT},
	{"SECURE", NI_SECURE},
	{"MSG", NI_MSG},
	{"MEMO_HARDMAX", NI_MEMO_HARDMAX},
	{"MEMO_SIGNON", NI_MEMO_SIGNON},
	{"MEMO_RECEIVE", NI_MEMO_RECEIVE},
	{"PRIVATE", NI_PRIVATE},
	{"HIDE_EMAIL", NI_HIDE_EMAIL},
	{"HIDE_MASK", NI_HIDE_MASK},
	{"HIDE_QUIT", NI_HIDE_QUIT},
	{"KILL_QUICK", NI_KILL_QUICK},
	{"KILL_IMMED", NI_KILL_IMMED},
	{"MEMO_MAIL", NI_MEMO_MAIL},
	{"HIDE_STATUS", NI_HIDE_STATUS},
	{"SUSPENDED", NI_SUSPENDED},
	{"AUTOOP", NI_AUTOOP},
	{"FORBIDDEN", NI_FORBIDDEN},
	{"", static_cast<NickCoreFlag>(-1)}
};

static void LoadNickCore(const std::vector<Anope::string> &params)
{
	NickCore *nc = new NickCore(params[0]);
	/* Clear default flags */
	nc->ClearFlags();

	nc->pass = params[1];

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

static void LoadNickRequest(const std::vector<Anope::string> &params)
{
	NickRequest *nr = new NickRequest(params[0]);
	nr->passcode = params[1];
	nr->password = params[2];
	nr->email = params[3];
	nr->requested = params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0;

	Log(LOG_DEBUG_2) << "[db_plain]: Loaded nickrequest for " << nr->nick;
}

static void LoadBotInfo(const std::vector<Anope::string> &params)
{
	BotInfo *bi = findbot(params[0]);
	if (!bi)
		bi = new BotInfo(params[0], params[1], params[2]);

	bi->created = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
	bi->chancount = params[4].is_pos_number_only() ? convertTo<uint32>(params[4]) : 0;
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
		maxusercnt = params[1].is_pos_number_only() ? convertTo<uint32>(params[1]) : 0;
		maxusertime = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
	}
	else if (params[0].equals_ci("SNLINE") || params[0].equals_ci("SQLINE") || params[0].equals_ci("SZLINE"))
	{
		Anope::string mask = params[1];
		Anope::string by = params[2];
		time_t seton = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
		time_t expires = params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0;
		Anope::string reason = params[5];

		XLine *x = NULL;
		if (params[0].equals_ci("SNLINE") && SNLine)
			x = SNLine->Add(NULL, NULL, mask, expires, reason);
		else if (params[0].equals_ci("SQLINE") && SQLine)
			x = SQLine->Add(NULL, NULL, mask, expires, reason);
		else if (params[0].equals_ci("SZLINE") && SZLine)
			x = SZLine->Add(NULL, NULL, mask, expires, reason);
		if (x)
		{
			x->By = by;
			x->Created = seton;
		}
	}
	else if (params[0].equals_ci("AKILL") && SGLine)
	{
		Anope::string user = params[1];
		Anope::string host = params[2];
		Anope::string by = params[3];
		time_t seton = params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0;
		time_t expires = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0;
		Anope::string reason = params[6];

		XLine *x = SGLine->Add(NULL, NULL, user + "@" + host, expires, reason);
		if (x)
		{
			x->By = by;
			x->Created = seton;
		}
	}
	else if (params[0].equals_ci("EXCEPTION"))
	{
		Exception *exception = new Exception();
		exception->mask = params[1];
		exception->limit = params[2].is_pos_number_only() ? convertTo<int>(params[2]) : 1;
		exception->who = params[3];
		exception->time = params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0;
		exception->expires = params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0;
		exception->reason = params[6];
		exceptions.push_back(exception);
	}
}

void Write(const Anope::string &buf)
{
	db << buf << endl;
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
	DBPlain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(DATABASE);

		Implementation i[] = { I_OnReload, I_OnDatabaseRead, I_OnLoadDatabase, I_OnDatabaseReadMetadata, I_OnSaveDatabase, I_OnModuleLoad };
		ModuleManager::Attach(i, this, 6);

		OnReload(true);

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
			Anope::string newname = "backups/" + DatabaseFile + "." + stringify(tm->tm_year) + stringify(tm->tm_mon) + stringify(tm->tm_mday);

			/* Backup already exists */
			if (IsFile(newname))
				return;

			Log(LOG_DEBUG) << "db_plain: Attemping to rename " << DatabaseFile << " to " << newname;
			if (rename(DatabaseFile.c_str(), newname.c_str()))
			{
				ircdproto->SendGlobops(OperServ, "Unable to backup database!");
				Log() << "Unable to back up database!";

				if (!Config->NoBackupOkay)
					quitting = true;

				return;
			}

			Backups.push_back(newname);

			unsigned KeepBackups = Config->KeepBackups;
			if (KeepBackups && Backups.size() > KeepBackups)
			{
				DeleteFile(Backups.front().c_str());
				Backups.pop_front();
			}
		}
	}

	void OnReload(bool)
	{
		ConfigReader config;
		DatabaseFile = config.ReadValue("db_plain", "database", "anope.db", 0);
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		Anope::string key = params[0];
		std::vector<Anope::string> otherparams = params;
		otherparams.erase(otherparams.begin());

		if (key.equals_ci("NC"))
			LoadNickCore(otherparams);
		else if (key.equals_ci("NA"))
			LoadNickAlias(otherparams);
		else if (key.equals_ci("NR"))
			LoadNickRequest(otherparams);
		else if (key.equals_ci("BI"))
			LoadBotInfo(otherparams);
		else if (key.equals_ci("CH"))
			LoadChanInfo(otherparams);
		else if (key.equals_ci("OS"))
			LoadOperInfo(otherparams);

		return EVENT_CONTINUE;
	}

	EventReturn OnLoadDatabase()
	{
		ReadDatabase();

		/* No need to ever reload this again, although this should never be trigged again */
		ModuleManager::Detach(I_OnLoadDatabase, this);
		ModuleManager::Detach(I_OnDatabaseReadMetadata, this);

		return EVENT_STOP;
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.equals_ci("LANGUAGE"))
			nc->language = params[0];
		else if (key.equals_ci("MEMOMAX"))
			nc->memos.memomax = params[0].is_pos_number_only() ? convertTo<int16>(params[0]) : 1;
		else if (key.equals_ci("CHANCOUNT"))
			nc->channelcount = params[0].is_pos_number_only() ? convertTo<uint16>(params[0]) : 0;
		else if (key.equals_ci("EMAIL"))
			nc->email = params[0];
		else if (key.equals_ci("GREET"))
			nc->greet = params[0];
		else if (key.equals_ci("ACCESS"))
			nc->AddAccess(params[0]);
		else if (key.equals_ci("FLAGS"))
		{
			for (unsigned j = 0, end = params.size(); j < end; ++j)
				for (int i = 0; NickCoreFlags[i].Flag != -1; ++i)
					if (params[j].equals_ci(NickCoreFlags[i].Name))
						nc->SetFlag(NickCoreFlags[i].Flag);
		}
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			// params[0] is the old number of the memo, no longer used
			m->time = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
			m->sender = params[2];
			for (unsigned j = 3; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT") || params[j].equals_ci("NOTIFYS"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->SetFlag(MF_UNREAD);
				else if (params[j].equals_ci("RECEIPT"))
					m->SetFlag(MF_RECEIPT);
				else if (params[j].equals_ci("NOTIFYS"))
					m->SetFlag(MF_NOTIFYS);
			}
			m->text = params[params.size() - 1];
			nc->memos.memos.push_back(m);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(NickAlias *na, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.equals_ci("LAST_USERMASK"))
			na->last_usermask = params[0];
		else if (key.equals_ci("LAST_REALNAME"))
			na->last_realname = params[0];
		else if (key.equals_ci("LAST_QUIT"))
			na->last_quit = params[0];
		else if (key.equals_ci("FLAGS"))
		{
			for (unsigned j = 0, end = params.size(); j < end; ++j)
			{
				if (params[j].equals_ci("FORBIDDEN"))
					na->SetFlag(NS_FORBIDDEN);
				else if (params[j].equals_ci("NOEXPIRE"))
					na->SetFlag(NS_NO_EXPIRE);
			}
		}
		else if (key.equals_ci("VHOST"))
			na->hostinfo.SetVhost(params.size() > 3 ? params[3] : "", params[2], params[0], params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0);

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(BotInfo *bi, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.equals_ci("FLAGS"))
			for (unsigned j = 0, end = params.size(); j < end; ++j)
				if (params[j].equals_ci("PRIVATE"))
					bi->SetFlag(BI_PRIVATE);

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.equals_ci("BANTYPE"))
			ci->bantype = params[0].is_pos_number_only() ? convertTo<int16>(params[0]) : Config->CSDefBantype;
		else if (key.equals_ci("MEMOMAX"))
			ci->memos.memomax = params[0].is_pos_number_only() ? convertTo<int16>(params[0]) : 1;
		else if (key.equals_ci("FOUNDER"))
		{
			ci->founder = findcore(params[0]);
			if (!ci->founder)
			{
				std::stringstream reason;
				reason << "Deleting founderless channel " << ci->name << " (founder: " << params[0] << ")";
				throw DatabaseException(reason.str());
			}
		}
		else if (key.equals_ci("SUCCESSOR"))
			ci->successor = findcore(params[0]);
		else if (key.equals_ci("LEVELS"))
		{
			for (unsigned j = 0, end = params.size(); j < end; j += 2)
				for (int i = 0; ChannelLevels[i].Level != -1; ++i)
					if (params[j].equals_ci(ChannelLevels[i].Name))
						ci->levels[ChannelLevels[i].Level] = params[j + 1].is_number_only() ? convertTo<int16>(params[j + 1]) : 0;
		}
		else if (key.equals_ci("FLAGS"))
		{
			for (unsigned j = 0, end = params.size(); j < end; ++j)
				for (int i = 0; ChannelInfoFlags[i].Flag != -1; ++i)
					if (params[j].equals_ci(ChannelInfoFlags[i].Name))
						ci->SetFlag(ChannelInfoFlags[i].Flag);
		}
		else if (key.equals_ci("DESC"))
			ci->desc = params[0];
		else if (key.equals_ci("TOPIC"))
		{
			ci->last_topic_setter = params[0];
			ci->last_topic_time = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
			ci->last_topic = params[2];
		}
		else if (key.equals_ci("FORBID"))
		{
			ci->forbidby = params[0];
			ci->forbidreason = params[1];
		}
		else if (key.equals_ci("ACCESS"))
		{
			NickCore *nc = findcore(params[0]);
			if (!nc)
			{
				std::stringstream reason;
				reason << "Access entry for nonexistant core " << params[0] << " on " << ci->name;
				throw DatabaseException(reason.str());
			}

			int level = params[1].is_number_only() ? convertTo<int>(params[1]) : 0;
			time_t last_seen = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0;
			ci->AddAccess(nc, level, params[3], last_seen);
		}
		else if (key.equals_ci("AKICK"))
		{
			bool Stuck = params[0].equals_ci("STUCK");
			bool Nick = params[1].equals_ci("NICK");
			NickCore *nc = NULL;
			if (Nick)
			{
				nc = findcore(params[2]);
				if (!nc)
				{
					std::stringstream reason;
					reason << "Akick for nonexistant core " << params[2] << " on " << ci->name;
					throw DatabaseException(reason.str());
				}
			}
			AutoKick *ak;
			if (Nick)
				ak = ci->AddAkick(params[3], nc, params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);
			else
				ak = ci->AddAkick(params[3], params[2], params.size() > 6 ? params[6] : "", params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0, params[5].is_pos_number_only() ? convertTo<time_t>(params[5]) : 0);
			if (Stuck)
				ak->SetFlag(AK_STUCK);
			if (Nick)
				ak->SetFlag(AK_ISNICK);

		}
		else if (key.equals_ci("MLOCK_ON") || key.equals_ci("MLOCK_OFF"))
		{
			bool Set = key.equals_ci("MLOCK_ON");

			/* For now store mlock in extensible, Anope hasn't yet connected to the IRCd and doesn't know what modes exist */
			ci->Extend(Set ? "db_mlock_modes_on" : "db_mlock_modes_off", new ExtensibleItemRegular<std::vector<Anope::string> >(params));
		}
		else if (key.equals_ci("MLP"))
		{
			std::vector<std::pair<Anope::string, Anope::string> > mlp;
			ci->GetExtRegular("db_mlp", mlp);
	
			mlp.push_back(std::make_pair(params[0], params[1]));

			/* For now store mlocked modes in extensible, Anope hasn't yet connected to the IRCd and doesn't know what modes exist */
			ci->Extend("db_mlp", new ExtensibleItemRegular<std::vector<std::pair<Anope::string, Anope::string> > >(mlp));
		}
		else if (key.equals_ci("MI"))
		{
			Memo *m = new Memo;
			// params[0] is the old number of the memo, no longer used
			m->time = params[1].is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
			m->sender = params[2];
			for (unsigned j = 3; params[j].equals_ci("UNREAD") || params[j].equals_ci("RECEIPT") || params[j].equals_ci("NOTIFYS"); ++j)
			{
				if (params[j].equals_ci("UNREAD"))
					m->SetFlag(MF_UNREAD);
				else if (params[j].equals_ci("RECEIPT"))
					m->SetFlag(MF_RECEIPT);
				else if (params[j].equals_ci("NOTIFYS"))
					m->SetFlag(MF_NOTIFYS);
			}
			m->text = params[params.size() - 1];
			ci->memos.memos.push_back(m);
		}
		else if (key.equals_ci("ENTRYMSG"))
			ci->entry_message = params[0];
		else if (key.equals_ci("BI"))
		{
			if (params[0].equals_ci("NAME"))
				ci->bi = findbot(params[1]);
			else if (params[0].equals_ci("FLAGS"))
			{
				for (unsigned j = 1, end = params.size(); j < end; ++j)
					for (int i = 0; BotFlags[i].Flag != -1; ++i)
						if (params[j].equals_ci(BotFlags[i].Name))
							ci->botflags.SetFlag(BotFlags[i].Flag);
			}
			else if (params[0].equals_ci("TTB"))
			{
				for (unsigned j = 1, end = params.size(); j < end; j += 2)
				{
					if (params[j].equals_ci("BOLDS"))
						ci->ttb[0] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("COLORS"))
						ci->ttb[1] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REVERSES"))
						ci->ttb[2] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("UNDERLINES"))
						ci->ttb[3] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("BADWORDS"))
						ci->ttb[4] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("CAPS"))
						ci->ttb[5] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("FLOOD"))
						ci->ttb[6] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("REPEAT"))
						ci->ttb[7] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
					else if (params[j].equals_ci("ITALICS"))
						ci->ttb[8] = params[j + 1].is_pos_number_only() ? convertTo<int16>(params[j + 1]) : 0;
				}
			}
			else if (params[0].equals_ci("CAPSMIN"))
				ci->capsmin = params[1].is_pos_number_only() ? convertTo<int16>(params[1]) : 0;
			else if (params[0].equals_ci("CAPSPERCENT"))
				ci->capspercent = params[1].is_pos_number_only() ? convertTo<int16>(params[1]) : 0;
			else if (params[0].equals_ci("FLOODLINES"))
				ci->floodlines = params[1].is_pos_number_only() ? convertTo<int16>(params[1]) : 0;
			else if (params[0].equals_ci("FLOODSECS"))
				ci->floodsecs = params[1].is_pos_number_only() ? convertTo<int16>(params[1]) : 0;
			else if (params[0].equals_ci("REPEATTIMES"))
				ci->repeattimes = params[1].is_pos_number_only() ? convertTo<int16>(params[1]) : 0;
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

		return EVENT_CONTINUE;
	}

	EventReturn OnSaveDatabase()
	{
		BackupDatabase();

		db.clear();
		db.open(DatabaseFile.c_str(), std::ios_base::out | std::ios_base::trunc);

		if (!db.is_open())
		{
			ircdproto->SendGlobops(NULL, "Unable to open %s for writing!", DatabaseFile.c_str());
			return EVENT_CONTINUE;
		}

		db << "VER 2" << endl;

		for (nickrequest_map::const_iterator it = NickRequestList.begin(), it_end = NickRequestList.end(); it != it_end; ++it)
		{
			NickRequest *nr = it->second;

			db << "NR " << nr->nick << " " << nr->passcode << " " << nr->password << " " << nr->email << " " << nr->requested << endl;

			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, nr));
		}

		for (nickcore_map::const_iterator nit = NickCoreList.begin(), nit_end = NickCoreList.end(); nit != nit_end; ++nit)
		{
			NickCore *nc = nit->second;

			db << "NC " << nc->display << " " << nc->pass << endl;

			db << "MD MEMOMAX " << nc->memos.memomax << endl;
			db << "MD CHANCOUNT " << nc->channelcount << endl;

			if (!nc->language.empty())
				db << "MD LANGUAGE " << nc->language << endl;
			if (!nc->email.empty())
				db << "MD EMAIL " << nc->email << endl;
			if (!nc->greet.empty())
				db << "MD GREET :" << nc->greet << endl;

			if (!nc->access.empty())
			{
				for (std::vector<Anope::string>::iterator it = nc->access.begin(), it_end = nc->access.end(); it != it_end; ++it)
					db << "MD ACCESS " << *it << endl;
			}
			if (nc->FlagCount())
			{
				db << "MD FLAGS";
				for (int j = 0; NickCoreFlags[j].Flag != -1; ++j)
					if (nc->HasFlag(NickCoreFlags[j].Flag))
						db << " " << NickCoreFlags[j].Name;
				db << endl;
			}
			if (!nc->memos.memos.empty())
			{
				MemoInfo *mi = &nc->memos;
				for (unsigned k = 0, end = mi->memos.size(); k < end; ++k)
				{
					db << "MD MI 0 " << mi->memos[k]->time << " " << mi->memos[k]->sender;
					if (mi->memos[k]->HasFlag(MF_UNREAD))
						db << " UNREAD";
					if (mi->memos[k]->HasFlag(MF_RECEIPT))
						db << " RECEIPT";
					if (mi->memos[k]->HasFlag(MF_NOTIFYS))
						db << " NOTIFYS";
					db << " :" << mi->memos[k]->text << endl;
				}
			}
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, nc));
		}

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;

			db << "NA " << na->nc->display << " " << na->nick << " " << na->time_registered << " " << na->last_seen << endl;
			if (!na->last_usermask.empty())
				db << "MD LAST_USERMASK " << na->last_usermask << endl;
			if (!na->last_realname.empty())
				db << "MD LAST_REALNAME :" << na->last_realname << endl;
			if (!na->last_quit.empty())
				db << "MD LAST_QUIT :" << na->last_quit << endl;
			if (na->HasFlag(NS_FORBIDDEN) || na->HasFlag(NS_NO_EXPIRE))
				db << "MD FLAGS" << (na->HasFlag(NS_FORBIDDEN) ? " FORBIDDEN" : "") << (na->HasFlag(NS_NO_EXPIRE) ? " NOEXPIRE " : "") << endl;
			if (na->hostinfo.HasVhost())
				db << "MD VHOST " << na->hostinfo.GetCreator() << " " << na->hostinfo.GetTime() << " " << na->hostinfo.GetHost() << " :" << na->hostinfo.GetIdent() << endl;

			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, na));
		}

		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			db << "BI " << bi->nick << " " << bi->GetIdent() << " " << bi->host << " " << bi->created << " " << bi->chancount << " :" << bi->realname << endl;
			if (bi->HasFlag(BI_PRIVATE))
			{
				db << "MD FLAGS PRIVATE" << endl;
			}
		}

		for (registered_channel_map::const_iterator cit = RegisteredChannelList.begin(), cit_end = RegisteredChannelList.end(); cit != cit_end; ++cit)
		{
			ChannelInfo *ci = cit->second;

			db << "CH " << ci->name << " " << ci->time_registered << " " << ci->last_used << endl;
			db << "MD BANTYPE " << ci->bantype << endl;
			db << "MD MEMOMAX " << ci->memos.memomax << endl;
			if (ci->founder)
				db << "MD FOUNDER " << ci->founder->display << endl;
			if (ci->successor)
				db << "MD SUCCESSOR " << ci->successor->display << endl;
			if (!ci->desc.empty())
				db << "MD DESC :" << ci->desc << endl;
			if (!ci->last_topic.empty())
				db << "MD TOPIC " << ci->last_topic_setter << " " << ci->last_topic_time << " :" << ci->last_topic << endl;
			db << "MD LEVELS";
			for (int j = 0; ChannelLevels[j].Level != -1; ++j)
				db << " " << ChannelLevels[j].Name << " " << ci->levels[ChannelLevels[j].Level];
			db << endl;
			if (ci->FlagCount())
			{
				db << "MD FLAGS";
				for (int j = 0; ChannelInfoFlags[j].Flag != -1; ++j)
					if (ci->HasFlag(ChannelInfoFlags[j].Flag))
						db << " " << ChannelInfoFlags[j].Name;
				db << endl;
				if (ci->HasFlag(CI_FORBIDDEN))
					db << "MD FORBID " << ci->forbidby << " :" << ci->forbidreason << endl;
			}
			for (unsigned k = 0, end = ci->GetAccessCount(); k < end; ++k)
				db << "MD ACCESS " << ci->GetAccess(k)->nc->display << " " << ci->GetAccess(k)->level << " " << ci->GetAccess(k)->last_seen << " " << ci->GetAccess(k)->creator << endl;
			for (unsigned k = 0, end = ci->GetAkickCount(); k < end; ++k)
			{
				db << "MD AKICK " << (ci->GetAkick(k)->HasFlag(AK_STUCK) ? "STUCK " : "UNSTUCK ") << (ci->GetAkick(k)->HasFlag(AK_ISNICK) ? "NICK " : "MASK ") <<
					(ci->GetAkick(k)->HasFlag(AK_ISNICK) ? ci->GetAkick(k)->nc->display : ci->GetAkick(k)->mask) << " " << ci->GetAkick(k)->creator << " " << ci->GetAkick(k)->addtime << " " << ci->last_used << " :";
				if (!ci->GetAkick(k)->reason.empty())
					db << ci->GetAkick(k)->reason;
				db << endl;
			}
			if (ci->GetMLockCount(true))
			{
				db << "MD MLOCK_ON";

				Anope::string oldmodes;
				if ((!Me || !Me->IsSynced()) && ci->GetExtRegular("db_mlock_modes_on", oldmodes))
				{
					db << " " << oldmodes;
				}
				else
				{
					for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
					{
						ChannelMode *cm = it->second;
						if (ci->HasMLock(cm->Name, true))
							db << " " << cm->NameAsString;
					}
				}
				db << endl;
			}
			if (ci->GetMLockCount(false))
			{
				db << "MD MLOCK_OFF";

				Anope::string oldmodes;
				if ((!Me || !Me->IsSynced()) && ci->GetExtRegular("db_mlock_modes_off", oldmodes))
				{
					db << " " << oldmodes;
				}
				else
				{
					for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
					{
						ChannelMode *cm = it->second;
						if (ci->HasMLock(cm->Name, false))
							db << " " << cm->NameAsString;
					}
				}
				db << endl;
			}
			std::vector<std::pair<Anope::string, Anope::string> > oldparams;;
			if ((!Me || !Me->IsSynced()) && ci->GetExtRegular("db_mlp", oldparams))
			{
				for (std::vector<std::pair<Anope::string, Anope::string> >::iterator it = oldparams.begin(), it_end = oldparams.end(); it != it_end; ++it)
				{
					db << "MD MLP " << it->first << " " << it->second << endl;
				}
			}
			else
			{
				for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
				{
					ChannelMode *cm = it->second;
					Anope::string Param;

					if (ci->GetParam(cm->Name, Param))
						db << "MD MLP " << cm->NameAsString << " " << Param << endl;
				}
			}
			if (!ci->memos.memos.empty())
			{
				MemoInfo *memos = &ci->memos;

				for (unsigned k = 0, end = memos->memos.size(); k < end; ++k)
				{
					db << "MD MI 0 " << memos->memos[k]->time << " " << memos->memos[k]->sender;
					if (memos->memos[k]->HasFlag(MF_UNREAD))
						db << " UNREAD";
					if (memos->memos[k]->HasFlag(MF_RECEIPT))
						db << " RECEIPT";
					if (memos->memos[k]->HasFlag(MF_NOTIFYS))
						db << " NOTIFYS";
					db << " :" << memos->memos[k]->text << endl;
				}
			}
			if (!ci->entry_message.empty())
				db << "MD ENTRYMSG :" << ci->entry_message << endl;
			if (ci->bi)
				db << "MD BI NAME " << ci->bi->nick << endl;
			if (ci->botflags.FlagCount())
			{
				db << "MD BI FLAGS";
				for (int j = 0; BotFlags[j].Flag != -1; ++j)
					if (ci->botflags.HasFlag(BotFlags[j].Flag))
						db << " " << BotFlags[j].Name;
				db << endl;
			}
			db << "MD BI TTB BOLDS " << ci->ttb[0] << " COLORS " << ci->ttb[1] << " REVERSES " << ci->ttb[2] << " UNDERLINES " << ci->ttb[3] << " BADWORDS " << ci->ttb[4] << " CAPS " << ci->ttb[5] << " FLOOD " << ci->ttb[6] << " REPEAT " << ci->ttb[7] << " ITALICS " << ci->ttb[8] << endl;
			if (ci->capsmin)
				db << "MD BI CAPSMIN " << ci->capsmin << endl;
			if (ci->capspercent)
				db << "MD BI CAPSPERCENT " << ci->capspercent << endl;
			if (ci->floodlines)
				db << "MD BI FLOODLINES " << ci->floodlines << endl;
			if (ci->floodsecs)
				db << "MD BI FLOODSECS " << ci->floodsecs << endl;
			if (ci->repeattimes)
				db << "MD BI REPEATTIMES " << ci->repeattimes << endl;
			for (unsigned k = 0, end = ci->GetBadWordCount(); k < end; ++k)
				db << "MD BI BADWORD " << (ci->GetBadWord(k)->type == BW_ANY ? "ANY " : "") << (ci->GetBadWord(k)->type == BW_SINGLE ? "SINGLE " : "") << (ci->GetBadWord(k)->type == BW_START ? "START " : "") <<
					(ci->GetBadWord(k)->type == BW_END ? "END " : "") << ":" << ci->GetBadWord(k)->word << endl;

			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, ci));
		}

		db << "OS STATS " << maxusercnt << " " << maxusertime << endl;

		if (SGLine)
			for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			{
				XLine *x = SGLine->GetEntry(i);
				db << "OS AKILL " << x->GetUser() << " " << x->GetHost() << " " << x->By << " " << x->Created << " " << x->Expires << " :" << x->Reason << endl;
			}

		if (SNLine)
			for (unsigned i = 0, end = SNLine->GetCount(); i < end; ++i)
			{
				XLine *x = SNLine->GetEntry(i);
				db << "OS SNLINE " << x->Mask << " " << x->By << " " << x->Created << " " << x->Expires << " :" << x->Reason << endl;
			}

		if (SQLine)
			for (unsigned i = 0, end = SQLine->GetCount(); i < end; ++i)
			{
				XLine *x = SQLine->GetEntry(i);
				db << "OS SQLINE " << x->Mask << " " << x->By << " " << x->Created << " " << x->Expires << " :" << x->Reason << endl;
			}

		if (SZLine)
			for (unsigned i = 0, end = SZLine->GetCount(); i < end; ++i)
			{
				XLine *x = SZLine->GetEntry(i);
				db << "OS SZLINE " << x->Mask << " " << x->By << " " << x->Created << " " << x->Expires << " :" << x->Reason << endl;
			}

		for (std::vector<Exception *>::iterator it = exceptions.begin(), it_end = exceptions.end(); it != it_end; ++it)
		{
			Exception *e = *it;
			db << "OS EXCEPTION " << e->mask << " " << e->limit << " " << e->who << " " << e->time << " " << e->expires << " " << e->reason << endl;
		}

		FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		db.close();

		return EVENT_CONTINUE;
	}

	void OnModuleLoad(User *u, Module *m)
	{
		if (!u)
			return;

		Implementation events[] = {I_OnDatabaseRead, I_OnDatabaseReadMetadata};
		for (int i = 0; i < 2; ++i)
		{
			std::vector<Module *>::iterator it = std::find(ModuleManager::EventHandlers[events[i]].begin(), ModuleManager::EventHandlers[events[i]].end(), m);
			/* This module wants to read from the database */
			if (it != ModuleManager::EventHandlers[events[i]].end())
				/* Loop over the database and call the events it would on a normal startup */
				ReadDatabase(m);
		}
	}
};

MODULE_INIT(DBPlain)
