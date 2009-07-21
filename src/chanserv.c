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
	{ -1 }
};


LevelInfo levelinfo[] = {
	{ CA_AUTODEOP,	  "AUTODEOP",	 CHAN_LEVEL_AUTODEOP },
	{ CA_AUTOHALFOP,	"AUTOHALFOP",   CHAN_LEVEL_AUTOHALFOP },
	{ CA_AUTOOP,		"AUTOOP",	   CHAN_LEVEL_AUTOOP },
	{ CA_AUTOPROTECT,   "",  CHAN_LEVEL_AUTOPROTECT },
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
	{ CA_PROTECT,	   "",	  CHAN_LEVEL_PROTECT },
	{ CA_PROTECTME,	 "",	CHAN_LEVEL_PROTECTME },
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
		{ -1 }
};
int levelinfo_maxwidth = 0;

CSModeUtil csmodeutils[] = {
	{ "DEOP",	  "deop",	 "-o", CI_OPNOTICE, CA_OPDEOP,  CA_OPDEOPME },
	{ "OP",		"op",	   "+o", CI_OPNOTICE, CA_OPDEOP,  CA_OPDEOPME },
	{ "DEVOICE",   "devoice",  "-v", 0,		   CA_VOICE,   CA_VOICEME  },
	{ "VOICE",	 "voice",	"+v", 0,		   CA_VOICE,   CA_VOICEME  },
	{ "DEHALFOP",  "dehalfop", "-h", 0,		   CA_HALFOP,  CA_HALFOPME },
	{ "HALFOP",	"halfop",   "+h", 0,		   CA_HALFOP,  CA_HALFOPME },
	/* These get set later */
	{ "DEPROTECT", "",		 "",   0,		   CA_PROTECT, CA_PROTECTME },
	{ "PROTECT",   "",		 "",   0,		   CA_PROTECT, CA_PROTECTME },
	{ "DEOWNER",	"",		"",		0,		ACCESS_FOUNDER,		ACCESS_FOUNDER},
	{ "OWNER",		"",		"",		0,		ACCESS_FOUNDER,		ACCESS_FOUNDER},
	{ NULL }
};

/*************************************************************************/

void moduleAddChanServCmds() {
	ModuleManager::LoadModuleList(ChanServCoreNumber, ChanServCoreModules);
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
				ci->flags &= ~CI_INHABIT;

			ircdproto->SendPart(findbot(s_ChanServ), channel.c_str(), NULL);
		}
};

/*************************************************************************/

/* Returns modes for mlock in a nice way. */

char *get_mlock_modes(ChannelInfo * ci, int complete)
{
	static char res[BUFSIZE];

	char *end = res;

	if (ci->mlock_on || ci->mlock_off) {
		unsigned int n = 0;
		CBModeInfo *cbmi = cbmodeinfos;

		if (ci->mlock_on) {
			*end++ = '+';
			n++;

			do {
				if (ci->mlock_on & cbmi->flag)
					*end++ = cbmi->mode;
			} while ((++cbmi)->flag != 0 && ++n < sizeof(res) - 1);

			cbmi = cbmodeinfos;
		}

		if (ci->mlock_off) {
			*end++ = '-';
			n++;

			do {
				if (ci->mlock_off & cbmi->flag)
					*end++ = cbmi->mode;
			} while ((++cbmi)->flag != 0 && ++n < sizeof(res) - 1);

			cbmi = cbmodeinfos;
		}

		if (ci->mlock_on && complete) {
			do {
				if (cbmi->csgetvalue && (ci->mlock_on & cbmi->flag)) {
					char *value = cbmi->csgetvalue(ci);

					if (value) {
						*end++ = ' ';
						while (*value)
							*end++ = *value++;
					}
				}
			} while ((++cbmi)->flag != 0 && ++n < sizeof(res) - 1);
		}
	}

	*end = 0;

	return res;
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_chanserv_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	int i, j;
	ChannelInfo *ci;

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
			if (!ci->access.empty())
				mem += ci->access.size() * sizeof(ChanAccess);
			mem += ci->akickcount * sizeof(AutoKick);
			for (j = 0; j < ci->akickcount; j++) {
				if (!(ci->akick[j].flags & AK_ISNICK)
					&& ci->akick[j].u.mask)
					mem += strlen(ci->akick[j].u.mask) + 1;
				if (ci->akick[j].reason)
					mem += strlen(ci->akick[j].reason) + 1;
				if (ci->akick[j].creator)
					mem += strlen(ci->akick[j].creator) + 1;
			}
			if (ci->mlock_key)
				mem += strlen(ci->mlock_key) + 1;
			if (ircd->fmode) {
				if (ci->mlock_flood)
					mem += strlen(ci->mlock_flood) + 1;
			}
			if (ircd->Lmode) {
				if (ci->mlock_redirect)
					mem += strlen(ci->mlock_redirect) + 1;
			}
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
	int ver, i, j, c;
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
			SAFE(read = read_buffer(ci->founderpass, f));
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
			SAFE(read_int32(&ci->flags, f));

			/* Leaveops cleanup */
			if (ver <= 13 && (ci->flags & 0x00000020))
				ci->flags &= ~0x00000020;
			/* Temporary flags cleanup */
			ci->flags &= ~CI_INHABIT;

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
						if (nc)
							ci->AddAccess(nc, level, last_seen);
					}
				}
			}

			SAFE(read_int16(&ci->akickcount, f));
			if (ci->akickcount) {
				ci->akick = static_cast<AutoKick *>(scalloc(ci->akickcount, sizeof(AutoKick)));
				for (j = 0; j < ci->akickcount; j++) {
					SAFE(read_int16(&ci->akick[j].flags, f));
					if (ci->akick[j].flags & AK_USED) {
						SAFE(read_string(&s, f));
						if (ci->akick[j].flags & AK_ISNICK) {
							ci->akick[j].u.nc = findcore(s);
							if (!ci->akick[j].u.nc)
								ci->akick[j].flags &= ~AK_USED;
							delete [] s;
						} else {
							ci->akick[j].u.mask = s;
						}
						SAFE(read_string(&s, f));
						if (ci->akick[j].flags & AK_USED)
							ci->akick[j].reason = s;
						else if (s)
							delete [] s;
						SAFE(read_string(&s, f));
						if (ci->akick[j].flags & AK_USED) {
							ci->akick[j].creator = s;
						} else if (s) {
							delete [] s;
						}
						SAFE(read_int32(&tmp32, f));
						if (ci->akick[j].flags & AK_USED)
							ci->akick[j].addtime = tmp32;
					}
				}
			} else {
				ci->akick = NULL;
			}

			SAFE(read_int32(&ci->mlock_on, f));
			SAFE(read_int32(&ci->mlock_off, f));
			SAFE(read_int32(&ci->mlock_limit, f));
			SAFE(read_string(&ci->mlock_key, f));
			SAFE(read_string(&ci->mlock_flood, f));
			SAFE(read_string(&ci->mlock_redirect, f));

			SAFE(read_int16(&tmp16, f));
			if (tmp16) ci->memos.memos.resize(tmp16);
			SAFE(read_int16(&tmp16, f));
			ci->memos.memomax = static_cast<int16>(tmp16);
			if (!ci->memos.memos.empty()) {
				for (j = 0; j < ci->memos.memos.size(); j++) {
					ci->memos.memos[j] = new Memo;
					Memo *memo = ci->memos.memos[j];
					SAFE(read_int32(&memo->number, f));
					SAFE(read_int16(&memo->flags, f));
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
			ci->botflags = tmp32;
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
						SAFE(read_int16(&ci->badwords[j].type, f));
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
			if (!(ci->flags & CI_FORBIDDEN) && !ci->founder) {
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
	int i, j;
	ChannelInfo *ci;
	static time_t lastwarn = 0;

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
			SAFE(written = write_buffer(ci->founderpass, f));
			SAFE(write_string(ci->desc, f));
			SAFE(write_string(ci->url, f));
			SAFE(write_string(ci->email, f));
			SAFE(write_int32(ci->time_registered, f));
			SAFE(write_int32(ci->last_used, f));
			SAFE(write_string(ci->last_topic, f));
			SAFE(written = write_buffer(ci->last_topic_setter, f));
			SAFE(write_int32(ci->last_topic_time, f));
			SAFE(write_int32(ci->flags, f));
			SAFE(write_string(ci->forbidby, f));
			SAFE(write_string(ci->forbidreason, f));
			SAFE(write_int16(ci->bantype, f));

			tmp16 = CA_SIZE;
			SAFE(write_int16(tmp16, f));
			for (j = 0; j < CA_SIZE; j++)
				SAFE(write_int16(ci->levels[j], f));

			SAFE(write_int16(ci->access.empty() ? 0 : ci->access.size(), f));
			for (j = 0; j < ci->access.size(); j++) {
				ChanAccess *access = ci->GetAccess(j);
				if (!access->in_use)
					continue;
				SAFE(write_int16(access->in_use, f));
				SAFE(write_int16(access->level, f));
				SAFE(write_string(access->nc->display, f));
				SAFE(write_int32(access->last_seen, f));
			}

			SAFE(write_int16(ci->akickcount, f));
			for (j = 0; j < ci->akickcount; j++) {
				SAFE(write_int16(ci->akick[j].flags, f));
				if (ci->akick[j].flags & AK_USED) {
					if (ci->akick[j].flags & AK_ISNICK)
						SAFE(write_string(ci->akick[j].u.nc->display, f));
					else
						SAFE(write_string(ci->akick[j].u.mask, f));
					SAFE(write_string(ci->akick[j].reason, f));
					SAFE(write_string(ci->akick[j].creator, f));
					SAFE(write_int32(ci->akick[j].addtime, f));
				}
			}

			SAFE(write_int32(ci->mlock_on, f));
			SAFE(write_int32(ci->mlock_off, f));
			SAFE(write_int32(ci->mlock_limit, f));
			SAFE(write_string(ci->mlock_key, f));
			SAFE(write_string(ci->mlock_flood, f));
			SAFE(write_string(ci->mlock_redirect, f));
			SAFE(write_int16(ci->memos.memos.size(), f));
			SAFE(write_int16(ci->memos.memomax, f));
			for (j = 0; j < ci->memos.memos.size(); j++) {
				Memo *memo = ci->memos.memos[j];
				SAFE(write_int32(memo->number, f));
				SAFE(write_int16(memo->flags, f));
				SAFE(write_int32(memo->time, f));
				SAFE(written = write_buffer(memo->sender, f));
				SAFE(write_string(memo->text, f));
			}

			SAFE(write_string(ci->entry_message, f));

			if (ci->bi)
				SAFE(write_string(ci->bi->nick, f));
			else
				SAFE(write_string(NULL, f));

			SAFE(write_int32(ci->botflags, f));

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
	char modebuf[64], argbuf[BUFSIZE], *end = modebuf, *end2 = argbuf;
	uint32 modes;
	ChannelInfo *ci;
	CBModeInfo *cbmi;
	CBMode *cbm;

	if (!c) {
		if (debug) {
			alog("debug: check_modes called with NULL values");
		}
		return;
	}

	if (c->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (c->server_modecount >= 3 && c->chanserv_modecount >= 3) {
		ircdproto->SendGlobops(NULL,
						 "Warning: unable to set modes on channel %s.  "
						 "Are your servers' U:lines configured correctly?",
						 c->name);
		alog("%s: Bouncy modes on channel %s", s_ChanServ, c->name);
		c->bouncy_modes = 1;
		return;
	}

	if (c->chanserv_modetime != time(NULL)) {
		c->chanserv_modecount = 0;
		c->chanserv_modetime = time(NULL);
	}
	c->chanserv_modecount++;

	/* Check if the channel is registered; if not remove mode -r */
	if (!(ci = c->ci)) {
		if (ircd->regmode) {
			if (c->mode & ircd->regmode) {
				c->mode &= ~ircd->regmode;
				ircdproto->SendMode(whosends(ci), c->name, "-r");
			}
		}
		return;
	}

	/* Initialize te modes-var to set all modes not set yet but which should
	 * be set as by mlock and defcon.
	 */
	modes = ~c->mode & ci->mlock_on;
	if (DefConModesSet)
		modes |= (~c->mode & DefConModesOn);

	/* Initialize the buffers */
	*end++ = '+';
	cbmi = cbmodeinfos;

	do {
		if (modes & cbmi->flag) {
			*end++ = cbmi->mode;
			c->mode |= cbmi->flag;

			/* Add the eventual parameter and modify the Channel structure */
			if (cbmi->getvalue && cbmi->csgetvalue) {
				char *value;
				/* Check if it's a defcon or mlock mode */
				if (DefConModesOn & cbmi->flag)
					value = cbmi->csgetvalue(&DefConModesCI);
				else
					value = cbmi->csgetvalue(ci);

				cbm = &cbmodes[static_cast<int>(cbmi->mode)];
				cbm->setvalue(c, value);

				if (value) {
					*end2++ = ' ';
					while (*value)
						*end2++ = *value++;
				}
			}
		} else if (cbmi->getvalue && cbmi->csgetvalue
				   && ((ci->mlock_on & cbmi->flag)
					   || (DefConModesOn & cbmi->flag))
				   && (c->mode & cbmi->flag)) {
			char *value = cbmi->getvalue(c);
			char *csvalue;

			/* Check if it's a defcon or mlock mode */
			if (DefConModesOn & cbmi->flag)
				csvalue = cbmi->csgetvalue(&DefConModesCI);
			else
				csvalue = cbmi->csgetvalue(ci);

			/* Lock and actual values don't match, so fix the mode */
			if (value && csvalue && strcmp(value, csvalue)) {
				*end++ = cbmi->mode;

				cbm = &cbmodes[static_cast<int>(cbmi->mode)];
				cbm->setvalue(c, csvalue);

				*end2++ = ' ';
				while (*csvalue)
					*end2++ = *csvalue++;
			}
		}
	} while ((++cbmi)->flag != 0);

	if (*(end - 1) == '+')
		end--;

	modes = c->mode & ci->mlock_off;
	if (DefConModesSet)
		modes |= (~c->mode & DefConModesOff);

	if (modes) {
		*end++ = '-';
		cbmi = cbmodeinfos;

		do {
			if (modes & cbmi->flag) {
				*end++ = cbmi->mode;
				c->mode &= ~cbmi->flag;

				/* Add the eventual parameter and clean up the Channel structure */
				if (cbmi->getvalue) {
					cbm = &cbmodes[static_cast<int>(cbmi->mode)];

					if (!(cbm->flags & CBM_MINUS_NO_ARG)) {
						char *value = cbmi->getvalue(c);

						if (value) {
							*end2++ = ' ';
							while (*value)
								*end2++ = *value++;
						}
					}

					cbm->setvalue(c, NULL);
				}
			}
		} while ((++cbmi)->flag != 0);
	}

	if (end == modebuf)
		return;

	*end = 0;
	*end2 = 0;

	ircdproto->SendMode(whosends(ci), c->name, "%s%s", modebuf,
				   (end2 == argbuf ? "" : argbuf));
}

/*************************************************************************/

int check_valid_admin(User * user, Channel * chan, int servermode)
{
	if (!chan || !chan->ci)
		return 1;

	if (!ircd->admin) {
		return 0;
	}

	/* They will be kicked; no need to deop, no need to update our internal struct too */
	if (chan->ci->flags & CI_FORBIDDEN)
		return 0;

	if (servermode && !check_access(user, chan->ci, CA_AUTOPROTECT)) {
		notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
		ircdproto->SendMode(whosends(chan->ci), chan->name, "%s %s",
					   ircd->adminunset, user->nick);
		return 0;
	}

	if (check_access(user, chan->ci, CA_AUTODEOP)) {
		ircdproto->SendMode(whosends(chan->ci), chan->name, "%s %s",
					   ircd->adminunset, user->nick);
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
	char *tmp;
	if (!chan || !chan->ci)
		return 1;

	/* They will be kicked; no need to deop, no need to update our internal struct too */
	if (chan->ci->flags & CI_FORBIDDEN)
		return 0;

	if (servermode && !check_access(user, chan->ci, CA_AUTOOP)) {
		notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
		if (ircd->halfop) {
			if (ircd->owner && ircd->protect) {
				if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
					tmp = stripModePrefix(ircd->ownerunset);
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "%so%s %s %s %s", ircd->adminunset,
								   tmp, user->nick,
								   user->nick, user->nick);
					delete [] tmp;
				} else {
					tmp = stripModePrefix(ircd->ownerunset);
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "%sho%s %s %s %s %s",
								   ircd->adminunset, tmp,
								   user->nick, user->nick, user->nick,
								   user->nick);
					delete [] tmp;
				}
			} else if (!ircd->owner && ircd->protect) {
				if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "%so %s %s", ircd->adminunset,
								   user->nick, user->nick);
				} else {
					ircdproto->SendMode(whosends(chan->ci), chan->name,
								   "%soh %s %s %s", ircd->adminunset,
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
		if (ircd->halfop) {
			if (ircd->owner && ircd->protect) {
				tmp = stripModePrefix(ircd->ownerunset);
				ircdproto->SendMode(whosends(chan->ci), chan->name,
							   "%sho%s %s %s %s %s", ircd->adminunset,
							   tmp, user->nick, user->nick,
							   user->nick, user->nick);
				delete [] tmp;
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

	if (!ci || (ci->flags & CI_FORBIDDEN) || *chan == '+')
		return 0;

	if ((ci->flags & CI_SECURE) && !nick_identified(user))
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

	if (!ci || (ci->flags & CI_FORBIDDEN) || *chan == '+')
		return 0;

	if ((ci->flags & CI_SECURE) && !nick_identified(user))
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

	if (!ci || (ci->flags & CI_FORBIDDEN) || *chan == '+')
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
	char *tmp;
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || (ci->flags & CI_FORBIDDEN) || *chan == '+')
		return 0;

	if (((ci->flags & CI_SECUREFOUNDER) && is_real_founder(user, ci))
		|| (!(ci->flags & CI_SECUREFOUNDER) && is_founder(user, ci))) {
		tmp = stripModePrefix(ircd->ownerset);
		ircdproto->SendMode(whosends(ci), chan, "+o%s %s %s", tmp, user->nick,
					   user->nick);
		delete [] tmp;
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_protect(User * user, char *chan)
{
	char *tmp;
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || (ci->flags & CI_FORBIDDEN) || *chan == '+')
		return 0;

	if (check_access(user, ci, CA_AUTOPROTECT)) {
		tmp = stripModePrefix(ircd->adminset);
		ircdproto->SendMode(whosends(ci), chan, "+o%s %s %s", tmp, user->nick,
					   user->nick);
		delete [] tmp;
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
	int i;
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

	if (ci->flags & CI_SUSPENDED || ci->flags & CI_FORBIDDEN)
	{
		if (is_oper(user))
			return 0;

		get_idealban(ci, user, mask, sizeof(mask));
		reason = ci->forbidreason ? ci->forbidreason : getstring(user, CHAN_MAY_NOT_BE_USED);
		set_modes = true;
		goto kick;
	}

	if (nick_recognized(user))
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
	if (ircd->except && is_excepted(ci, user) == 1) {
		return 0;
	}

	for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
		if (!(akick->flags & AK_USED))
			continue;
		if ((akick->flags & AK_ISNICK && akick->u.nc == nc)
			|| (!(akick->flags & AK_ISNICK)
				&& match_usermask(akick->u.mask, user))) {
			if (debug >= 2)
				alog("debug: %s matched akick %s", user->nick,
					 (akick->flags & AK_ISNICK) ? akick->u.nc->
					 display : akick->u.mask);
			if (akick->flags & AK_ISNICK)
				get_idealban(ci, user, mask, sizeof(mask));
			else
				strcpy(mask, akick->u.mask);
			reason = akick->reason ? akick->reason : CSAutokickReason;
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
	if ((!(c = findchan(chan)) || c->usercount == 0) && !(ci->flags & CI_INHABIT))
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
		ci->flags |= CI_INHABIT;
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
	if (!(ci->flags & CI_KEEPTOPIC)) {
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

	if (!(ci = c->ci) || !(ci->flags & CI_TOPICLOCK))
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
			if (!ci->c && now - ci->last_used >= CSExpire
				&& !(ci->
					 flags & (CI_FORBIDDEN | CI_NO_EXPIRE | CI_SUSPENDED)))
			{
				FOREACH_MOD(I_OnChanExpire, OnChanExpire(ci->name));
				alog("Expiring channel %s (founder: %s)", ci->name,
					 (ci->founder ? ci->founder->display : "(none)"));
				delchan(ci);
			}
		}
	}
}

/*************************************************************************/

/* Remove a (deleted or expired) nickname from all channel lists. */

void cs_remove_nick(const NickCore * nc)
{
	int i, j, k;
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
					if (ircd->regmode) {
						/* Maybe move this to delchan() ? */
						if ((ci->c) && (ci->c->mode & ircd->regmode)) {
							ci->c->mode &= ~ircd->regmode;
							ircdproto->SendMode(whosends(ci), ci->name, "-r");
						}
					}

					delchan(ci);
					continue;
				}
			}

			if (ci->successor == nc)
				ci->successor = NULL;

			for (j = ci->access.size(); j > 0; --j)
			{
				ca = ci->GetAccess(j - 1);

				if (ca->in_use && ca->nc == nc)
					ci->EraseAccess(j - 1);
			}

			for (akick = ci->akick, j = 0; j < ci->akickcount; akick++, j++) {
				if ((akick->flags & AK_USED) && (akick->flags & AK_ISNICK)
					&& akick->u.nc == nc) {
					if (akick->creator) {
						delete [] akick->creator;
						akick->creator = NULL;
					}
					if (akick->reason) {
						delete [] akick->reason;
						akick->reason = NULL;
					}
					akick->flags = 0;
					akick->addtime = 0;
					akick->u.nc = NULL;

					/* Only one occurance can exist in every akick list.. ~ Viper */
					break;
				}
			}

			/* Are there any akicks behind us?
			 * If so, move all following akicks.. ~ Viper */
			if (j < ci->akickcount - 1) {
				for (k = j + 1; k < ci->akickcount; j++, k++) {
					if (ci->akick[k].flags & AK_USED) {
						/* Move the akick one place ahead and clear the original */
						if (ci->akick[k].flags & AK_ISNICK) {
							ci->akick[j].u.nc = ci->akick[k].u.nc;
							ci->akick[k].u.nc = NULL;
						} else {
							ci->akick[j].u.mask = sstrdup(ci->akick[k].u.mask);
							delete [] ci->akick[k].u.mask;
							ci->akick[k].u.mask = NULL;
						}

						if (ci->akick[k].reason) {
							ci->akick[j].reason = sstrdup(ci->akick[k].reason);
							delete [] ci->akick[k].reason;
							ci->akick[k].reason = NULL;
						} else
							ci->akick[j].reason = NULL;

						ci->akick[j].creator = sstrdup(ci->akick[k].creator);
						delete [] ci->akick[k].creator;
						ci->akick[k].creator = NULL;

						ci->akick[j].flags = ci->akick[k].flags;
						ci->akick[k].flags = 0;

						ci->akick[j].addtime = ci->akick[k].addtime;
						ci->akick[k].addtime = 0;
					}
				}
			}

			/* After moving only the last entry should still be empty.
			 * Free the place no longer in use... ~ Viper */
			ci->akickcount = j;
			ci->akick = static_cast<AutoKick *>(srealloc(ci->akick,sizeof(AutoKick) * ci->akickcount));
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
		return is_founder(user, ci);
	if (level >= ACCESS_FOUNDER)
		return (what == CA_AUTODEOP || what == CA_NOJOIN) ? 0 : 1;
	/* Hacks to make flags work */
	if (what == CA_AUTODEOP && (ci->flags & CI_SECUREOPS) && level == 0)
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
	int i;
	NickCore *nc;
	User *u;
	struct u_chaninfolist *cilist, *cilist_next;

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

	if (debug >= 2) {
		alog("debug: delchan() founder cleanup");
	}
	for (i = 0; i < 1024; i++) {
		for (u = userlist[i]; u; u = u->next) {
			cilist = u->founder_chans;
			while (cilist) {
				cilist_next = cilist->next;
				if (cilist->chan == ci) {
					if (debug)
						alog("debug: Dropping founder login of %s for %s",
							 u->nick, ci->name);
					if (cilist->next)
						cilist->next->prev = cilist->prev;
					if (cilist->prev)
						cilist->prev->next = cilist->next;
					else
						u->founder_chans = cilist->next;
					delete cilist;
				}
				cilist = cilist_next;
			}
		}
	}
	if (debug >= 2) {
		alog("debug: delchan() founder cleanup done");
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

	if (ci->mlock_key)
		delete [] ci->mlock_key;
	if (ircd->fmode) {
		if (ci->mlock_flood)
			delete [] ci->mlock_flood;
	}
	if (ircd->Lmode) {
		if (ci->mlock_redirect)
			delete [] ci->mlock_redirect;
	}
	if (ci->last_topic)
		delete [] ci->last_topic;
	if (ci->forbidby)
		delete [] ci->forbidby;
	if (ci->forbidreason)
		delete [] ci->forbidreason;
	if (debug >= 2) {
		alog("debug: delchan() top of the akick list");
	}
	for (i = 0; i < ci->akickcount; i++) {
		if (!(ci->akick[i].flags & AK_ISNICK) && ci->akick[i].u.mask)
			delete [] ci->akick[i].u.mask;
		if (ci->akick[i].reason)
			delete [] ci->akick[i].reason;
		if (ci->akick[i].creator)
			delete [] ci->akick[i].creator;
	}
	if (debug >= 2) {
		alog("debug: delchan() done with the akick list");
	}
	if (ci->akick)
		free(ci->akick);
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

/* Does the given user have founder access to the channel? */

int is_founder(User * user, ChannelInfo * ci)
{
	if (!user || !ci) {
		return 0;
	}

	if (user->isSuperAdmin) {
		return 1;
	}

	if (user->nc && user->nc == ci->founder) {
		if ((nick_identified(user)
			 || (nick_recognized(user) && !(ci->flags & CI_SECURE))))
			return 1;
	}
	if (is_identified(user, ci))
		return 1;
	return 0;
}

/*************************************************************************/

int is_real_founder(User * user, ChannelInfo * ci)
{
	if (user->isSuperAdmin) {
		return 1;
	}

	if (user->nc && user->nc == ci->founder) {
		if ((nick_identified(user)
			 || (nick_recognized(user) && !(ci->flags & CI_SECURE))))
			return 1;
	}
	return 0;
}

/*************************************************************************/

/* Has the given user password-identified as founder for the channel? */

int is_identified(User * user, ChannelInfo * ci)
{
	struct u_chaninfolist *c;

	for (c = user->founder_chans; c; c = c->next) {
		if (c->chan == ci)
			return 1;
	}
	return 0;
}

/*************************************************************************/

/* Return the access level the given user has on the channel.  If the
 * channel doesn't exist, the user isn't on the access list, or the channel
 * is CS_SECURE and the user hasn't IDENTIFY'd with NickServ, return 0. */

int get_access(User * user, ChannelInfo * ci)
{
	ChanAccess *access;

	if (!ci || !user)
		return 0;

	/* SuperAdmin always has highest level */
	if (user->isSuperAdmin)
		return ACCESS_SUPERADMIN;

	if (is_founder(user, ci))
		return ACCESS_FOUNDER;

	if (!user->nc)
		return 0;

	if (nick_identified(user)
		|| (nick_recognized(user) && !(ci->flags & CI_SECURE)))
		if ((access = ci->GetAccess(user->nc)))
			return access->level;

	if (nick_identified(user))
		return 0;

	return 0;
}

/*************************************************************************/

void update_cs_lastseen(User * user, ChannelInfo * ci)
{
	ChanAccess *access;

	if (!ci || !user || !user->nc)
		return;

	if (is_founder(user, ci) || nick_identified(user)
		|| (nick_recognized(user) && !(ci->flags & CI_SECURE)))
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

char *cs_get_flood(ChannelInfo * ci)
{
	if (!ci) {
		return NULL;
	} else {
		if (ircd->fmode)
			return ci->mlock_flood;
		else
			return NULL;
	}
}

/*************************************************************************/

char *cs_get_key(ChannelInfo * ci)
{
	if (!ci) {
		return NULL;
	} else {
		return ci->mlock_key;
	}
}

/*************************************************************************/

char *cs_get_limit(ChannelInfo * ci)
{
	static char limit[16];

	if (!ci) {
		return NULL;
	}

	if (ci->mlock_limit == 0)
		return NULL;

	snprintf(limit, sizeof(limit), "%lu",
			 static_cast<unsigned long>(ci->mlock_limit));
	return limit;
}

/*************************************************************************/

char *cs_get_redirect(ChannelInfo * ci)
{
	if (!ci) {
		return NULL;
	} else {
		if (ircd->Lmode)
			return ci->mlock_redirect;
		else
			return NULL;
	}
}

/*************************************************************************/

void cs_set_flood(ChannelInfo * ci, const char *value)
{
	if (!ci) {
		return;
	}

	if (ci->mlock_flood)
		delete [] ci->mlock_flood;

	/* This looks ugly, but it works ;) */
	if (ircdproto->IsFloodModeParamValid(value)) {
		ci->mlock_flood = sstrdup(value);
	} else {
		ci->mlock_on &= ~ircd->chan_fmode;
		ci->mlock_flood = NULL;
	}
}

/*************************************************************************/

void cs_set_key(ChannelInfo * ci, const char *value)
{
	if (!ci) {
		return;
	}

	if (ci->mlock_key)
		delete [] ci->mlock_key;

	/* Don't allow keys with a coma */
	if (value && *value != ':' && !strchr(value, ',')) {
		ci->mlock_key = sstrdup(value);
	} else {
		ci->mlock_on &= ~anope_get_key_mode();
		ci->mlock_key = NULL;
	}
}

/*************************************************************************/

void cs_set_limit(ChannelInfo * ci, const char *value)
{
	if (!ci) {
		return;
	}

	ci->mlock_limit = value ? strtoul(value, NULL, 10) : 0;

	if (ci->mlock_limit <= 0)
		ci->mlock_on &= ~anope_get_limit_mode();
}

/*************************************************************************/

void cs_set_redirect(ChannelInfo * ci, const char *value)
{
	if (!ci) {
		return;
	}

	if (ci->mlock_redirect)
		delete [] ci->mlock_redirect;

	/* Don't allow keys with a coma */
	if (value && *value == '#') {
		ci->mlock_redirect = sstrdup(value);
	} else {
		ci->mlock_on &= ~ircd->chan_lmode;
		ci->mlock_redirect = NULL;
	}
}

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
	if (level < ACCESS_VOP) {
		return "Err";
	} else if (ircd->halfop && level < ACCESS_HOP) {
		return "VOP";
	} else if (!ircd->halfop && level < ACCESS_AOP) {
		return "VOP";
	} else if (ircd->halfop && level < ACCESS_AOP) {
		return "HOP";
	} else if (level < ACCESS_SOP) {
		return "AOP";
	} else if (level < ACCESS_FOUNDER) {
		return "SOP";
	} else {
		return "Founder";
	}

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
	int i;
	AutoKick *akick;

	if (!ci) {
		return NULL;
	}

	for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
		if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK)
			|| !(akick->flags & AK_STUCK))
			continue;
		/* Example: mask = *!*@*.org and akick->u.mask = *!*@*.anope.org */
		if (Anope::Match(akick->u.mask, mask, false))
			return akick;
		if (ircd->reversekickcheck) {
			/* Example: mask = *!*@irc.anope.org and akick->u.mask = *!*@*.anope.org */
			if (Anope::Match(mask, akick->u.mask, false))
				return akick;
		}
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
			char *mask = sstrdup(akick->u.mask);
			if (entry_match_mask(ban, mask, 0))
			{
				delete [] mask;
				return;
			}
			delete [] mask;

			if (ircd->reversekickcheck) {
				/* If akick is wider than a ban already in place.
				   Example: c->bans[i] = *!*@irc.epona.org and akick->u.mask = *!*@*.epona.org */
				if (Anope::Match(ban->mask, akick->u.mask, false))
					return;
			}
		}
	}

	/* Falling there means set the ban */
	av[0] = "+b";
	av[1] = akick->u.mask;
	ircdproto->SendMode(whosends(ci), ci->c->name, "+b %s", akick->u.mask);
	chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
}

/* Ban the stuck mask in a safe manner. */

void stick_all(ChannelInfo * ci)
{
	int i;
	const char *av[2];
	AutoKick *akick;

	if (!ci) {
		return;
	}

	for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
		if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK)
			|| !(akick->flags & AK_STUCK))
			continue;

		av[0] = "+b";
		av[1] = akick->u.mask;
		ircdproto->SendMode(whosends(ci), ci->c->name, "+b %s", akick->u.mask);
		chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
	}
}
