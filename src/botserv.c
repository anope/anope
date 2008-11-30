/* BotServ functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

BotInfo *botlists[256];		 /* Hash list of bots */
int nbots = 0;

/*************************************************************************/

static UserData *get_user_data(Channel * c, User * u);

static void check_ban(ChannelInfo * ci, User * u, int ttbtype);
static void bot_kick(ChannelInfo * ci, User * u, int message, ...);

E void moduleAddBotServCmds();

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddBotServCmds() {
	ModuleManager::LoadModuleList(BotServCoreNumber, BotServCoreModules);
}
/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_botserv_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	int i;
	BotInfo *bi;

	for (i = 0; i < 256; i++) {
		for (bi = botlists[i]; bi; bi = bi->next) {
			count++;
			mem += sizeof(*bi);
			mem += strlen(bi->nick) + 1;
			mem += strlen(bi->user) + 1;
			mem += strlen(bi->host) + 1;
			mem += strlen(bi->real) + 1;
		}
	}

	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* BotServ initialization. */

void bs_init()
{
	if (s_BotServ) {
		moduleAddBotServCmds();
	}
}

/*************************************************************************/

/* Main BotServ routine. */

void botserv(User * u, char *buf)
{
	char *cmd, *s;

	cmd = strtok(buf, " ");

	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			*s = 0;
		}
		ircdproto->SendCTCP(findbot(s_BotServ), u->nick, "PING %s", s);
	} else {
		mod_run_cmd(s_BotServ, u, BOTSERV, cmd);
	}

}

/*************************************************************************/

/* Handles all messages sent to bots. (Currently only answers to pings ;) */

void botmsgs(User * u, BotInfo * bi, char *buf)
{
	char *cmd = strtok(buf, " ");
	char *s;

	if (!cmd || !u)
		return;

	if (!stricmp(cmd, "\1PING")) {
		if (!(s = strtok(NULL, ""))) {
			*s = 0;
		}
		ircdproto->SendCTCP(bi, u->nick, "PING %s", s);
	}
	else if (cmd && bi->cmdTable)
	{
		mod_run_cmd(bi->nick, u, bi->cmdTable, cmd);
	}
}

/*************************************************************************/

/* Handles all messages that are sent to registered channels where a
 * bot is on.
 *
 */

void botchanmsgs(User * u, ChannelInfo * ci, char *buf)
{
	int c;
	int16 cstatus = 0;
	char *cmd;
	UserData *ud;

	if (!u || !buf || !ci) {
		return;
	}

	/* Answer to ping if needed, without breaking the buffer. */
	if (!strnicmp(buf, "\1PING", 5)) {
		ircdproto->SendCTCP(ci->bi, u->nick, "PING %s", buf);
	}

	/* If it's a /me, cut the CTCP part at the beginning (not
	 * at the end, because one character just doesn't matter,
	 * but the ACTION may create strange behaviours with the
	 * caps or badwords kickers */
	if (!strnicmp(buf, "\1ACTION ", 8))
		buf += 8;

	/* Now we can make kicker stuff. We try to order the checks
	 * from the fastest one to the slowest one, since there's
	 * no need to process other kickers if an user is kicked before
	 * the last kicker check.
	 *
	 * But FIRST we check whether the user is protected in any
	 * way.
	 */

	/* We first retrieve the user status on the channel if needed */
	if (ci->botflags & (BS_DONTKICKOPS | BS_DONTKICKVOICES))
		cstatus = chan_get_user_status(ci->c, u);

	if (buf && !check_access(u, ci, CA_NOKICK) &&
		(!(ci->botflags & BS_DONTKICKOPS)
		 || !(cstatus & (CUS_HALFOP | CUS_OP | CUS_OWNER | CUS_PROTECT)))

		&& (!(ci->botflags & BS_DONTKICKVOICES) || !(cstatus & CUS_VOICE))) {
		/* Bolds kicker */
		if ((ci->botflags & BS_KICK_BOLDS) && strchr(buf, 2)) {
			check_ban(ci, u, TTB_BOLDS);
			bot_kick(ci, u, BOT_REASON_BOLD);
			return;
		}

		/* Color kicker */
		if ((ci->botflags & BS_KICK_COLORS) && strchr(buf, 3)) {
			check_ban(ci, u, TTB_COLORS);
			bot_kick(ci, u, BOT_REASON_COLOR);
			return;
		}

		/* Reverses kicker */
		if ((ci->botflags & BS_KICK_REVERSES) && strchr(buf, 22)) {
			check_ban(ci, u, TTB_REVERSES);
			bot_kick(ci, u, BOT_REASON_REVERSE);
			return;
		}

		/* Underlines kicker */
		if ((ci->botflags & BS_KICK_UNDERLINES) && strchr(buf, 31)) {
			check_ban(ci, u, TTB_UNDERLINES);
			bot_kick(ci, u, BOT_REASON_UNDERLINE);
			return;
		}

		/* Caps kicker */
		if ((ci->botflags & BS_KICK_CAPS)
			&& ((c = strlen(buf)) >= ci->capsmin)) {
			int i = 0;
			int l = 0;
			char *s = buf;

			do {
				if (isupper(*s))
					i++;
				else if (islower(*s))
					l++;
			} while (*s++);

			/* i counts uppercase chars, l counts lowercase chars. Only
			 * alphabetic chars (so islower || isupper) qualify for the
			 * percentage of caps to kick for; the rest is ignored. -GD
			 */

			if (i >= ci->capsmin && i * 100 / (i + l) >= ci->capspercent) {
				check_ban(ci, u, TTB_CAPS);
				bot_kick(ci, u, BOT_REASON_CAPS);
				return;
			}
		}

		/* Bad words kicker */
		if (ci->botflags & BS_KICK_BADWORDS) {
			int i;
			int mustkick = 0;
			char *nbuf;
			BadWord *bw;

			/* Normalize the buffer */
			nbuf = normalizeBuffer(buf);

			for (i = 0, bw = ci->badwords; i < ci->bwcount; i++, bw++) {
				if (!bw->in_use)
					continue;

				if (bw->type == BW_ANY
					&& ((BSCaseSensitive && strstr(nbuf, bw->word))
						|| (!BSCaseSensitive && stristr(nbuf, bw->word)))) {
					mustkick = 1;
				} else if (bw->type == BW_SINGLE) {
					int len = strlen(bw->word);

					if ((BSCaseSensitive && !strcmp(nbuf, bw->word))
						|| (!BSCaseSensitive
							&& (!stricmp(nbuf, bw->word)))) {
						mustkick = 1;
						/* two next if are quite odd isn't it? =) */
					} else if ((strchr(nbuf, ' ') == nbuf + len)
							   &&
							   ((BSCaseSensitive
								 && !strcmp(nbuf, bw->word))
								|| (!BSCaseSensitive
									&& (stristr(nbuf, bw->word) ==
										nbuf)))) {
						mustkick = 1;
					} else {
						if ((strrchr(nbuf, ' ') ==
							 nbuf + strlen(nbuf) - len - 1)
							&&
							((BSCaseSensitive
							  && (strstr(nbuf, bw->word) ==
								  nbuf + strlen(nbuf) - len))
							 || (!BSCaseSensitive
								 && (stristr(nbuf, bw->word) ==
									 nbuf + strlen(nbuf) - len)))) {
							mustkick = 1;
						} else {
							char *wordbuf = new char[len + 3];

							wordbuf[0] = ' ';
							wordbuf[len + 1] = ' ';
							wordbuf[len + 2] = '\0';
							memcpy(wordbuf + 1, bw->word, len);

							if ((BSCaseSensitive
								 && (strstr(nbuf, wordbuf)))
								|| (!BSCaseSensitive
									&& (stristr(nbuf, wordbuf)))) {
								mustkick = 1;
							}

							/* free previous (sc)allocated memory (#850) */
							delete [] wordbuf;
						}
					}
				} else if (bw->type == BW_START) {
					int len = strlen(bw->word);

					if ((BSCaseSensitive
						 && (!strncmp(nbuf, bw->word, len)))
						|| (!BSCaseSensitive
							&& (!strnicmp(nbuf, bw->word, len)))) {
						mustkick = 1;
					} else {
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf + 1, bw->word, len);
						wordbuf[0] = ' ';
						wordbuf[len + 1] = '\0';

						if ((BSCaseSensitive && (strstr(nbuf, wordbuf)))
							|| (!BSCaseSensitive
								&& (stristr(nbuf, wordbuf))))
							mustkick = 1;

						delete [] wordbuf;
					}
				} else if (bw->type == BW_END) {
					int len = strlen(bw->word);

					if ((BSCaseSensitive
						 &&
						 (!strncmp
						  (nbuf + strlen(nbuf) - len, bw->word, len)))
						|| (!BSCaseSensitive
							&&
							(!strnicmp
							 (nbuf + strlen(nbuf) - len, bw->word,
							  len)))) {
						mustkick = 1;
					} else {
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf, bw->word, len);
						wordbuf[len] = ' ';
						wordbuf[len + 1] = '\0';

						if ((BSCaseSensitive && (strstr(nbuf, wordbuf)))
							|| (!BSCaseSensitive
								&& (stristr(nbuf, wordbuf))))
							mustkick = 1;

						delete [] wordbuf;
					}
				}

				if (mustkick) {
					check_ban(ci, u, TTB_BADWORDS);
					if (BSGentleBWReason)
						bot_kick(ci, u, BOT_REASON_BADWORD_GENTLE);
					else
						bot_kick(ci, u, BOT_REASON_BADWORD, bw->word);

					/* free the normalized buffer before return (#850) */
					delete [] nbuf;

					return;
				}
			}

			/* Free the normalized buffer */
			delete [] nbuf;
		}

		/* Flood kicker */
		if (ci->botflags & BS_KICK_FLOOD) {
			time_t now = time(NULL);

			ud = get_user_data(ci->c, u);
			if (!ud) {
				return;
			}

			if (now - ud->last_start > ci->floodsecs) {
				ud->last_start = time(NULL);
				ud->lines = 0;
			}

			ud->lines++;
			if (ud->lines >= ci->floodlines) {
				check_ban(ci, u, TTB_FLOOD);
				bot_kick(ci, u, BOT_REASON_FLOOD);
				return;
			}
		}

		/* Repeat kicker */
		if (ci->botflags & BS_KICK_REPEAT) {
			ud = get_user_data(ci->c, u);
			if (!ud) {
				return;
			}
			if (ud->lastline && stricmp(ud->lastline, buf)) {
				delete [] ud->lastline;
				ud->lastline = sstrdup(buf);
				ud->times = 0;
			} else {
				if (!ud->lastline)
					ud->lastline = sstrdup(buf);
				ud->times++;
			}

			if (ud->times >= ci->repeattimes) {
				check_ban(ci, u, TTB_REPEAT);
				bot_kick(ci, u, BOT_REASON_REPEAT);
				return;
			}
		}
	}


	/* return if the user is on the ignore list  */
	if (get_ignore(u->nick) != NULL) {
		return;
	}

	/* Fantaisist commands */

	if (buf && (ci->botflags & BS_FANTASY) && *buf == *BSFantasyCharacter) {
		cmd = strtok(buf, " ");

		if (cmd && (cmd[0] == *BSFantasyCharacter)) {
			char *params = strtok(NULL, "");
			const char *event_name = EVENT_BOT_FANTASY_NO_ACCESS;

			/* Strip off the fantasy character */
			cmd++;

			if (check_access(u, ci, CA_FANTASIA))
				event_name = EVENT_BOT_FANTASY;

			if (params)
				send_event(event_name, 4, cmd, u->nick, ci->name, params);
			else
				send_event(event_name, 3, cmd, u->nick, ci->name);
		}
	}
}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", BotDBName);	\
	failed = 1;					\
	break;						\
	}							\
} while (0)

void load_bs_dbase()
{
	dbFILE *f;
	int c, ver;
	uint16 tmp16;
	uint32 tmp32;
	BotInfo *bi;
	int failed = 0;

	if (!(f = open_db(s_BotServ, BotDBName, "r", BOT_VERSION)))
		return;

	ver = get_file_version(f);

	while (!failed && (c = getc_db(f)) != 0) {
		char *s;

		if (c != 1)
			fatal("Invalid format in %s %d", BotDBName, c);

		SAFE(read_string(&s, f));
		bi = new BotInfo(s);
		delete [] s;
		SAFE(read_string(&bi->user, f));
		SAFE(read_string(&bi->host, f));
		SAFE(read_string(&bi->real, f));
		if (ver >= 10) {
			SAFE(read_int16(&tmp16, f));
			bi->flags = tmp16;
		}
		SAFE(read_int32(&tmp32, f));
		bi->created = tmp32;
		SAFE(read_int16(&tmp16, f));
		bi->chancount = tmp16;
	}

	close_db(f);
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", BotDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", BotDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)

void save_bs_dbase()
{
	dbFILE *f;
	BotInfo *bi;
	static time_t lastwarn = 0;
	int i;

	if (!(f = open_db(s_BotServ, BotDBName, "w", BOT_VERSION)))
		return;

	for (i = 0; i < 256; i++) {
		for (bi = botlists[i]; bi; bi = bi->next) {
			SAFE(write_int8(1, f));
			SAFE(write_string(bi->nick, f));
			SAFE(write_string(bi->user, f));
			SAFE(write_string(bi->host, f));
			SAFE(write_string(bi->real, f));
			SAFE(write_int16(bi->flags, f));
			SAFE(write_int32(bi->created, f));
			SAFE(write_int16(bi->chancount, f));
		}
	}
	SAFE(write_int8(0, f));

	close_db(f);

}

#undef SAFE

/*************************************************************************/

/* Inserts a bot in the bot list. I can't be much explicit mh? */

void insert_bot(BotInfo * bi)
{
	BotInfo *ptr, *prev;

	for (prev = NULL, ptr = botlists[tolower(*bi->nick)];
		 ptr != NULL && stricmp(ptr->nick, bi->nick) < 0;
		 prev = ptr, ptr = ptr->next);
	bi->prev = prev;
	bi->next = ptr;
	if (!prev)
		botlists[tolower(*bi->nick)] = bi;
	else
		prev->next = bi;
	if (ptr)
		ptr->prev = bi;
}

/*************************************************************************/
/*************************************************************************/

BotInfo *findbot(const char *nick)
{
	BotInfo *bi;

	if (!nick || !*nick)
		return NULL;

	/*
	 * XXX Less than efficient, but we need to do this for good TS6 support currently. This *will* improve. -- w00t
	 */
	for (int i = 0; i < 256; i++)
	{
		for (bi = botlists[i]; bi; bi = bi->next)
		{
			if (!stricmp(nick, bi->nick))
				return bi;

			if (nick == bi->uid)
				return bi;
		}
	}

	return NULL;
}


/*************************************************************************/

/* Returns ban data associated with an user if it exists, allocates it
   otherwise. */

static BanData *get_ban_data(Channel * c, User * u)
{
	char mask[BUFSIZE];
	BanData *bd, *next;
	time_t now = time(NULL);

	if (!c || !u)
		return NULL;

	snprintf(mask, sizeof(mask), "%s@%s", u->username,
			 common_get_vhost(u));

	for (bd = c->bd; bd; bd = next) {
		if (now - bd->last_use > BSKeepData) {
			if (bd->next)
				bd->next->prev = bd->prev;
			if (bd->prev)
				bd->prev->next = bd->next;
			else
				c->bd = bd->next;
			if (bd->mask)
				delete [] bd->mask;
			next = bd->next;
			delete bd;
			continue;
		}
		if (!stricmp(bd->mask, mask)) {
			bd->last_use = now;
			return bd;
		}
		next = bd->next;
	}

	/* If we fall here it is that we haven't found the record */
	bd = new BanData;
	bd->mask = sstrdup(mask);
	bd->last_use = now;
	for (int x = 0; x < TTB_SIZE; ++x)
		bd->ttb[x] = 0;

	bd->prev = NULL;
	bd->next = c->bd;
	if (bd->next)
		bd->next->prev = bd;
	c->bd = bd;

	return bd;
}

/*************************************************************************/

/* Returns BotServ data associated with an user on a given channel.
 * Allocates it if necessary.
 */

static UserData *get_user_data(Channel * c, User * u)
{
	struct c_userlist *user;

	if (!c || !u)
		return NULL;

	for (user = c->users; user; user = user->next) {
		if (user->user == u) {
			if (user->ud) {
				time_t now = time(NULL);

				/* Checks whether data is obsolete */
				if (now - user->ud->last_use > BSKeepData) {
					if (user->ud->lastline)
						delete [] user->ud->lastline;
					/* We should not free and realloc, but reset to 0
					   instead. */
					memset(user->ud, 0, sizeof(UserData));
					user->ud->last_use = now;
				}

				return user->ud;
			} else {
				user->ud = new UserData;
				user->ud->last_use = time(NULL);
				user->ud->lines = 0;
				user->ud->last_start = 0;
				user->ud->lastline = NULL;
				user->ud->times = 0;
				return user->ud;
			}
		}
	}

	return NULL;
}

/*************************************************************************/

/* Makes the bot join a channel and op himself. */

void bot_join(ChannelInfo * ci)
{
	if (!ci || !ci->c || !ci->bi)
		return;

	if (BSSmartJoin) {
		/* We check for bans */
		if (ci->c->bans && ci->c->bans->count) {
			char buf[BUFSIZE];
			const char *av[4];
			Entry *ban, *next;
			int ac;

			if (ircdcap->tsmode) {
				snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
				av[0] = ci->c->name;
				av[1] = buf;
				av[2] = "-b";
				ac = 4;
			} else {
				av[0] = ci->c->name;
				av[1] = "-b";
				ac = 3;
			}

			for (ban = ci->c->bans->entries; ban; ban = next) {
				next = ban->next;
				if (entry_match
					(ban, ci->bi->nick, ci->bi->user, ci->bi->host, 0)) {
					ircdproto->SendMode(whosends(ci), ci->name, "-b %s",
								   ban->mask);
					if (ircdcap->tsmode)
						av[3] = ban->mask;
					else
						av[2] = ban->mask;

					do_cmode(whosends(ci)->nick, ac, av);
				}
			}
		}

		/* Should we be invited? */
		if ((ci->c->mode & anope_get_invite_mode())
			|| (ci->c->limit && ci->c->usercount >= ci->c->limit))
			ircdproto->SendNoticeChanops(NULL, ci->c->name,
								 "%s invited %s into the channel.",
								 ci->bi->nick, ci->bi->nick);
	}
	ircdproto->SendJoin(ci->bi, ci->c->name, ci->c->creation_time);
	ircdproto->SendBotOp(ci->bi->nick, ci->c->name);
	send_event(EVENT_BOT_JOIN, 2, ci->name, ci->bi->nick);
}

/*************************************************************************/

/* This makes a ban if the user has to have one. In every cases it increments
   the kick count for the user. */

static void check_ban(ChannelInfo * ci, User * u, int ttbtype)
{
	BanData *bd = get_ban_data(ci->c, u);

	if (!bd)
		return;

	bd->ttb[ttbtype]++;
	if (bd->ttb[ttbtype] == ci->ttb[ttbtype]) {
		const char *av[4];
		int ac;
		char mask[BUFSIZE];
		char buf[BUFSIZE];

		bd->ttb[ttbtype] = 0;

		get_idealban(ci, u, mask, sizeof(mask));

		if (ircdcap->tsmode) {
			snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
			av[0] = ci->name;
			av[1] = buf;
			av[2] = "+b";
			av[3] = mask;
			ac = 4;
		} else {
			av[0] = ci->name;
			av[1] = "+b";
			av[2] = mask;
			ac = 3;
		}

		ircdproto->SendMode(ci->bi, ci->name, "+b %s", mask);
		do_cmode(ci->bi->nick, ac, av);
		send_event(EVENT_BOT_BAN, 3, u->nick, ci->name, mask);
	}
}

/*************************************************************************/

/* This makes a bot kick an user. Works somewhat like notice_lang in fact ;) */

static void bot_kick(ChannelInfo * ci, User * u, int message, ...)
{
	va_list args;
	char buf[1024];
	const char *fmt;
	const char *av[3];

	if (!ci || !ci->bi || !ci->c || !u)
		return;

	va_start(args, message);
	fmt = getstring(u->na, message);
	if (!fmt)
		return;
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	av[0] = ci->name;
	av[1] = u->nick;
	av[2] = buf;
	ircdproto->SendKick(ci->bi, av[0], av[1], "%s", av[2]);
	do_kick(ci->bi->nick, 3, av);
	send_event(EVENT_BOT_KICK, 3, u->nick, ci->name, buf);
}

/*************************************************************************/

/* Makes a simple ban and kicks the target */

void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick,
				 const char *reason)
{
	int ac;
	const char *av[4];
	const char *kav[4]; // seperate as not everything is constified XXX -- w00t
	char mask[BUFSIZE];
	char buf[BUFSIZE];
	User *u = finduser(nick);

	if (!u)
		return;

	if (ircd->protectedumode) {
		if (is_protected(u) && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring2(NULL, PERMISSION_DENIED));
			return;
		}
	}

	if ((ci->flags & CI_PEACE) && stricmp(requester->nick, nick)
		&& (get_access(u, ci) >= get_access(requester, ci)))
		return;

	if (ircd->except) {
		if (is_excepted(ci, u) == 1) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring2(NULL, BOT_EXCEPT));
			return;
		}
	}

	get_idealban(ci, u, mask, sizeof(mask));

	if (ircdcap->tsmode) {
		snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
		av[0] = ci->name;
		av[1] = buf;
		av[2] = "+b";
		av[3] = mask;
		ac = 4;
	} else {
		av[0] = ci->name;
		av[1] = "+b";
		av[2] = mask;
		ac = 3;
	}

	ircdproto->SendMode(ci->bi, ci->name, "+b %s", mask);
	do_cmode(ci->bi->nick, ac, av);

	kav[0] = ci->name;
	kav[1] = nick;

	if (!reason) {
		kav[2] = ci->bi->nick;
	} else {
		if (strlen(reason) > 200)
			(*const_cast<char **>(&reason))[200] = '\0'; // Unsafe cast -- will review later -- CyberBotX
		kav[2] = reason;
	}

	/* Check if we need to do a signkick or not -GD */
	if ((ci->flags & CI_SIGNKICK)
		|| ((ci->flags & CI_SIGNKICK_LEVEL)
			&& !check_access(requester, ci, CA_SIGNKICK)))
		ircdproto->SendKick(ci->bi, kav[0], kav[1], "%s (%s)", kav[2],
					   requester->nick);
	else
		ircdproto->SendKick(ci->bi, kav[0], kav[1], "%s", kav[2]);

	do_kick(ci->bi->nick, 3, kav);
	send_event(EVENT_BOT_KICK, 3, kav[1], kav[0], kav[2]);
}

/*************************************************************************/

/* Makes a kick with a "dynamic" reason ;) */

void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick,
				  const char *reason)
{
	const char *av[3];
	User *u = finduser(nick);

	if (!u || !is_on_chan(ci->c, u))
		return;

	if (ircd->protectedumode) {
		if (is_protected(u) && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring2(NULL, PERMISSION_DENIED));
			return;
		}
	}

	if ((ci->flags & CI_PEACE) && stricmp(requester->nick, nick)
		&& (get_access(u, ci) >= get_access(requester, ci)))
		return;

	av[0] = ci->name;
	av[1] = nick;

	if (!reason) {
		av[2] = ci->bi->nick;
	} else {
		if (strlen(reason) > 200)
			(*const_cast<char **>(&reason))[200] = '\0'; // Unsafe cast -- will review later -- CyberBotX
		av[2] = reason;
	}

	if ((ci->flags & CI_SIGNKICK)
		|| ((ci->flags & CI_SIGNKICK_LEVEL)
			&& !check_access(requester, ci, CA_SIGNKICK)))
		ircdproto->SendKick(ci->bi, av[0], av[1], "%s (%s)", av[2],
					   requester->nick);
	else
		ircdproto->SendKick(ci->bi, av[0], av[1], "%s", av[2]);
	do_kick(ci->bi->nick, 3, av);
	send_event(EVENT_BOT_KICK, 3, av[1], av[0], av[2]);
}

/*************************************************************************/

/* Makes a mode operation on a channel for a nick */

void bot_raw_mode(User * requester, ChannelInfo * ci, const char *mode,
				  char *nick)
{
	const char *av[4];
	int ac;
	char buf[BUFSIZE];
	User *u;

	*buf = '\0';
	u = finduser(nick);

	if (!u || !is_on_chan(ci->c, u))
		return;

	snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));

	if (ircd->protectedumode) {
		if (is_protected(u) && *mode == '-' && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring2(NULL, PERMISSION_DENIED));
			return;
		}
	}

	if (*mode == '-' && (ci->flags & CI_PEACE)
		&& stricmp(requester->nick, nick)
		&& (get_access(u, ci) >= get_access(requester, ci)))
		return;

	if (ircdcap->tsmode) {
		av[0] = ci->name;
		av[1] = buf;
		av[2] = mode;
		av[3] = nick;
		ac = 4;
		ircdproto->SendMode(ci->bi, av[0], "%s %s", av[2], av[3]);
	} else {
		av[0] = ci->name;
		av[1] = mode;
		av[2] = nick;
		ac = 3;
		ircdproto->SendMode(ci->bi, av[0], "%s %s", av[1], av[2]);
	}

	do_cmode(ci->bi->nick, ac, av);
}

/*************************************************************************/
/**
 * Normalize buffer stripping control characters and colors
 * @param A string to be parsed for control and color codes
 * @return A string stripped of control and color codes
 */
char *normalizeBuffer(const char *buf)
{
	char *newbuf;
	int i, len, j = 0;

	len = strlen(buf);
	newbuf = new char[len + 1];

	for (i = 0; i < len; i++) {
		switch (buf[i]) {
			/* ctrl char */
		case 1:
			break;
			/* Bold ctrl char */
		case 2:
			break;
			/* Color ctrl char */
		case 3:
			/* If the next character is a digit, its also removed */
			if (isdigit(buf[i + 1])) {
				i++;

				/* not the best way to remove colors
				 * which are two digit but no worse then
				 * how the Unreal does with +S - TSL
				 */
				if (isdigit(buf[i + 1])) {
					i++;
				}

				/* Check for background color code
				 * and remove it as well
				 */
				if (buf[i + 1] == ',') {
					i++;

					if (isdigit(buf[i + 1])) {
						i++;
					}
					/* not the best way to remove colors
					 * which are two digit but no worse then
					 * how the Unreal does with +S - TSL
					 */
					if (isdigit(buf[i + 1])) {
						i++;
					}
				}
			}

			break;
			/* line feed char */
		case 10:
			break;
			/* carriage returns char */
		case 13:
			break;
			/* Reverse ctrl char */
		case 22:
			break;
			/* Underline ctrl char */
		case 31:
			break;
			/* A valid char gets copied into the new buffer */
		default:
			newbuf[j] = buf[i];
			j++;
		}
	}

	/* Terminate the string */
	newbuf[j] = 0;

	return (newbuf);
}
