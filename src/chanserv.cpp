/* ChanServ functions.
 *
 * (C) 2003-2011 Anope Team
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
#include "chanserv.h"

registered_channel_map RegisteredChannelList;

static int def_levels[][2] = {
	{ CA_AUTOOP,		5 },
	{ CA_AUTOVOICE,		3 },
	{ CA_NOJOIN,		-2 },
	{ CA_INVITE,		5 },
	{ CA_AKICK,			10 },
	{ CA_SET,			ACCESS_QOP },
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
	{ CA_MODE,			ACCESS_FOUNDER },
	{ CA_INFO,			ACCESS_QOP },
	{ CA_AUTOOWNER,		ACCESS_QOP },
	{ CA_OWNER,			ACCESS_FOUNDER },
	{ CA_OWNERME,		ACCESS_QOP },
	{ CA_FOUNDER,		ACCESS_QOP },
	{ -1 }
};

LevelInfo levelinfo[] = {
	{ CA_AUTOHALFOP,	"AUTOHALFOP",		_("Automatic mode +h") },
	{ CA_AUTOOP,		"AUTOOP",		_("Automatic channel operator status") },
	{ CA_AUTOPROTECT,	"AUTOPROTECT",		_("Automatic mode +a") },
	{ CA_AUTOVOICE,		"AUTOVOICE",		_("Automatic mode +v") },
	{ CA_NOJOIN,		"NOJOIN",		_("Not allowed to join channel") },
	{ CA_SIGNKICK,		"SIGNKICK",		_("No signed kick when SIGNKICK LEVEL is used") },
	{ CA_ACCESS_LIST,	"ACC-LIST",		_("Allowed to view the access list") },
	{ CA_ACCESS_CHANGE,	"ACC-CHANGE",		_("Allowed to modify the access list") },
	{ CA_AKICK,		"AKICK",		_("Allowed to use AKICK command") },
	{ CA_SET,		"SET",			_("Allowed to use SET command (not FOUNDER/PASSWORD)"), },
	{ CA_BAN,		"BAN",			_("Allowed to use BAN command") },
	{ CA_BANME,		"BANME",		_("Allowed to ban him/herself") },
	{ CA_GETKEY,		"GETKEY",		_("Allowed to use GETKEY command") },
	{ CA_HALFOP,		"HALFOP",		_("Allowed to (de)halfop him/herself") },
	{ CA_HALFOPME,		"HALFOPME",		_("Allowed to (de)halfop him/herself") },
	{ CA_INFO,		"INFO",			_("Allowed to use INFO command with ALL option") },
	{ CA_KICK,		"KICK",			_("Allowed to use KICK command") },
	{ CA_KICKME,		"KICKME",		_("Allowed to kick him/herself") },
	{ CA_INVITE,		"INVITE",		_("Allowed to use INVITE command") },
	{ CA_OPDEOP,		"OPDEOP",		_("Allowed to use OP/DEOP commands") },
	{ CA_OPDEOPME,		"OPDEOPME",		_("Allowed to (de)op him/herself") },
	{ CA_PROTECT,		"PROTECT",		_("Allowed to use PROTECT/DEPROTECT commands") },
	{ CA_PROTECTME,		"PROTECTME",		_("Allowed to (de)protect him/herself"), },
	{ CA_TOPIC,		"TOPIC",		_("Allowed to use TOPIC command") },
	{ CA_MODE,		"MODE",			_("Allowed to use MODE command") },
	{ CA_UNBAN,		"UNBAN",		_("Allowed to use UNBAN command") },
	{ CA_VOICE,		"VOICE",		_("Allowed to use VOICE/DEVOICE commands") },
	{ CA_VOICEME,		"VOICEME",		_("Allowed to (de)voice him/herself") },
	{ CA_MEMO,		"MEMO",			_("Allowed to list/read channel memos") },
	{ CA_ASSIGN,		"ASSIGN",		_("Allowed to assign/unassign a bot") },
	{ CA_BADWORDS,		"BADWORDS",		_("Allowed to use BADWORDS command") },
	{ CA_FANTASIA,		"FANTASIA",		_("Allowed to use fantaisist commands") },
	{ CA_GREET,		"GREET",		_("Greet message displayed") },
	{ CA_NOKICK,		"NOKICK",		_("Never kicked by the bot's kickers") },
	{ CA_SAY,		"SAY",			_("Allowed to use SAY and ACT commands") },
	{ CA_AUTOOWNER,		"AUTOOWNER",		_("Automatic mode +q") },
	{ CA_OWNER,		"OWNER",		_("Allowed to use OWNER command") },
	{ CA_OWNERME,		"OWNERME",		_("Allowed to (de)owner him/herself") },
	{ CA_FOUNDER,		"FOUNDER",		_("Allowed to issue commands restricted to channel founders") },
	{ -1 }
};
int levelinfo_maxwidth = 0;

/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them.
 */

void check_modes(Channel *c)
{
	if (!c)
	{
		Log() << "check_modes called with NULL values";
		return;
	}

	if (c->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (c->server_modecount >= 3 && c->chanserv_modecount >= 3)
	{
		ircdproto->SendGlobops(NULL, "Warning: unable to set modes on channel %s. Are your servers' U:lines configured correctly?", c->name.c_str());
		Log() << "Bouncy modes on channel " << c->name;
		c->bouncy_modes = 1;
		return;
	}

	if (c->chanserv_modetime != Anope::CurTime)
	{
		c->chanserv_modecount = 0;
		c->chanserv_modetime = Anope::CurTime;
	}
	c->chanserv_modecount++;

	/* Check if the channel is registered; if not remove mode -r */
	ChannelInfo *ci = c->ci;
	if (!ci)
	{
		if (c->HasMode(CMODE_REGISTERED))
			c->RemoveMode(NULL, CMODE_REGISTERED);
		return;
	}

	for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock &ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
		if (!cm)
			continue;

		if (cm->Type == MODE_REGULAR)
		{
			if (!c->HasMode(cm->Name) && ml.set)
				c->SetMode(NULL, cm);
			else if (c->HasMode(cm->Name) && !ml.set)
				c->RemoveMode(NULL, cm);
		}
		else if (cm->Type == MODE_PARAM)
		{
			Anope::string param;
			c->GetParam(cm->Name, param);

			/* If the channel doesnt have the mode, or it does and it isn't set correctly */
			if (ml.set)
			{
				if (!c->HasMode(cm->Name) || (!param.empty() && !ml.param.empty() && !param.equals_cs(ml.param)))
					c->SetMode(NULL, cm, ml.param);
			}
			else
			{
				if (c->HasMode(cm->Name))
					c->RemoveMode(NULL, cm);
			}

		}
		else if (cm->Type == MODE_LIST)
		{
			if (ml.set)
				c->SetMode(NULL, cm, ml.param);
			else
				c->RemoveMode(NULL, cm, ml.param);
		}
	}
}

/*************************************************************************/

ChannelInfo *cs_findchan(const Anope::string &chan)
{
	FOREACH_MOD(I_OnFindChan, OnFindChan(chan));

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

	ChanAccess *u_access = ci->GetAccess(user);
	level = u_access ? u_access->level : 0;
	limit = ci->levels[what];

	// Set should never be disabled, if it is it is db-converter screwup
	// This all needs rewritten anyway...
	if (what == CA_SET && limit == ACCESS_INVALID)
	{
		ci->levels[what] = ACCESS_FOUNDER;
		limit = ACCESS_FOUNDER;
	}

	/* Resetting the last used time */
	if (level > 0)
		ci->last_used = Anope::CurTime;

	/* Superadmin always wins. Always. */
	if (user->isSuperAdmin)
		return what == CA_NOJOIN ? 0 : 1;
	/* If the access of the level we are checking is disabled, they *always* get denied */
	if (limit == ACCESS_INVALID)
		return 0;
	/* If the level of the user is >= the level for "founder" of this channel and "founder" isn't disabled, they can do anything */
	if (ci->levels[CA_FOUNDER] != ACCESS_INVALID && level >= ci->levels[CA_FOUNDER])
		return what == CA_NOJOIN ? 0 : 1;

	if (what == CA_NOJOIN)
		return level <= ci->levels[what];
	else
		return level >= ci->levels[what];
}

/* Reset channel access level values to their default state. */

void reset_levels(ChannelInfo *ci)
{
	int i;

	if (!ci)
	{
		Log() << "reset_levels() called with NULL values";
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

	if (user->Account() && user->Account() == ci->GetFounder())
		return true;

	return false;
}

/*************************************************************************/

void update_cs_lastseen(User *user, ChannelInfo *ci)
{
	ChanAccess *access;

	if (!ci || !user || !user->Account())
		return;

	if (IsFounder(user, ci) || user->IsIdentified() || (user->IsRecognized() && !ci->HasFlag(CI_SECURE)))
		if ((access = ci->GetAccess(user)))
			access->last_seen = Anope::CurTime;
}

/*************************************************************************/

/* Returns the best ban possible for a user depending of the bantype
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

ChanServTimer::ChanServTimer(Channel *chan) : Timer(Config->CSInhabit), c(chan)
{
	if (!chanserv || !c)
		return;
	c->SetFlag(CH_INHABIT);
	if (!c->ci || !c->ci->bi)
		chanserv->Bot()->Join(c);
	else if (!c->FindUser(c->ci->bi))
		c->ci->bi->Join(c);
}

void ChanServTimer::Tick(time_t)
{
	if (!c)
		return;

	c->UnsetFlag(CH_INHABIT);

	if (!c->ci || !c->ci->bi)
	{
		if (chanserv)
			chanserv->Bot()->Part(c);
	}
	else if (c->users.size() == 1 || c->users.size() < Config->BSMinUsers)
		c->ci->bi->Part(c);
}

