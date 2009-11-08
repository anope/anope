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

int xop_del_callback(User *u, int num, va_list args);
int xop_list_callback(User *u, int num, va_list args);
int xop_list(User *u, int index, ChannelInfo *ci, int *sent_header, int xlev, int xmsg);

enum
{
	XOP_AOP,
	XOP_SOP,
	XOP_VOP,
	XOP_HOP,
	XOP_QOP,
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
	 CHAN_HOP_CLEAR},
	 {CHAN_QOP_DISABLED,
	 CHAN_QOP_NICKS_ONLY,
	 CHAN_QOP_ADDED,
	 CHAN_QOP_MOVED,
	 CHAN_QOP_NO_SUCH_ENTRY,
	 CHAN_QOP_NOT_FOUND,
	 CHAN_QOP_NO_MATCH,
	 CHAN_QOP_DELETED,
	 CHAN_QOP_DELETED_ONE,
	 CHAN_QOP_DELETED_SEVERAL,
	 CHAN_QOP_LIST_EMPTY,
	 CHAN_QOP_LIST_HEADER,
	 CHAN_QOP_CLEAR}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;
		int change = 0;

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

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->nc->HasPriv("chanserv/access/modify"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			notice_lang(s_ChanServ, u, messages[XOP_NICKS_ONLY]);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		NickCore *nc = na->nc;
		access = ci->GetAccess(nc);
		if (access)
		{
			/**
			 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
			 **/
			if (access->level >= ulev && !u->nc->HasPriv("chanserv/access/modify"))
			{
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			++change;
		}

		if (!change && ci->GetAccessCount() >= CSAccessMax)
		{
			notice_lang(s_ChanServ, u, CHAN_XOP_REACHED_LIMIT, CSAccessMax);
			return MOD_CONT;
		}

		if (!change)
		{
			std::string usernick = u->nick;
			ci->AddAccess(nc, level, usernick);
		}
		else
		{
			access->level = level;
			access->last_seen = 0;
			access->creator = u->nick;
		}

		alog("%s: %s!%s@%s (level %d) %s access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ulev, change ? "changed" : "set", level, na->nick, nc->display, ci->name);

		if (!change)
		{
			FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, na->nick, level));
			notice_lang(s_ChanServ, u, messages[XOP_ADDED], nc->display, ci->name);
		}
		else
		{
			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nick, level));
			notice_lang(s_ChanServ, u, messages[XOP_MOVED], nc->display, ci->name);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;

		int deleted;

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

		if (!ci->GetAccessCount())
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->nc->HasPriv("chanserv/access/modify"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
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
					notice_lang(s_ChanServ, u, ACCESS_DENIED);
				else if (count == 1)
				{
					last = atoi(nick);
					notice_lang(s_ChanServ, u, messages[XOP_NO_SUCH_ENTRY], last, ci->name);
				}
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
			NickCore *nc = na->nc;
			access = ci->GetAccess(nc, level);

			if (!access)
			{
				notice_lang(s_ChanServ, u, messages[XOP_NOT_FOUND], nick, ci->name);
				return MOD_CONT;
			}

			if (ulev <= access->level && !u->nc->HasPriv("chanserv/access/modify"))
			{
				deleted = 0;
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			}
			else
			{
				notice_lang(s_ChanServ, u, messages[XOP_DELETED], access->nc->display, ci->name);
				access->nc = NULL;
				access->in_use = 0;

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, na->nick));

				deleted = 1;
			}
		}
		if (deleted)
		{
			/* If the patch provided in bug #706 is applied, this should be placed
			 * before sending the events! */
			/* We'll free the access entries no longer in use... */
			ci->CleanAccess();
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		int sent_header = 0;
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;

		if (level < ACCESS_AOP && !u->nc->HasCommand("chanserv/aop/list"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		if (nick && strspn(nick, "1234567890,-") == strlen(nick))
			process_numlist(nick, NULL, xop_list_callback, u, ci, &sent_header, level, messages[XOP_LIST_HEADER]);
		else
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanAccess *access = ci->GetAccess(i);
				if (nick && access->nc && !Anope::Match(access->nc->display, nick, false))
					continue;
				xop_list(u, i, ci, &sent_header, level, messages[XOP_LIST_HEADER]);
			}
		}
		if (!sent_header)
			notice_lang(s_ChanServ, u, messages[XOP_NO_MATCH], ci->name);

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		if (readonly)
		{
			notice_lang(s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			notice_lang(s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name);
			return MOD_CONT;
		}

		if (!IsFounder(u, ci) && !u->nc->HasPriv("chanserv/access/modify"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanAccess *access = ci->GetAccess(i - 1);
			if (access->in_use && access->level == level)
				ci->EraseAccess(i - 1);
		}

		FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

		notice_lang(s_ChanServ, u, messages[XOP_CLEAR], ci->name);

		return MOD_CONT;
	}
 protected:
	CommandReturn DoXop(User *u, std::vector<ci::string> &params, int level, int *messages)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];

		ChannelInfo *ci = cs_findchan(chan);

		if (!(ci->HasFlag(CI_XOP)))
			notice_lang(s_ChanServ, u, CHAN_XOP_ACCESS, s_ChanServ);
		else if (cmd == "ADD")
			return this->DoAdd(u, params, ci, level, messages);
		else if (cmd == "DEL")
			return this->DoDel(u, params, ci, level, messages);
		else if (cmd == "LIST")
			return this->DoList(u, params, ci, level, messages);
		else if (cmd == "CLEAR")
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

	virtual CommandReturn Execute(User *u, std::vector<ci::string> &params) = 0;

	virtual bool OnHelp(User *u, const ci::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u) = 0;
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP() : XOPBase("QOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_QOP, xop_msgs[XOP_QOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_QOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "QOP", CHAN_QOP_SYNTAX);
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_AOP, xop_msgs[XOP_AOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_AOP);
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

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_HOP, xop_msgs[XOP_HOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_HOP);
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

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_SOP, xop_msgs[XOP_SOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_SOP);
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

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_VOP, xop_msgs[XOP_VOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_VOP);
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

		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			this->AddCommand(CHANSERV, new CommandCSQOP());
		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
			this->AddCommand(CHANSERV, new CommandCSAOP());
		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
			this->AddCommand(CHANSERV, new CommandCSHOP());
		this->AddCommand(CHANSERV, new CommandCSSOP());
		this->AddCommand(CHANSERV, new CommandCSVOP());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			notice_lang(s_ChanServ, u, CHAN_HELP_CMD_QOP);
		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
			notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SOP);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_AOP);
		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
			notice_lang(s_ChanServ, u, CHAN_HELP_CMD_HOP);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_VOP);
	}
};

/* `last' is set to the last index this routine was called with
 * `perm' is incremented whenever a permission-denied error occurs
 */
int xop_del(User *u, ChannelInfo *ci, ChanAccess *access, int *perm, int uacc, int xlev)
{
	char *nick = access->nc->display;
	if (!access->in_use || access->level != xlev)
		return 0;
	if (uacc <= access->level && !u->nc->HasPriv("chanserv/access/modify"))
	{
		++(*perm);
		return 0;
	}
	access->nc = NULL;
	access->in_use = 0;

	FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, nick));

	return 1;
}

int xop_del_callback(User *u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);
	int *perm = va_arg(args, int *);
	int uacc = va_arg(args, int);
	int xlev = va_arg(args, int);

	if (num < 1 || num > ci->GetAccessCount())
		return 0;
	*last = num;

	return xop_del(u, ci, ci->GetAccess(num - 1), perm, uacc, xlev);
}


int xop_list(User *u, int index, ChannelInfo *ci, int *sent_header, int xlev, int xmsg)
{
	ChanAccess *access = ci->GetAccess(index);

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

	if (num < 1 || num > ci->GetAccessCount())
		return 0;

	return xop_list(u, num - 1, ci, sent_header, xlev, xmsg);
}

MODULE_INIT(CSXOP)
