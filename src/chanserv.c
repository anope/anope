/* ChanServ functions.
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

#include "services.h"
#include "pseudo.h"

/*************************************************************************/
/* *INDENT-OFF* */

ChannelInfo *chanlists[256];

static int def_levels[][2] = {
	{ CA_AUTOOP,					 5 },
	{ CA_AUTOVOICE,				  3 },
	{ CA_AUTODEOP,				  -1 },
	{ CA_NOJOIN,					-2 },
	{ CA_INVITE,					 5 },
	{ CA_AKICK,					 10 },
	{ CA_SET,	 		ACCESS_INVALID },
	{ CA_CLEAR,   		ACCESS_INVALID },
	{ CA_UNBAN,					  5 },
	{ CA_OPDEOP,					 5 },
	{ CA_ACCESS_LIST,				1 },
	{ CA_ACCESS_CHANGE,			 10 },
	{ CA_MEMO,					  10 },
	{ CA_ASSIGN,  		ACCESS_INVALID },
	{ CA_BADWORDS,				  10 },
	{ CA_NOKICK,					 1 },
	{ CA_FANTASIA,					 3 },
	{ CA_SAY,						 5 },
	{ CA_GREET,					  5 },
	{ CA_VOICEME,					 3 },
	{ CA_VOICE,						 5 },
	{ CA_GETKEY,					 5 },
	{ CA_AUTOHALFOP,				 4 },
	{ CA_AUTOPROTECT,			   10 },
	{ CA_OPDEOPME,				   5 },
	{ CA_HALFOPME,				   4 },
	{ CA_HALFOP,					 5 },
	{ CA_PROTECTME,				 10 },
	{ CA_PROTECT,  		ACCESS_INVALID },
	{ CA_KICKME,			   		 5 },
	{ CA_KICK,					   5 },
	{ CA_SIGNKICK, 		ACCESS_INVALID },
	{ CA_BANME,					  5 },
	{ CA_BAN,						5 },
	{ CA_TOPIC,		 ACCESS_INVALID },
	{ CA_INFO,		  ACCESS_INVALID },
	{ CA_AUTOOWNER,		ACCESS_INVALID },
	{ CA_OWNER,		ACCESS_INVALID },
	{ CA_OWNERME,		ACCESS_INVALID },
	{ -1 }
};


LevelInfo levelinfo[] = {
	{ CA_AUTODEOP,	  "AUTODEOP",	 CHAN_LEVEL_AUTODEOP },
	{ CA_AUTOHALFOP,	"AUTOHALFOP",   CHAN_LEVEL_AUTOHALFOP },
	{ CA_AUTOOP,		"AUTOOP",	   CHAN_LEVEL_AUTOOP },
	{ CA_AUTOPROTECT,   "AUTOPROTECT",  CHAN_LEVEL_AUTOPROTECT },
	{ CA_AUTOVOICE,	 "AUTOVOICE",	CHAN_LEVEL_AUTOVOICE },
	{ CA_NOJOIN,		"NOJOIN",	   CHAN_LEVEL_NOJOIN },
	{ CA_SIGNKICK,	  "SIGNKICK",	 CHAN_LEVEL_SIGNKICK },
	{ CA_ACCESS_LIST,   "ACC-LIST",	 CHAN_LEVEL_ACCESS_LIST },
	{ CA_ACCESS_CHANGE, "ACC-CHANGE",   CHAN_LEVEL_ACCESS_CHANGE },
	{ CA_AKICK,		 "AKICK",		CHAN_LEVEL_AKICK },
	{ CA_SET,		   "SET",		  CHAN_LEVEL_SET },
	{ CA_BAN,		   "BAN",		  CHAN_LEVEL_BAN },
	{ CA_BANME,		 "BANME",		CHAN_LEVEL_BANME },
	{ CA_CLEAR,		 "CLEAR",		CHAN_LEVEL_CLEAR },
	{ CA_GETKEY,		"GETKEY",	   CHAN_LEVEL_GETKEY },
	{ CA_HALFOP,		"HALFOP",	   CHAN_LEVEL_HALFOP },
	{ CA_HALFOPME,	  "HALFOPME",	 CHAN_LEVEL_HALFOPME },
	{ CA_INFO,		  "INFO",		 CHAN_LEVEL_INFO },
	{ CA_KICK,		  "KICK",		 CHAN_LEVEL_KICK },
	{ CA_KICKME,		"KICKME",	   CHAN_LEVEL_KICKME },
	{ CA_INVITE,		"INVITE",	   CHAN_LEVEL_INVITE },
	{ CA_OPDEOP,		"OPDEOP",	   CHAN_LEVEL_OPDEOP },
	{ CA_OPDEOPME,	  "OPDEOPME",	 CHAN_LEVEL_OPDEOPME },
	{ CA_PROTECT,	   "PROTECT",	  CHAN_LEVEL_PROTECT },
	{ CA_PROTECTME,	 "PROTECTME",	CHAN_LEVEL_PROTECTME },
	{ CA_TOPIC,		 "TOPIC",		CHAN_LEVEL_TOPIC },
	{ CA_UNBAN,		 "UNBAN",		CHAN_LEVEL_UNBAN },
	{ CA_VOICE,		 "VOICE",		CHAN_LEVEL_VOICE },
	{ CA_VOICEME,	   "VOICEME",	  CHAN_LEVEL_VOICEME },
	{ CA_MEMO,		  "MEMO",		 CHAN_LEVEL_MEMO },
	{ CA_ASSIGN,		"ASSIGN",	   CHAN_LEVEL_ASSIGN },
	{ CA_BADWORDS,	  "BADWORDS",	 CHAN_LEVEL_BADWORDS },
	{ CA_FANTASIA,	  "FANTASIA",	 CHAN_LEVEL_FANTASIA },
	{ CA_GREET,		"GREET",		CHAN_LEVEL_GREET },
	{ CA_NOKICK,		"NOKICK",	   CHAN_LEVEL_NOKICK },
	{ CA_SAY,		"SAY",		CHAN_LEVEL_SAY },
	{ CA_AUTOOWNER,		"AUTOOWNER",	CHAN_LEVEL_AUTOOWNER },
	{ CA_OWNER,		"OWNER",	CHAN_LEVEL_OWNER },
	{ CA_OWNERME,		"OWNERME",	CHAN_LEVEL_OWNERME },
		{ -1 }
};
int levelinfo_maxwidth = 0;

/*************************************************************************/

void moduleAddChanServCmds() {
	ModuleManager::LoadModuleList(ChanServCoreModules);
}

/* *INDENT-ON* */
/*************************************************************************/

class ChanServTimer : public Timer
{
	private:
		std::string channel;

	public:
		ChanServTimer(long delay, const std::string &chan) : Timer(delay), channel(chan)
		{
		}

		void Tick(time_t ctime)
		{
			ChannelInfo *ci = cs_findchan(channel.c_str());

			if (ci)
				ci->UnsetFlag(CI_INHABIT);

			ircdproto->SendPart(findbot(s_ChanServ), channel.c_str(), NULL);
		}
};

/*************************************************************************/

/* Returns modes for mlock in a nice way. */

char *get_mlock_modes(ChannelInfo * ci, int complete)
{
	static char res[BUFSIZE];
	char *end, *value;
	ChannelMode *cm;
	ChannelModeParam *cmp;
	std::map<char, ChannelMode *>::iterator it;
	std::string param;

	memset(&res, '\0', sizeof(res));
	end = res;

	if (ci->GetMLockCount(true) || ci->GetMLockCount(false))
	{
		if (ci->GetMLockCount(true))
		{
			*end++ = '+';

			for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
			{
				cm = it->second;

				if (ci->HasMLock(cm->Name, true))
					*end++ = it->first;
			}
		}

		if (ci->GetMLockCount(false))
		{
			*end++ = '-';

			for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
			{
				cm = it->second;

				if (ci->HasMLock(cm->Name, false))
					*end++ = it->first;
			}
		}

		if (ci->GetMLockCount(true) && complete)
		{
			for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
			{
				cm = it->second;

				if (cm->Type == MODE_PARAM)
				{
					cmp = static_cast<ChannelModeParam *>(cm);

					ci->GetParam(cmp->Name, &param);

					if (!param.empty())
					{
						value = const_cast<char *>(param.c_str());

						*end++ = ' ';
						while (*value)
							*end++ = *value++;
					}
				}
			}
		}
	}

	return res;
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_chanserv_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	unsigned i, j;
	ChannelInfo *ci;
	std::string param;

	for (i = 0; i < 256; i++) {
		for (ci = chanlists[i]; ci; ci = ci->next) {
			count++;
			mem += sizeof(*ci);
			if (ci->desc)
				mem += strlen(ci->desc) + 1;
			if (ci->url)
				mem += strlen(ci->url) + 1;
			if (ci->email)
				mem += strlen(ci->email) + 1;
			mem += ci->GetAccessCount() * sizeof(ChanAccess);
			mem += ci->GetAkickCount() * sizeof(AutoKick);

			if (ci->GetParam(CMODE_KEY, &param))
				mem += param.length() + 1;

			if (ci->GetParam(CMODE_FLOOD, &param))
				mem += param.length() + 1;

			if (ci->GetParam(CMODE_REDIRECT, &param))
				mem += param.length() + 1;

			if (ci->last_topic)
				mem += strlen(ci->last_topic) + 1;
			if (ci->entry_message)
				mem += strlen(ci->entry_message) + 1;
			if (ci->forbidby)
				mem += strlen(ci->forbidby) + 1;
			if (ci->forbidreason)
				mem += strlen(ci->forbidreason) + 1;
			if (ci->levels)
				mem += sizeof(*ci->levels) * CA_SIZE;
			mem += ci->memos.memos.size() * sizeof(Memo);
			for (j = 0; j < ci->memos.memos.size(); j++) {
				if (ci->memos.memos[j]->text)
					mem += strlen(ci->memos.memos[j]->text) + 1;
			}
			if (ci->ttb)
				mem += sizeof(*ci->ttb) * TTB_SIZE;
			mem += ci->bwcount * sizeof(BadWord);
			for (j = 0; j < ci->bwcount; j++)
				if (ci->badwords[j].word)
					mem += strlen(ci->badwords[j].word) + 1;
		}
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* ChanServ initialization. */

void cs_init()
{
	moduleAddChanServCmds();
}

/*************************************************************************/

/* Main ChanServ routine. */

void chanserv(User * u, char *buf)
{
	char *cmd, *s;

	cmd = strtok(buf, " ");

	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			*s = 0;
		}
		ircdproto->SendCTCP(findbot(s_ChanServ), u->nick, "PING %s", s);
	} else {
		mod_run_cmd(s_ChanServ, u, CHANSERV, cmd);
	}
}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", ChanDBName);	\
	failed = 1;					\
	break;						\
	}							\
} while (0)

void load_cs_dbase()
{
	dbFILE *f;
	int ver, c;
	unsigned i, j;
	ChannelInfo *ci, **last, *prev;
	int failed = 0;

	if (!(f = open_db(s_ChanServ, ChanDBName, "r", CHAN_VERSION)))
		return;

	ver = get_file_version(f);

	if (ver != 16)
		fatal("Invalid database version! (I only understand %d)", CHAN_VERSION);

	for (i = 0; i < 256 && !failed; i++) {
		uint16 tmp16;
		uint32 tmp32;
		unsigned int read;
		int n_levels;
		char *s;
		NickAlias *na;

		last = &chanlists[i];
		prev = NULL;
		while ((c = getc_db(f)) != 0) {
			if (c != 1)
				fatal("Invalid format in %s", ChanDBName);
			ci = new ChannelInfo();
			*last = ci;
			last = &ci->next;
			ci->prev = prev;
			prev = ci;
			SAFE(read = read_buffer(ci->name, f));
			SAFE(read_string(&s, f));
			if (s) {
				ci->founder = findcore(s);
				delete [] s;
			} else
				ci->founder = NULL;
			if (ver >= 7) {
				SAFE(read_string(&s, f));
				if (s) {
					if (ver >= 13)
						ci->successor = findcore(s);
					else {
						na = findnick(s);
						if (na)
							ci->successor = na->nc;
						else
							ci->successor = NULL;
					}
					delete [] s;
				} else
					ci->successor = NULL;
			} else {
				ci->successor = NULL;
			}

			char nothing[PASSMAX];
			SAFE(read = read_buffer(nothing, f)); // XXX founder pass was removed.. just here so it works

			SAFE(read_string(&ci->desc, f));
			if (!ci->desc)
				ci->desc = sstrdup("");
			SAFE(read_string(&ci->url, f));
			SAFE(read_string(&ci->email, f));
			SAFE(read_int32(&tmp32, f));
			ci->time_registered = tmp32;
			SAFE(read_int32(&tmp32, f));
			ci->last_used = tmp32;
			SAFE(read_string(&ci->last_topic, f));
			SAFE(read = read_buffer(ci->last_topic_setter, f));
			SAFE(read_int32(&tmp32, f));
			ci->last_topic_time = tmp32;
			//SAFE(read_int32(&ci->flags, f));
			SAFE(read_int32(&tmp32, f));

			/* Leaveops cleanup */
//			if (ver <= 13 && (ci->HasFlag()0x00000020))
//				ci->UnsetFlag()0x00000020;
			/* Temporary flags cleanup */
			ci->UnsetFlag(CI_INHABIT);

			SAFE(read_string(&ci->forbidby, f));
			SAFE(read_string(&ci->forbidreason, f));
			SAFE(read_int16(&tmp16, f));
			ci->bantype = tmp16;
			SAFE(read_int16(&tmp16, f));
			n_levels = tmp16;
			ci->levels = new int16[CA_SIZE];
			reset_levels(ci);
			for (j = 0; j < n_levels; j++) {
				SAFE(read_int16(&tmp16, f));
				if (j < CA_SIZE)
					ci->levels[j] = static_cast<int16>(tmp16);
			}

			uint16 accesscount = 0;
			SAFE(read_int16(&accesscount, f));
			if (accesscount) {
				for (j = 0; j < accesscount; j++) {
					uint16 in_use = 0;
					SAFE(read_int16(&in_use, f));
					if (in_use) {
						uint16 level;
						SAFE(read_int16(&level, f));
						NickCore *nc;
						SAFE(read_string(&s, f));
						if (s) {
							nc = findcore(s);
							delete [] s;
						}
						else
							nc = NULL;
						uint32 last_seen;
						SAFE(read_int32(&last_seen, f));
						//SAFE(read_string(&s, f));
						if (nc)
						{
							//std::string creator = s ? s : "";
							std::string creator = "";
							ci->AddAccess(nc, level, creator, last_seen);
						}
					}
				}
			}

			uint16 akickcount = 0;
			NickCore *nc;
			SAFE(read_int16(&akickcount, f));
			if (akickcount) {
				for (j = 0; j < akickcount; ++j)
				{
					uint16 flags;
					SAFE(read_int16(&flags, f));
					SAFE(read_string(&s, f));
					char *akickreason;
					SAFE(read_string(&akickreason, f));
					char *akickcreator;
					SAFE(read_string(&akickcreator, f));
					SAFE(read_int32(&tmp32, f));

					if (flags & AK_ISNICK)
					{
						nc = findcore(s);
						if (!nc)
							continue;
						ci->AddAkick(akickcreator, nc, akickreason ? akickreason : "", tmp32);
					}
					else
						ci->AddAkick(akickcreator, s, akickreason ? akickreason : "", tmp32);
				}
			}

			//SAFE(read_int32(&ci->mlock_on, f));
			//SAFE(read_int32(&ci->mlock_off, f));
			// Clearly this doesn't work
			SAFE(read_int32(&tmp32, f));
			SAFE(read_int32(&tmp32, f));

			SAFE(read_int32(&tmp32, f));
			if (tmp32)
			{
				std::ostringstream limit;
				limit << tmp32;
				ci->SetParam(CMODE_LIMIT, limit.str());
			}

			SAFE(read_string(&s, f));
			if (s)
			{
				ci->SetParam(CMODE_KEY, std::string(s));
				delete [] s;
			}

			SAFE(read_string(&s, f));
			if (s)
			{
				ci->SetParam(CMODE_FLOOD, std::string(s));
				delete [] s;
			}

			SAFE(read_string(&s, f));
			if (s)
			{
				ci->SetParam(CMODE_REDIRECT, std::string(s));
				delete [] s;
			}

			SAFE(read_int16(&tmp16, f));
			if (tmp16) ci->memos.memos.resize(tmp16);
			SAFE(read_int16(&tmp16, f));
			ci->memos.memomax = static_cast<int16>(tmp16);
			if (!ci->memos.memos.empty()) {
				for (j = 0; j < ci->memos.memos.size(); j++) {
					ci->memos.memos[j] = new Memo;
					Memo *memo = ci->memos.memos[j];
					SAFE(read_int32(&memo->number, f));
					//SAFE(read_int16(&memo->flags, f));
					SAFE(read_int16(&tmp16, f));
					SAFE(read_int32(&tmp32, f));
					memo->time = tmp32;
					SAFE(read = read_buffer(memo->sender, f));
					SAFE(read_string(&memo->text, f));
				}
			}

			SAFE(read_string(&ci->entry_message, f));

			ci->c = NULL;

			/* BotServ options */
			int n_ttb;

			SAFE(read_string(&s, f));
			if (s) {
				ci->bi = findbot(s);
				delete [] s;
			} else
				ci->bi = NULL;

			SAFE(read_int32(&tmp32, f));
			//ci->botflags = tmp32;
			SAFE(read_int16(&tmp16, f));
			n_ttb = tmp16;
			ci->ttb = new int16[2 * TTB_SIZE];
			for (j = 0; j < n_ttb; j++) {
				SAFE(read_int16(&tmp16, f));
				if (j < TTB_SIZE)
					ci->ttb[j] = static_cast<int16>(tmp16);
			}
			for (j = n_ttb; j < TTB_SIZE; j++)
				ci->ttb[j] = 0;
			SAFE(read_int16(&tmp16, f));
			ci->capsmin = tmp16;
			SAFE(read_int16(&tmp16, f));
			ci->capspercent = tmp16;
			SAFE(read_int16(&tmp16, f));
			ci->floodlines = tmp16;
			SAFE(read_int16(&tmp16, f));
			ci->floodsecs = tmp16;
			SAFE(read_int16(&tmp16, f));
			ci->repeattimes = tmp16;

			SAFE(read_int16(&ci->bwcount, f));
			if (ci->bwcount) {
				ci->badwords = static_cast<BadWord *>(scalloc(ci->bwcount, sizeof(BadWord)));
				for (j = 0; j < ci->bwcount; j++) {
					SAFE(read_int16(&ci->badwords[j].in_use, f));
					if (ci->badwords[j].in_use) {
						SAFE(read_string(&ci->badwords[j].word, f));
						//SAFE(read_int16(&ci->badwords[j].type, f));
						SAFE(read_int16(&tmp16, f));
						ci->badwords[j].type = BW_ANY; // for now
					}
				}
			} else {
				ci->badwords = NULL;
			}
		}					   /* while (getc_db(f) != 0) */

		*last = NULL;

	}						   /* for (i) */

	close_db(f);

	/* Check for non-forbidden channels with no founder.
	   Makes also other essential tasks. */
	for (i = 0; i < 256; i++) {
		ChannelInfo *next;
		for (ci = chanlists[i]; ci; ci = next) {
			next = ci->next;
			if (!(ci->HasFlag(CI_FORBIDDEN)) && !ci->founder) {
				alog("%s: database load: Deleting founderless channel %s",
					 s_ChanServ, ci->name);
				delchan(ci);
				continue;
			}
		}
	}
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", ChanDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", ChanDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)

void save_cs_dbase()
{
	dbFILE *f;
	unsigned i, j;
	ChannelInfo *ci;
	static time_t lastwarn = 0;
	std::string param;

	if (!(f = open_db(s_ChanServ, ChanDBName, "w", CHAN_VERSION)))
		return;

	for (i = 0; i < 256; i++) {
		int16 tmp16;
		unsigned int written;

		for (ci = chanlists[i]; ci; ci = ci->next) {
			SAFE(write_int8(1, f));
			SAFE(written = write_buffer(ci->name, f));
			if (ci->founder)
				SAFE(write_string(ci->founder->display, f));
			else
				SAFE(write_string(NULL, f));
			if (ci->successor)
				SAFE(write_string(ci->successor->display, f));
			else
				SAFE(write_string(NULL, f));

			char nothing[PASSMAX]; /* founder passwords were removed! */
			strcpy(nothing, "nothing\0");
			SAFE(written = write_buffer(nothing, f));

			SAFE(write_string(ci->desc, f));
			SAFE(write_string(ci->url, f));
			SAFE(write_string(ci->email, f));
			SAFE(write_int32(ci->time_registered, f));
			SAFE(write_int32(ci->last_used, f));
			SAFE(write_string(ci->last_topic, f));
			SAFE(written = write_buffer(ci->last_topic_setter, f));
			SAFE(write_int32(ci->last_topic_time, f));
			//SAFE(write_int32(ci->flags, f));
			SAFE(write_int32(0, f));
			SAFE(write_string(ci->forbidby, f));
			SAFE(write_string(ci->forbidreason, f));
			SAFE(write_int16(ci->bantype, f));

			tmp16 = CA_SIZE;
			SAFE(write_int16(tmp16, f));
			for (j = 0; j < CA_SIZE; j++)
				SAFE(write_int16(ci->levels[j], f));

			SAFE(write_int16(ci->GetAccessCount(), f));
			for (j = 0; j < ci->GetAccessCount(); j++) {
				ChanAccess *access = ci->GetAccess(j);
				if (!access->in_use)
					continue;
				SAFE(write_int16(access->in_use, f));
				SAFE(write_int16(access->level, f));
				SAFE(write_string(access->nc->display, f));
				SAFE(write_int32(access->last_seen, f));
				//SAFE(write_string(access->creator.c_str(), f));
			}

			SAFE(write_int16(ci->GetAkickCount(), f));
			for (j = 0; j < ci->GetAkickCount(); ++j)
			{
				AutoKick *akick = ci->GetAkick(j);
				//SAFE(write_int16(akick->flags, f));
				SAFE(write_int16(0, f));
				if (akick->HasFlag(AK_ISNICK))
					SAFE(write_string(akick->nc->display, f));
				else
					SAFE(write_string(akick->mask.c_str(), f));
				SAFE(write_string(akick->reason.c_str(), f));
				SAFE(write_string(akick->creator.c_str(), f));
				SAFE(write_int32(akick->addtime, f));
			}

			//SAFE(write_int32(ci->mlock_on, f));
			//SAFE(write_int32(ci->mlock_off, f));
			// Clearly this doesnt work
			SAFE(write_int32(NULL, f));
			SAFE(write_int32(NULL, f));

			ci->GetParam(CMODE_LIMIT, &param);
			SAFE(write_int32(param.empty() ? NULL : atoi(param.c_str()), f));

			ci->GetParam(CMODE_KEY, &param);
			SAFE(write_string(param.empty() ? NULL : param.c_str(), f));

			ci->GetParam(CMODE_FLOOD, &param);
			SAFE(write_string(param.empty() ? NULL : param.c_str(), f));

			ci->GetParam(CMODE_REDIRECT, &param);
			SAFE(write_string(param.empty() ? NULL : param.c_str(), f));

			SAFE(write_int16(ci->memos.memos.size(), f));
			SAFE(write_int16(ci->memos.memomax, f));
			for (j = 0; j < ci->memos.memos.size(); j++) {
				Memo *memo = ci->memos.memos[j];
				SAFE(write_int32(memo->number, f));
				SAFE(write_int16(0, f));
				//SAFE(write_int16(memo->flags, f));
				SAFE(write_int32(memo->time, f));
				SAFE(written = write_buffer(memo->sender, f));
				SAFE(write_string(memo->text, f));
			}

			SAFE(write_string(ci->entry_message, f));

			if (ci->bi)
				SAFE(write_string(ci->bi->nick, f));
			else
				SAFE(write_string(NULL, f));

			//SAFE(write_int32(ci->botflags, f));
			SAFE(write_int32(0, f));

			tmp16 = TTB_SIZE;
			SAFE(write_int16(tmp16, f));
			for (j = 0; j < TTB_SIZE; j++)
				SAFE(write_int16(ci->ttb[j], f));

			SAFE(write_int16(ci->capsmin, f));
			SAFE(write_int16(ci->capspercent, f));
			SAFE(write_int16(ci->floodlines, f));
			SAFE(write_int16(ci->floodsecs, f));
			SAFE(write_int16(ci->repeattimes, f));

			SAFE(write_int16(ci->bwcount, f));
			for (j = 0; j < ci->bwcount; j++) {
				SAFE(write_int16(ci->badwords[j].in_use, f));
				if (ci->badwords[j].in_use) {
					SAFE(write_string(ci->badwords[j].word, f));
					SAFE(write_int16(ci->badwords[j].type, f));
				}
			}
		}					   /* for (chanlists[i]) */

		SAFE(write_int8(0, f));

	}						   /* for (i) */

	close_db(f);

}

#undef SAFE

/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them.
 *
 * Also check to make sure that every mode set or unset is allowed by the
 * defcon mlock settings. This is more important than any normal channel
 * mlock'd mode. --gdex (21-04-07)
 */

void check_modes(Channel * c)
{
	time_t t = time(NULL);
	ChannelInfo *ci;
	ChannelMode *cm;
	ChannelModeParam *cmp;
	std::string modebuf, argbuf;
	std::map<char, ChannelMode *>::iterator it;
	std::string param, ciparam;

	if (!c)
	{
		if (debug)
			alog("debug: check_modes called with NULL values");
		return;
	}

	if (c->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (c->server_modecount >= 3 && c->chanserv_modecount >= 3)
	{
		ircdproto->SendGlobops(NULL, "Warning: unable to set modes on channel %s. Are your servers' U:lines configured correctly?", c->name);
		alog("%s: Bouncy modes on channel %s", s_ChanServ, c->name);
		c->bouncy_modes = 1;
		return;
	}

	if (c->chanserv_modetime != time(NULL))
	{
		c->chanserv_modecount = 0;
		c->chanserv_modetime = t;
	}
	c->chanserv_modecount++;

	/* Check if the channel is registered; if not remove mode -r */
	if (!(ci = c->ci))
	{
		if (c->HasMode(CMODE_REGISTERED))
		{
			c->RemoveMode(CMODE_REGISTERED);
			ircdproto->SendMode(whosends(ci), c->name, "-r");
		}
		return;
	}

	modebuf = "+";

	for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
	{
		cm = it->second;

		/* If this channel does not have the mode and the mode is mlocked */
		if (!c->HasMode(cm->Name) && ci->HasMLock(cm->Name, true))
		{
			modebuf += it->first;
			c->SetMode(cm->Name);

			/* Add the eventual parameter and modify the Channel structure */
			if (cm->Type == MODE_PARAM)
			{
				cmp = static_cast<ChannelModeParam *>(cm);
				ci->GetParam(cmp->Name, &param);

				if (!param.empty())
				{
					argbuf += " " + param;
				}
			}
		}
		/* If this is a param mode and its mlocked and is set negative */
		else if (cm->Type == MODE_PARAM && c->HasMode(cm->Name) && ci->HasMLock(cm->Name, true))
		{
			cmp = static_cast<ChannelModeParam *>(cm);
			c->GetParam(cmp->Name, &param);
			ci->GetParam(cmp->Name, &ciparam);

			if (!param.empty() && !ciparam.empty() && param != ciparam)
			{
				modebuf += it->first;

				c->SetParam(cmp->Name, ciparam);

				argbuf += " " + ciparam;
			}
		}
	}

	if (modebuf == "+")
		modebuf.clear();

	modebuf += "-";

	for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
	{
		cm = it->second;

		/* If the channel has the mode */
		if (c->HasMode(cm->Name) && ci->HasMLock(cm->Name, false))
		{
			modebuf += it->first;
			c->RemoveMode(cm->Name);

			/* Add the eventual parameter */
			if (cm->Type == MODE_PARAM)
			{
				cmp = static_cast<ChannelModeParam *>(cm);

				if (!cmp->MinusNoArg)
				{
					c->GetParam(cmp->Name, &param);

					if (!param.empty())
					{
						argbuf += " " + param;
					}
				}
			}
		}
	}

	if (modebuf[modebuf.length() - 1] == '-')
		modebuf.erase(modebuf.length() - 1);
	
	if (modebuf.empty())
		return;

	ircdproto->SendMode((ci ? whosends(ci) : findbot(s_OperServ)), c->name, "%s%s", modebuf.c_str(), argbuf.empty() ? "" : argbuf.c_str());
}

/*************************************************************************/

int check_valid_admin(User * user, Channel * chan, int servermode)
{
	ChannelMode *cm;

	if (!chan || !chan->ci)
		return 1;

	if (!(cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
		return 0;

	/* They will be kicked; no need to deop, no need to update our internal struct too */
	if (chan->ci->HasFlag(CI_FORBIDDEN))
		return 0;

	if (servermode && !check_access(user, chan->ci, CA_AUTOPROTECT)) {
		notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
		ircdproto->SendMode(whosends(chan->ci), chan->name, "-%s %s",
					   cm->ModeChar, user->nick);
		return 0;
	}

	if (check_access(user, chan->ci, CA_AUTODEOP)) {
		ircdproto->SendMode(whosends(chan->ci), chan->name, "-%s %s",
					   cm->ModeChar, user->nick);
		return 0;
	}

	return 1;
}

/*************************************************************************/

/* Check whether a user is allowed to be opped on a channel; if they
 * aren't, deop them.  If serverop is 1, the +o was done by a server.
 * Return 1 if the user is allowed to be opped, 0 otherwise. */

int check_valid_op(User * user, Channel * chan, int servermode)
{
	ChannelMode *owner, *protect, *halfop;
	if (!chan || !chan->ci)
		return 1;

	/* They will be kicked; no need to deop, no need to update our internal struct too */
	if (chan->ci->HasFlag(CI_FORBIDDEN))
		return 0;

	owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
	protect = ModeManager::FindChannelModeByName(CMODE_PROTECT);
	halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);

	if (servermode && !check_access(user, chan->ci, CA_AUTOOP)) {
		notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
		if (halfop)
		{
			if (owner && protect)
			{
				if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "-%so%s %s %s %s", protect->ModeChar,
								   owner->ModeChar, user->nick,
								   user->nick, user->nick);
				} else {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "-%sho%s %s %s %s %s",
								   protect->ModeChar,
								   owner->ModeChar,
								   user->nick, user->nick, user->nick,
								   user->nick);
				}
			} else if (!owner && protect) {
				if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "-%so %s %s", protect->ModeChar,
								   user->nick, user->nick);
				} else {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "-%soh %s %s %s", protect->ModeChar,
								   user->nick, user->nick, user->nick);
				}
			} else {
				if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
					ircdproto->SendMode(whosends(chan->ci), chan->name, "-o %s",
								   user->nick);
				} else {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "-ho %s %s", user->nick, user->nick);
				}
			}
		} else {
			ircdproto->SendMode(whosends(chan->ci), chan->name, "-o %s",
						   user->nick);
		}
		return 0;
	}

	if (check_access(user, chan->ci, CA_AUTODEOP)) {
		if (halfop) {
			if (owner && protect) {
				ircdproto->SendMode(whosends(chan->ci), chan->name,
							   "-%sho%s %s %s %s %s", protect->ModeChar,
							   owner->ModeChar, user->nick, user->nick,
							   user->nick, user->nick);
			} else {
				ircdproto->SendMode(whosends(chan->ci), chan->name, "-ho %s %s",
							   user->nick, user->nick);
			}
		} else {
			ircdproto->SendMode(whosends(chan->ci), chan->name, "-o %s",
						   user->nick);
		}
		return 0;
	}

	return 1;
}

/*************************************************************************/

/* Check whether a user should be opped on a channel, and if so, do it.
 * Return 1 if the user was opped, 0 otherwise.  (Updates the channel's
 * last used time if the user was opped.) */

int check_should_op(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || (ci->HasFlag(CI_FORBIDDEN)) || *chan == '+')
		return 0;

	if ((ci->HasFlag(CI_SECURE)) && !nick_identified(user))
		return 0;

	if (check_access(user, ci, CA_AUTOOP)) {
		ircdproto->SendMode(whosends(ci), chan, "+o %s", user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

/* Check whether a user should be voiced on a channel, and if so, do it.
 * Return 1 if the user was voiced, 0 otherwise. */

int check_should_voice(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || (ci->HasFlag(CI_FORBIDDEN)) || *chan == '+')
		return 0;

	if ((ci->HasFlag(CI_SECURE)) && !nick_identified(user))
		return 0;

	if (check_access(user, ci, CA_AUTOVOICE)) {
		ircdproto->SendMode(whosends(ci), chan, "+v %s", user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_halfop(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || (ci->HasFlag(CI_FORBIDDEN)) || *chan == '+')
		return 0;

	if (check_access(user, ci, CA_AUTOHALFOP)) {
		ircdproto->SendMode(whosends(ci), chan, "+h %s", user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_owner(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);
	ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

	if (!ci || (ci->HasFlag(CI_FORBIDDEN)) || *chan == '+')
		return 0;

	if (((ci->HasFlag(CI_SECUREFOUNDER)) && IsRealFounder(user, ci))
		|| (!(ci->HasFlag(CI_SECUREFOUNDER)) && IsFounder(user, ci))) {
		ircdproto->SendMode(whosends(ci), chan, "+o%s %s %s", cm->ModeChar, user->nick,
					   user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_protect(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);
	ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

	if (!ci || (ci->HasFlag(CI_FORBIDDEN)) || *chan == '+')
		return 0;

	if (check_access(user, ci, CA_AUTOPROTECT)) {
		ircdproto->SendMode(whosends(ci), chan, "+o%s %s %s", cm->ModeChar, user->nick,
					   user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

/* Check whether a user is permitted to be on a channel.  If so, return 0;
 * else, kickban the user with an appropriate message (could be either
 * AKICK or restricted access) and return 1.  Note that this is called
 * _before_ the user is added to internal channel lists (so do_kick() is
 * not called). The channel TS must be given for a new channel.
 */

int check_kick(User * user, const char *chan, time_t chants)
{
	ChannelInfo *ci = cs_findchan(chan);
	Channel *c;
	AutoKick *akick;
	bool set_modes = false;
	NickCore *nc;
	const char *av[4];
	int ac;
	char buf[BUFSIZE];
	char mask[BUFSIZE];
	const char *reason;
	ChanServTimer *t;

	if (!ci)
		return 0;

	if (user->isSuperAdmin == 1)
		return 0;

	/* We don't enforce services restrictions on clients on ulined services
	 * as this will likely lead to kick/rejoin floods. ~ Viper */
	if (is_ulined(user->server->name)) {
		return 0;
	}

	if (ci->HasFlag(CI_SUSPENDED) || ci->HasFlag(CI_FORBIDDEN))
	{
		if (is_oper(user))
			return 0;

		get_idealban(ci, user, mask, sizeof(mask));
		reason = ci->forbidreason ? ci->forbidreason : getstring(user, CHAN_MAY_NOT_BE_USED);
		set_modes = true;
		goto kick;
	}

	if (user->nc || user->IsRecognized())
		nc = user->nc;
	else
		nc = NULL;

	/*
	 * Before we go through akick lists, see if they're excepted FIRST
	 * We cannot kick excempted users that are akicked or not on the channel access list
	 * as that will start services <-> server wars which ends up as a DoS against services.
	 *
	 * UltimateIRCd 3.x at least informs channel staff when a joining user is matching an exempt.
	 */
	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted(ci, user) == 1) {
		return 0;
	}

	for (unsigned j = 0; j < ci->GetAkickCount(); ++j)
	{
		akick = ci->GetAkick(j);

		if (!akick->HasFlag(AK_USED))
			continue;

		if ((akick->HasFlag(AK_ISNICK) && akick->nc == nc)
			|| (!akick->HasFlag(AK_ISNICK)
			&& match_usermask(akick->mask.c_str(), user)))
			{
			if (debug >= 2)
				alog("debug: %s matched akick %s", user->nick, akick->HasFlag(AK_ISNICK) ? akick->nc->display : akick->mask.c_str());
			if (akick->HasFlag(AK_ISNICK))
				get_idealban(ci, user, mask, sizeof(mask));
			else
				strlcpy(mask, akick->mask.c_str(), sizeof(mask));
			reason = !akick->reason.empty() ? akick->reason.c_str() : CSAutokickReason;
			goto kick;
			}
	}


	if (check_access(user, ci, CA_NOJOIN)) {
		get_idealban(ci, user, mask, sizeof(mask));
		reason = getstring(user, CHAN_NOT_ALLOWED_TO_JOIN);
		goto kick;
	}

	return 0;

  kick:
	if (debug)
		alog("debug: channel: AutoKicking %s!%s@%s from %s", user->nick,
			 user->GetIdent().c_str(), user->host, chan);

	/* Remember that the user has not been added to our channel user list
	 * yet, so we check whether the channel does not exist OR has no user
	 * on it (before SJOIN would have created the channel structure, while
	 * JOIN would not). */
	/* Don't check for CI_INHABIT before for the Channel record cos else
	 * c may be NULL even if it exists */
	if ((!(c = findchan(chan)) || c->usercount == 0) && !ci->HasFlag(CI_INHABIT))
	{
		ircdproto->SendJoin(findbot(s_ChanServ), chan, (c ? c->creation_time : chants));
		/*
		 * If channel was forbidden, etc, set it +si to prevent rejoin
		 */
		if (set_modes)
		{
			ircdproto->SendMode(findbot(s_ChanServ), chan, "+ntsi");
		}

		t = new ChanServTimer(CSInhabit, chan);
		ci->SetFlag(CI_INHABIT);
	}

	if (c) {
		if (ircdcap->tsmode) {
			snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
			av[0] = chan;
			av[1] = buf;
			av[2] = "+b";
			av[3] = mask;
			ac = 4;
		} else {
			av[0] = chan;
			av[1] = "+b";
			av[2] = mask;
			ac = 3;
		}

		do_cmode(whosends(ci)->nick, ac, av);
	}

	ircdproto->SendMode(whosends(ci), chan, "+b %s", mask);
	ircdproto->SendKick(whosends(ci), chan, user->nick, "%s", reason);

	return 1;
}

/*************************************************************************/

/* Record the current channel topic in the ChannelInfo structure. */

void record_topic(const char *chan)
{
	Channel *c;
	ChannelInfo *ci;

	if (readonly)
		return;

	c = findchan(chan);
	if (!c || !(ci = c->ci))
		return;

	if (ci->last_topic)
		delete [] ci->last_topic;

	if (c->topic)
		ci->last_topic = sstrdup(c->topic);
	else
		ci->last_topic = NULL;

	strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
	ci->last_topic_time = c->topic_time;
}

/*************************************************************************/

/* Restore the topic in a channel when it's created, if we should. */

void restore_topic(const char *chan)
{
	Channel *c = findchan(chan);
	ChannelInfo *ci;

	if (!c || !(ci = c->ci))
		return;
	/* We can be sure that the topic will be in sync when we return -GD */
	c->topic_sync = 1;
	if (!(ci->HasFlag(CI_KEEPTOPIC))) {
		/* We need to reset the topic here, since it's currently empty and
		 * should be updated with a TOPIC from the IRCd soon. -GD
		 */
		ci->last_topic = NULL;
		strscpy(ci->last_topic_setter, whosends(ci)->nick, NICKMAX);
		ci->last_topic_time = time(NULL);
		return;
	}
	if (c->topic)
		delete [] c->topic;
	if (ci->last_topic) {
		c->topic = sstrdup(ci->last_topic);
		strscpy(c->topic_setter, ci->last_topic_setter, NICKMAX);
		c->topic_time = ci->last_topic_time;
	} else {
		c->topic = NULL;
		strscpy(c->topic_setter, whosends(ci)->nick, NICKMAX);
	}
	if (ircd->join2set) {
		if (whosends(ci) == findbot(s_ChanServ)) {
			ircdproto->SendJoin(findbot(s_ChanServ), chan, c->creation_time);
			ircdproto->SendMode(NULL, chan, "+o %s", s_ChanServ);
		}
	}
	ircdproto->SendTopic(whosends(ci), c->name, c->topic_setter,
					c->topic ? c->topic : "", c->topic_time);
	if (ircd->join2set) {
		if (whosends(ci) == findbot(s_ChanServ)) {
			ircdproto->SendPart(findbot(s_ChanServ), c->name, NULL);
		}
	}
}

/*************************************************************************/

/* See if the topic is locked on the given channel, and return 1 (and fix
 * the topic) if so. */

int check_topiclock(Channel * c, time_t topic_time)
{
	ChannelInfo *ci;

	if (!c) {
		if (debug) {
			alog("debug: check_topiclock called with NULL values");
		}
		return 0;
	}

	if (!(ci = c->ci) || !(ci->HasFlag(CI_TOPICLOCK)))
		return 0;

	if (c->topic)
		delete [] c->topic;
	if (ci->last_topic) {
		c->topic = sstrdup(ci->last_topic);
		strscpy(c->topic_setter, ci->last_topic_setter, NICKMAX);
	} else {
		c->topic = NULL;
		/* Bot assigned & Symbiosis ON?, the bot will set the topic - doc */
		/* Altough whosends() also checks for BSMinUsers -GD */
		strscpy(c->topic_setter, whosends(ci)->nick, NICKMAX);
	}

	if (ircd->topictsforward) {
		/* Because older timestamps are rejected */
		/* Some how the topic_time from do_topic is 0 set it to current + 1 */
		if (!topic_time) {
			c->topic_time = time(NULL) + 1;
		} else {
			c->topic_time = topic_time + 1;
		}
	} else {
		/* If no last topic, we can't use last topic time! - doc */
		if (ci->last_topic)
			c->topic_time = ci->last_topic_time;
		else
			c->topic_time = time(NULL) + 1;
	}

	if (ircd->join2set) {
		if (whosends(ci) == findbot(s_ChanServ)) {
			ircdproto->SendJoin(findbot(s_ChanServ), c->name, c->creation_time);
			ircdproto->SendMode(NULL, c->name, "+o %s", s_ChanServ);
		}
	}

	ircdproto->SendTopic(whosends(ci), c->name, c->topic_setter,
					c->topic ? c->topic : "", c->topic_time);

	if (ircd->join2set) {
		if (whosends(ci) == findbot(s_ChanServ)) {
			ircdproto->SendPart(findbot(s_ChanServ), c->ci->name, NULL);
		}
	}
	return 1;
}

/*************************************************************************/

/* Remove all channels which have expired. */

void expire_chans()
{
	ChannelInfo *ci, *next;
	int i;
	time_t now = time(NULL);

	if (!CSExpire)
		return;

	for (i = 0; i < 256; i++) {
		for (ci = chanlists[i]; ci; ci = next) {
			next = ci->next;
			if (!ci->c && now - ci->last_used >= CSExpire && !ci->HasFlag(CI_FORBIDDEN) && !ci->HasFlag(CI_NO_EXPIRE) && !ci->HasFlag(CI_SUSPENDED))
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreChanExpire, OnPreChanExpire(ci));
				if (MOD_RESULT == EVENT_STOP)
					continue;

				char *chname = sstrdup(ci->name);
				alog("Expiring channel %s (founder: %s)", ci->name,
					 (ci->founder ? ci->founder->display : "(none)"));
				delchan(ci);
				FOREACH_MOD(I_OnChanExpire, OnChanExpire(chname));
				delete [] chname;
			}
		}
	}
}

/*************************************************************************/

/* Remove a (deleted or expired) nickname from all channel lists. */

void cs_remove_nick(const NickCore * nc)
{
	int i, j;
	ChannelInfo *ci, *next;
	ChanAccess *ca;
	AutoKick *akick;

	for (i = 0; i < 256; i++) {
		for (ci = chanlists[i]; ci; ci = next) {
			next = ci->next;
			if (ci->founder == nc) {
				if (ci->successor) {
					NickCore *nc2 = ci->successor;
					if (!nc2->IsServicesOper() && CSMaxReg && nc2->channelcount >= CSMaxReg) {
						alog("%s: Successor (%s) of %s owns too many channels, " "deleting channel", s_ChanServ, nc2->display, ci->name);
						delchan(ci);
						continue;
					} else {
						alog("%s: Transferring foundership of %s from deleted " "nick %s to successor %s", s_ChanServ, ci->name, nc->display, nc2->display);
						ci->founder = nc2;
						ci->successor = NULL;
						nc2->channelcount++;
					}
				} else {
					alog("%s: Deleting channel %s owned by deleted nick %s", s_ChanServ, ci->name, nc->display);

					if ((ModeManager::FindChannelModeByName(CMODE_REGISTERED)))
					{
						/* Maybe move this to delchan() ? */
						if (ci->c && ci->c->HasMode(CMODE_REGISTERED))
						{
							ci->c->RemoveMode(CMODE_REGISTERED);
							ircdproto->SendMode(whosends(ci), ci->name, "-r");
						}
					}

					delchan(ci);
					continue;
				}
			}

			if (ci->successor == nc)
				ci->successor = NULL;

			for (j = ci->GetAccessCount(); j > 0; --j)
			{
				ca = ci->GetAccess(j - 1);

				if (ca->in_use && ca->nc == nc)
					ci->EraseAccess(j - 1);
			}

			for (j = ci->GetAkickCount(); j > 0; --j)
			{
				akick = ci->GetAkick(j - 1);
				if (akick->HasFlag(AK_USED) && akick->HasFlag(AK_ISNICK) && akick->nc == nc)
					ci->EraseAkick(akick);
			}
		}
	}
}

/*************************************************************************/

/* Return the ChannelInfo structure for the given channel, or NULL if the
 * channel isn't registered. */

ChannelInfo *cs_findchan(const char *chan)
{
	ChannelInfo *ci;

	if (!chan || !*chan) {
		if (debug) {
			alog("debug: cs_findchan() called with NULL values");
		}
		return NULL;
	}

	for (ci = chanlists[static_cast<unsigned char>(tolower(chan[1]))]; ci;
		 ci = ci->next) {
		if (stricmp(ci->name, chan) == 0)
			return ci;
	}
	return NULL;
}

/*************************************************************************/

/* Return 1 if the user's access level on the given channel falls into the
 * given category, 0 otherwise.  Note that this may seem slightly confusing
 * in some cases: for example, check_access(..., CA_NOJOIN) returns true if
 * the user does _not_ have access to the channel (i.e. matches the NOJOIN
 * criterion). */

int check_access(User * user, ChannelInfo * ci, int what)
{
	int level;
	int limit;

	if (!user || !ci) {
		return 0;
	}

	level = get_access(user, ci);
	limit = ci->levels[what];

	/* Resetting the last used time */
	if (level > 0)
		ci->last_used = time(NULL);

	if (what == ACCESS_FOUNDER)
		return IsFounder(user, ci);
	if (level >= ACCESS_FOUNDER)
		return (what == CA_AUTODEOP || what == CA_NOJOIN) ? 0 : 1;
	/* Hacks to make flags work */
	if (what == CA_AUTODEOP && (ci->HasFlag(CI_SECUREOPS)) && level == 0)
		return 1;
	if (limit == ACCESS_INVALID)
		return 0;
	if (what == CA_AUTODEOP || what == CA_NOJOIN)
		return level <= ci->levels[what];
	else
		return level >= ci->levels[what];
}

/*************************************************************************/
/*********************** ChanServ private routines ***********************/
/*************************************************************************/

/* Insert a channel alphabetically into the database. */

void alpha_insert_chan(ChannelInfo * ci)
{
	ChannelInfo *ptr, *prev;
	char *chan;

	if (!ci) {
		if (debug) {
			alog("debug: alpha_insert_chan called with NULL values");
		}
		return;
	}

	chan = ci->name;

	for (prev = NULL, ptr = chanlists[static_cast<unsigned char>(tolower(chan[1]))];
		 ptr != NULL && stricmp(ptr->name, chan) < 0;
		 prev = ptr, ptr = ptr->next);
	ci->prev = prev;
	ci->next = ptr;
	if (!prev)
		chanlists[static_cast<unsigned char>(tolower(chan[1]))] = ci;
	else
		prev->next = ci;
	if (ptr)
		ptr->prev = ci;
}

/*************************************************************************/

/* Add a channel to the database.  Returns a pointer to the new ChannelInfo
 * structure if the channel was successfully registered, NULL otherwise.
 * Assumes channel does not already exist. */

ChannelInfo *makechan(const char *chan)
{
	int i;
	ChannelInfo *ci;

	ci = new ChannelInfo();
	strscpy(ci->name, chan, CHANMAX);
	ci->time_registered = time(NULL);
	reset_levels(ci);
	ci->ttb = new int16[2 * TTB_SIZE];
	for (i = 0; i < TTB_SIZE; i++)
		ci->ttb[i] = 0;
	alpha_insert_chan(ci);
	return ci;
}

/*************************************************************************/

/* Remove a channel from the ChanServ database.  Return 1 on success, 0
 * otherwise. */

int delchan(ChannelInfo * ci)
{
	unsigned i;
	NickCore *nc;

	if (!ci) {
		if (debug) {
			alog("debug: delchan called with NULL values");
		}
		return 0;
	}

	FOREACH_MOD(I_OnDelChan, OnDelChan(ci));

	nc = ci->founder;

	if (debug >= 2) {
		alog("debug: delchan removing %s", ci->name);
	}

	if (ci->bi) {
		ci->bi->chancount--;
	}

	if (debug >= 2) {
		alog("debug: delchan top of removing the bot");
	}
	if (ci->c) {
		if (ci->bi && ci->c->usercount >= BSMinUsers) {
			ircdproto->SendPart(ci->bi, ci->c->name, NULL);
		}
		ci->c->ci = NULL;
	}
	if (debug >= 2) {
		alog("debug: delchan() Bot has been removed moving on");
	}

	if (ci->next)
		ci->next->prev = ci->prev;
	if (ci->prev)
		ci->prev->next = ci->next;
	else
		chanlists[static_cast<unsigned char>(tolower(ci->name[1]))] = ci->next;
	if (ci->desc)
		delete [] ci->desc;
	if (ci->url)
		delete [] ci->url;
	if (ci->email)
		delete [] ci->email;
	if (ci->entry_message)
		delete [] ci->entry_message;

	if (ci->last_topic)
		delete [] ci->last_topic;
	if (ci->forbidby)
		delete [] ci->forbidby;
	if (ci->forbidreason)
		delete [] ci->forbidreason;
	ci->ClearAkick();
	if (ci->levels)
		delete [] ci->levels;
	if (debug >= 2) {
		alog("debug: delchan() top of the memo list");
	}
	if (!ci->memos.memos.empty()) {
		for (i = 0; i < ci->memos.memos.size(); ++i) {
			if (ci->memos.memos[i]->text)
				delete [] ci->memos.memos[i]->text;
			delete ci->memos.memos[i];
		}
		ci->memos.memos.clear();
	}
	if (debug >= 2) {
		alog("debug: delchan() done with the memo list");
	}
	if (ci->ttb)
		delete [] ci->ttb;

	if (debug >= 2) {
		alog("debug: delchan() top of the badword list");
	}
	for (i = 0; i < ci->bwcount; i++) {
		if (ci->badwords[i].word)
			delete [] ci->badwords[i].word;
	}
	if (ci->badwords)
		free(ci->badwords);
	if (debug >= 2) {
		alog("debug: delchan() done with the badword list");
	}


	if (debug >= 2) {
		alog("debug: delchan() calling on moduleCleanStruct()");
	}

	delete ci;
	if (nc)
		nc->channelcount--;

	if (debug >= 2) {
		alog("debug: delchan() all done");
	}
	return 1;
}

/*************************************************************************/

/* Reset channel access level values to their default state. */

void reset_levels(ChannelInfo * ci)
{
	int i;

	if (!ci) {
		if (debug) {
			alog("debug: reset_levels called with NULL values");
		}
		return;
	}

	if (ci->levels)
		delete [] ci->levels;
	ci->levels = new int16[CA_SIZE];
	for (i = 0; def_levels[i][0] >= 0; i++)
		ci->levels[def_levels[i][0]] = def_levels[i][1];
}

/*************************************************************************/

/** Is the user a channel founder? (owner)
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
bool IsFounder(User *user, ChannelInfo *ci)
{
	ChanAccess *access = NULL;

	if (!user || !ci)
		return false;

	if (IsRealFounder(user, ci))
		return true;
	
	if (user->nc)
		access = ci->GetAccess(user->nc);
	else
	{
		NickAlias *na = findnick(user->nick);
		if (na)
			access = ci->GetAccess(na->nc);
	}
	
	/* If they're QOP+ and theyre identified or theyre recognized and the channel isn't secure */
	if (access && access->level >= ACCESS_QOP && (user->nc || (user->IsRecognized() && !(ci->HasFlag(CI_SECURE)))))
		return true;

	return false;
}

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
bool IsRealFounder(User *user, ChannelInfo *ci)
{
	if (!user || !ci)
		return false;

	if (user->isSuperAdmin)
		return true;

	if (user->nc && user->nc == ci->founder)
		return true;

	return false;
}


/** Return the access level for the user on the channel.
 * If the channel doesn't exist, the user isn't on the access list, or the
 * channel is CI_SECURE and the user isn't identified, return 0
 * @param user The user
 * @param ci The cahnnel
 * @return The level, or 0
 */
int get_access(User *user, ChannelInfo *ci)
{
	ChanAccess *access = NULL;

	if (!ci || !user)
		return 0;

	/* SuperAdmin always has highest level */
	if (user->isSuperAdmin)
		return ACCESS_SUPERADMIN;
	
	if (IsFounder(user, ci))
		return ACCESS_FOUNDER;
	
	if (nick_identified(user))
	{
		access = ci->GetAccess(user->nc);
		if (access)
			return access->level;
	}
	else
	{
		NickAlias *na = findnick(user->nick);
		if (na)
			access = ci->GetAccess(na->nc);
		if (access && user->IsRecognized() && !(ci->HasFlag(CI_SECURE)))
			return access->level;
	}

	return 0;
}

/*************************************************************************/

void update_cs_lastseen(User * user, ChannelInfo * ci)
{
	ChanAccess *access;

	if (!ci || !user || !user->nc)
		return;

	if (IsFounder(user, ci) || nick_identified(user)
		|| (user->IsRecognized() && !ci->HasFlag(CI_SECURE)))
		if ((access = ci->GetAccess(user->nc)))
			access->last_seen = time(NULL);
}

/*************************************************************************/

/* Returns the best ban possible for an user depending of the bantype
   value. */

int get_idealban(ChannelInfo * ci, User * u, char *ret, int retlen)
{
	char *mask;

	if (!ci || !u || !ret || retlen == 0)
		return 0;

	std::string vident = u->GetIdent();

	switch (ci->bantype) {
	case 0:
		snprintf(ret, retlen, "*!%s@%s", vident.c_str(),
				 u->GetDisplayedHost().c_str());
		return 1;
	case 1:
		if (vident[0] == '~')
			snprintf(ret, retlen, "*!*%s@%s",
				vident.c_str(), u->GetDisplayedHost().c_str());
		else
			snprintf(ret, retlen, "*!%s@%s",
				vident.c_str(), u->GetDisplayedHost().c_str());

		return 1;
	case 2:
		snprintf(ret, retlen, "*!*@%s", u->GetDisplayedHost().c_str());
		return 1;
	case 3:
		mask = create_mask(u);
		snprintf(ret, retlen, "*!%s", mask);
		delete [] mask;
		return 1;

	default:
		return 0;
	}
}


/*************************************************************************/

int get_access_level(ChannelInfo * ci, NickAlias * na)
{
	ChanAccess *access;

	if (!ci || !na)
		return 0;

	if (na->nc == ci->founder)
		return ACCESS_FOUNDER;

	access = ci->GetAccess(na->nc);

	if (!access)
		return 0;
	else
		return access->level;
}

const char *get_xop_level(int level)
{
	ChannelMode *halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);

	if (level < ACCESS_VOP)
		return "Err";
	else if (halfop && level < ACCESS_HOP)
		return "VOP";
	else if (!halfop && level < ACCESS_AOP)
		return "VOP";
	else if (halfop && level < ACCESS_AOP)
		return "HOP";
	else if (level < ACCESS_SOP)
		return "AOP";
	else if (level < ACCESS_QOP)
		return "SOP";
	else if (level < ACCESS_FOUNDER)
		return "QOP";
	else
		return "Founder";
}

/*************************************************************************/
/*********************** ChanServ command routines ***********************/
/*************************************************************************/

/*************************************************************************/


/*************************************************************************/

/* `last' is set to the last index this routine was called with
 * `perm' is incremented whenever a permission-denied error occurs
 */


/*************************************************************************/

/* Is the mask stuck? */

AutoKick *is_stuck(ChannelInfo * ci, const char *mask)
{
	if (!ci)
		return NULL;
	
	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (!akick->HasFlag(AK_USED) || akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK))
			continue;

		if (Anope::Match(akick->mask, mask, false))
			return akick;

		if (ircd->reversekickcheck)
			if (Anope::Match(mask, akick->mask, false))
				return akick;
	}

	return NULL;
}

/* Ban the stuck mask in a safe manner. */

void stick_mask(ChannelInfo * ci, AutoKick * akick)
{
	const char *av[2];
	Entry *ban;

	if (!ci) {
		return;
	}

	if (ci->c->bans && ci->c->bans->entries != 0) {
		for (ban = ci->c->bans->entries; ban; ban = ban->next) {
			/* If akick is already covered by a wider ban.
			   Example: c->bans[i] = *!*@*.org and akick->u.mask = *!*@*.epona.org */
			if (entry_match_mask(ban, akick->mask.c_str(), 0))
				return;

			if (ircd->reversekickcheck) {
				/* If akick is wider than a ban already in place.
				   Example: c->bans[i] = *!*@irc.epona.org and akick->u.mask = *!*@*.epona.org */
				if (Anope::Match(ban->mask, akick->mask.c_str(), false))
					return;
			}
		}
	}

	/* Falling there means set the ban */
	av[0] = "+b";
	av[1] = akick->mask.c_str();
	ircdproto->SendMode(whosends(ci), ci->c->name, "+b %s", akick->mask.c_str());
	chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
}

/* Ban the stuck mask in a safe manner. */

void stick_all(ChannelInfo * ci)
{
	const char *av[2];

	if (!ci)
		return;

	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (!akick->HasFlag(AK_USED) || (akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK)))
			continue;

		av[0] = "+b";
		av[1] = akick->mask.c_str();
		ircdproto->SendMode(whosends(ci), ci->c->name, "+b %s", akick->mask.c_str());
		chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
	}
}
