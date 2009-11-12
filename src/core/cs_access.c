/* ChanServ core functions
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

static int access_del(User * u, ChannelInfo *ci, ChanAccess * access, int *perm, int uacc)
{
	char *nick;
	if (!access->in_use)
		return 0;
	if (uacc <= access->level && !u->nc->HasPriv("chanserv/access/modify"))
	{
		(*perm)++;
		return 0;
	}
	nick = access->nc->display;
	access->nc = NULL;
	access->in_use = 0;

	FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, nick));

	return 1;
}

static int access_del_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);
	int *perm = va_arg(args, int *);
	int uacc = va_arg(args, int);
	if (num < 1 || num > ci->GetAccessCount())
		return 0;
	*last = num;
	return access_del(u, ci, ci->GetAccess(num - 1), perm, uacc);
}


static int access_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
	ChanAccess *access = ci->GetAccess(index);
	const char *xop;

	if (!access->in_use)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name);
		*sent_header = 1;
	}

	if (ci->HasFlag(CI_XOP))
	{
		xop = get_xop_level(access->level);
		notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_XOP_FORMAT, index + 1, xop, access->nc->display);
	}
	else
		notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_AXS_FORMAT, index + 1, access->level, access->nc->display);
	return 1;
}

static int access_list_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
	if (num < 1 || num > ci->GetAccessCount())
		return 0;
	return access_list(u, num - 1, ci, sent_header);
}

static int access_view(User *u, int index, ChannelInfo *ci, int *sent_header)
{
	ChanAccess *access = ci->GetAccess(index);
	const char *xop;
	char timebuf[64];
	tm tm;

	if (!access || !access->in_use)
		return 0;
	
	if (!*sent_header)
	{
		notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name);
		*sent_header = 1;
	}

	memset(&timebuf, 0, sizeof(timebuf));
	if (ci->c && u->nc && nc_on_chan(ci->c, u->nc))
		sprintf(timebuf, "Now");
	else if (access->last_seen == 0)
		sprintf(timebuf, "Never");
	else
	{
		tm = *localtime(&access->last_seen);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
	}

	if (ci->HasFlag(CI_XOP))
	{
		xop = get_xop_level(access->level);
		notice_lang(s_ChanServ, u, CHAN_ACCESS_VIEW_XOP_FORMAT, index + 1, xop, access->nc->display, access->creator.c_str(), timebuf);
	}
	else
		notice_lang(s_ChanServ, u, CHAN_ACCESS_VIEW_AXS_FORMAT, index + 1, access->level, access->nc->display, access->creator.c_str(), timebuf);

	return 1;
}

static int access_view_callback(User *u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
		return 0;
	return access_view(u, num - 1, ci, sent_header);
}

class CommandCSAccess : public Command
{
 public:
	CommandCSAccess() : Command("ACCESS", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		const char *s = params.size() > 3 ? params[3].c_str() : NULL;

		ChannelInfo *ci = cs_findchan(chan);
		NickAlias *na = NULL;
		NickCore *nc;
		ChanAccess *access;

		unsigned i;
		int level = 0, ulev;
		bool is_list = cmd == "LIST" || cmd == "VIEW";

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || cmd == "CLEAR" ? 0 : (cmd == "DEL" ? (!nick || s) : !s))
			this->OnSyntaxError(u, cmd);
		/* We still allow LIST in xOP mode, but not others */
		else if ((ci->HasFlag(CI_XOP)) && !is_list)
		{
			if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
				notice_lang(s_ChanServ, u, CHAN_ACCESS_XOP_HOP, s_ChanServ);
			else
				notice_lang(s_ChanServ, u, CHAN_ACCESS_XOP, s_ChanServ);
		}
		else if ((
				 (is_list && !check_access(u, ci, CA_ACCESS_LIST) && !u->nc->HasCommand("chanserv/access/list"))
				 ||
				 (!is_list && !check_access(u, ci, CA_ACCESS_CHANGE) && !u->nc->HasPriv("chanserv/access/modify"))
				))
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		else if (cmd == "ADD")
		{
			if (readonly)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
				return MOD_CONT;
			}

			level = atoi(s);
			ulev = get_access(u, ci);

			if (level >= ulev && !u->nc->HasPriv("chanserv/access/modify"))
			{
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			if (!level)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_NONZERO);
				return MOD_CONT;
			}
			else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
				return MOD_CONT;
			}

			na = findnick(nick);
			if (!na)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_NICKS_ONLY);
				return MOD_CONT;
			}
			else if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, nick);
				return MOD_CONT;
			}

			nc = na->nc;
			access = ci->GetAccess(nc);
			if (access)
			{
				/* Don't allow lowering from a level >= ulev */
				if (access->level >= ulev && !u->nc->HasPriv("chanserv/access/modify"))
				{
					notice_lang(s_ChanServ, u, ACCESS_DENIED);
					return MOD_CONT;
				}
				if (access->level == level)
				{
					notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_UNCHANGED, access->nc->display, chan, level);
					return MOD_CONT;
				}
				access->level = level;

				FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nick, level));

				alog("%s: %s!%s@%s (level %d) set access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ulev, access->level, na->nick, nc->display, ci->name);
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_CHANGED, nc->display, chan, level);
				return MOD_CONT;
			}

			if (ci->GetAccessCount() >= CSAccessMax)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_REACHED_LIMIT, CSAccessMax);
				return MOD_CONT;
			}

			std::string usernick = u->nick;
			ci->AddAccess(nc, level, usernick);

			FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, na->nick, level));

			alog("%s: %s!%s@%s (level %d) set access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ulev, level, na->nick, nc->display, ci->name);
			notice_lang(s_ChanServ, u, CHAN_ACCESS_ADDED, nc->display, ci->name, level);
		}
		else if (cmd == "DEL")
		{
			int deleted;
			if (readonly)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
				return MOD_CONT;
			}

			if (!ci->GetAccessCount())
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
				return MOD_CONT;
			}

			/* Special case: is it a number/list?  Only do search if it isn't. */
			if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick))
			{
				int count, last = -1, perm = 0;
				deleted = process_numlist(nick, &count, access_del_callback, u, ci, &last, &perm, get_access(u, ci));
				if (!deleted)
				{
					if (perm)
						notice_lang(s_ChanServ, u, ACCESS_DENIED);
					else if (count == 1)
					{
						last = atoi(nick);
						notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_SUCH_ENTRY, last, ci->name);
					}
					else
						notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name);
				}
				else
				{
					alog("%s: %s!%s@%s (level %d) deleted access of user%s %s on %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, get_access(u, ci), deleted == 1 ? "" : "s", nick, chan);
					if (deleted == 1)
						notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED_ONE, ci->name);
					else
						notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED_SEVERAL, deleted, ci->name);
				}
			}
			else
			{
				na = findnick(nick);
				if (!na)
				{
					notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
					return MOD_CONT;
				}
				nc = na->nc;
				access = ci->GetAccess(nc);
				if (!access)
				{
					notice_lang(s_ChanServ, u, CHAN_ACCESS_NOT_FOUND, nick, chan);
					return MOD_CONT;
				}
				if (get_access(u, ci) <= access->level && !u->nc->HasPriv("chanserv/access/modify"))
				{
					deleted = 0;
					notice_lang(s_ChanServ, u, ACCESS_DENIED);
				}
				else
				{
					notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED, access->nc->display, ci->name);
					alog("%s: %s!%s@%s (level %d) deleted access of %s (group %s) on %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, get_access(u, ci), na->nick, access->nc->display, chan);
					access->nc = NULL;
					access->in_use = 0;
					deleted = 1;
				}
			}

			if (deleted)
			{
				/* We'll free the access entries no longer in use... */
				ci->CleanAccess();

				/* We don't know the nick if someone used numbers, so we trigger the event without
				 * nick param. We just do this once, even if someone enters a range. -Certus */
				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, (na->nick ? na->nick : NULL)));
			}
		}
		else if (cmd == "LIST")
		{
			int sent_header = 0;

			if (!ci->GetAccessCount())
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
				return MOD_CONT;
			}
			if (nick && strspn(nick, "1234567890,-") == strlen(nick))
				process_numlist(nick, NULL, access_list_callback, u, ci, &sent_header);
			else
			{
				for (i = 0; i < ci->GetAccessCount(); i++)
				{
					access = ci->GetAccess(i);
					if (nick && access->nc && !Anope::Match(access->nc->display, nick, false))
						continue;
					access_list(u, i, ci, &sent_header);
				}
			}
			if (!sent_header)
				notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, chan);
			else
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name);
		}
		else if (cmd == "VIEW")
		{
			int sent_header = 0;

			if (!ci->GetAccessCount())
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
				return MOD_CONT;
			}
			if (nick && strspn(nick, "1234567890,-") == strlen(nick))
				process_numlist(nick, NULL, access_view_callback, u, ci, &sent_header);
			else
			{
				for (i = 0; i < ci->GetAccessCount(); ++i)
				{
					access = ci->GetAccess(i);
					if (nick && access->nc && !Anope::Match(access->nc->display, nick, false))
						continue;
					access_view(u, i, ci, &sent_header);
				}
			}
			if (!sent_header)
				notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, chan);
			else
				notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name);
		}
		else if (cmd == "CLEAR")
		{
			if (readonly)
			{
				notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
				return MOD_CONT;
			}

			if (!IsFounder(u, ci) && !u->nc->HasPriv("chanserv/access/modify"))
			{
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

			notice_lang(s_ChanServ, u, CHAN_ACCESS_CLEAR, ci->name);
			alog("%s: %s!%s@%s (level %d) cleared access list on %s",
				 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host,
				 get_access(u, ci), chan);

		}
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_ACCESS);
		notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
	}
};


class CommandCSLevels : public Command
{
 public:
	CommandCSLevels() : Command("LEVELS", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];
		const char *what = params.size() > 2 ? params[2].c_str() : NULL;
		const char *s = params.size() > 3 ? params[3].c_str() : NULL;
		char *error;

		ChannelInfo *ci = cs_findchan(chan);
		int level;
		int i;

		/* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
		 * one; else, we want none.
		 */
		if (cmd == "SET" ? !s : (cmd.substr(0, 3) == "DIS" ? (!what || s) : !!what))
			this->OnSyntaxError(u, cmd);
		else if (ci->HasFlag(CI_XOP))
			notice_lang(s_ChanServ, u, CHAN_LEVELS_XOP);
		else if (!IsFounder(u, ci) && !u->nc->HasPriv("chanserv/access/modify"))
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		else if (cmd == "SET") {
			level = strtol(s, &error, 10);

			if (*error != '\0') {
				this->OnSyntaxError(u, "SET");
				return MOD_CONT;
			}

			if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER) {
				notice_lang(s_ChanServ, u, CHAN_LEVELS_RANGE,
							ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
				return MOD_CONT;
			}

			for (i = 0; levelinfo[i].what >= 0; i++) {
				if (stricmp(levelinfo[i].name, what) == 0) {
					ci->levels[levelinfo[i].what] = level;

					alog("%s: %s!%s@%s set level %s on channel %s to %d",
						 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host,
						 levelinfo[i].name, ci->name, level);
					notice_lang(s_ChanServ, u, CHAN_LEVELS_CHANGED,
								levelinfo[i].name, chan, level);
					return MOD_CONT;
				}
			}

			notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);

		} else if (cmd == "DIS" || cmd == "DISABLE") {
			for (i = 0; levelinfo[i].what >= 0; i++) {
				if (stricmp(levelinfo[i].name, what) == 0) {
					ci->levels[levelinfo[i].what] = ACCESS_INVALID;

					alog("%s: %s!%s@%s disabled level %s on channel %s",
						 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host,
						 levelinfo[i].name, ci->name);
					notice_lang(s_ChanServ, u, CHAN_LEVELS_DISABLED,
								levelinfo[i].name, chan);
					return MOD_CONT;
				}
			}

			notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);
		} else if (cmd == "LIST") {
			notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_HEADER, chan);

			if (!levelinfo_maxwidth) {
				for (i = 0; levelinfo[i].what >= 0; i++) {
					int len = strlen(levelinfo[i].name);
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			}

			for (i = 0; levelinfo[i].what >= 0; i++) {
				int j = ci->levels[levelinfo[i].what];

				if (j == ACCESS_INVALID) {
					j = levelinfo[i].what;

					if (j == CA_AUTOOP || j == CA_AUTODEOP || j == CA_AUTOVOICE
						|| j == CA_NOJOIN) {
						notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED,
									levelinfo_maxwidth, levelinfo[i].name);
					} else {
						notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED,
									levelinfo_maxwidth, levelinfo[i].name);
					}
				} else if (j == ACCESS_FOUNDER) {
					notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_FOUNDER,
								levelinfo_maxwidth, levelinfo[i].name);
				} else {
					notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_NORMAL,
								levelinfo_maxwidth, levelinfo[i].name, j);
				}
			}

		} else if (cmd == "RESET") {
			reset_levels(ci);

			alog("%s: %s!%s@%s reset levels definitions on channel %s",
				 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ci->name);
			notice_lang(s_ChanServ, u, CHAN_LEVELS_RESET, chan);
		} else {
			this->OnSyntaxError(u, "");
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
	}
};


class CSAccess : public Module
{
 public:
	CSAccess(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSAccess());
		this->AddCommand(CHANSERV, new CommandCSLevels());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_ACCESS);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_LEVELS);
	}
};


MODULE_INIT(CSAccess)
