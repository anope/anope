/* ChanServ core functions
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

#include "module.h"

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

class XOPListCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	int level;
	int *messages;
	bool SentHeader;
 public:
	XOPListCallback(User *_u, ChannelInfo *_ci, const std::string &numlist, int _level, int *_messages) : NumberList(numlist, false), u(_u), ci(_ci), level(_level), messages(_messages), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAccessCount())
			return;

		ChanAccess *access = ci->GetAccess(Number - 1);
		
		if (level != access->level)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_ChanServ, u, messages[XOP_LIST_HEADER], ci->name.c_str());
		}

		DoList(u, ci, access, Number - 1, level, messages);
	}

	static void DoList(User *u, ChannelInfo *ci, ChanAccess *access, unsigned index, int level, int *messages)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_XOP_LIST_FORMAT, index, access->nc->display);
	}
};

class XOPDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	int *messages;
	unsigned Deleted;
	std::string Nicks;
 public:
	XOPDelCallback(User *_u, ChannelInfo *_ci, int *_messages, const std::string &numlist) : NumberList(numlist, true), u(_u), ci(_ci), messages(_messages), Deleted(0)
	{
	}

	~XOPDelCallback()
	{
		if (!Deleted)
			 notice_lang(Config.s_ChanServ, u, messages[XOP_NO_MATCH], ci->name.c_str());
		else
		{
			 Alog() << Config.s_ChanServ << ": " << u->GetMask() << " (level " << get_access(u, ci) << ") deleted access of users " << Nicks << " on " << ci->name;

			if (Deleted == 1)
				notice_lang(Config.s_ChanServ, u, messages[XOP_DELETED_ONE], ci->name.c_str());
			else
				notice_lang(Config.s_ChanServ, u, messages[XOP_DELETED_SEVERAL], Deleted, ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAccessCount())
			return;

		ChanAccess *access = ci->GetAccess(Number - 1);

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + std::string(access->nc->display);
		else
			Nicks = access->nc->display;

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access->nc));

		ci->EraseAccess(Number - 1);
	}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;
		int change = 0;

		if (!nick)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_NICKS_ONLY]);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		NickCore *nc = na->nc;
		access = ci->GetAccess(nc);
		if (access)
		{
			/**
			 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
			 **/
			if (access->level >= ulev && !u->Account()->HasPriv("chanserv/access/modify"))
			{
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			++change;
		}

		if (!change && ci->GetAccessCount() >= Config.CSAccessMax)
		{
			notice_lang(Config.s_ChanServ, u, CHAN_XOP_REACHED_LIMIT, Config.CSAccessMax);
			return MOD_CONT;
		}

		if (!change)
		{
			ci->AddAccess(nc, level, u->nick);
		}
		else
		{
			access->level = level;
			access->last_seen = 0;
			access->creator = u->nick;
		}

		Alog() << Config.s_ChanServ << ": " << u->GetMask() << " (level " << ulev << ") " 
			<< (change ? "changed" : "set") << " access level " << level << " to " << na->nick 
			<< " (group " << nc->display << ") on channel " << ci->name;

		if (!change)
		{
			FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, nc, level));
			notice_lang(Config.s_ChanServ, u, messages[XOP_ADDED], nc->display, ci->name.c_str());
		}
		else
		{
			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nc, level));
			notice_lang(Config.s_ChanServ, u, messages[XOP_MOVED], nc->display, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		ChanAccess *access;

		if (!nick)
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick))
			(new XOPDelCallback(u, ci, messages, nick))->Process();
		else
		{
			NickAlias *na = findnick(nick);
			if (!na)
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
				return MOD_CONT;
			}
			NickCore *nc = na->nc;

			unsigned i;
			for (i = 0; i < ci->GetAccessCount(); ++i)
			{
				access = ci->GetAccess(nc, level);

				if (access->nc == nc)
					break;
			}

			if (i == ci->GetAccessCount())
			{
				notice_lang(Config.s_ChanServ, u, messages[XOP_NOT_FOUND], nick, ci->name.c_str());
				return MOD_CONT;
			}

			if (ulev <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
			{
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			}
			else
			{
				Alog() << Config.s_ChanServ << ": " << u->GetMask() << " (level " << get_access(u, ci) << ") deleted access of user " << access->nc->display << " on " << ci->name;

				notice_lang(Config.s_ChanServ, u, messages[XOP_DELETED], access->nc->display, ci->name.c_str());

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, na->nc));

				ci->EraseAccess(i);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;

		if (!get_access(u, ci) && !u->Account()->HasCommand("chanserv/access/list"))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (nick && strspn(nick, "1234567890,-") == strlen(nick))
			(new XOPListCallback(u, ci, nick, level, messages))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (nick && access->nc && !Anope::Match(access->nc->display, nick, false))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					notice_lang(Config.s_ChanServ, u, messages[XOP_LIST_HEADER], ci->name.c_str());
				}

				XOPListCallback::DoList(u, ci, access, i, level, messages);
			}

			if (!SentHeader)
				notice_lang(Config.s_ChanServ, u, messages[XOP_NO_MATCH], ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, const std::vector<ci::string> &params, ChannelInfo *ci, int level, int *messages)
	{
		if (readonly)
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			notice_lang(Config.s_ChanServ, u, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanAccess *access = ci->GetAccess(i - 1);
			if (access->level == level)
				ci->EraseAccess(i - 1);
		}

		FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

		notice_lang(Config.s_ChanServ, u, messages[XOP_CLEAR], ci->name.c_str());

		return MOD_CONT;
	}
 protected:
	CommandReturn DoXop(User *u, const std::vector<ci::string> &params, int level, int *messages)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];

		ChannelInfo *ci = cs_findchan(chan);

		if (!(ci->HasFlag(CI_XOP)))
			notice_lang(Config.s_ChanServ, u, CHAN_XOP_ACCESS, Config.s_ChanServ);
		else if (cmd == "ADD")
			return this->DoAdd(u, params, ci, level, messages);
		else if (cmd == "DEL")
			return this->DoDel(u, params, ci, level, messages);
		else if (cmd == "LIST")
			return this->DoList(u, params, ci, level, messages);
		else if (cmd == "CLEAR")
			return this->DoClear(u, params, ci, level, messages);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}
 public:
	XOPBase(const ci::string &command) : Command(command, 2, 3)
	{
	}

	virtual ~XOPBase()
	{
	}

	virtual CommandReturn Execute(User *u, const std::vector<ci::string> &params) = 0;

	virtual bool OnHelp(User *u, const ci::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u, const ci::string &subcommand) = 0;
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP() : XOPBase("QOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_QOP, xop_msgs[XOP_QOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_QOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "QOP", CHAN_QOP_SYNTAX);
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_AOP, xop_msgs[XOP_AOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_AOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "AOP", CHAN_AOP_SYNTAX);
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP() : XOPBase("HOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_HOP, xop_msgs[XOP_HOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_HOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "HOP", CHAN_HOP_SYNTAX);
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP() : XOPBase("SOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_SOP, xop_msgs[XOP_SOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "SOP", CHAN_SOP_SYNTAX);
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP() : XOPBase("VOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoXop(u, params, ACCESS_VOP, xop_msgs[XOP_VOP]);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_VOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "VOP", CHAN_VOP_SYNTAX);
	}
};

class CSXOP : public Module
{
 public:
	CSXOP(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(ChanServ, new CommandCSSOP());
		this->AddCommand(ChanServ, new CommandCSAOP());
		this->AddCommand(ChanServ, new CommandCSVOP());

		if (Me && Me->IsSynced())
			OnUplinkSync(NULL);

		Implementation i[] = {
			I_OnUplinkSync, I_OnServerDisconnect, I_OnChanServHelp
		};
		ModuleManager::Attach(i, this, 3);
	}

	void OnUplinkSync(Server *)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			this->AddCommand(ChanServ, new CommandCSQOP());
		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
			this->AddCommand(ChanServ, new CommandCSHOP());
	}

	void OnServerDisconnect()
	{
		this->DelCommand(ChanServ, FindCommand(ChanServ, "QOP"));
		this->DelCommand(ChanServ, FindCommand(ChanServ, "HOP"));
	}

	void OnChanServHelp(User *u)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_QOP);
		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SOP);
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_AOP);
		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_HOP);
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_VOP);
	}
};

MODULE_INIT(CSXOP)
