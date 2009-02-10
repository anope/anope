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

void myChanServHelp(User *u);

enum
{
	XOP_AOP,
	XOP_SOP,
	XOP_VOP,
	XOP_HOP,
	XOP_TYPES
};

enum
{
	XOP_DISABLED,
	XOP_NICKS_ONLY,
	XOP_ADDED,
	XOP_MOVED,
	XOP_NO_SUCH_ENTRY,
	XOP_NOT_FOUND,
	XOP_NO_MATCH,
	XOP_DELETED,
	XOP_DELETED_ONE,
	XOP_DELETED_SEVERAL,
	XOP_LIST_EMPTY,
	XOP_LIST_HEADER,
	XOP_CLEAR,
	XOP_MESSAGES
};

int xop_msgs[XOP_TYPES][XOP_MESSAGES] = {
	{CHAN_AOP_DISABLED,
	 CHAN_AOP_NICKS_ONLY,
	 CHAN_AOP_ADDED,
	 CHAN_AOP_MOVED,
	 CHAN_AOP_NO_SUCH_ENTRY,
	 CHAN_AOP_NOT_FOUND,
	 CHAN_AOP_NO_MATCH,
	 CHAN_AOP_DELETED,
	 CHAN_AOP_DELETED_ONE,
	 CHAN_AOP_DELETED_SEVERAL,
	 CHAN_AOP_LIST_EMPTY,
	 CHAN_AOP_LIST_HEADER,
	 CHAN_AOP_CLEAR},
	{CHAN_SOP_DISABLED,
	 CHAN_SOP_NICKS_ONLY,
	 CHAN_SOP_ADDED,
	 CHAN_SOP_MOVED,
	 CHAN_SOP_NO_SUCH_ENTRY,
	 CHAN_SOP_NOT_FOUND,
	 CHAN_SOP_NO_MATCH,
	 CHAN_SOP_DELETED,
	 CHAN_SOP_DELETED_ONE,
	 CHAN_SOP_DELETED_SEVERAL,
	 CHAN_SOP_LIST_EMPTY,
	 CHAN_SOP_LIST_HEADER,
	 CHAN_SOP_CLEAR},
	{CHAN_VOP_DISABLED,
	 CHAN_VOP_NICKS_ONLY,
	 CHAN_VOP_ADDED,
	 CHAN_VOP_MOVED,
	 CHAN_VOP_NO_SUCH_ENTRY,
	 CHAN_VOP_NOT_FOUND,
	 CHAN_VOP_NO_MATCH,
	 CHAN_VOP_DELETED,
	 CHAN_VOP_DELETED_ONE,
	 CHAN_VOP_DELETED_SEVERAL,
	 CHAN_VOP_LIST_EMPTY,
	 CHAN_VOP_LIST_HEADER,
	 CHAN_VOP_CLEAR},
	{CHAN_HOP_DISABLED,
	 CHAN_HOP_NICKS_ONLY,
	 CHAN_HOP_ADDED,
	 CHAN_HOP_MOVED,
	 CHAN_HOP_NO_SUCH_ENTRY,
	 CHAN_HOP_NOT_FOUND,
	 CHAN_HOP_NO_MATCH,
	 CHAN_HOP_DELETED,
	 CHAN_HOP_DELETED_ONE,
	 CHAN_HOP_DELETED_SEVERAL,
	 CHAN_HOP_LIST_EMPTY,
	 CHAN_HOP_LIST_HEADER,
	 CHAN_HOP_CLEAR}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;
		int change = 0;
		char event_access[BUFSIZE];

		if (!nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !is_services_admin(u))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			notice_lang(s_ChanServ, u, messages[XOP_NICKS_ONLY]);
			return MOD_CONT;
		}
		else if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		NickCore *nc = na->nc;
		for (access = ci->access, i = 0; i < ci->accesscount; ++access, ++i)
		{
			if (access->nc == nc)
			{
				/**
				 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
				 **/
				if (access->level >= ulev && !is_services_admin(u))
				{
					notice_lang(s_ChanServ, u, PERMISSION_DENIED);
					return MOD_CONT;
				}
				++change;
				break;
			}
		}

		if (!change)
		{
			if (i < CSAccessMax)
			{
				++ci->accesscount;
				ci->access = static_cast<ChanAccess *>(srealloc(ci->access, sizeof(ChanAccess) * ci->accesscount));
			}
			else
			{
				notice_lang(s_ChanServ, u, CHAN_XOP_REACHED_LIMIT, CSAccessMax);
				return MOD_CONT;
			}

			access = &ci->access[i];
			access->nc = nc;
		}

		access->in_use = 1;
		access->level = xlev;
		access->last_seen = 0;

		alog("%s: %s!%s@%s (level %d) %s access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ulev, change ? "changed" : "set", access->level, na->nick, nc->display, ci->name);

		snprintf(event_access, BUFSIZE, "%d", access->level);

		if (!change)
		{
			send_event(EVENT_ACCESS_ADD, 4, ci->name, u->nick, na->nick, event_access);
			notice_lang(s_ChanServ, u, messages[XOP_ADDED], access->nc->display, ci->name);
		}
		else
		{
			send_event(EVENT_ACCESS_CHANGE, 4, ci->name, u->nick, na->nick, event_access);
			notice_lang(s_ChanServ, u, messages[XOP_MOVED], access->nc->display, ci->name);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;

		int deleted, a, b;

		if (!nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->accesscount)
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !is_services_admin(u))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick))
		{
			int count, last = -1, perm = 0;
			deleted = process_numlist(nick, &count, xop_del_callback, u, ci, &last, &perm, ulev, level);
			if (!deleted)
			{
				if (perm)
					notice_lang(s_ChanServ, u, PERMISSION_DENIED);
				else if (count == 1)
					notice_lang(s_ChanServ, u, messages[XOP_NO_SUCH_ENTRY], last, ci->name);
				else
					notice_lang(s_ChanServ, u, messages[XOP_NO_MATCH], ci->name);
			}
			else if (deleted == 1)
				notice_lang(s_ChanServ, u, messages[XOP_DELETED_ONE], ci->name);
			else
				notice_lang(s_ChanServ, u, messages[XOP_DELETED_SEVERAL], deleted, ci->name);
		}
		else
		{
			NickAlias *na = findnick(nick);
			if (!na)
			{
				notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
				return MOD_CONT;
			}
			nc = na->nc;

			int i;

			for (i = 0; i < ci->accesscount; ++i)
				if (ci->access[i].nc == nc && ci->access[i].level == level)
					break;

			if (i == ci->accesscount)
			{
				notice_lang(s_ChanServ, u, messages[XOP_NOT_FOUND], nick, chan);
				return MOD_CONT;
			}

			access = &ci->access[i];
			if (!is_services_admin(u) && ulev <= access->level)
			{
				deleted = 0;
				notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			}
			else
			{
				notice_lang(s_ChanServ, u, messages[XOP_DELETED], access->nc->display, ci->name);
				access->nc = NULL;
				access->in_use = 0;
				send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick, na->nick);
				deleted = 1;
			}
		}
		if (deleted)
		{
			/* Reordering - DrStein */
			for (b = 0; b < ci->accesscount; ++b)
			{
				if (ci->access[b].in_use)
				{
					for (a = 0; a < ci->accesscount; ++a)
					{
						if (a > b)
							break;
						if (!ci->access[a].in_use)
						{
							ci->access[a].in_use = 1;
							ci->access[a].level = ci->access[b].level;
							ci->access[a].nc = ci->access[b].nc;
							ci->access[a].last_seen = ci->access[b].last_seen;
							ci->access[b].nc = NULL;
							ci->access[b].in_use = 0;
							break;
						}
					}
				}
			}

			/* If the patch provided in bug #706 is applied, this should be placed
			 * before sending the events! */
			/* After reordering only the entries at the end could still be empty.
			 * We ll free the places no longer in use... */
			for (i = ci->accesscount - 1; i >= 0; --i)
			{
				if (ci->access[i].in_use == 1)
					break;

				--ci->accesscount;
			}
			ci->access = static_cast<ChanAccess *>(srealloc(ci->access, sizeof(ChanAccess) * ci->accesscount));
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		int sent_header = 0;
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;

		short ulev = get_access(u, ci);

		if (!is_services_admin(u) && level < ACCESS_AOP)
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->accesscount)
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		if (nick && strspn(nick, "1234567890,-") == strlen(nick))
			process_numlist(nick, NULL, xop_list_callback, u, ci, &sent_header, level, messages[XOP_LIST_HEADER]);
		else
		{
			for (int i = 0; i < ci->accesscount; ++i)
			{
				if (nick && ci->access[i].nc && !match_wild_nocase(nick, ci->access[i].nc->display))
					continue;
				xop_list(u, i, ci, &sent_header, level, message[XOP_LIST_HEADER]);
			}
		}
		if (!sent_header)
			notice_lang(s_ChanServ, u, messages[XOP_NO_MATCH], ci->name);

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<std::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		if (readonly)
		{
			notice_lang(s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->accesscount)
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		if (!is_services_admin(u) && !is_founder(u, ci))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		for (int i = 0; i < ci->accesscount; ++i)
		{
			if (ci->access[i].in_use && ci->access[i].level == level)
			{
				ci->access[i].nc = NULL;
				ci->access[i].in_use = 0;
			}
		}

		send_event(EVENT_ACCESS_CLEAR, 2, ci->name, u->nick);

		notice_lang(s_ChanServ, u, messages[XOP_CLEAR], ci->name);

		return MOD_CONT;
	}
	CommandReturn DoXop(User *u, std::vector<std::string> &params, int level, int *messages)
	{
		const char *chan = params[0].c_str();
		const char *cmd = params[1].c_str();

		ChannelInfo *ci;

		if (!(ci = cs_findchan(chan)))
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (ci->flags & CI_FORBIDDEN)
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
		else if (!(ci->flags & CI_XOP))
			notice_lang(s_ChanServ, u, CHAN_XOP_ACCESS, s_ChanServ);
		else if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params, ci, level, messages);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params, ci, level, messages);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params, ci, level, messages);
		else if (!stricmp(cmd, "CLEAR"))
			return this->DoClear(u, params, ci, level, messages);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}
 public:
	XOPBase(const std::string &command) : Command(command, 2, 3)
	{
	}

	virtual ~XOPBase()
	{
	}

	virtual CommandReturn Execute(User *u, std::vector<std::string> &params) = 0;

	virtual bool OnHelp(User *u, const std::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u) = 0;
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoXop(u, params, ACCESS_AOP, xop_msgs[XOP_AOP]);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_land(s_ChanServ, u, CHAN_HELP_AOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "AOP", CHAN_AOP_SYNTAX);
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP() : XOPBase("HOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoXop(u, params, ACCESS_HOP, xop_msgs[XOP_HOP]);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_land(s_ChanServ, u, CHAN_HELP_HOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "HOP", CHAN_HOP_SYNTAX);
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP() : XOPBase("SOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoXop(u, params, ACCESS_SOP, xop_msgs[XOP_SOP]);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_land(s_ChanServ, u, CHAN_HELP_SOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "SOP", CHAN_SOP_SYNTAX);
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP() : XOPBase("VOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoXop(u, params, ACCESS_VOP, xop_msgs[XOP_VOP]);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_land(s_ChanServ, u, CHAN_HELP_VOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "VOP", CHAN_VOP_SYNTAX);
	}
};

class CSXOP : public Module
{
 public:
	CSXOP(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSAOP(), MOD_UNIQUE);
		if (ircd->halfop)
			this->AddCommand(CHANSERV, new CommandCSHOP(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSSOP(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSVOP(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User *u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SOP);
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_AOP);
	if (ircd->halfop)
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_HOP);
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_VOP);
}

/* `last' is set to the last index this routine was called with
 * `perm' is incremented whenever a permission-denied error occurs
 */
int xop_del(User *u, ChannelInfo *ci, ChanAccess *access, int *perm, int uacc, int xlev)
{
	char *nick = access->nc->display;
	if (!access->in_use || access->level != xlev)
		return 0;
	if (!is_services_admin(u) && uacc <= access->level)
	{
		++(*perm);
		return 0;
	}
	access->nc = NULL;
	access->in_use = 0;
	send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick, nick);
	return 1;
}

int xop_del_callback(User *u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);
	int *perm = va_arg(args, int *);
	int uacc = va_arg(args, int);
	int xlev = va_arg(args, int);

	if (num < 1 || num > ci->accesscount)
		return 0;
	*last = num;

	return xop_del(u, ci, &ci->access[num - 1], perm, uacc, xlev);
}


int xop_list(User *u, int index, ChannelInfo *ci, int *sent_header, int xlev, int xmsg)
{
	ChanAccess *access = &ci->access[index];

	if (!access->in_use || access->level != xlev)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_ChanServ, u, xmsg, ci->name);
		*sent_header = 1;
	}

	notice_lang(s_ChanServ, u, CHAN_XOP_LIST_FORMAT, index + 1, access->nc->display);
	return 1;
}

int xop_list_callback(User *u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
	int xlev = va_arg(args, int);
	int xmsg = va_arg(args, int);

	if (num < 1 || num > ci->accesscount)
		return 0;

	return xop_list(u, num - 1, ci, sent_header, xlev, xmsg);
}

MODULE_INIT("cs_xop", CSXOP)
