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
	{ CA_SET,	 		ACCESS_FOUNDER },
	{ CA_CLEAR,   		ACCESS_FOUNDER },
	{ CA_UNBAN,					  5 },
	{ CA_OPDEOP,					 5 },
	{ CA_ACCESS_LIST,				1 },
	{ CA_ACCESS_CHANGE,			 10 },
	{ CA_MEMO,					  10 },
	{ CA_ASSIGN,  		ACCESS_FOUNDER },
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
	{ CA_PROTECT,  		ACCESS_FOUNDER },
	{ CA_KICKME,			   		 5 },
	{ CA_KICK,					   5 },
	{ CA_SIGNKICK, 		ACCESS_FOUNDER },
	{ CA_BANME,					  5 },
	{ CA_BAN,						5 },
	{ CA_TOPIC,		 ACCESS_FOUNDER },
	{ CA_INFO,		  ACCESS_FOUNDER },
	{ CA_AUTOOWNER,		ACCESS_FOUNDER },
	{ CA_OWNER,		ACCESS_FOUNDER },
	{ CA_OWNERME,		ACCESS_FOUNDER },
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
	ModuleManager::LoadModuleList(Config.ChanServCoreModules);
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

			if (ci->c)
				ircdproto->SendPart(findbot(Config.s_ChanServ), ci->c, NULL);
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
					cmp = dynamic_cast<ChannelModeParam *>(cm);

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
			mem += ci->GetBadWordCount() * sizeof(BadWord);
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
		ircdproto->SendCTCP(findbot(Config.s_ChanServ), u->nick.c_str(), "PING %s", s);
	} else {
		mod_run_cmd(Config.s_ChanServ, u, CHANSERV, cmd);
	}
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
	ChannelModeParam *cmp;
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
		ircdproto->SendGlobops(NULL, "Warning: unable to set modes on channel %s. Are your servers' U:lines configured correctly?", c->name.c_str());
		alog("%s: Bouncy modes on channel %s", Config.s_ChanServ, c->name.c_str());
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
		{
			c->RemoveMode(NULL, CMODE_REGISTERED);
		}
		return;
	}

	for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
	{
		cm = it->second;

		/* If this channel does not have the mode and the mode is mlocked */
		if (cm->Type == MODE_REGULAR && !c->HasMode(cm->Name) && ci->HasMLock(cm->Name, true))
		{
			c->SetMode(NULL, cm);

			/* Add the eventual parameter and modify the Channel structure */
			if (cm->Type == MODE_PARAM)
			{
				if (ci->GetParam(cmp->Name, &param))
					c->SetMode(NULL, cm, param);
			}
			else
				c->SetMode(NULL, cm);
		}
		/* If this is a param mode and its mlocked, check to ensure it is set and set to the correct value */
		else if (cm->Type == MODE_PARAM && ci->HasMLock(cm->Name, true))
		{
			c->GetParam(cm->Name, &param);
			ci->GetParam(cm->Name, &ciparam);

			/* If the channel doesnt have the mode, or it does and it isn't set correctly */
			if (!c->HasMode(cm->Name) || (!param.empty() && !ciparam.empty() && param != ciparam))
			{
				c->SetMode(NULL, cm, ciparam);
			}
		}
	}

	for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
	{
		cm = it->second;

		/* If the channel has the mode */
		if (c->HasMode(cm->Name) && ci->HasMLock(cm->Name, false))
		{
			/* Add the eventual parameter */
			if (cm->Type == MODE_PARAM)
			{
				cmp = dynamic_cast<ChannelModeParam *>(cm);

				if (!cmp->MinusNoArg)
				{
					if (c->GetParam(cmp->Name, &param))
						c->RemoveMode(NULL, cm, param);
				}
			}
			else
				c->RemoveMode(NULL, cm);
		}
	}
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

	if (servermode && !check_access(user, chan->ci, CA_AUTOPROTECT))
	{
		notice_lang(Config.s_ChanServ, user, CHAN_IS_REGISTERED, Config.s_ChanServ);
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

	if (servermode && !check_access(user, chan->ci, CA_AUTOOP))
	{
		notice_lang(Config.s_ChanServ, user, CHAN_IS_REGISTERED, Config.s_ChanServ);

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

	if (check_access(user, ci, CA_AUTOOP))
	{
		ci->c->SetMode(NULL, CMODE_OP, user->nick);	
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

	if (check_access(user, ci, CA_AUTOVOICE))
	{
		ci->c->SetMode(NULL, CMODE_VOICE, user->nick);
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

	if (check_access(user, ci, CA_AUTOHALFOP))
	{
		ci->c->SetMode(NULL, CMODE_HALFOP, user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_owner(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || !ci->c || ci->HasFlag(CI_FORBIDDEN) || *chan == '+')
		return 0;

	if ((ci->HasFlag(CI_SECUREFOUNDER) && IsRealFounder(user, ci))
		|| (!ci->HasFlag(CI_SECUREFOUNDER) && IsFounder(user, ci))) {
		ci->c->SetMode(NULL, CMODE_OP, user->nick);
		ci->c->SetMode(NULL, CMODE_OWNER, user->nick);
		return 1;
	}

	return 0;
}

/*************************************************************************/

int check_should_protect(User * user, char *chan)
{
	ChannelInfo *ci = cs_findchan(chan);

	if (!ci || !ci->c || ci->HasFlag(CI_FORBIDDEN) || *chan == '+')
		return 0;

	if (check_access(user, ci, CA_AUTOPROTECT))
	{
		ci->c->SetMode(NULL, CMODE_OWNER, user->nick);
		ci->c->SetMode(NULL, CMODE_PROTECT, user->nick);
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

		if (!akick->InUse)
			continue;

		if ((akick->HasFlag(AK_ISNICK) && akick->nc == nc)
			|| (!akick->HasFlag(AK_ISNICK)
			&& match_usermask(akick->mask.c_str(), user)))
			{
			if (debug >= 2)
				alog("debug: %s matched akick %s", user->nick.c_str(), akick->HasFlag(AK_ISNICK) ? akick->nc->display : akick->mask.c_str());
			if (akick->HasFlag(AK_ISNICK))
				get_idealban(ci, user, mask, sizeof(mask));
			else
				strlcpy(mask, akick->mask.c_str(), sizeof(mask));
			reason = !akick->reason.empty() ? akick->reason.c_str() : Config.CSAutokickReason;
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
		alog("debug: channel: AutoKicking %s!%s@%s from %s", user->nick.c_str(),
			 user->GetIdent().c_str(), user->host, chan);

	/* Remember that the user has not been added to our channel user list
	 * yet, so we check whether the channel does not exist OR has no user
	 * on it (before SJOIN would have created the channel structure, while
	 * JOIN would not). */
	/* Don't check for CI_INHABIT before for the Channel record cos else
	 * c may be NULL even if it exists */
	if ((!(c = findchan(chan)) || c->usercount == 0) && !ci->HasFlag(CI_INHABIT))
	{
		ircdproto->SendJoin(findbot(Config.s_ChanServ), chan, (c ? c->creation_time : chants));
		/*
		 * If channel was forbidden, etc, set it +si to prevent rejoin
		 */
		if (set_modes)
		{
			c->SetMode(NULL, CMODE_NOEXTERNAL);
			c->SetMode(NULL, CMODE_TOPIC);
			c->SetMode(NULL, CMODE_SECRET);
			c->SetMode(NULL, CMODE_INVITE);
		}

		t = new ChanServTimer(Config.CSInhabit, chan);
		ci->SetFlag(CI_INHABIT);
	}

	if (c)
	{
		c->SetMode(NULL, CMODE_BAN, mask);
		ircdproto->SendKick(whosends(ci), c, user, "%s", reason);
	}

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

	ci->last_topic_setter = c->topic_setter;
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
		ci->last_topic_setter = whosends(ci)->nick;
		ci->last_topic_time = time(NULL);
		return;
	}
	if (c->topic)
		delete [] c->topic;
	if (ci->last_topic) {
		c->topic = sstrdup(ci->last_topic);
		c->topic_setter = ci->last_topic_setter;
		c->topic_time = ci->last_topic_time;
	} else {
		c->topic = NULL;
		c->topic_setter = whosends(ci)->nick;
	}
	if (ircd->join2set) {
		if (whosends(ci) == findbot(Config.s_ChanServ)) {
			ircdproto->SendJoin(findbot(Config.s_ChanServ), chan, c->creation_time);
			c->SetMode(NULL, CMODE_OP, Config.s_ChanServ);
		}
	}
	ircdproto->SendTopic(whosends(ci), c, c->topic_setter.c_str(), c->topic ? c->topic : "");
	if (ircd->join2set) {
		if (whosends(ci) == findbot(Config.s_ChanServ)) {
			ircdproto->SendPart(findbot(Config.s_ChanServ), c, NULL);
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
		c->topic_setter = ci->last_topic_setter;
	} else {
		c->topic = NULL;
		/* Bot assigned & Symbiosis ON?, the bot will set the topic - doc */
		/* Altough whosends() also checks for Config.BSMinUsers -GD */
		c->topic_setter = whosends(ci)->nick;
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
		if (whosends(ci) == findbot(Config.s_ChanServ)) {
			ircdproto->SendJoin(findbot(Config.s_ChanServ), c->name.c_str(), c->creation_time);
			c->SetMode(NULL, CMODE_OP, Config.s_ChanServ);
		}
	}

	ircdproto->SendTopic(whosends(ci), c, c->topic_setter.c_str(), c->topic ? c->topic : "");

	if (ircd->join2set) {
		if (whosends(ci) == findbot(Config.s_ChanServ)) {
			ircdproto->SendPart(findbot(Config.s_ChanServ), c, NULL);
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

	if (!Config.CSExpire)
		return;

	for (i = 0; i < 256; i++) {
		for (ci = chanlists[i]; ci; ci = next) {
			next = ci->next;
			if (!ci->c && now - ci->last_used >= Config.CSExpire && !ci->HasFlag(CI_FORBIDDEN) && !ci->HasFlag(CI_NO_EXPIRE) && !ci->HasFlag(CI_SUSPENDED))
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreChanExpire, OnPreChanExpire(ci));
				if (MOD_RESULT == EVENT_STOP)
					continue;

				char *chname = sstrdup(ci->name.c_str());
				alog("Expiring channel %s (founder: %s)", ci->name.c_str(),
					 (ci->founder ? ci->founder->display : "(none)"));
				delete ci;
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
					if (!nc2->IsServicesOper() && Config.CSMaxReg && nc2->channelcount >= Config.CSMaxReg) {
						alog("%s: Successor (%s) of %s owns too many channels, " "deleting channel", Config.s_ChanServ, nc2->display, ci->name.c_str());
						delete ci;
						continue;
					} else {
						alog("%s: Transferring foundership of %s from deleted " "nick %s to successor %s", Config.s_ChanServ, ci->name.c_str(), nc->display, nc2->display);
						ci->founder = nc2;
						ci->successor = NULL;
						nc2->channelcount++;
					}
				} else {
					alog("%s: Deleting channel %s owned by deleted nick %s", Config.s_ChanServ, ci->name.c_str(), nc->display);

					if ((ModeManager::FindChannelModeByName(CMODE_REGISTERED)))
					{
						/* Maybe move this to delchan() ? */
						if (ci->c && ci->c->HasMode(CMODE_REGISTERED))
						{
							ci->c->RemoveMode(NULL, CMODE_REGISTERED);
						}
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

				if (ca->in_use && ca->nc == nc)
					ci->EraseAccess(j - 1);
			}

			for (j = ci->GetAkickCount(); j > 0; --j)
			{
				akick = ci->GetAkick(j - 1);
				if (akick->InUse && akick->HasFlag(AK_ISNICK) && akick->nc == nc)
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
		if (stricmp(ci->name.c_str(), chan) == 0)
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

	if (limit == ACCESS_INVALID)
		return 0;
	if (what == ACCESS_FOUNDER)
		return IsFounder(user, ci);
	if (level >= ACCESS_FOUNDER)
		return (what == CA_AUTODEOP || what == CA_NOJOIN) ? 0 : 1;
	/* Hacks to make flags work */
	if (what == CA_AUTODEOP && (ci->HasFlag(CI_SECUREOPS)) && level == 0)
		return 1;
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

	if (!ci) {
		if (debug) {
			alog("debug: alpha_insert_chan called with NULL values");
		}
		return;
	}

	const char *chan = ci->name.c_str();

	for (prev = NULL, ptr = chanlists[static_cast<unsigned char>(tolower(chan[1]))];
		 ptr != NULL && stricmp(ptr->name.c_str(), chan) < 0;
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

		if (!akick->InUse || akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK))
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
	ci->c->SetMode(NULL, CMODE_BAN, akick->mask.c_str());
}

/* Ban the stuck mask in a safe manner. */

void stick_all(ChannelInfo * ci)
{
	if (!ci)
		return;

	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (!akick->InUse || (akick->HasFlag(AK_ISNICK) || !akick->HasFlag(AK_STUCK)))
			continue;

		ci->c->SetMode(NULL, CMODE_BAN, akick->mask.c_str());
	}
}
