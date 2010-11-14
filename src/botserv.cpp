/* BotServ functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "services.h"
#include "modules.h"

static UserData *get_user_data(Channel *c, User *u);

static void check_ban(ChannelInfo *ci, User *u, int ttbtype);
static void bot_kick(ChannelInfo *ci, User *u, LanguageString message, ...);

E void moduleAddBotServCmds();

/*************************************************************************/

void moduleAddBotServCmds()
{
	ModuleManager::LoadModuleList(Config->BotServCoreModules);
}

/*************************************************************************/
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_botserv_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;

	for (patricia_tree<BotInfo>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
	{
		BotInfo *bi = *it;

		++count;
		mem += sizeof(*bi);
		mem += bi->nick.length() + 1;
		mem += bi->GetIdent().length() + 1;
		mem += bi->host.length() + 1;
		mem += bi->realname.length() + 1;
	}

	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* BotServ initialization. */

void bs_init()
{
	if (!Config->s_BotServ.empty())
		moduleAddBotServCmds();
}

/*************************************************************************/

/* Handles all messages that are sent to registered channels where a
 * bot is on.
 */

void botchanmsgs(User *u, ChannelInfo *ci, const Anope::string &buf)
{
	if (!u || !ci || !ci->c || buf.empty())
		return;

	/* Answer to ping if needed */
	if (buf.substr(0, 6).equals_ci("\1PING ") && buf[buf.length() - 1] == '\1')
	{
		Anope::string ctcp = buf;
		ctcp.erase(ctcp.begin());
		ctcp.erase(ctcp.length() - 1);
		ircdproto->SendCTCP(ci->bi, u->nick, "%s", ctcp.c_str());
	}

	bool was_action = false;
	Anope::string realbuf = buf;

	/* If it's a /me, cut the CTCP part because the ACTION will cause
	 * problems with the caps or badwords kicker
	 */
	if (!realbuf.substr(0, 8).equals_ci("\1ACTION ") && realbuf[buf.length() - 1] == '\1')
	{
		realbuf.erase(0, 8);
		realbuf.erase(realbuf.length() - 1);
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
		if (ci->botflags.HasFlag(BS_KICK_BOLDS) && realbuf.find(2) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_BOLDS);
			bot_kick(ci, u, BOT_REASON_BOLD);
			return;
		}

		/* Color kicker */
		if (ci->botflags.HasFlag(BS_KICK_COLORS) && realbuf.find(3) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_COLORS);
			bot_kick(ci, u, BOT_REASON_COLOR);
			return;
		}

		/* Reverses kicker */
		if (ci->botflags.HasFlag(BS_KICK_REVERSES) && realbuf.find(22) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_REVERSES);
			bot_kick(ci, u, BOT_REASON_REVERSE);
			return;
		}

		/* Italics kicker */
		if (ci->botflags.HasFlag(BS_KICK_ITALICS) && realbuf.find(29) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_ITALICS);
			bot_kick(ci, u, BOT_REASON_ITALIC);
			return;
		}

		/* Underlines kicker */
		if (ci->botflags.HasFlag(BS_KICK_UNDERLINES) && realbuf.find(31) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_UNDERLINES);
			bot_kick(ci, u, BOT_REASON_UNDERLINE);
			return;
		}

		/* Caps kicker */
		if (ci->botflags.HasFlag(BS_KICK_CAPS) && realbuf.length() >= ci->capsmin)
		{
			int i = 0, l = 0;

			for (unsigned j = 0, end = realbuf.length(); j < end; ++j)
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
			Anope::string nbuf = normalizeBuffer(realbuf);

			for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				BadWord *bw = ci->GetBadWord(i);

				if (bw->type == BW_ANY && ((Config->BSCaseSensitive && nbuf.find(bw->word) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(bw->word) != Anope::string::npos)))
					mustkick = true;
				else if (bw->type == BW_SINGLE)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && bw->word.equals_cs(nbuf)) || (!Config->BSCaseSensitive && bw->word.equals_ci(nbuf)))
						mustkick = true;
					else if (nbuf.find(' ') == len && ((Config->BSCaseSensitive && bw->word.equals_cs(nbuf)) || (!Config->BSCaseSensitive && bw->word.equals_ci(nbuf))))
						mustkick = true;
					else
					{
						if (nbuf.rfind(' ') == nbuf.length() - len - 1 && ((Config->BSCaseSensitive && nbuf.find(bw->word) == nbuf.length() - len) || (!Config->BSCaseSensitive && nbuf.find_ci(bw->word) == nbuf.length() - len)))
							mustkick = true;
						else
						{
							Anope::string wordbuf = " " + bw->word + " ";

							if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
								mustkick = true;
						}
					}
				}
				else if (bw->type == BW_START)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && nbuf.substr(0, len).equals_cs(bw->word)) || (!Config->BSCaseSensitive && nbuf.substr(0, len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = " " + bw->word;

						if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}
				else if (bw->type == BW_END)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && nbuf.substr(nbuf.length() - len).equals_cs(bw->word)) || (!Config->BSCaseSensitive && nbuf.substr(nbuf.length() - len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = bw->word + " ";

						if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}

				if (mustkick)
				{
					check_ban(ci, u, TTB_BADWORDS);
					if (Config->BSGentleBWReason)
						bot_kick(ci, u, BOT_REASON_BADWORD_GENTLE);
					else
						bot_kick(ci, u, BOT_REASON_BADWORD, bw->word.c_str());

					return;
				}
			}
		}

		/* Flood kicker */
		if (ci->botflags.HasFlag(BS_KICK_FLOOD))
		{
			UserData *ud = get_user_data(ci->c, u);
			if (!ud)
				return;

			if (Anope::CurTime - ud->last_start > ci->floodsecs)
			{
				ud->last_start = Anope::CurTime;
				ud->lines = 0;
			}

			++ud->lines;
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

			if (!ud->lastline.empty() && !ud->lastline.equals_ci(buf))
			{
				ud->lastline = buf;
				ud->times = 0;
			}
			else
			{
				if (ud->lastline.empty())
					ud->lastline = buf;
				++ud->times;
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
	if (get_ignore(u->nick))
		return;

	/* Fantaisist commands */
	if (ci->botflags.HasFlag(BS_FANTASY) && buf[0] == Config->BSFantasyCharacter[0] && !was_action)
	{
		spacesepstream sep(buf);
		Anope::string command;

		if (sep.GetToken(command) && command[0] == Config->BSFantasyCharacter[0])
		{
			/* Strip off the fantasy character */
			command.erase(command.begin());

			if (check_access(u, ci, CA_FANTASIA))
			{
				Anope::string message = sep.GetRemaining();

				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreCommandRun, OnPreCommandRun(u, ci->bi, command, message, true));
				if (MOD_RESULT == EVENT_STOP)
					return;

				Command *cmd = FindCommand(ChanServ, command);

				/* Command exists and can be called by fantasy */
				if (cmd && !cmd->HasFlag(CFLAG_DISABLE_FANTASY))
				{
					/* Some commands don't need the channel name added.. eg !help */
					if (!cmd->HasFlag(CFLAG_STRIP_CHANNEL))
						message = ci->name + " " + message;
					message = command + " " + message;

					mod_run_cmd(ChanServ, u, message, true);
				}

				FOREACH_MOD(I_OnBotFantasy, OnBotFantasy(command, u, ci, sep.GetRemaining()));
			}
			else
			{
				FOREACH_MOD(I_OnBotNoFantasyAccess, OnBotNoFantasyAccess(command, u, ci, sep.GetRemaining()));
			}
		}
	}
}

/*************************************************************************/

BotInfo *findbot(const Anope::string &nick)
{
	if (isdigit(nick[0]) && ircd->ts6)
		return BotListByUID.find(nick);

	return BotListByNick.find(nick);
}

/*************************************************************************/

/* Returns ban data associated with an user if it exists, allocates it
   otherwise. */

static BanData *get_ban_data(Channel *c, User *u)
{
	if (!c || !u)
		return NULL;

	Anope::string mask = u->GetIdent() + "@" + u->GetDisplayedHost();
	/* XXX This should really be on some sort of timer/garbage collector, and use std::map */
	for (std::list<BanData *>::iterator it = c->bd.begin(), it_end = c->bd.end(), it_next; it != it_end; it = it_next)
	{
		it_next = it;
		++it_next;

		if (Anope::CurTime - (*it)->last_use > Config->BSKeepData)
		{
			delete *it;
			c->bd.erase(it);
			continue;
		}
		if ((*it)->mask.equals_ci(mask))
		{
			(*it)->last_use = Anope::CurTime;
			return *it;
		}
	}

	/* If we fall here it is that we haven't found the record */
	BanData *bd = new BanData();
	bd->mask = mask;
	bd->last_use = Anope::CurTime;
	for (int x = 0; x < TTB_SIZE; ++x)
		bd->ttb[x] = 0;

	c->bd.push_front(bd);

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

	for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
	{
		UserContainer *uc = *it;

		if (uc->user == u)
		{
			/* Checks whether data is obsolete */
			if (Anope::CurTime - uc->ud.last_use > Config->BSKeepData)
			{
				/* We should not free and realloc, but reset to 0
				   instead. */
				uc->ud.Clear();
				uc->ud.last_use = Anope::CurTime;
			}

			return &uc->ud;
		}
	}

	return NULL;
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

	++bd->ttb[ttbtype];
	if (ci->ttb[ttbtype] && bd->ttb[ttbtype] >= ci->ttb[ttbtype])
	{
		/* Should not use == here because bd->ttb[ttbtype] could possibly be > ci->ttb[ttbtype]
		 * if the TTB was changed after it was not set (0) before and the user had already been
		 * kicked a few times. Bug #1056 - Adam */
		Anope::string mask;

		bd->ttb[ttbtype] = 0;

		get_idealban(ci, u, mask);

		if (ci->c)
			ci->c->SetMode(NULL, CMODE_BAN, mask);
		FOREACH_MOD(I_OnBotBan, OnBotBan(u, ci, mask));
	}
}

/*************************************************************************/

/* This makes a bot kick an user. Works somewhat like notice_lang in fact ;) */

static void bot_kick(ChannelInfo *ci, User *u, LanguageString message, ...)
{
	va_list args;
	char buf[1024];

	if (!ci || !ci->bi || !ci->c || !u)
		return;

	Anope::string fmt = GetString(u, message);
	va_start(args, message);
	if (fmt.empty())
		return;
	vsnprintf(buf, sizeof(buf), fmt.c_str(), args);
	va_end(args);

	ci->c->Kick(ci->bi, u, "%s", buf);
}

/*************************************************************************/

/* Makes a simple ban and kicks the target */

void bot_raw_ban(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason)
{
	Anope::string mask;
	User *u = finduser(nick);

	if (!u)
		return;

	if (ModeManager::FindUserModeByName(UMODE_PROTECTED) && u->IsProtected() && requester != u)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester, ACCESS_DENIED).c_str());
		return;
	}

	if (ci->HasFlag(CI_PEACE) && !requester->nick.equals_ci(nick) && get_access(u, ci) >= get_access(requester, ci))
		return;

	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted(ci, u) == 1)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester, BOT_EXCEPT).c_str());
		return;
	}

	get_idealban(ci, u, mask);

	ci->c->SetMode(NULL, CMODE_BAN, mask);

	/* Check if we need to do a signkick or not -GD */
	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
		ci->c->Kick(ci->bi, u, "%s (%s)", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str());
}

/*************************************************************************/

/* Makes a kick with a "dynamic" reason ;) */

void bot_raw_kick(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason)
{
	User *u = finduser(nick);

	if (!u || !ci->c->FindUser(u))
		return;

	if (ModeManager::FindUserModeByName(UMODE_PROTECTED) && u->IsProtected() && requester != u)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester, ACCESS_DENIED).c_str());
		return;
	}

	if (ci->HasFlag(CI_PEACE) && !requester->nick.equals_ci(nick) && get_access(u, ci) >= get_access(requester, ci))
		return;

	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
		ci->c->Kick(ci->bi, u, "%s (%s)", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str());
}

/*************************************************************************/

/* Makes a mode operation on a channel for a nick */

void bot_raw_mode(User *requester, ChannelInfo *ci, const Anope::string &mode, const Anope::string &nick)
{
	char buf[BUFSIZE] = "";
	User *u;

	u = finduser(nick);

	if (!u || !ci->c->FindUser(u))
		return;

	snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(Anope::CurTime));

	if (ModeManager::FindUserModeByName(UMODE_PROTECTED) && u->IsProtected() && mode[0] == '-' && requester != u)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester, ACCESS_DENIED).c_str());
		return;
	}

	if (mode[0] == '-' && ci->HasFlag(CI_PEACE) && !requester->nick.equals_ci(nick) && get_access(u, ci) >= get_access(requester, ci))
		return;

	ci->c->SetModes(NULL, "%s %s", mode.c_str(), nick.c_str());
}

/*************************************************************************/
/**
 * Normalize buffer stripping control characters and colors
 * @param A string to be parsed for control and color codes
 * @return A string stripped of control and color codes
 */
Anope::string normalizeBuffer(const Anope::string &buf)
{
	Anope::string newbuf;

	for (unsigned i = 0, end = buf.length(); i < end; ++i)
	{
		switch (buf[i])
		{
			/* ctrl char */
			case 1:
				break;
			/* Bold ctrl char */
			case 2:
				break;
			/* Color ctrl char */
			case 3:
				/* If the next character is a digit, its also removed */
				if (isdigit(buf[i + 1]))
				{
					++i;

					/* not the best way to remove colors
					 * which are two digit but no worse then
					 * how the Unreal does with +S - TSL
					 */
					if (isdigit(buf[i + 1]))
						++i;

					/* Check for background color code
					 * and remove it as well
					 */
					if (buf[i + 1] == ',')
					{
						++i;

						if (isdigit(buf[i + 1]))
							++i;
						/* not the best way to remove colors
						 * which are two digit but no worse then
						 * how the Unreal does with +S - TSL
						 */
						if (isdigit(buf[i + 1]))
							++i;
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
			/* Italic ctrl char */
			case 29:
				break;
			/* A valid char gets copied into the new buffer */
			default:
				newbuf += buf[i];
		}
	}

	return newbuf;
}
