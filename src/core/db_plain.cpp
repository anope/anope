/*
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

std::fstream db;

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
	std::string buf;
	EventReturn MOD_RESULT;
	MDType Type = MD_NONE;

	db.open(DATABASE_FILE, std::ios_base::in);

	if (!db.is_open())
	{
		ircdproto->SendGlobops(NULL, "Unable to open %s for reading!", DATABASE_FILE);
		return;
	}

	while (std::getline(db, buf))
	{
		if (buf.empty())
			continue;

		spacesepstream sep(buf);
		std::vector<std::string> params;
		while (sep.GetToken(buf))
		{
			if (buf[0] == ':')
			{
				buf.erase(buf.begin());
				if (!buf.empty())
					params.push_back(buf + " " + sep.GetRemaining());
			}
			else
				params.push_back(buf);
		}

		if (m)
			MOD_RESULT = m->OnDatabaseRead(params);
			else
		{
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		}
		if (MOD_RESULT == EVENT_STOP)
			continue;

		std::string mdbuf;
		NickCore *nc;
		NickAlias *na;
		BotInfo *bi;
		ChannelInfo *ci;
		if (!params.empty())
		{
			if (params[0] == "NC")
			{
				nc = findcore(params[1].c_str());
				Type = MD_NC;
			}
			else if (params[0] == "NA")
			{
				na = findnick(params[2].c_str());
				Type = MD_NA;
			}
			else if (params[0] == "BI")
			{
				bi = findbot(params[1]);
				Type = MD_BI;
			}
			else if (params[0] == "CH")
			{
				ci = cs_findchan(params[1].c_str());
				Type = MD_CH;
			}
			else if (params[0] == "MD")
			{
				std::string key = params[1];
				params.erase(params.begin());
				params.erase(params.begin());

				if (nc && Type == MD_NC)
				{
					try
					{
						if (m)
							MOD_RESULT = m->OnDatabaseReadMetadata(nc, key, params);
						else
						{
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nc, key, params));
						}
					}
					catch (const char *err)
					{
						alog("[db_plain}: %s", err);
					}
				}
				else if (na && Type == MD_NA)
				{
					try
					{
						if (m)
							MOD_RESULT = m->OnDatabaseReadMetadata(na, key, params);
						else
						{
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, key, params));
						}
					}
					catch (const char *err)
					{
						alog("[db_plain}: %s", err);
					}
				}
				else if (bi && Type == MD_BI)
				{
					try
					{
						if (m)
							MOD_RESULT = m->OnDatabaseReadMetadata(bi, key, params);
						else
						{
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(bi, key, params));
						}
					}
					catch (const char *err)
					{
						alog("[db_plain}: %s", err);
					}
					}
				else if (ci && Type == MD_CH)
				{
					try
					{
						if (m)
							MOD_RESULT = m->OnDatabaseReadMetadata(ci, key, params);
						else
						{
							FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, key, params));
						}
					}
					catch (const char *err)
					{
						alog("[db_plain}: %s", err);
					}
				}
			}
		}
	}

	db.close();
}

struct LangInfo
{
	std::string Name;
	int LanguageId;
};

struct ChannelModeInfo
{
	std::string Name;
	ChannelModeName Value;
};

struct ChannelFlagInfo
{
	std::string Name;
	ChannelInfoFlag Flag;
};

struct ChannelLevel
{
	std::string Name;
	int Level;
};

struct BotFlagInfo
{
	std::string Name;
	BotServFlag Flag;
};

struct NickCoreFlagInfo
{
	std::string Name;
	NickCoreFlag Flag;
};

LangInfo LangInfos[] = {
	{"en", LANG_EN_US},
	{"es", LANG_ES},
	{"pt", LANG_PT},
	{"fr", LANG_FR},
	{"tr", LANG_TR},
	{"it", LANG_IT},
	{"de", LANG_DE},
	{"ca", LANG_CAT},
	{"gr", LANG_GR},
	{"nl", LANG_NL},
	{"ru", LANG_RU},
	{"hu", LANG_HUN},
	{"pl", LANG_PL},
	{"", -1}
};

ChannelModeInfo ChannelModes[] = {
	{"BLOCKCOLOR", CMODE_BLOCKCOLOR},
	{"FLOOD", CMODE_FLOOD},
	{"INVITE", CMODE_INVITE},
	{"KEY", CMODE_KEY},
	{"LIMIT", CMODE_LIMIT},
	{"MODERATED", CMODE_MODERATED},
	{"NOEXTERNAL", CMODE_NOEXTERNAL},
	{"PRIVATE", CMODE_PRIVATE},
	{"REGISTERED", CMODE_REGISTERED},
	{"SECRET", CMODE_SECRET},
	{"TOPIC", CMODE_TOPIC},
	{"AUDITORIUM", CMODE_AUDITORIUM},
	{"SSL", CMODE_SSL},
	{"ADMINONLY", CMODE_ADMINONLY},
	{"NOCTCP", CMODE_NOCTCP},
	{"FILTER", CMODE_FILTER},
	{"NOKNOCK", CMODE_NOKNOCK},
	{"REDIRECT", CMODE_REDIRECT},
	{"REGMODERATED", CMODE_REGMODERATED},
	{"NONICK", CMODE_NONICK},
	{"OPERONLY", CMODE_OPERONLY},
	{"NONICK", CMODE_NONICK},
	{"REGISTEREDONLY", CMODE_REGISTEREDONLY},
	{"STRIPCOLOR", CMODE_STRIPCOLOR},
	{"NONOTICE", CMODE_NONOTICE},
	{"NOINVITE", CMODE_NOINVITE},
	{"ALLINVITE", CMODE_ALLINVITE},
	{"BLOCKCAPS", CMODE_BLOCKCAPS},
	{"PERM", CMODE_PERM},
	{"NICKFLOOD", CMODE_NICKFLOOD},
	{"JOINFLOOD", CMODE_JOINFLOOD},
	{"", static_cast<ChannelModeName>(-1)}
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

static void LoadNickCore(const std::vector<std::string> &params)
{
	NickCore *nc = new NickCore(params[0]);
	/* Clear default flags */
	nc->ClearFlags();

	if (params.size() <= 2)
	{
		/* This is a forbidden nick */
		return;
	}

	size_t tmp = params[1].find(':'); // XXX till we store the hash method internally with the nickcore
	const char *pass = params[1].c_str() + tmp + 1;
	char passbuf[PASSMAX];
	memset(&passbuf, 0, sizeof(passbuf));
	b64_decode(pass, passbuf, PASSMAX);
	strscpy(nc->pass, passbuf, PASSMAX);

	for (int i = 0; LangInfos[i].LanguageId != -1; ++i)
		if (params[2] == LangInfos[i].Name)
			nc->language = LangInfos[i].LanguageId;

	nc->memos.memomax = atoi(params[3].c_str());
	nc->channelcount = atoi(params[4].c_str());

	if (debug > 1)
		alog("[db_plain]: Loaded NickCore %s", nc->display);
}

static void LoadNickAlias(const std::vector<std::string> &params)
{
	NickCore *nc = findcore(params[0].c_str());
	if (!nc)
	{
		alog("[db_plain]: Unable to find core %s", params[0].c_str());
		return;
	}

	NickAlias *na = new NickAlias(params[1], nc);

	na->time_registered = strtol(params[2].c_str(), NULL, 10);

	na->last_seen = strtol(params[3].c_str(), NULL, 10);

	if (debug > 1)
		alog("[db_plain}: Loaded nickalias for %s", na->nick);
}

static void LoadBotInfo(const std::vector<std::string> &params)
{
	BotInfo *bi = new BotInfo(params[0].c_str());
	bi->user = sstrdup(params[1].c_str());
	bi->host = sstrdup(params[2].c_str());
	bi->created = strtol(params[4].c_str(), NULL, 10);
	bi->chancount = atol(params[5].c_str());
	bi->real = sstrdup(params[6].c_str());

	if (debug > 1)
		alog("[db_plain]: Loaded botinfo for %s", bi->nick);
}

static void LoadChanInfo(const std::vector<std::string> &params)
{
	ChannelInfo *ci = new ChannelInfo(params[0]);
	/* CLear default mlock */
	ci->ClearMLock();
	/* Remove default channel flags */
	ci->ClearFlags();
	ci->botflags.ClearFlags();

	ci->time_registered = strtol(params[1].c_str(), NULL, 10);

	ci->last_used = strtol(params[2].c_str(), NULL, 10);

	ci->bantype = atoi(params[3].c_str());

	ci->memos.memomax = atoi(params[4].c_str());

	if (debug > 1)
		alog("[db_plain]: loaded channel %s", ci->name);
}

static void LoadHostInfo(const std::vector<std::string> &params)
{
	if (!findnick(params[0].c_str()))
		return;
	addHostCore(params[0].c_str(), params[4].empty() ? NULL : const_cast<char *>(params[4].c_str()), const_cast<char *>(params[3].c_str()), params[1].c_str(), strtol(params[2].c_str(), NULL, 10)); // XXX
	if (debug > 1)
		alog("[db_plain]: loaded vhost for %s", params[0].c_str());
}

static void LoadOperInfo(const std::vector<std::string> &params)
{
	if (params[0] == "STATS")
	{
		maxusercnt = atol(params[1].c_str());
		maxusertime = strtol(params[2].c_str(), NULL, 10);
	}
	else if (params[0] == "AKILL" || params[0] == "SGLINE" || params[0] == "SQLINE" || params[0] == "SZLINE")
	{
		int (*add_sxline)(User *, const char *, const char *, time_t, const char *);
		if (params[0] == "AKILL")
			add_sxline = add_akill;
		else if (params[0] == "SGLINE")
			add_sxline = add_sgline;
		else if (params[0] == "SQLINE")
			add_sxline = add_sqline;
		else if (params[0] == "SZLINE")
			add_sxline = add_szline;

		add_sxline(NULL, (params[1] + "@" + params[2]).c_str(), params[3].c_str(), strtol(params[5].c_str(), NULL, 10), params[6].c_str());
	}
	else if (params[0] == "EXCEPTION")
	{
		nexceptions++;
		exceptions = static_cast<Exception *>(srealloc(exceptions, sizeof(Exception) * nexceptions));
		exceptions[nexceptions - 1].mask = sstrdup(params[1].c_str());
		exceptions[nexceptions - 1].limit = atol(params[2].c_str());
		strscpy(exceptions[nexceptions - 1].who, params[3].c_str(), NICKMAX);
		exceptions[nexceptions - 1].time = strtol(params[4].c_str(), NULL, 10);
		exceptions[nexceptions - 1].expires = strtol(params[5].c_str(), NULL, 10);
		exceptions[nexceptions - 1].reason = sstrdup(params[6].c_str());
		exceptions[nexceptions - 1].num = nexceptions - 1;
	}
}

void Write(const std::string &buf)
{
	db << buf << endl;
}

void WriteMetadata(const std::string &key, const std::string &data)
{
	Write("MD " + key + " " + data);
}

class DBPlain : public Module
{
 public:
	DBPlain(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(DATABASE);

		Implementation i[] = { I_OnDatabaseRead, I_OnLoadDatabase, I_OnDatabaseReadMetadata, I_OnSaveDatabase, I_OnModuleLoad };
		ModuleManager::Attach(i, this, 5);
	}

	EventReturn OnDatabaseRead(const std::vector<std::string> &params)
	{
		std::string key = params[0];
		std::vector<std::string> otherparams = params;
		otherparams.erase(otherparams.begin());

		if (key == "NC")
			LoadNickCore(otherparams);
		else if (key == "NA")
			LoadNickAlias(otherparams);
		else if (key == "BI")
			LoadBotInfo(otherparams);
		else if (key == "CH")
			LoadChanInfo(otherparams);
		else if (key == "HI")
			LoadHostInfo(otherparams);
		else if (key == "OS")
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

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const std::string &key, const std::vector<std::string> &params)
	{
		if (key == "EMAIL")
			nc->email = sstrdup(params[0].c_str());
		else if (key == "GREET")
			nc->greet = sstrdup(params[0].c_str());
		else if (key == "ICQ")
			nc->icq = atoi(params[0].c_str());
		else if (key == "URL")
			nc->url = sstrdup(params[0].c_str());
		else if (key == "ACCESS")
			nc->AddAccess(params[0]);
		else if (key == "FLAGS")
		{
			for (unsigned j = 0; j < params.size(); ++j)
				for (int i = 0; NickCoreFlags[i].Flag != -1; ++i)
					if (NickCoreFlags[i].Name == params[j])
						nc->SetFlag(NickCoreFlags[i].Flag);
		}
		else if (key == "MI")
		{
			Memo *m = new Memo;
			m->number = atoi(params[0].c_str());
			m->time = strtol(params[1].c_str(), NULL, 10);
			strscpy(m->sender, params[2].c_str(), NICKMAX);
			unsigned j;
			for (j = 3; (params[j] == "UNREAD" || params[j] == "RECEIPT" || params[j] == "NOTIFYS"); ++j)
			{
				if (params[j] == "UNREAD")
					m->SetFlag(MF_UNREAD);
				else if (params[j] == "RECEIPT")
					m->SetFlag(MF_RECEIPT);
				else if (params[j] == "NOTIFYS")
					m->SetFlag(MF_NOTIFYS);
			}
			m->text = sstrdup(params[j + 1].c_str());
			nc->memos.memos.push_back(m);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(NickAlias *na, const std::string &key, const std::vector<std::string> &params)
	{
		if (key == "LAST_USERMASK")
			na->last_usermask = sstrdup(params[0].c_str());
		else if (key == "LAST_REALNAME")
			na->last_realname = sstrdup(params[0].c_str());
		else if (key == "LAST_QUIT")
			na->last_quit = sstrdup(params[0].c_str());
		else if (key == "FLAGS")
		{
			for (unsigned j = 0; j < params.size(); ++j)
			{
				if (params[j] == "FORBIDDEN")
					na->SetFlag(NS_FORBIDDEN);
				else if (params[j] == "NOEXPIRE")
					na->SetFlag(NS_NO_EXPIRE);
			}
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(BotInfo *bi, const std::string &key, const std::vector<std::string> &params)
	{
		std::string buf;

		if (key == "FLAGS")
		{
			for (unsigned j = 0; j < params.size(); ++j)
			{
				if (params[j] == "PRIVATE")
					bi->SetFlag(BI_PRIVATE);
				else if (params[j] == "CHANSERV")
					bi->SetFlag(BI_CHANSERV);
				else if (params[j] == "BOTSERV")
					bi->SetFlag(BI_BOTSERV);
				else if (params[j] == "HOSTSERV")
					bi->SetFlag(BI_HOSTSERV);
				else if (params[j] == "OPERSERV")
					bi->SetFlag(BI_OPERSERV);
				else if (params[j] == "MEMOSERV")
					bi->SetFlag(BI_MEMOSERV);
				else if (params[j] == "NICKSERV")
					bi->SetFlag(BI_NICKSERV);
				else if (params[j] == "GLOBAL")
					bi->SetFlag(BI_GLOBAL);
			}
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const std::string &key, const std::vector<std::string> &params)
	{
		std::string buf;
		int i;

		if (key == "FOUNDER")
		{
			ci->founder = findcore(params[0].c_str());
			if (!ci->founder)
			{
				alog("[db_plain]: Deleting founderless channel %s", ci->name);
				delete ci;
				ci = NULL;
				throw "no founder";
			}
		}
		else if (key == "SUCCESSOR")
			ci->successor = findcore(params[0].c_str());
		else if (key == "LEVELS")
		{
			for (unsigned j = 0; j < params.size(); ++j, ++j)
				for (i = 0; ChannelLevels[i].Level != -1; ++i)
					if (ChannelLevels[i].Name == params[j])
						ci->levels[ChannelLevels[i].Level] = atoi(params[j + 1].c_str());
		}
		else if (key == "FLAGS")
		{
			for (unsigned j = 0; j < params.size(); ++j)
				for (i = 0; ChannelInfoFlags[i].Flag != -1; ++i)
					if (ChannelInfoFlags[i].Name == params[j])
						ci->SetFlag(ChannelInfoFlags[i].Flag);
		}
		else if (key == "DESC")
			ci->desc = sstrdup(params[0].c_str());
		else if (key == "URL")
			ci->url = sstrdup(params[0].c_str());
		else if (key == "EMAIL")
			ci->email = sstrdup(params[0].c_str());
		else if (key == "TOPIC")
		{
			strscpy(ci->last_topic_setter, params[0].c_str(), NICKMAX);
			ci->last_topic_time = strtol(params[1].c_str(), NULL, 10);
			ci->last_topic = sstrdup(params[2].c_str());
		}
		else if (key == "FORBID")
		{
			ci->forbidby = sstrdup(params[0].c_str());
			ci->forbidreason = sstrdup(params[1].c_str());
		}
		else if (key == "ACCESS")
		{
			NickCore *nc = findcore(params[0].c_str());
			if (!nc)
				throw "access entry for nonexistant core";
			int level = atoi(params[1].c_str());
			time_t last_seen = strtol(params[2].c_str(), NULL, 10);
			ci->AddAccess(nc, level, buf, last_seen);
		}
		else if (key == "AKICK")
		{
			bool Stuck = params[0] == "STUCK" ? true : false;
			bool Nick = params[1] == "NICK" ? true : false;
			NickCore *nc = NULL;
			if (Nick)
			{
				nc = findcore(params[2].c_str());
				if (!nc)
				{
					throw "akick for nonexistant core";
				}
			}
			AutoKick *ak;
			if (Nick)
				ak = ci->AddAkick(params[3], nc, params.size() > 5 ? params[5] : "", strtol(params[4].c_str(), NULL, 10));
			else
				ak = ci->AddAkick(params[3], params[2],  params.size() > 5 ? params[5] : "", strtol(params[4].c_str(), NULL, 10));
			if (Stuck)
				ak->SetFlag(AK_STUCK);
			if (Nick)
				ak->SetFlag(AK_ISNICK);

		}
		else if (key == "MLOCK_ON" || buf == "MLOCK_OFF")
		{
			bool Set = key == "MLOCK_ON" ? true : false;

			for (unsigned j = 0; j < params.size(); ++j)
				for (i = 0; ChannelModes[i].Value != -1; ++i)
					if (ChannelModes[i].Name == params[j])
						ci->SetMLock(ChannelModes[i].Value, Set);
		}
		else if (key == "MLP")
		{
			for (unsigned j = 0; j < params.size(); ++j, ++j)
				for (i = 0; ChannelModes[i].Value != -1; ++i)
					if (ChannelModes[i].Name == params[j])
						ci->SetMLock(ChannelModes[i].Value, true, params[j + 1]);
		}
		else if (key == "MI")
		{
			Memo *m = new Memo;
			m->number = atoi(params[0].c_str());
			m->time = strtol(params[1].c_str(), NULL, 10);
			strscpy(m->sender, params[2].c_str(), NICKMAX);
			unsigned j;
			for (j = 3; j < params.size() - 1 && (buf == "UNREAD" || buf == "RECEIPT" || buf == "NOTIFYS"); ++j)
			{
				if (params[j] == "UNREAD")
					m->SetFlag(MF_UNREAD);
				else if (params[j] == "RECEIPT")
					m->SetFlag(MF_RECEIPT);
				else if (params[j] == "NOTIFYS")
					m->SetFlag(MF_NOTIFYS);
			}
			m->text = sstrdup(params[j + 1].c_str());
			ci->memos.memos.push_back(m);
		}
		else if (key == "ENTRYMSG")
			ci->entry_message = sstrdup(params[0].c_str());
		else if (key == "BI")
		{
			if (params[0] == "NAME")
				ci->bi = findbot(params[1]);
			else if (params[0] == "FLAGS")
			{
				for (unsigned j = 1; j < params.size(); ++j)
					for (i = 0; BotFlags[i].Flag != -1; ++i)
						if (BotFlags[i].Name == params[j])
							ci->botflags.SetFlag(BotFlags[i].Flag);
			}
			else if (params[0] == "TTB")
			{
				for (unsigned j = 1; j < params.size(); ++j, ++j)
				{
					if (params[j] == "BOLDS")
						ci->ttb[0] = atoi(params[j + 1].c_str());
					else if (params[j] == "COLORS")
						ci->ttb[1] = atoi(params[j + 1].c_str());
					else if (params[j] == "REVERSES")
						ci->ttb[2] = atoi(params[j + 1].c_str());
					else if (params[j] == "UNDERLINES")
						ci->ttb[3] = atoi(params[j + 1].c_str());
					else if (params[j] == "BADWORDS")
						ci->ttb[4] = atoi(params[j + 1].c_str());
					else if (params[j] == "CAPS")
						ci->ttb[5] = atoi(params[j + 1].c_str());
					else if (params[j] == "FLOOD")
						ci->ttb[6] = atoi(params[j + 1].c_str());
					else if (params[j] == "REPEAT")
						ci->ttb[7] = atoi(params[j + 1].c_str());
				}
			}
			else if (params[0] == "CAPSMIN")
				ci->capsmin = atoi(params[1].c_str());
			else if (params[0] == "CAPSPERCENT")
				ci->capspercent = atoi(params[1].c_str());
			else if (params[0] == "FLOODLINES")
				ci->floodlines = atoi(params[1].c_str());
			else if (params[0] == "FLOODSECS")
				ci->floodsecs = atoi(params[1].c_str());
			else if (params[0] == "REPEATTIMES")
				ci->repeattimes = atoi(params[1].c_str());
			else if (params[0] == "BADWORD")
			{
				BadWordType Type;
				if (params[1] == "SINGLE")
					Type = BW_SINGLE;
				else if (params[1] == "START")
					Type = BW_START;
				else if (params[1] == "END")
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
		db.open(DATABASE_FILE, std::ios_base::out | std::ios_base::trunc);

		if (!db.is_open())
		{
			ircdproto->SendGlobops(NULL, "Unable to open %s for writing!", DATABASE_FILE);
			return EVENT_CONTINUE;
		}

		db << "VER 1" << endl;

		int i, j;
		unsigned k;
		NickCore *nc;
		for (i = 0; i < 1024; ++i)
		{
			for (nc = nclists[i]; nc; nc = nc->next)
			{
				if (nc->HasFlag(NI_FORBIDDEN))
				{
					db << "NC " << nc->display << endl;
					db << "MD FLAGS FORBIDDEN" << endl;
					FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, nc));
					continue;
				}
				else
				{
					//XXX forced plain till we store hashm in pw
					char temppass[5000];
					b64_encode(nc->pass, strlen(nc->pass), temppass, 5000);
					db << "NC " << nc->display << " plain:" << temppass << " ";
				}
				for (j = 0; LangInfos[j].LanguageId != -1; ++j)
					if (nc->language ==  LangInfos[j].LanguageId)
						db << LangInfos[j].Name;
				db << " " << nc->memos.memomax << " " << nc->channelcount << endl;

				if (nc->email)
					db << "MD EMAIL " << nc->email << endl;
				if (nc->greet)
					db << "MD GREET :" << nc->greet << endl;
				if (nc->icq)
					db << "MD ICQ :" << nc->icq << endl;
				if (nc->url)
					db << "MD URL :" << nc->url << endl;
				if (!nc->access.empty())
				{
					for (std::vector<std::string>::iterator it = nc->access.begin(); it != nc->access.end(); ++it)
						db << "MD ACCESS " << *it << endl;
				}
				if (nc->FlagCount())
				{
					db << "MD FLAGS";
					for (j = 0; NickCoreFlags[j].Flag != -1; ++j)
						if (nc->HasFlag(NickCoreFlags[j].Flag))
							db << " " << NickCoreFlags[j].Name;
					db << endl;
				}
				if (!nc->memos.memos.empty())
				{
					MemoInfo *mi = &nc->memos;
					for (k = 0; k < mi->memos.size(); ++k)
					{
						db << "MD MI " << mi->memos[k]->number << " " << mi->memos[k]->time << " " << mi->memos[k]->sender;
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
		}

		NickAlias *na;
		for (i = 0; i < 1024; ++i)
		{
			for (na = nalists[i]; na; na = na->next)
			{
				db << "NA " << na->nc->display << " " << na->nick << " " << na->time_registered << " " << na->last_seen << endl;
				if (na->last_usermask)
					db << "MD LAST_USERMASK " << na->last_usermask << endl;
				if (na->last_realname)
					db << "MD LAST_REALNAME :" << na->last_realname << endl;
				if (na->last_quit)
					db << "MD LAST_QUIT :" << na->last_quit << endl;
				if (na->HasFlag(NS_FORBIDDEN) || na->HasFlag(NS_NO_EXPIRE))
				{
					db << "MD FLAGS" << (na->HasFlag(NS_FORBIDDEN) ? " FORBIDDEN" : "") << (na->HasFlag(NS_NO_EXPIRE) ? " NOEXPIRE " : "") << endl;
				}

				FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, na));
			}
		}

		BotInfo *bi;
		for (i = 0; i < 256; ++i)
		{
			for (bi = botlists[i]; bi; bi = bi->next)
			{
				db << "BI " << bi->nick << " " << bi->user << " " << bi->host << " " << bi->created << " " << bi->chancount << " :" << bi->real << endl;
				if (bi->FlagCount())
				{
					db << "MD FLAGS";
					if (bi->HasFlag(BI_PRIVATE))
						db << " PRIVATE";
					if (bi->HasFlag(BI_CHANSERV))
						db << " CHANSERV";
					else if (bi->HasFlag(BI_BOTSERV))
						db << " BOTSERV";
					else if (bi->HasFlag(BI_HOSTSERV))
						db << " HOSTSERV";
					else if (bi->HasFlag(BI_OPERSERV))
						db << " OPERSERV";
					else if (bi->HasFlag(BI_MEMOSERV))
						db << " MEMOSERV";
					else if (bi->HasFlag(BI_NICKSERV))
						db << " NICKSERV";
					else if (bi->HasFlag(BI_GLOBAL))
						db << " GLOBAL";
					db << endl;
				}

				FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, bi));
			}
		}

		ChannelInfo *ci;
		for (i = 0; i < 256; ++i)
		{
			for (ci = chanlists[i]; ci; ci = ci->next)
			{
				db << "CH " << ci->name << " " << ci->time_registered << " " << ci->last_used;
				db << " " << ci->bantype << " " << ci->memos.memomax << endl;
				if (ci->founder)
					db << "MD FOUNDER " << ci->founder->display << endl;
				if (ci->successor)
					db << "MD SUCCESSOR " << ci->successor->display << endl;
				if (ci->desc)
					db << "MD DESC :" << ci->desc << endl;
				if (ci->url)
					db << "MD URL :" << ci->url << endl;
				if (ci->email)
					db << "MD EMAIL :" << ci->email << endl;
				if (ci->last_topic)
					db << "MD TOPIC " << ci->last_topic_setter << " " << ci->last_topic_time << " :" << ci->last_topic << endl;
				db << "MD LEVELS";
				for (j = 0; ChannelLevels[j].Level != -1; ++j)
					db << " " << ChannelLevels[j].Name << " " << ci->levels[ChannelLevels[j].Level];
				db << endl;
				if (ci->FlagCount())
				{
					db << "MD FLAGS";
					for (j = 0; ChannelInfoFlags[j].Flag != -1; ++j)
						if (ci->HasFlag(ChannelInfoFlags[j].Flag))
							db << " " << ChannelInfoFlags[j].Name;
					db << endl;
					if (ci->HasFlag(CI_FORBIDDEN))
						db << "MD FORBID " << ci->forbidby << " :" << ci->forbidreason << endl;
				}
				for (k = 0; k < ci->GetAccessCount(); ++k)
					if (ci->GetAccess(k)->in_use)
						db << "MD ACCESS " << ci->GetAccess(k)->nc->display << " " << ci->GetAccess(k)->level << " "
						<< ci->GetAccess(k)->last_seen << " " << ci->GetAccess(k)->creator << endl;
				for (k = 0; k < ci->GetAkickCount(); ++k)
				{
					db << "MD AKICK "
					<< (ci->GetAkick(k)->HasFlag(AK_STUCK) ? "STUCK " : "UNSTUCK ")
					<< (ci->GetAkick(k)->HasFlag(AK_ISNICK) ? "NICK " : "MASK ")
					<< (ci->GetAkick(k)->HasFlag(AK_ISNICK) ? ci->GetAkick(k)->nc->display : ci->GetAkick(k)->mask)
					<< " " << ci->GetAkick(k)->creator << " " << ci->GetAkick(k)->addtime << " :";
					if (!ci->GetAkick(k)->reason.empty())
						db << ci->GetAkick(k)->reason;
					db << endl;
				}
				if (ci->GetMLockCount(true))
				{
					db << "MD MLOCK_ON";
					for (j = 0; ChannelModes[j].Value != -1; ++j)
						if (ci->HasMLock(ChannelModes[j].Value, true))
							db << " " << ChannelModes[j].Name;
					db << endl;
				}
				if (ci->GetMLockCount(false))
				{
					db << "MD MLOCK_OFF";
					for (j = 0; ChannelModes[j].Value != -1; ++j)
						if (ci->HasMLock(ChannelModes[j].Value, false))
							db << " " << ChannelModes[j].Name;
					db << endl;
				}
				std::string Param;
				for (j = 0; ChannelModes[j].Value != -1; ++j)
					if (ci->GetParam(ChannelModes[j].Value, &Param))
						db << "MD MLP " << ChannelModes[j].Name << " " << Param;
				if (!ci->memos.memos.empty())
				{
					MemoInfo *memos = &ci->memos;

					for (k = 0; k < memos->memos.size(); ++k)
					{
						db << "MD MI " << memos->memos[k]->number << " " << memos->memos[k]->time << " " << memos->memos[k]->sender;
						if (memos->memos[k]->HasFlag(MF_UNREAD))
							db << " UNREAD";
						if (memos->memos[k]->HasFlag(MF_RECEIPT))
							db << " RECEIPT";
						if (memos->memos[k]->HasFlag(MF_NOTIFYS))
							db << " NOTIFYS";
						db << " :" << memos->memos[k]->text << endl;
					}
				}
				if (ci->entry_message)
					db << "MD ENTRYMSG :" << ci->entry_message << endl;
				if (ci->bi)
					db << "MD BI NAME " << ci->bi->nick << endl;
				if (ci->botflags.FlagCount())
				{
					db << "MD BI FLAGS";
					for (j = 0; BotFlags[j].Flag != -1; ++j)
						if (ci->botflags.HasFlag(BotFlags[j].Flag))
							db << " " << BotFlags[j].Name;
					db << endl;
				}
				db << "MD BI TTB BOLDS " << ci->ttb[0] << " COLORS " << ci->ttb[1] << " REVERSES " << ci->ttb[2] << " UNDERLINES " << ci->ttb[3] << " BADWORDS " << ci->ttb[4] << " CAPS " << ci->ttb[5] << " FLOOD " << ci->ttb[6] << " REPEAT " << ci->ttb[7] << endl;
				if (ci->capsmin)
					db << "MD BI CAPSMINS " << ci->capsmin << endl;
				if (ci->capspercent)
					db << "MD BI CAPSPERCENT " << ci->capspercent << endl;
				if (ci->floodlines)
					db << "MD BI FLOODLINES " << ci->floodlines << endl;
				if (ci->floodsecs)
					db << "MD BI FLOODSECS " << ci->floodsecs << endl;
				if (ci->repeattimes)
					db << "MD BI REPEATTIMES " << ci->repeattimes << endl;
				for (k = 0; k < ci->GetBadWordCount(); ++k)
					db << "MD BI BADWORD "
						<< (ci->GetBadWord(k)->type == BW_ANY ? "ANY " : "")
						<< (ci->GetBadWord(k)->type == BW_SINGLE ? "SINGLE " : "")
						<< (ci->GetBadWord(k)->type == BW_START ? "START " : "")
						<< (ci->GetBadWord(k)->type == BW_END ? "END " : "")
						<< ":" << ci->GetBadWord(k)->word << endl;

				FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteMetadata, ci));
			}
		}

		HostCore *hc;
		for (hc = hostCoreListHead(); hc; hc = hc->next)
		{
			db << "HI " << hc->nick << " " << hc->creator << " " << hc->time << " " << hc->vHost << " :" << hc->vIdent << endl;
		}

		for (i = 0; i < akills.count; ++i)
		{
			Akill *ak = static_cast<Akill *>(akills.list[i]);
			db << "OS AKILL " << ak->user << " " << ak->host << " " << ak->by << " " << ak->seton << " " << ak->expires << " :" << ak->reason << endl;
		}

		db << "OS STATS " << maxusercnt << " " << maxusertime << endl;
		SXLine *sx;
		for (i = 0; i < sglines.count; ++i)
		{
			sx = static_cast<SXLine *>(sglines.list[i]);
			db << "OS SGLINE " << sx->mask << " " << sx->by << " " << sx->seton << " " << sx->expires << " :" << sx->reason << endl;
		}
		for (i = 0; i < sqlines.count; ++i)
		{
			sx = static_cast<SXLine *>(sqlines.list[i]);
			db << "OS SQLINE " << sx->mask << " " << sx->by << " " << sx->seton << " " << sx->expires << " :" << sx->reason << endl;
		}
		for (i = 0; i < szlines.count; ++i)
		{
			sx = static_cast<SXLine *>(szlines.list[i]);
			db << "OS SZLINE " << sx->mask << " " << sx->by << " " << sx->seton << " " << sx->expires << " :" << sx->reason << endl;
		}

		for (i = 0; i < nexceptions; i++)
		{
			db << "OS EXCEPTION " << exceptions[i].mask << " " << exceptions[i].limit << " " << exceptions[i].who << " " << exceptions[i].time << " " << exceptions[i].expires << " " << exceptions[i].reason << endl;
		}

		FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		db.close();

		return EVENT_CONTINUE;
	}

	void OnModuleLoad(User *u, Module *m)
	{
		if (!u)
			return;

		Implementation events[] = { I_OnDatabaseRead, I_OnDatabaseReadMetadata };
		for (int i = 0; i < 2; ++i)
		{
			std::vector<Module *>::iterator it = std::find(ModuleManager::EventHandlers[events[i]].begin(), ModuleManager::EventHandlers[events[i]].end(), m);
			/* This module wants to read from the database */
			if (it != ModuleManager::EventHandlers[events[i]].end())
			{
				/* Loop over the database and call the events it would on a normal startup */
				ReadDatabase(m);
			}
		}
	}
};

MODULE_INIT(DBPlain)

