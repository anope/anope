/* ChanServ core functions
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
	CommandSource &source;
	int level;
	LanguageString *messages;
	bool SentHeader;
 public:
	XOPListCallback(CommandSource &_source, const Anope::string &numlist, int _level, LanguageString *_messages) : NumberList(numlist, false), source(_source), level(_level), messages(_messages), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAccessCount())
			return;

		ChanAccess *access = source.ci->GetAccess(Number - 1);

		if (level != access->level)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(messages[XOP_LIST_HEADER], source.ci->name.c_str());
		}

		DoList(source, access, Number - 1, level, messages);
	}

	static void DoList(CommandSource &source, ChanAccess *access, unsigned index, int level, LanguageString *messages)
	{
		source.Reply(CHAN_XOP_LIST_FORMAT, index, access->mask.c_str());
	}
};

class XOPDelCallback : public NumberList
{
	CommandSource &source;
	Command *c;
	LanguageString *messages;
	unsigned Deleted;
	Anope::string Nicks;
	bool override;
 public:
	XOPDelCallback(CommandSource &_source, Command *_c, LanguageString *_messages, bool _override, const Anope::string &numlist) : NumberList(numlist, true), source(_source), c(_c), messages(_messages), Deleted(0), override(_override)
	{
	}

	~XOPDelCallback()
	{
		if (!Deleted)
			 source.Reply(messages[XOP_NO_MATCH], source.ci->name.c_str());
		else
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, c, source.ci) << "deleted access of users " << Nicks;

			if (Deleted == 1)
				source.Reply(messages[XOP_DELETED_ONE], source.ci->name.c_str());
			else
				source.Reply(messages[XOP_DELETED_SEVERAL], Deleted, source.ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAccessCount())
			return;

		ChanAccess *access = source.ci->GetAccess(Number - 1);

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->mask;
		else
			Nicks = access->mask;

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(source.ci, source.u, access));

		source.ci->EraseAccess(Number - 1);
	}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params, int level, LanguageString *messages)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		Anope::string mask = params.size() > 2 ? params[2] : "";
		int change = 0;

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(mask);
		if (!na && mask.find_first_of("!@*") == Anope::string::npos)
			mask += "!*@*";
		else if (na && na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
			return MOD_CONT;
		}

		access = ci->GetAccess(mask, 0, false);
		if (access)
		{
			/**
			 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
			 **/
			if (access->level >= ulev && !u->Account()->HasPriv("chanserv/access/modify"))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}
			++change;
		}

		if (!change && ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(CHAN_XOP_REACHED_LIMIT, Config->CSAccessMax);
			return MOD_CONT;
		}

		if (!change)
			access = ci->AddAccess(mask, level, u->nick);
		else
		{
			access->level = level;
			access->last_seen = 0;
			access->creator = u->nick;
		}

		bool override = (level >= ulev || ulev < ACCESS_AOP || (access && access->level > ulev));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << " as level " << level;

		if (!change)
		{
			FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, access));
			source.Reply(messages[XOP_ADDED], access->mask.c_str(), ci->name.c_str());
		}
		else
		{
			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, access));
			source.Reply(messages[XOP_MOVED], access->mask.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params, int level, LanguageString *messages)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if ((!access || access->nc != u->Account()) && (level >= ulev || ulev < ACCESS_AOP) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		access = ci->GetAccess(mask, 0, false);

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			bool override = level >= ulev || ulev < ACCESS_AOP;
			XOPDelCallback list(source, this, messages, override, mask);
			list.Process();
		}
		else if (!access || access->level != level)
		{
			source.Reply(messages[XOP_NOT_FOUND], mask.c_str(), ci->name.c_str());
			return MOD_CONT;
		}
		else
		{
			if (access->nc != u->Account() && ulev <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
				source.Reply(ACCESS_DENIED);
			else
			{
				bool override = ulev <= access->level;
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << access->mask;

				source.Reply(messages[XOP_DELETED], access->mask.c_str(), ci->name.c_str());

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access));

				ci->EraseAccess(access);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params, int level, LanguageString *messages)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if (!ulev && !u->Account()->HasCommand("chanserv/access/list"))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		bool override = !ulev;
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		if (!ci->GetAccessCount())
		{
			source.Reply(messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			XOPListCallback list(source, nick, level, messages);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				access = ci->GetAccess(i);

				if (access->level != level)
					continue;
				else if (!nick.empty() && !Anope::Match(access->mask, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(messages[XOP_LIST_HEADER], ci->name.c_str());
				}

				XOPListCallback::DoList(source, access, i + 1, level, messages);
			}

			if (!SentHeader)
				source.Reply(messages[XOP_NO_MATCH], ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source, int level, LanguageString *messages)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
		{
			source.Reply(messages[XOP_DISABLED]);
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(messages[XOP_LIST_EMPTY], ci->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
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

		source.Reply(messages[XOP_CLEAR], ci->name.c_str());

		return MOD_CONT;
	}
 protected:
	CommandReturn DoXop(CommandSource &source, const std::vector<Anope::string> &params, int level, LanguageString *messages)
	{
		ChannelInfo *ci = source.ci;

		const Anope::string &cmd = params[1];

		if (!ci->HasFlag(CI_XOP))
			source.Reply(CHAN_XOP_ACCESS, Config->s_ChanServ.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params, level, messages);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params, level, messages);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params, level, messages);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, level, messages);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}
 public:
	XOPBase(const Anope::string &command) : Command(command, 2, 4)
	{
	}

	virtual ~XOPBase()
	{
	}

	virtual CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) = 0;

	virtual void OnSyntaxError(CommandSource &source, const Anope::string &subcommand) = 0;

	virtual void OnServHelp(CommandSource &source) = 0;
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP() : XOPBase("QOP")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_QOP, xop_msgs[XOP_QOP]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_QOP);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "QOP", CHAN_QOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_QOP);
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_AOP, xop_msgs[XOP_AOP]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_AOP);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AOP", CHAN_AOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_AOP);
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP() : XOPBase("HOP")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_HOP, xop_msgs[XOP_HOP]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_HOP);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "HOP", CHAN_HOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_HOP);
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP() : XOPBase("SOP")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_SOP, xop_msgs[XOP_SOP]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_SOP);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SOP", CHAN_SOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SOP);
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP() : XOPBase("VOP")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_VOP, xop_msgs[XOP_VOP]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_VOP);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "VOP", CHAN_VOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_VOP);
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
