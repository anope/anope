/* BotServ functions
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
	ModuleManager::LoadModuleList(Config.BotServCoreModules);
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
	if (Config.s_BotServ) {
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
		ircdproto->SendCTCP(findbot(Config.s_BotServ), u->nick, "PING %s", s);
	} else {
		mod_run_cmd(Config.s_BotServ, u, BOTSERV, cmd);
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
	bool was_action = false;
	Command *command;
	std::string bbuf;

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
	{
		buf += 8;
		was_action = true;
	}

	/* Now we can make kicker stuff. We try to order the checks
	 * from the fastest one to the slowest one, since there's
	 * no need to process other kickers if an user is kicked before
	 * the last kicker check.
	 *
	 * But FIRST we check whether the user is protected in any
	 * way.
	 */

	/* We first retrieve the user status on the channel if needed */
	if (ci->botflags.HasFlag(BS_DONTKICKOPS) || ci->botflags.HasFlag(BS_DONTKICKVOICES))
		cstatus = chan_get_user_status(ci->c, u);

	if (buf && !check_access(u, ci, CA_NOKICK) &&
		(!ci->botflags.HasFlag(BS_DONTKICKOPS)
		 || !(cstatus & (CUS_HALFOP | CUS_OP | CUS_OWNER | CUS_PROTECT)))
		&& (!ci->botflags.HasFlag(BS_DONTKICKVOICES) || !(cstatus & CUS_VOICE))) {

		/* Bolds kicker */
		if (ci->botflags.HasFlag(BS_KICK_BOLDS) && strchr(buf, 2)) {
			check_ban(ci, u, TTB_BOLDS);
			bot_kick(ci, u, BOT_REASON_BOLD);
			return;
		}

		/* Color kicker */
		if (ci->botflags.HasFlag(BS_KICK_COLORS) && strchr(buf, 3)) {
			check_ban(ci, u, TTB_COLORS);
			bot_kick(ci, u, BOT_REASON_COLOR);
			return;
		}

		/* Reverses kicker */
		if (ci->botflags.HasFlag(BS_KICK_REVERSES) && strchr(buf, 22)) {
			check_ban(ci, u, TTB_REVERSES);
			bot_kick(ci, u, BOT_REASON_REVERSE);
			return;
		}

		/* Underlines kicker */
		if (ci->botflags.HasFlag(BS_KICK_UNDERLINES) && strchr(buf, 31)) {
			check_ban(ci, u, TTB_UNDERLINES);
			bot_kick(ci, u, BOT_REASON_UNDERLINE);
			return;
		}

		/* Caps kicker */
		if (ci->botflags.HasFlag(BS_KICK_CAPS)
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
		if (ci->botflags.HasFlag(BS_KICK_BADWORDS)) {
			int mustkick = 0;
			char *nbuf;
			BadWord *bw;

			/* Normalize the buffer */
			nbuf = normalizeBuffer(buf);

			for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
			{
				bw = ci->GetBadWord(i);

				if (bw->type == BW_ANY
					&& ((Config.BSCaseSensitive && strstr(nbuf, bw->word.c_str()))
						|| (!Config.BSCaseSensitive && stristr(nbuf, bw->word.c_str())))) {
					mustkick = 1;
				} else if (bw->type == BW_SINGLE) {
					int len = bw->word.length();

					if ((Config.BSCaseSensitive && nbuf == bw->word)
						|| (!Config.BSCaseSensitive
							&& (!stricmp(nbuf, bw->word.c_str())))) {
						mustkick = 1;
						/* two next if are quite odd isn't it? =) */
					} else if ((strchr(nbuf, ' ') == nbuf + len)
							   &&
							   ((Config.BSCaseSensitive && nbuf == bw->word)
								|| (!Config.BSCaseSensitive
									&& (stristr(nbuf, bw->word.c_str()) ==
										nbuf)))) {
						mustkick = 1;
					} else {
						if ((strrchr(nbuf, ' ') ==
							 nbuf + strlen(nbuf) - len - 1)
							&&
							((Config.BSCaseSensitive
							  && (strstr(nbuf, bw->word.c_str()) ==
								  nbuf + strlen(nbuf) - len))
							 || (!Config.BSCaseSensitive
								 && (stristr(nbuf, bw->word.c_str()) ==
									 nbuf + strlen(nbuf) - len)))) {
							mustkick = 1;
						} else {
							char *wordbuf = new char[len + 3];

							wordbuf[0] = ' ';
							wordbuf[len + 1] = ' ';
							wordbuf[len + 2] = '\0';
							memcpy(wordbuf + 1, bw->word.c_str(), len);

							if ((Config.BSCaseSensitive
								 && (strstr(nbuf, wordbuf)))
								|| (!Config.BSCaseSensitive
									&& (stristr(nbuf, wordbuf)))) {
								mustkick = 1;
							}

							/* free previous (sc)allocated memory (#850) */
							delete [] wordbuf;
						}
					}
				} else if (bw->type == BW_START) {
					int len = bw->word.length();

					if ((Config.BSCaseSensitive
						 && (!strncmp(nbuf, bw->word.c_str(), len)))
						|| (!Config.BSCaseSensitive
							&& (!strnicmp(nbuf, bw->word.c_str(), len)))) {
						mustkick = 1;
					} else {
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf + 1, bw->word.c_str(), len);
						wordbuf[0] = ' ';
						wordbuf[len + 1] = '\0';

						if ((Config.BSCaseSensitive && (strstr(nbuf, wordbuf)))
							|| (!Config.BSCaseSensitive
								&& (stristr(nbuf, wordbuf))))
							mustkick = 1;

						delete [] wordbuf;
					}
				} else if (bw->type == BW_END) {
					int len = bw->word.length();

					if ((Config.BSCaseSensitive
						 &&
						 (!strncmp
						  (nbuf + strlen(nbuf) - len, bw->word.c_str(), len)))
						|| (!Config.BSCaseSensitive
							&&
							(!strnicmp
							 (nbuf + strlen(nbuf) - len, bw->word.c_str(),
							  len)))) {
						mustkick = 1;
					} else {
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf, bw->word.c_str(), len);
						wordbuf[len] = ' ';
						wordbuf[len + 1] = '\0';

						if ((Config.BSCaseSensitive && (strstr(nbuf, wordbuf)))
							|| (!Config.BSCaseSensitive
								&& (stristr(nbuf, wordbuf))))
							mustkick = 1;

						delete [] wordbuf;
					}
				}

				if (mustkick) {
					check_ban(ci, u, TTB_BADWORDS);
					if (Config.BSGentleBWReason)
						bot_kick(ci, u, BOT_REASON_BADWORD_GENTLE);
					else
						bot_kick(ci, u, BOT_REASON_BADWORD, bw->word.c_str());

					/* free the normalized buffer before return (#850) */
					delete [] nbuf;

					return;
				}
			}

			/* Free the normalized buffer */
			delete [] nbuf;
		}

		/* Flood kicker */
		if (ci->botflags.HasFlag(BS_KICK_FLOOD)) {
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
		if (ci->botflags.HasFlag(BS_KICK_REPEAT)) {
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

	if (buf && ci->botflags.HasFlag(BS_FANTASY) && *buf == *Config.BSFantasyCharacter && !was_action) {
		cmd = strtok(buf, " ");

		if (cmd && (cmd[0] == *Config.BSFantasyCharacter)) {
			char *params = strtok(NULL, "");

			/* Strip off the fantasy character */
			cmd++;

			if (check_access(u, ci, CA_FANTASIA))
			{
				command = findCommand(CHANSERV, cmd);

				/* Command exists and can not be called by fantasy */
				if (command && !command->HasFlag(CFLAG_DISABLE_FANTASY))
				{
					bbuf = std::string(cmd);

					/* Some commands don't need the channel name added.. eg !help */
					if (!command->HasFlag(CFLAG_STRIP_CHANNEL))
					{
						bbuf += " ";
						bbuf += ci->name;
					}

					if (params)
					{
						bbuf += " ";
						bbuf += params;
					}

					chanserv(u, const_cast<char *>(bbuf.c_str())); // XXX Unsafe cast, this needs reviewing -- CyberBotX
				}

				FOREACH_MOD(I_OnBotFantasy, OnBotFantasy(cmd, u, ci, params));
			}
			else
			{
				FOREACH_MOD(I_OnBotNoFantasyAccess, OnBotNoFantasyAccess(cmd, u, ci, params));
			}
		}
	}
}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", Config.BotDBName);	\
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

	if (!(f = open_db(Config.s_BotServ, Config.BotDBName, "r", BOT_VERSION)))
		return;

	ver = get_file_version(f);

	while (!failed && (c = getc_db(f)) != 0) {
		char *s;

		if (c != 1)
			fatal("Invalid format in %s %d", Config.BotDBName, c);

		SAFE(read_string(&s, f));
		bi = new BotInfo(s);
		delete [] s;
		SAFE(read_string(&bi->user, f));
		SAFE(read_string(&bi->host, f));
		SAFE(read_string(&bi->real, f));
		if (ver >= 10) {
			SAFE(read_int16(&tmp16, f));
			//bi->flags |= tmp16;
		}
		SAFE(read_int32(&tmp32, f));
		bi->created = tmp32;
		SAFE(read_int16(&tmp16, f));
		bi->chancount = tmp16;

		/* Fixes bug #1080, services bot names may have been
		 * changed in the config and different from database
		 * names
		 */
		if (Config.s_ChanServ && bi->HasFlag(BI_CHANSERV) && strcmp(bi->nick, Config.s_ChanServ))
			bi->ChangeNick(Config.s_ChanServ);
		else if (Config.s_BotServ && bi->HasFlag(BI_BOTSERV) && strcmp(bi->nick, Config.s_BotServ))
			bi->ChangeNick(Config.s_BotServ);
		else if (Config.s_HostServ && bi->HasFlag(BI_HOSTSERV) && strcmp(bi->nick, Config.s_HostServ))
			bi->ChangeNick(Config.s_HostServ);
		else if (Config.s_OperServ && bi->HasFlag(BI_OPERSERV) && strcmp(bi->nick, Config.s_OperServ))
			bi->ChangeNick(Config.s_OperServ);
		else if (Config.s_MemoServ && bi->HasFlag(BI_MEMOSERV) && strcmp(bi->nick, Config.s_MemoServ))
			bi->ChangeNick(Config.s_MemoServ);
		else if (Config.s_NickServ && bi->HasFlag(BI_NICKSERV) && strcmp(bi->nick, Config.s_NickServ))
			bi->ChangeNick(Config.s_NickServ);
		else if (Config.s_GlobalNoticer && bi->HasFlag(BI_GLOBAL) && strcmp(bi->nick, Config.s_GlobalNoticer))
			bi->ChangeNick(Config.s_GlobalNoticer);
	}

	close_db(f);
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", Config.BotDBName);		\
	if (time(NULL) - lastwarn > Config.WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", Config.BotDBName,	\
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

	if (!(f = open_db(Config.s_BotServ, Config.BotDBName, "w", BOT_VERSION)))
		return;

	for (i = 0; i < 256; i++) {
		for (bi = botlists[i]; bi; bi = bi->next) {
			SAFE(write_int8(1, f));
			SAFE(write_string(bi->nick, f));
			SAFE(write_string(bi->user, f));
			SAFE(write_string(bi->host, f));
			SAFE(write_string(bi->real, f));
			//SAFE(write_int16(bi->flags, f));
			SAFE(write_int16(0, f));
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

	snprintf(mask, sizeof(mask), "%s@%s", u->GetIdent().c_str(),
			 u->GetDisplayedHost().c_str());

	for (bd = c->bd; bd; bd = next) {
		if (now - bd->last_use > Config.BSKeepData) {
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
				if (now - user->ud->last_use > Config.BSKeepData) {
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

	if (Config.BSSmartJoin)
	{
		/* We check for bans */
		if (ci->c->bans && ci->c->bans->count)
		{
			Entry *ban, *next;

			for (ban = ci->c->bans->entries; ban; ban = next)
			{
				next = ban->next;

				if (entry_match(ban, ci->bi->nick, ci->bi->user, ci->bi->host, 0))
				{
					ci->c->RemoveMode(NULL, CMODE_BAN, ban->mask);
				}
			}
		}

		std::string Limit;
		int limit = 0;
		if (ci->c->GetParam(CMODE_LIMIT, &Limit))
		{
			limit = atoi(Limit.c_str());
		}

		/* Should we be invited? */
		if (ci->c->HasMode(CMODE_INVITE)
			|| (limit && ci->c->usercount >= limit))
			ircdproto->SendNoticeChanops(ci->bi, ci->c->name,
				 "%s invited %s into the channel.",
				 ci->bi->nick, ci->bi->nick);
	}
	ircdproto->SendJoin(ci->bi, ci->c->name, ci->c->creation_time);
	ci->c->SetMode(NULL, CMODE_PROTECT, ci->bi->nick);
	ci->c->SetMode(NULL, CMODE_OP, ci->bi->nick);
	FOREACH_MOD(I_OnBotJoin, OnBotJoin(ci, ci->bi));
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
	if (ci->ttb[ttbtype] && bd->ttb[ttbtype] >= ci->ttb[ttbtype])
	{
		/* Should not use == here because bd->ttb[ttbtype] could possibly be > ci->ttb[ttbtype]
		 * if the TTB was changed after it was not set (0) before and the user had already been
		 * kicked a few times. Bug #1056 - Adam */
		char mask[BUFSIZE];

		bd->ttb[ttbtype] = 0;

		get_idealban(ci, u, mask, sizeof(mask));

		if (ci->c)
			ci->c->SetMode(NULL, CMODE_BAN, mask);
		FOREACH_MOD(I_OnBotBan, OnBotBan(u, ci, mask));
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
	fmt = getstring(u, message);
	if (!fmt)
		return;
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	av[0] = ci->name;
	av[1] = u->nick;
	av[2] = buf;
	ircdproto->SendKick(ci->bi, av[0], av[1], "%s", av[2]);
	do_kick(ci->bi->nick, 3, av);
	FOREACH_MOD(I_OnBotKick, OnBotKick(u, ci, buf));
}

/*************************************************************************/

/* Makes a simple ban and kicks the target */

void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick,
				 const char *reason)
{
	const char *kav[4]; // seperate as not everything is constified XXX -- w00t
	char mask[BUFSIZE];
	User *u = finduser(nick);

	if (!u)
		return;

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED)))
	{
		if (is_protected(u) && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring(ACCESS_DENIED));
			return;
		}
	}

	if (ci->HasFlag(CI_PEACE) && stricmp(requester->nick, nick) && (get_access(u, ci) >= get_access(requester, ci)))
		return;

	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
	{
		if (is_excepted(ci, u) == 1) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring(BOT_EXCEPT));
			return;
		}
	}

	get_idealban(ci, u, mask, sizeof(mask));

	ci->c->SetMode(NULL, CMODE_BAN, mask);

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
	if ((ci->HasFlag(CI_SIGNKICK) || ci->HasFlag(CI_SIGNKICK_LEVEL)) && !check_access(requester, ci, CA_SIGNKICK))
		ircdproto->SendKick(ci->bi, kav[0], kav[1], "%s (%s)", kav[2],
					   requester->nick);
	else
		ircdproto->SendKick(ci->bi, kav[0], kav[1], "%s", kav[2]);

	do_kick(ci->bi->nick, 3, kav);
	FOREACH_MOD(I_OnBotKick, OnBotKick(u, ci, kav[2]));
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

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED)))
	{
		if (is_protected(u) && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring(ACCESS_DENIED));
			return;
		}
	}

	if (ci->HasFlag(CI_PEACE) && stricmp(requester->nick, nick) && (get_access(u, ci) >= get_access(requester, ci)))
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

	if (ci->HasFlag(CI_SIGNKICK) || ((ci->HasFlag(CI_SIGNKICK_LEVEL)) && !check_access(requester, ci, CA_SIGNKICK)))
		ircdproto->SendKick(ci->bi, av[0], av[1], "%s (%s)", av[2],
					   requester->nick);
	else
		ircdproto->SendKick(ci->bi, av[0], av[1], "%s", av[2]);
	do_kick(ci->bi->nick, 3, av);
	FOREACH_MOD(I_OnBotKick, OnBotKick(u, ci, av[2]));
}

/*************************************************************************/

/* Makes a mode operation on a channel for a nick */

void bot_raw_mode(User * requester, ChannelInfo * ci, const char *mode,
				  char *nick)
{
	char buf[BUFSIZE];
	User *u;

	*buf = '\0';
	u = finduser(nick);

	if (!u || !is_on_chan(ci->c, u))
		return;

	snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED))) {
		if (is_protected(u) && *mode == '-' && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name, "%s",
							  getstring(ACCESS_DENIED));
			return;
		}
	}

	if (*mode == '-' && ci->HasFlag(CI_PEACE)
		&& stricmp(requester->nick, nick)
		&& (get_access(u, ci) >= get_access(requester, ci)))
		return;

	ci->c->SetModes(NULL, "%s %s", mode, nick);
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
