/* BotServ functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

/*************************************************************************/

#include "services.h"
#include "modules.h"
#include "language.h"

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

	for (botinfo_map::const_iterator it = BotListByNick.begin(); it != BotListByNick.end(); ++it)
	{
		BotInfo *bi = it->second;

		count++;
		mem += sizeof(*bi);
		mem += bi->nick.size() + 1;
		mem += bi->user.size() + 1;
		mem += bi->host.size() + 1;
		mem += bi->real.size() + 1;
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

void botserv(User *u, BotInfo *bi, const std::string &buf)
{
	if (!u || !bi || buf.empty())
		return;
	
	if (buf.find("\1PING ", 0, 6) != std::string::npos && buf[buf.length() - 1] == '\1')
	{
		std::string command = buf;
		command.erase(command.begin());
		command.erase(command.end());
		ircdproto->SendCTCP(bi, u->nick.c_str(), "%s", command.c_str());
	}
	else
	{
		mod_run_cmd(bi, u, buf);
	}
}

/*************************************************************************/

/* Handles all messages that are sent to registered channels where a
 * bot is on.
 *
 */

void botchanmsgs(User *u, ChannelInfo *ci, const std::string &buf)
{
	if (!u || !ci || !ci->c || buf.empty())
		return;
	
	/* Answer to ping if needed */
	if (buf.find("\1PING ", 0, 6) != std::string::npos && buf[buf.length() - 1] == '\1')
	{
		std::string ctcp = buf;
		ctcp.erase(ctcp.begin());
		ctcp.erase(ctcp.end());
		ircdproto->SendCTCP(ci->bi, u->nick.c_str(), "%s", ctcp.c_str());
	}

	bool was_action = false;
	std::string realbuf = buf;

	/* If it's a /me, cut the CTCP part because the ACTION will cause
	 * problems with the caps or badwords kicker
	 */
	if (realbuf.find("\1ACTION ", 0, 8) && realbuf[buf.length() - 1] == '\1')
	{
		realbuf.erase(0, 8);
		realbuf.erase(realbuf.end());
		was_action = true;
	}

	if (realbuf.empty())
		return;

	/* Now we can make kicker stuff. We try to order the checks
	 * from the fastest one to the slowest one, since there's
	 * no need to process other kickers if an user is kicked before
	 * the last kicker check.
	 *
	 * But FIRST we check whether the user is protected in any
	 * way.
	 */

	bool Allow = false;
	if (!ci->botflags.HasFlag(BS_DONTKICKOPS) && !ci->botflags.HasFlag(BS_DONTKICKVOICES))
		Allow = true;
	else if (ci->botflags.HasFlag(BS_DONTKICKOPS) && (ci->c->HasUserStatus(u, CMODE_HALFOP) || ci->c->HasUserStatus(u, CMODE_OP) || ci->c->HasUserStatus(u, CMODE_PROTECT) || ci->c->HasUserStatus(u, CMODE_OWNER)))
		Allow = true;
	else if (ci->botflags.HasFlag(BS_DONTKICKVOICES) && ci->c->HasUserStatus(u, CMODE_VOICE))
		Allow = true;

	if (!check_access(u, ci, CA_NOKICK) && Allow)
	{
		/* Bolds kicker */
		if (ci->botflags.HasFlag(BS_KICK_BOLDS) && realbuf.find_first_of(2) != std::string::npos)
		{
			check_ban(ci, u, TTB_BOLDS);
			bot_kick(ci, u, BOT_REASON_BOLD);
			return;
		}

		/* Color kicker */
		if (ci->botflags.HasFlag(BS_KICK_COLORS) && realbuf.find_first_of(3) != std::string::npos)
		{
			check_ban(ci, u, TTB_COLORS);
			bot_kick(ci, u, BOT_REASON_COLOR);
			return;
		}

		/* Reverses kicker */
		if (ci->botflags.HasFlag(BS_KICK_REVERSES) && realbuf.find_first_of(22) != std::string::npos)
		{
			check_ban(ci, u, TTB_REVERSES);
			bot_kick(ci, u, BOT_REASON_REVERSE);
			return;
		}

		/* Underlines kicker */
		if (ci->botflags.HasFlag(BS_KICK_UNDERLINES) && realbuf.find_first_of(31) != std::string::npos)
		{
			check_ban(ci, u, TTB_UNDERLINES);
			bot_kick(ci, u, BOT_REASON_UNDERLINE);
			return;
		}

		/* Caps kicker */
		if (ci->botflags.HasFlag(BS_KICK_CAPS) && realbuf.length() >= ci->capsmin)
		{
			int i = 0;
			int l = 0;

			for (unsigned j = 0; j < realbuf.length(); ++j)
			{
				if (isupper(realbuf[j]))
					++i;
				else if (islower(realbuf[j]))
					++l;
			}
			
			/* i counts uppercase chars, l counts lowercase chars. Only
			 * alphabetic chars (so islower || isupper) qualify for the
			 * percentage of caps to kick for; the rest is ignored. -GD
			 */

			if (i && l && i >= ci->capsmin && i * 100 / (i + l) >= ci->capspercent)
			{
				check_ban(ci, u, TTB_CAPS);
				bot_kick(ci, u, BOT_REASON_CAPS);
				return;
			}
		}

		/* Bad words kicker */
		if (ci->botflags.HasFlag(BS_KICK_BADWORDS))
		{
			bool mustkick = false;

			/* Normalize the buffer */
			const char *nbuf = normalizeBuffer(realbuf.c_str());

			for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
			{
				BadWord *bw = ci->GetBadWord(i);

				if (bw->type == BW_ANY && ((Config.BSCaseSensitive && strstr(nbuf, bw->word.c_str())) || (!Config.BSCaseSensitive && stristr(nbuf, bw->word.c_str()))))
				{
					mustkick = true;
				}
				else if (bw->type == BW_SINGLE)
				{
					size_t len = bw->word.length();

					if ((Config.BSCaseSensitive && nbuf == bw->word) || (!Config.BSCaseSensitive && (!stricmp(nbuf, bw->word.c_str())))) 
					{
						mustkick = true;
					}
					else if ((strchr(nbuf, ' ') == nbuf + len) && ((Config.BSCaseSensitive && nbuf == bw->word)
						|| (!Config.BSCaseSensitive && (stristr(nbuf, bw->word.c_str()) == nbuf))))
					{
						mustkick = true;
					}
					else
					{
						if ((strrchr(nbuf, ' ') == nbuf + strlen(nbuf) - len - 1) && ((Config.BSCaseSensitive && (strstr(nbuf, bw->word.c_str()) == nbuf + strlen(nbuf) - len)) || (!Config.BSCaseSensitive && (stristr(nbuf, bw->word.c_str()) == nbuf + strlen(nbuf) - len))))
						{
							mustkick = true;
						}
						else
						{
							char *wordbuf = new char[len + 3];

							wordbuf[0] = ' ';
							wordbuf[len + 1] = ' ';
							wordbuf[len + 2] = '\0';
							memcpy(wordbuf + 1, bw->word.c_str(), len);

							if ((Config.BSCaseSensitive && (strstr(nbuf, wordbuf))) || (!Config.BSCaseSensitive && (stristr(nbuf, wordbuf))))
							{
								mustkick = true;
							}

							delete [] wordbuf;
						}
					}
				}
				else if (bw->type == BW_START)
				{
					size_t len = bw->word.length();

					if ((Config.BSCaseSensitive && (!strncmp(nbuf, bw->word.c_str(), len))) || (!Config.BSCaseSensitive && (!strnicmp(nbuf, bw->word.c_str(), len))))
					{
						mustkick = true;
					}
					else
					{
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf + 1, bw->word.c_str(), len);
						wordbuf[0] = ' ';
						wordbuf[len + 1] = '\0';

						if ((Config.BSCaseSensitive && (strstr(nbuf, wordbuf))) || (!Config.BSCaseSensitive && (stristr(nbuf, wordbuf))))
						{
							mustkick = true;
						}

						delete [] wordbuf;
					}
				}
				else if (bw->type == BW_END)
				{
					size_t len = bw->word.length();

					if ((Config.BSCaseSensitive && (!strncmp(nbuf + strlen(nbuf) - len, bw->word.c_str(), len)))
						|| (!Config.BSCaseSensitive && (!strnicmp(nbuf + strlen(nbuf) - len, bw->word.c_str(), len))))
					{
						mustkick = true;
					}
					else
					{
						char *wordbuf = new char[len + 2];

						memcpy(wordbuf, bw->word.c_str(), len);
						wordbuf[len] = ' ';
						wordbuf[len + 1] = '\0';

						if ((Config.BSCaseSensitive && (strstr(nbuf, wordbuf))) || (!Config.BSCaseSensitive && (stristr(nbuf, wordbuf))))
						{
							mustkick = true;
						}

						delete [] wordbuf;
					}
				}

				if (mustkick)
				{
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
		if (ci->botflags.HasFlag(BS_KICK_FLOOD))
		{
			time_t now = time(NULL);

			UserData *ud = get_user_data(ci->c, u);
			if (!ud)
				return;

			if (now - ud->last_start > ci->floodsecs)
			{
				ud->last_start = time(NULL);
				ud->lines = 0;
			}

			ud->lines++;
			if (ud->lines >= ci->floodlines)
			{
				check_ban(ci, u, TTB_FLOOD);
				bot_kick(ci, u, BOT_REASON_FLOOD);
				return;
			}
		}

		/* Repeat kicker */
		if (ci->botflags.HasFlag(BS_KICK_REPEAT))
		{
			UserData *ud = get_user_data(ci->c, u);
			if (!ud)
				return;
			
			if (ud->lastline && stricmp(ud->lastline, buf.c_str()))
			{
				delete [] ud->lastline;
				ud->lastline = sstrdup(buf.c_str());
				ud->times = 0;
			}
			else
			{
				if (!ud->lastline)
					ud->lastline = sstrdup(buf.c_str());
				ud->times++;
			}

			if (ud->times >= ci->repeattimes)
			{
				check_ban(ci, u, TTB_REPEAT);
				bot_kick(ci, u, BOT_REASON_REPEAT);
				return;
			}
		}
	}

	/* return if the user is on the ignore list  */
	if (get_ignore(u->nick.c_str()) != NULL)
	{
		return;
	}

	/* Fantaisist commands */
	if (ci->botflags.HasFlag(BS_FANTASY) && buf[0] == *Config.BSFantasyCharacter && !was_action)
	{
		spacesepstream sep(buf);
		std::string token;

		if (sep.GetToken(token) && (token[0] == *Config.BSFantasyCharacter))
		{
			/* Strip off the fantasy character */
			token.erase(token.begin());

			if (check_access(u, ci, CA_FANTASIA))
			{
				Command *command = FindCommand(ChanServ, token.c_str());

				/* Command exists and can not be called by fantasy */
				if (command && !command->HasFlag(CFLAG_DISABLE_FANTASY))
				{
					std::string bbuf = std::string(token);

					/* Some commands don't need the channel name added.. eg !help */
					if (!command->HasFlag(CFLAG_STRIP_CHANNEL))
					{
						bbuf += " ";
						bbuf += ci->name;
					}

					if (!sep.StreamEnd())
					{
						bbuf += " ";
						bbuf += sep.GetRemaining();
					}

					chanserv(u, bbuf);
				}

				FOREACH_MOD(I_OnBotFantasy, OnBotFantasy(token, u, ci, sep.GetRemaining()));
			}
			else
			{
				FOREACH_MOD(I_OnBotNoFantasyAccess, OnBotNoFantasyAccess(token, u, ci, sep.GetRemaining()));
			}
		}
	}
}

/*************************************************************************/

BotInfo *findbot(const char *nick)
{
	return findbot(ci::string(nick));
}

BotInfo *findbot(const std::string &nick)
{
	return findbot(ci::string(nick.c_str()));
}

BotInfo *findbot(const ci::string &nick)
{
	if (isdigit(nick[0]) && ircd->ts6)
	{
		botinfo_uid_map::const_iterator it = BotListByUID.find(nick.c_str());

		if (it != BotListByUID.end())
			return it->second;
		return NULL;
	}

	botinfo_map::const_iterator it = BotListByNick.find(nick);

	if (it != BotListByNick.end())
		return it->second;
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

static UserData *get_user_data(Channel *c, User *u)
{
	if (!c || !u)
		return NULL;

	for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
	{
		UserContainer *uc = *it;

		if (uc->user == u)
		{
			time_t now = time(NULL);

			/* Checks whether data is obsolete */
			if (now - uc->ud.last_use > Config.BSKeepData)
			{
				if (uc->ud.lastline)
					delete [] uc->ud.lastline;
				/* We should not free and realloc, but reset to 0
				   instead. */
				memset(&uc->ud, 0, sizeof(UserData));
				uc->ud.last_use = now;
			}

			return &uc->ud;
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

				if (entry_match(ban, ci->bi->nick.c_str(), ci->bi->user.c_str(), ci->bi->host.c_str(), 0))
				{
					ci->c->RemoveMode(NULL, CMODE_BAN, ban->mask);
				}
			}
		}

		std::string Limit;
		int limit = 0;
		if (ci->c->GetParam(CMODE_LIMIT, Limit))
		{
			limit = atoi(Limit.c_str());
		}

		/* Should we be invited? */
		if (ci->c->HasMode(CMODE_INVITE)
			|| (limit && ci->c->users.size() >= limit))
			ircdproto->SendNoticeChanops(ci->bi, ci->c,
				 "%s invited %s into the channel.",
				 ci->bi->nick.c_str(), ci->bi->nick.c_str());
	}
	ircdproto->SendJoin(ci->bi, ci->c->name.c_str(), ci->c->creation_time);
	for (std::list<ChannelModeStatus *>::iterator it = BotModes.begin(); it != BotModes.end(); ++it)
	{
		ci->c->SetMode(ci->bi, *it, ci->bi->nick, false);
	}
	FOREACH_MOD(I_OnBotJoin, OnBotJoin(ci, ci->bi));
}

/*************************************************************************/

/** Check if a user should be banned by botserv
 * @param ci The channel the user is on
 * @param u The user
 * @param ttbtype The type of bot kicker the user should be checked against
 */
static void check_ban(ChannelInfo *ci, User *u, int ttbtype)
{
	BanData *bd = get_ban_data(ci->c, u);

	if (!bd)
		return;

	/* Don't ban ulines */
	if (u->server->IsULined())
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

	if (!ci || !ci->bi || !ci->c || !u)
		return;

	va_start(args, message);
	fmt = getstring(u, message);
	if (!fmt)
		return;
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	ci->c->Kick(ci->bi, u, "%s", buf);
}

/*************************************************************************/

/* Makes a simple ban and kicks the target */

void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick, const char *reason)
{
	char mask[BUFSIZE];
	User *u = finduser(nick);

	if (!u)
		return;

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED)))
	{
		if (u->IsProtected() && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name.c_str(), "%s", getstring(ACCESS_DENIED));
			return;
		}
	}

	if (ci->HasFlag(CI_PEACE) && stricmp(requester->nick.c_str(), nick) && (get_access(u, ci) >= get_access(requester, ci)))
		return;

	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
	{
		if (is_excepted(ci, u) == 1) {
			ircdproto->SendPrivmsg(ci->bi, ci->name.c_str(), "%s", getstring(BOT_EXCEPT));
			return;
		}
	}

	get_idealban(ci, u, mask, sizeof(mask));

	ci->c->SetMode(NULL, CMODE_BAN, mask);

	/* Check if we need to do a signkick or not -GD */
	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
		ci->c->Kick(ci->bi, u, "%s (%s)", reason ? reason : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", reason ? reason : ci->bi->nick.c_str());
}

/*************************************************************************/

/* Makes a kick with a "dynamic" reason ;) */

void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick, const char *reason)
{
	User *u = finduser(nick);

	if (!u || !ci->c->FindUser(u))
		return;

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED)))
	{
		if (u->IsProtected() && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name.c_str(), "%s", getstring(ACCESS_DENIED));
			return;
		}
	}

	if (ci->HasFlag(CI_PEACE) && stricmp(requester->nick.c_str(), nick) && (get_access(u, ci) >= get_access(requester, ci)))
		return;

	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
		ci->c->Kick(ci->bi, u, "%s (%s)", reason ? reason : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", reason ? reason : ci->bi->nick.c_str());
}

/*************************************************************************/

/* Makes a mode operation on a channel for a nick */

void bot_raw_mode(User * requester, ChannelInfo * ci, const char *mode, char *nick)
{
	char buf[BUFSIZE];
	User *u;

	*buf = '\0';
	u = finduser(nick);

	if (!u || !ci->c->FindUser(u))
		return;

	snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));

	if ((ModeManager::FindUserModeByName(UMODE_PROTECTED))) {
		if (u->IsProtected() && *mode == '-' && (requester != u)) {
			ircdproto->SendPrivmsg(ci->bi, ci->name.c_str(), "%s", getstring(ACCESS_DENIED));
			return;
		}
	}

	if (*mode == '-' && ci->HasFlag(CI_PEACE)
		&& stricmp(requester->nick.c_str(), nick) && (get_access(u, ci) >= get_access(requester, ci)))
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
