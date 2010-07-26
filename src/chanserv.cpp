/* ChanServ functions.
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
#include "language.h"

registered_channel_map RegisteredChannelList;

static int def_levels[][2] = {
	{ CA_AUTOOP,		5 },
	{ CA_AUTOVOICE,		3 },
	{ CA_AUTODEOP,		-1 },
	{ CA_NOJOIN,		-2 },
	{ CA_INVITE,		5 },
	{ CA_AKICK,			10 },
	{ CA_SET,			ACCESS_QOP },
	{ CA_CLEAR,			ACCESS_FOUNDER },
	{ CA_UNBAN,			5 },
	{ CA_OPDEOP,		5 },
	{ CA_ACCESS_LIST,	1 },
	{ CA_ACCESS_CHANGE,	10 },
	{ CA_MEMO,			10 },
	{ CA_ASSIGN,		ACCESS_FOUNDER },
	{ CA_BADWORDS,		10 },
	{ CA_NOKICK,		1 },
	{ CA_FANTASIA,		3 },
	{ CA_SAY,			5 },
	{ CA_GREET,			5 },
	{ CA_VOICEME,		3 },
	{ CA_VOICE,			5 },
	{ CA_GETKEY,		5 },
	{ CA_AUTOHALFOP,	4 },
	{ CA_AUTOPROTECT,	10 },
	{ CA_OPDEOPME,		5 },
	{ CA_HALFOPME,		4 },
	{ CA_HALFOP,		5 },
	{ CA_PROTECTME,		10 },
	{ CA_PROTECT,		ACCESS_QOP },
	{ CA_KICKME,		5 },
	{ CA_KICK,			5 },
	{ CA_SIGNKICK,		ACCESS_FOUNDER },
	{ CA_BANME,			5 },
	{ CA_BAN,			5 },
	{ CA_TOPIC,			ACCESS_FOUNDER },
	{ CA_INFO,			ACCESS_QOP },
	{ CA_AUTOOWNER,		ACCESS_QOP },
	{ CA_OWNER,			ACCESS_FOUNDER },
	{ CA_OWNERME,		ACCESS_QOP },
	{ CA_FOUNDER,		ACCESS_QOP },
	{ -1 }
};

LevelInfo levelinfo[] = {
	{ CA_AUTODEOP,		"AUTODEOP",		CHAN_LEVEL_AUTODEOP },
	{ CA_AUTOHALFOP,	"AUTOHALFOP",	CHAN_LEVEL_AUTOHALFOP },
	{ CA_AUTOOP,		"AUTOOP",		CHAN_LEVEL_AUTOOP },
	{ CA_AUTOPROTECT,	"AUTOPROTECT",	CHAN_LEVEL_AUTOPROTECT },
	{ CA_AUTOVOICE,		"AUTOVOICE",		CHAN_LEVEL_AUTOVOICE },
	{ CA_NOJOIN,		"NOJOIN",		CHAN_LEVEL_NOJOIN },
	{ CA_SIGNKICK,		"SIGNKICK",		CHAN_LEVEL_SIGNKICK },
	{ CA_ACCESS_LIST,	"ACC-LIST",		CHAN_LEVEL_ACCESS_LIST },
	{ CA_ACCESS_CHANGE,	"ACC-CHANGE",	CHAN_LEVEL_ACCESS_CHANGE },
	{ CA_AKICK,			"AKICK",			CHAN_LEVEL_AKICK },
	{ CA_SET,			"SET",			CHAN_LEVEL_SET },
	{ CA_BAN,			"BAN",			CHAN_LEVEL_BAN },
	{ CA_BANME,			"BANME",			CHAN_LEVEL_BANME },
	{ CA_CLEAR,			"CLEAR",			CHAN_LEVEL_CLEAR },
	{ CA_GETKEY,		"GETKEY",		CHAN_LEVEL_GETKEY },
	{ CA_HALFOP,		"HALFOP",		CHAN_LEVEL_HALFOP },
	{ CA_HALFOPME,		"HALFOPME",		CHAN_LEVEL_HALFOPME },
	{ CA_INFO,			"INFO",			CHAN_LEVEL_INFO },
	{ CA_KICK,			"KICK",			CHAN_LEVEL_KICK },
	{ CA_KICKME,		"KICKME",		CHAN_LEVEL_KICKME },
	{ CA_INVITE,		"INVITE",		CHAN_LEVEL_INVITE },
	{ CA_OPDEOP,		"OPDEOP",		CHAN_LEVEL_OPDEOP },
	{ CA_OPDEOPME,		"OPDEOPME",		CHAN_LEVEL_OPDEOPME },
	{ CA_PROTECT,		"PROTECT",		CHAN_LEVEL_PROTECT },
	{ CA_PROTECTME,		"PROTECTME",		CHAN_LEVEL_PROTECTME },
	{ CA_TOPIC,			"TOPIC",			CHAN_LEVEL_TOPIC },
	{ CA_UNBAN,			"UNBAN",			CHAN_LEVEL_UNBAN },
	{ CA_VOICE,			"VOICE",			CHAN_LEVEL_VOICE },
	{ CA_VOICEME,		"VOICEME",		CHAN_LEVEL_VOICEME },
	{ CA_MEMO,			"MEMO",			CHAN_LEVEL_MEMO },
	{ CA_ASSIGN,		"ASSIGN",		CHAN_LEVEL_ASSIGN },
	{ CA_BADWORDS,		"BADWORDS",		CHAN_LEVEL_BADWORDS },
	{ CA_FANTASIA,		"FANTASIA",		CHAN_LEVEL_FANTASIA },
	{ CA_GREET,			"GREET",			CHAN_LEVEL_GREET },
	{ CA_NOKICK,		"NOKICK",		CHAN_LEVEL_NOKICK },
	{ CA_SAY,			"SAY",			CHAN_LEVEL_SAY },
	{ CA_AUTOOWNER,		"AUTOOWNER",		CHAN_LEVEL_AUTOOWNER },
	{ CA_OWNER,			"OWNER",			CHAN_LEVEL_OWNER },
	{ CA_OWNERME,		"OWNERME",		CHAN_LEVEL_OWNERME },
	{ CA_FOUNDER,		"FOUNDER",		CHAN_LEVEL_FOUNDER },
	{ -1 }
};
int levelinfo_maxwidth = 0;

/*************************************************************************/

void moduleAddChanServCmds()
{
	ModuleManager::LoadModuleList(Config.ChanServCoreModules);
}

/*************************************************************************/

/* Returns modes for mlock in a nice way. */

Anope::string get_mlock_modes(ChannelInfo *ci, int complete)
{
	ChannelMode *cm;
	ChannelModeParam *cmp;
	std::map<char, ChannelMode *>::iterator it, it_end;
	Anope::string res, param;

	if (ci->GetMLockCount(true) || ci->GetMLockCount(false))
	{
		if (ci->GetMLockCount(true))
		{
			res += '+';

			for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
			{
				cm = it->second;

				if (ci->HasMLock(cm->Name, true))
					res += it->first;
			}
		}

		if (ci->GetMLockCount(false))
		{
			res += '-';

			for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
			{
				cm = it->second;

				if (ci->HasMLock(cm->Name, false))
					res += it->first;
			}
		}

		if (ci->GetMLockCount(true) && complete)
		{
			for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
			{
				cm = it->second;

				if (cm->Type == MODE_PARAM)
				{
					cmp = dynamic_cast<ChannelModeParam *>(cm);

					ci->GetParam(cmp->Name, param);

					if (!param.empty())
						res += " " + param;
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
	Anope::string param;

	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		++count;
		mem += sizeof(*ci);
		if (!ci->desc.empty())
			mem += ci->desc.length() + 1;
		mem += ci->GetAccessCount() * sizeof(ChanAccess);
		mem += ci->GetAkickCount() * sizeof(AutoKick);

		if (ci->GetParam(CMODE_KEY, param))
			mem += param.length() + 1;

		if (ci->GetParam(CMODE_FLOOD, param))
			mem += param.length() + 1;

		if (ci->GetParam(CMODE_REDIRECT, param))
			mem += param.length() + 1;

		if (!ci->last_topic.empty())
			mem += ci->last_topic.length() + 1;
		if (!ci->entry_message.empty())
			mem += ci->entry_message.length() + 1;
		if (!ci->forbidby.empty())
			mem += ci->forbidby.length() + 1;
		if (!ci->forbidreason.empty())
			mem += ci->forbidreason.length() + 1;
		if (ci->levels)
			mem += sizeof(*ci->levels) * CA_SIZE;
		unsigned memos = ci->memos.memos.size();
		mem += memos * sizeof(Memo);
		for (unsigned j = 0; j < memos; ++j)
			if (!ci->memos.memos[j]->text.empty())
				mem += ci->memos.memos[j]->text.length() + 1;
		if (ci->ttb)
			mem += sizeof(*ci->ttb) * TTB_SIZE;
		mem += ci->GetBadWordCount() * sizeof(BadWord);
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

void chanserv(User *u, const Anope::string &buf)
{
	if (!u || buf.empty())
		return;

	if (buf.substr(0, 6).equals_ci("\1PING ") && buf[buf.length() - 1] == '\1')
	{
		Anope::string command = buf;
		command.erase(command.begin());
		command.erase(command.end());
		ircdproto->SendCTCP(ChanServ, u->nick, "%s", command.c_str());
	}
	else
		mod_run_cmd(ChanServ, u, buf);
}

/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them.
 */

void check_modes(Channel *c)
{
	time_t t = time(NULL);
	ChannelInfo *ci;
	ChannelMode *cm;
	std::map<char, ChannelMode *>::iterator it, it_end;
	Anope::string param, ciparam;

	if (!c)
	{
		Alog(LOG_DEBUG) << "check_modes called with NULL values";
		return;
	}

	if (c->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (c->server_modecount >= 3 && c->chanserv_modecount >= 3)
	{
		ircdproto->SendGlobops(NULL, "Warning: unable to set modes on channel %s. Are your servers' U:lines configured correctly?", c->name.c_str());
		Alog() << Config.s_ChanServ << ": Bouncy modes on channel " << c->name;
		c->bouncy_modes = 1;
		return;
	}

	if (c->chanserv_modetime != t)
	{
		c->chanserv_modecount = 0;
		c->chanserv_modetime = t;
	}
	c->chanserv_modecount++;

	/* Check if the channel is registered; if not remove mode -r */
	if (!(ci = c->ci))
	{
		if (c->HasMode(CMODE_REGISTERED))
			c->RemoveMode(NULL, CMODE_REGISTERED);
		return;
	}

	for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
	{
		cm = it->second;

		/* If this channel does not have the mode and the mode is mlocked */
		if (cm->Type == MODE_REGULAR && !c->HasMode(cm->Name) && ci->HasMLock(cm->Name, true))
		{
			c->SetMode(NULL, cm);

			/* Add the eventual parameter and modify the Channel structure */
			if (cm->Type == MODE_PARAM)
			{
				if (ci->GetParam(cm->Name, ciparam))
					c->SetMode(NULL, cm, ciparam);
			}
			else
				c->SetMode(NULL, cm);
		}
		/* If this is a param mode and its mlocked, check to ensure it is set and set to the correct value */
		else if (cm->Type == MODE_PARAM && ci->HasMLock(cm->Name, true))
		{
			c->GetParam(cm->Name, param);
			ci->GetParam(cm->Name, ciparam);

			/* If the channel doesnt have the mode, or it does and it isn't set correctly */
			if (!c->HasMode(cm->Name) || (!param.empty() && !ciparam.empty() && !param.equals_cs(ciparam)))
				c->SetMode(NULL, cm, ciparam);
		}
	}

	for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
	{
		cm = it->second;

		/* If the channel has the mode */
		if (c->HasMode(cm->Name) && ci->HasMLock(cm->Name, false))
		{
			/* Add the eventual parameter */
			if (cm->Type == MODE_PARAM)
			{
				ChannelModeParam *cmp = dynamic_cast<ChannelModeParam *>(cm);

				if (!cmp->MinusNoArg)
				{
					if (c->GetParam(cmp->Name, param))
						c->RemoveMode(NULL, cm, param);
				}
			}
			else
				c->RemoveMode(NULL, cm);
		}
	}
}

/*************************************************************************/

int check_valid_admin(User *user, Channel *chan, int servermode)
{
	ChannelMode *cm;

	if (!chan || !chan->ci)
		return 1;

	if (!(cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
		return 0;

	/* They will be kicked; no need to deop, no need to update our internal struct too */
	if (chan->ci->HasFlag(CI_FORBIDDEN))
		return 0;

	if (servermode && !check_access(user, chan->ci, CA_AUTOPROTECT))
	{
		notice_lang(Config.s_ChanServ, user, CHAN_IS_REGISTERED, Config.s_ChanServ.c_str());
		chan->RemoveMode(NULL, CMODE_PROTECT, user->nick);
		return 0;
	}

	if (check_access(user, chan->ci, CA_AUTODEOP))
	{
		chan->RemoveMode(NULL, CMODE_PROTECT, user->nick);
		return 0;
	}

	return 1;
}

/*************************************************************************/

/* Check whether a user is allowed to be opped on a channel; if they
 * aren't, deop them.  If serverop is 1, the +o was done by a server.
 * Return 1 if the user is allowed to be opped, 0 otherwise. */

int check_valid_op(User *user, Channel *chan, int servermode)
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

	if (servermode && !check_access(user, chan->ci, CA_AUTOOP))
	{
		notice_lang(Config.s_ChanServ, user, CHAN_IS_REGISTERED, Config.s_ChanServ.c_str());

		if (owner)
			chan->RemoveMode(NULL, CMODE_OWNER, user->nick);
		if (protect)
			chan->RemoveMode(NULL, CMODE_PROTECT, user->nick);
		chan->RemoveMode(NULL, CMODE_OP, user->nick);
		if (halfop && !check_access(user, chan->ci, CA_AUTOHALFOP))
			chan->RemoveMode(NULL, CMODE_HALFOP, user->nick);

		return 0;
	}

	if (check_access(user, chan->ci, CA_AUTODEOP))
	{
		chan->RemoveMode(NULL, CMODE_OP, user->nick);

		if (owner)
			chan->RemoveMode(NULL, CMODE_OWNER, user->nick);
		if (protect)
			chan->RemoveMode(NULL, CMODE_PROTECT, user->nick);
		if (halfop)
			chan->RemoveMode(NULL, CMODE_HALFOP, user->nick);

		return 0;
	}

	return 1;
}

/*************************************************************************/

/* Record the current channel topic in the ChannelInfo structure. */

void record_topic(const Anope::string &chan)
{
	Channel *c;
	ChannelInfo *ci;

	if (readonly)
		return;

	c = findchan(chan);
	if (!c || !(ci = c->ci))
		return;

	ci->last_topic = c->topic;
	ci->last_topic_setter = c->topic_setter;
	ci->last_topic_time = c->topic_time;
}

/*************************************************************************/

/* Restore the topic in a channel when it's created, if we should. */

void restore_topic(const Anope::string &chan)
{
	Channel *c = findchan(chan);
	ChannelInfo *ci;

	if (!c || !(ci = c->ci))
		return;
	/* We can be sure that the topic will be in sync when we return -GD */
	c->topic_sync = 1;
	if (!ci->HasFlag(CI_KEEPTOPIC))
	{
		/* We need to reset the topic here, since it's currently empty and
		 * should be updated with a TOPIC from the IRCd soon. -GD
		 */
		ci->last_topic.clear();
		ci->last_topic_setter = whosends(ci)->nick;
		ci->last_topic_time = time(NULL);
		return;
	}
	if (!ci->last_topic.empty())
	{
		c->topic = ci->last_topic;
		c->topic_setter = ci->last_topic_setter;
		c->topic_time = ci->last_topic_time;
	}
	else
	{
		c->topic.clear();
		c->topic_setter = whosends(ci)->nick;
	}
	if (ircd->join2set && whosends(ci) == ChanServ)
	{
		ChanServ->Join(chan);
		c->SetMode(NULL, CMODE_OP, Config.s_ChanServ);
	}
	ircdproto->SendTopic(whosends(ci), c, c->topic_setter, c->topic);
	if (ircd->join2set && whosends(ci) == ChanServ)
		ChanServ->Part(c);
}

/*************************************************************************/

/* See if the topic is locked on the given channel, and return 1 (and fix
 * the topic) if so. */

int check_topiclock(Channel *c, time_t topic_time)
{
	ChannelInfo *ci;

	if (!c)
	{
		Alog(LOG_DEBUG) << "check_topiclock called with NULL values";
		return 0;
	}

	if (!(ci = c->ci) || !ci->HasFlag(CI_TOPICLOCK))
		return 0;

	if (!ci->last_topic.empty())
	{
		c->topic = ci->last_topic;
		c->topic_setter = ci->last_topic_setter;
	}
	else
	{
		c->topic.clear();
		/* Bot assigned & Symbiosis ON?, the bot will set the topic - doc */
		/* Altough whosends() also checks for Config.BSMinUsers -GD */
		c->topic_setter = whosends(ci)->nick;
	}

	if (ircd->topictsforward)
	{
		/* Because older timestamps are rejected */
		/* Some how the topic_time from do_topic is 0 set it to current + 1 */
		if (!topic_time)
			c->topic_time = time(NULL) + 1;
		else
			c->topic_time = topic_time + 1;
	}
	else
	{
		/* If no last topic, we can't use last topic time! - doc */
		if (!ci->last_topic.empty())
			c->topic_time = ci->last_topic_time;
		else
			c->topic_time = time(NULL) + 1;
	}

	if (ircd->join2set && whosends(ci) == ChanServ)
	{
		ChanServ->Join(c);
		c->SetMode(NULL, CMODE_OP, Config.s_ChanServ);
	}

	ircdproto->SendTopic(whosends(ci), c, c->topic_setter, c->topic);

	if (ircd->join2set && whosends(ci) == ChanServ)
		ChanServ->Part(c);
	return 1;
}

/*************************************************************************/

/* Remove all channels which have expired. */

void expire_chans()
{
	if (!Config.CSExpire)
		return;

	time_t now = time(NULL);

	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; )
	{
		ChannelInfo *ci = it->second;
		++it;

		if (!ci->c && now - ci->last_used >= Config.CSExpire && !ci->HasFlag(CI_FORBIDDEN) && !ci->HasFlag(CI_NO_EXPIRE) && !ci->HasFlag(CI_SUSPENDED))
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreChanExpire, OnPreChanExpire(ci));
			if (MOD_RESULT == EVENT_STOP)
				continue;

			Anope::string chname = ci->name;
			Alog() << "Expiring channel " << ci->name << " (founder: " << (ci->founder ? ci->founder->display : "(none)") << " )";
			delete ci;
			FOREACH_MOD(I_OnChanExpire, OnChanExpire(chname));
		}
	}
}

/*************************************************************************/

// XXX this is slightly inefficient
void cs_remove_nick(const NickCore *nc)
{
	int j;
	ChanAccess *ca;
	AutoKick *akick;

	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->founder == nc)
		{
			if (ci->successor)
			{
				NickCore *nc2 = ci->successor;
				if (!nc2->IsServicesOper() && Config.CSMaxReg && nc2->channelcount >= Config.CSMaxReg)
				{
					Alog() << Config.s_ChanServ << ": Successor (" << nc2->display << " ) of " << ci->name << " owns too many channels, deleting channel",
					delete ci;
					continue;
				}
				else
				{
					Alog() << Config.s_ChanServ << ": Transferring foundership of " << ci->name << " from deleted nick " << nc->display << " to successor " << nc2->display;
					ci->founder = nc2;
					ci->successor = NULL;
					++nc2->channelcount;
				}
			}
			else
			{
				Alog() << Config.s_ChanServ << ": Deleting channel " << ci->name << "owned by deleted nick " << nc->display;

				if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
				{
					/* Maybe move this to delchan() ? */
					if (ci->c && ci->c->HasMode(CMODE_REGISTERED))
						ci->c->RemoveMode(NULL, CMODE_REGISTERED);
				}

				delete ci;
				continue;
			}
		}

		if (ci->successor == nc)
			ci->successor = NULL;

		for (j = ci->GetAccessCount(); j > 0; --j)
		{
			ca = ci->GetAccess(j - 1);

			if (ca->nc == nc)
				ci->EraseAccess(j - 1);
		}

		for (j = ci->GetAkickCount(); j > 0; --j)
		{
			akick = ci->GetAkick(j - 1);
			if (akick->HasFlag(AK_ISNICK) && akick->nc == nc)
				ci->EraseAkick(j - 1);
		}
	}
}

/*************************************************************************/

ChannelInfo *cs_findchan(const Anope::string &chan)
{
	registered_channel_map::const_iterator it = RegisteredChannelList.find(chan);

	if (it != RegisteredChannelList.end())
		return it->second;
	return NULL;
}

/*************************************************************************/

/* Return 1 if the user's access level on the given channel falls into the
 * given category, 0 otherwise.  Note that this may seem slightly confusing
 * in some cases: for example, check_access(..., CA_NOJOIN) returns true if
 * the user does _not_ have access to the channel (i.e. matches the NOJOIN
 * criterion). */

int check_access(User *user, ChannelInfo *ci, int what)
{
	int level, limit;

	if (!user || !ci)
		return 0;

	level = get_access(user, ci);
	limit = ci->levels[what];

	/* Resetting the last used time */
	if (level > 0)
		ci->last_used = time(NULL);

	/* Superadmin always wins. Always. */
	if (user->isSuperAdmin)
		return what == CA_AUTODEOP || what == CA_NOJOIN ? 0 : 1;
	/* If the access of the level we are checking is disabled, they *always* get denied */
	if (limit == ACCESS_INVALID)
		return 0;
	/* If the level of the user is >= the level for "founder" of this channel and "founder" isn't disabled, they can do anything */
	if (ci->levels[CA_FOUNDER] != ACCESS_INVALID && level >= ci->levels[CA_FOUNDER])
		return what == CA_AUTODEOP || what == CA_NOJOIN ? 0 : 1;

	/* Hacks to make flags work */
	if (what == CA_AUTODEOP && ci->HasFlag(CI_SECUREOPS) && !level)
		return 1;

	if (what == CA_AUTODEOP || what == CA_NOJOIN)
		return level <= ci->levels[what];
	else
		return level >= ci->levels[what];
}

/*************************************************************************/
/*********************** ChanServ private routines ***********************/
/*************************************************************************/

/* Reset channel access level values to their default state. */

void reset_levels(ChannelInfo *ci)
{
	int i;

	if (!ci)
	{
		Alog(LOG_DEBUG) << "reset_levels() called with NULL values";
		return;
	}

	if (ci->levels)
		delete [] ci->levels;
	ci->levels = new int16[CA_SIZE];
	for (i = 0; def_levels[i][0] >= 0; ++i)
		ci->levels[def_levels[i][0]] = def_levels[i][1];
}

/*************************************************************************/

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
bool IsFounder(User *user, ChannelInfo *ci)
{
	if (!user || !ci)
		return false;

	if (user->isSuperAdmin)
		return true;

	if (user->Account() && user->Account() == ci->founder)
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

	if (user->IsIdentified())
	{
		access = ci->GetAccess(user->Account());
		if (access)
			return access->level;
	}
	else
	{
		NickAlias *na = findnick(user->nick);
		if (na)
			access = ci->GetAccess(na->nc);
		if (access && user->IsRecognized() && !ci->HasFlag(CI_SECURE))
			return access->level;
	}

	return 0;
}

/*************************************************************************/

void update_cs_lastseen(User *user, ChannelInfo *ci)
{
	ChanAccess *access;

	if (!ci || !user || !user->Account())
		return;

	if (IsFounder(user, ci) || user->IsIdentified() || (user->IsRecognized() && !ci->HasFlag(CI_SECURE)))
		if ((access = ci->GetAccess(user->Account())))
			access->last_seen = time(NULL);
}

/*************************************************************************/

/* Returns the best ban possible for an user depending of the bantype
   value. */

int get_idealban(ChannelInfo *ci, User *u, Anope::string &ret)
{
	Anope::string mask;

	if (!ci || !u)
		return 0;

	Anope::string vident = u->GetIdent();

	switch (ci->bantype)
	{
		case 0:
			ret = "*!" + vident + "@" + u->GetDisplayedHost();
			return 1;
		case 1:
			if (vident[0] == '~')
				ret = "*!*" + vident + "@" + u->GetDisplayedHost();
			else
				ret = "*!" + vident + "@" + u->GetDisplayedHost();

			return 1;
		case 2:
			ret = "*!*@" + u->GetDisplayedHost();
			return 1;
		case 3:
			mask = create_mask(u);
			ret = "*!" + mask;
			return 1;

		default:
			return 0;
	}
}

/*************************************************************************/

int get_access_level(ChannelInfo *ci, NickCore *nc)
{
	if (!ci || !nc)
		return 0;

	if (nc == ci->founder)
		return ACCESS_FOUNDER;

	ChanAccess *access = ci->GetAccess(nc);

	if (!access)
		return 0;
	else
		return access->level;
}

int get_access_level(ChannelInfo *ci, NickAlias *na)
{
	if (!na)
		return 0;

	return get_access_level(ci, na->nc);
}

Anope::string get_xop_level(int level)
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

/* Is the mask stuck? */

AutoKick *is_stuck(ChannelInfo *ci, const Anope::string &mask)
{
	if (!ci)
		return NULL;

	for (unsigned i = 0, akicks = ci->GetAkickCount(); i < akicks; ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK))
			continue;

		if (Anope::Match(akick->mask, mask))
			return akick;

		if (ircd->reversekickcheck)
			if (Anope::Match(mask, akick->mask))
				return akick;
	}

	return NULL;
}

/* Ban the stuck mask in a safe manner. */

void stick_mask(ChannelInfo *ci, AutoKick *akick)
{
	Entry *ban;

	if (!ci)
		return;

	if (ci->c->bans && ci->c->bans->entries)
	{
		for (ban = ci->c->bans->entries; ban; ban = ban->next)
		{
			/* If akick is already covered by a wider ban.
			   Example: c->bans[i] = *!*@*.org and akick->u.mask = *!*@*.epona.org */
			if (entry_match_mask(ban, akick->mask, 0))
				return;

			if (ircd->reversekickcheck)
			{
				/* If akick is wider than a ban already in place.
				   Example: c->bans[i] = *!*@irc.epona.org and akick->u.mask = *!*@*.epona.org */
				if (Anope::Match(ban->mask, akick->mask))
					return;
			}
		}
	}

	/* Falling there means set the ban */
	ci->c->SetMode(NULL, CMODE_BAN, akick->mask);
}

/* Ban the stuck mask in a safe manner. */

void stick_all(ChannelInfo *ci)
{
	if (!ci)
		return;

	for (unsigned i = 0, akicks = ci->GetAkickCount(); i < akicks; ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK))
			continue;

		ci->c->SetMode(NULL, CMODE_BAN, akick->mask);
	}
}

ChanServTimer::ChanServTimer(Channel *chan) : Timer(Config.CSInhabit), c(chan)
{
	if (c->ci)
		c->ci->SetFlag(CI_INHABIT);
}

void ChanServTimer::Tick(time_t)
{
	if (!c->ci)
		return;

	c->ci->UnsetFlag(CI_INHABIT);

	/* If the channel has users again, don't part it and halt */
	if (!c->users.empty())
		return;

	ChanServ->Part(c);

	/* Now delete the channel as it is empty */
	if (!c->HasFlag(CH_PERSIST) && !c->ci->HasFlag(CI_PERSIST))
		delete c;
}
