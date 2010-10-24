/* ChanServ core functions
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

LanguageString xop_msgs[XOP_TYPES][XOP_MESSAGES] = {
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
	LanguageString *messages;
	bool SentHeader;
 public:
	XOPListCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist, int _level, LanguageString *_messages) : NumberList(numlist, false), u(_u), ci(_ci), level(_level), messages(_messages), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		ChanAccess *access = ci->GetAccess(Number - 1);

		if (level != access->level)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(ChanServ, messages[XOP_LIST_HEADER], ci->name.c_str());
		}

		DoList(u, ci, access, Number - 1, level, messages);
	}

	static void DoList(User *u, ChannelInfo *ci, ChanAccess *access, unsigned index, int level, LanguageString *messages)
	{
		u->SendMessage(ChanServ, CHAN_XOP_LIST_FORMAT, index, access->nc->display.c_str());
	}
};

class XOPDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	Command *c;
	LanguageString *messages;
	unsigned Deleted;
	Anope::string Nicks;
	bool override;
 public:
	XOPDelCallback(User *_u, Command *_c, ChannelInfo *_ci, LanguageString *_messages, bool _override, const Anope::string &numlist) : NumberList(numlist, true), u(_u), ci(_ci), c(_c), messages(_messages), Deleted(0), override(_override)
	{
	}

	~XOPDelCallback()
	{
		if (!Deleted)
			 u->SendMessage(ChanServ, messages[XOP_NO_MATCH], ci->name.c_str());
		else
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, c, ci) << "deleted access of users " << Nicks;

			if (Deleted == 1)
				u->SendMessage(ChanServ, messages[XOP_DELETED_ONE], ci->name.c_str());
			else
				u->SendMessage(ChanServ, messages[XOP_DELETED_SEVERAL], Deleted, ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		ChanAccess *access = ci->GetAccess(Number - 1);

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->nc->display;
		else
			Nicks = access->nc->display;

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access->nc));

		ci->EraseAccess(Number - 1);
	}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<Anope::string> &params, ChannelInfo *ci, int level, LanguageString *messages)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";
		ChanAccess *access;
		int change = 0;

		if (nick.empty())
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		if (readonly)
		{
			u->SendMessage(ChanServ, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			u->SendMessage(ChanServ, messages[XOP_NICKS_ONLY]);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			u->SendMessage(ChanServ, NICK_X_FORBIDDEN, na->nick.c_str());
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
				u->SendMessage(ChanServ, ACCESS_DENIED);
				return MOD_CONT;
			}
			++change;
		}

		if (!change && ci->GetAccessCount() >= Config->CSAccessMax)
		{
			u->SendMessage(ChanServ, CHAN_XOP_REACHED_LIMIT, Config->CSAccessMax);
			return MOD_CONT;
		}

		if (!change)
			ci->AddAccess(nc, level, u->nick);
		else
		{
			access->level = level;
			access->last_seen = 0;
			access->creator = u->nick;
		}

		bool override = (level >= ulev || ulev < ACCESS_AOP || (access && access->level > ulev));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << na->nick << " (group: " << nc->display << ") as level " << level;

		if (!change)
		{
			FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, nc, level));
			u->SendMessage(ChanServ, messages[XOP_ADDED], nc->display.c_str(), ci->name.c_str());
		}
		else
		{
			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nc, level));
			u->SendMessage(ChanServ, messages[XOP_MOVED], nc->display.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<Anope::string> &params, ChannelInfo *ci, int level, LanguageString *messages)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";
		ChanAccess *access;

		if (nick.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (readonly)
		{
			u->SendMessage(ChanServ, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			u->SendMessage(ChanServ, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		short ulev = get_access(u, ci);

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(nick[0]) && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			bool override = level >= ulev || ulev < ACCESS_AOP;
			XOPDelCallback list(u, this, ci, messages, override, nick);
			list.Process();
		}
		else
		{
			NickAlias *na = findnick(nick);
			if (!na)
			{
				u->SendMessage(ChanServ, NICK_X_NOT_REGISTERED, nick.c_str());
				return MOD_CONT;
			}
			NickCore *nc = na->nc;
			unsigned i, end;
			for (i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				access = ci->GetAccess(nc, level);

				if (access->nc == nc)
					break;
			}

			if (i == end)
			{
				u->SendMessage(ChanServ, messages[XOP_NOT_FOUND], nick.c_str(), ci->name.c_str());
				return MOD_CONT;
			}

			if (ulev <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
				u->SendMessage(ChanServ, ACCESS_DENIED);
			else
			{
				bool override = ulev <= access->level;
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << access->nc->display;

				u->SendMessage(ChanServ, messages[XOP_DELETED], access->nc->display.c_str(), ci->name.c_str());

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, na->nc));

				ci->EraseAccess(i);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<Anope::string> &params, ChannelInfo *ci, int level, LanguageString *messages)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";

		if (!get_access(u, ci) && !u->Account()->HasCommand("chanserv/access/list"))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		bool override = !get_access(u, ci);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		if (!ci->GetAccessCount())
		{
			u->SendMessage(ChanServ, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			XOPListCallback list(u, ci, nick, level, messages);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (access->level != level)
					continue;
				else if (!nick.empty() && access->nc && !Anope::Match(access->nc->display, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					u->SendMessage(ChanServ, messages[XOP_LIST_HEADER], ci->name.c_str());
				}

				XOPListCallback::DoList(u, ci, access, i + 1, level, messages);
			}

			if (!SentHeader)
				u->SendMessage(ChanServ, messages[XOP_NO_MATCH], ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, ChannelInfo *ci, int level, LanguageString *messages)
	{
		if (readonly)
		{
			u->SendMessage(ChanServ, messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			u->SendMessage(ChanServ, messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_FOUNDER);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR level " << level;

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanAccess *access = ci->GetAccess(i - 1);
			if (access->level == level)
				ci->EraseAccess(i - 1);
		}

		FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

		u->SendMessage(ChanServ, messages[XOP_CLEAR], ci->name.c_str());

		return MOD_CONT;
	}
 protected:
	CommandReturn DoXop(User *u, const std::vector<Anope::string> &params, int level, LanguageString *messages)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];

		ChannelInfo *ci = cs_findchan(chan);

		if (!ci->HasFlag(CI_XOP))
			u->SendMessage(ChanServ, CHAN_XOP_ACCESS, Config->s_ChanServ.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, params, ci, level, messages);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(u, params, ci, level, messages);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u, params, ci, level, messages);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(u, ci, level, messages);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}
 public:
	XOPBase(const Anope::string &command) : Command(command, 2, 3)
	{
	}

	virtual ~XOPBase()
	{
	}

	virtual CommandReturn Execute(User *u, const std::vector<Anope::string> &params) = 0;

	virtual bool OnHelp(User *u, const Anope::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u, const Anope::string &subcommand) = 0;

	virtual void OnServHelp(User *u) = 0;
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP() : XOPBase("QOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoXop(u, params, ACCESS_QOP, xop_msgs[XOP_QOP]);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_QOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "QOP", CHAN_QOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_QOP);
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoXop(u, params, ACCESS_AOP, xop_msgs[XOP_AOP]);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_AOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "AOP", CHAN_AOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_AOP);
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP() : XOPBase("HOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoXop(u, params, ACCESS_HOP, xop_msgs[XOP_HOP]);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_HOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "HOP", CHAN_HOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_HOP);
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP() : XOPBase("SOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoXop(u, params, ACCESS_SOP, xop_msgs[XOP_SOP]);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "SOP", CHAN_SOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SOP);
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP() : XOPBase("VOP")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoXop(u, params, ACCESS_VOP, xop_msgs[XOP_VOP]);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_VOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "VOP", CHAN_VOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_VOP);
	}
};

class CSXOP : public Module
{
	CommandCSQOP commandcsqop;
	CommandCSSOP commandcssop;
	CommandCSAOP commandcsaop;
	CommandCSHOP commandcshop;
	CommandCSVOP commandcsvop;

 public:
	CSXOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcssop);
		this->AddCommand(ChanServ, &commandcsaop);
		this->AddCommand(ChanServ, &commandcsvop);

		if (Me && Me->IsSynced())
			OnUplinkSync(NULL);

		Implementation i[] = {I_OnUplinkSync, I_OnServerDisconnect};
		ModuleManager::Attach(i, this, 2);
	}

	void OnUplinkSync(Server *)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			this->AddCommand(ChanServ, &commandcsqop);
		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
			this->AddCommand(ChanServ, &commandcshop);
	}

	void OnServerDisconnect()
	{
		this->DelCommand(ChanServ, &commandcsqop);
		this->DelCommand(ChanServ, &commandcshop);
	}
};

MODULE_INIT(CSXOP)
