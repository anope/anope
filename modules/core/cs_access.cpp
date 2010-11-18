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

class AccessListCallback : public NumberList
{
 protected:
	User *u;
	ChannelInfo *ci;
	bool SentHeader;
 public:
	AccessListCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), u(_u), ci(_ci), SentHeader(false)
	{
	}

	~AccessListCallback()
	{
		if (SentHeader)
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
		else
			u->SendMessage(ChanServ, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned Number, ChanAccess *access)
	{
		if (ci->HasFlag(CI_XOP))
		{
			Anope::string xop = get_xop_level(access->level);
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_XOP_FORMAT, Number + 1, xop.c_str(), access->nc->display.c_str());
		}
		else
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_AXS_FORMAT, Number + 1, access->level, access->nc->display.c_str());
	}
};

class AccessViewCallback : public AccessListCallback
{
 public:
	AccessViewCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist) : AccessListCallback(_u, _ci, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned Number, ChanAccess *access)
	{
		Anope::string timebuf;
		if (ci->c && nc_on_chan(ci->c, access->nc))
			timebuf = "Now";
		else if (access->last_seen == 0)
			timebuf = "Never";
		else
			timebuf = do_strftime(access->last_seen);

		if (ci->HasFlag(CI_XOP))
		{
			Anope::string xop = get_xop_level(access->level);
			u->SendMessage(ChanServ, CHAN_ACCESS_VIEW_XOP_FORMAT, Number + 1, xop.c_str(), access->nc->display.c_str(), access->creator.c_str(), timebuf.c_str());
		}
		else
			u->SendMessage(ChanServ, CHAN_ACCESS_VIEW_AXS_FORMAT, Number + 1, access->level, access->nc->display.c_str(), access->creator.c_str(), timebuf.c_str());
	}
};

class AccessDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	Command *c;
	unsigned Deleted;
	Anope::string Nicks;
	bool Denied;
	bool override;
 public:
	AccessDelCallback(User *_u, ChannelInfo *_ci, Command *_c, const Anope::string &numlist) : NumberList(numlist, true), u(_u), ci(_ci), c(_c), Deleted(0), Denied(false)
	{
		if (!check_access(u, ci, CA_ACCESS_CHANGE) && u->Account()->HasPriv("chanserv/access/modify"))
			this->override = true;
	}

	~AccessDelCallback()
	{
		if (Denied && !Deleted)
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (!Deleted)
			u->SendMessage(ChanServ, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		else
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, c, ci) << "for user" << (Deleted == 1 ? " " : "s ") << Nicks;

			if (Deleted == 1)
				u->SendMessage(ChanServ, CHAN_ACCESS_DELETED_ONE, ci->name.c_str());
			else
				u->SendMessage(ChanServ, CHAN_ACCESS_DELETED_SEVERAL, Deleted, ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		ChanAccess *access = ci->GetAccess(Number - 1);

		if (get_access(u, ci) <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			Denied = true;
			return;
		}

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->nc->display;
		else
			Nicks = access->nc->display;

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access->nc));

		ci->EraseAccess(Number - 1);
	}
};

class CommandCSAccess : public Command
{
	CommandReturn DoAdd(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[2];
		int level = params[3].is_number_only() ? convertTo<int>(params[3]) : ACCESS_INVALID;
		int ulev = get_access(u, ci);

		if (level >= ulev && !u->Account()->HasPriv("chanserv/access/modify"))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!level)
		{
			u->SendMessage(ChanServ, CHAN_ACCESS_LEVEL_NONZERO);
			return MOD_CONT;
		}
		else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
		{
			u->SendMessage(ChanServ, CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_ACCESS_CHANGE) || level >= ulev;

		NickAlias *na = findnick(nick);
		if (!na)
		{
			u->SendMessage(ChanServ, CHAN_ACCESS_NICKS_ONLY);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			u->SendMessage(ChanServ, NICK_X_FORBIDDEN, nick.c_str());
			return MOD_CONT;
		}

		NickCore *nc = na->nc;
		ChanAccess *access = ci->GetAccess(nc);
		if (access)
		{
			/* Don't allow lowering from a level >= ulev */
			if (access->level >= ulev && !u->Account()->HasPriv("chanserv/access/modify"))
			{
				u->SendMessage(ChanServ, ACCESS_DENIED);
				return MOD_CONT;
			}
			if (access->level == level)
			{
				u->SendMessage(ChanServ, CHAN_ACCESS_LEVEL_UNCHANGED, access->nc->display.c_str(), ci->name.c_str(), level);
				return MOD_CONT;
			}
			access->level = level;

			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nc, level));

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << na->nick << " (group: " << nc->display << ") (level: " << level << ") as level " << ulev;
			u->SendMessage(ChanServ, CHAN_ACCESS_LEVEL_CHANGED, nc->display.c_str(), ci->name.c_str(), level);
			return MOD_CONT;
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			u->SendMessage(ChanServ, CHAN_ACCESS_REACHED_LIMIT, Config->CSAccessMax);
			return MOD_CONT;
		}

		ci->AddAccess(nc, level, u->nick);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, nc, level));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << na->nick << " (group: " << nc->display << ") (level: " << level << ") as level " << ulev;
		u->SendMessage(ChanServ, CHAN_ACCESS_ADDED, nc->display.c_str(), ci->name.c_str(), level);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[2];

		if (!ci->GetAccessCount())
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
		else if (isdigit(nick[0]) && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessDelCallback list(u, ci, this, nick);
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
			ChanAccess *access = NULL;
			for (i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				access = ci->GetAccess(i);

				if (access->nc == nc)
					break;
			}

			if (i == end)
				u->SendMessage(ChanServ, CHAN_ACCESS_NOT_FOUND, nick.c_str(), ci->name.c_str());
			else if (get_access(u, ci) <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
				u->SendMessage(ChanServ, ACCESS_DENIED);
			else
			{
				u->SendMessage(ChanServ, CHAN_ACCESS_DELETED, access->nc->display.c_str(), ci->name.c_str());
				bool override = !check_access(u, ci, CA_ACCESS_CHANGE);
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << na->nick << " (group: " << access->nc->display << ") from level " << access->level;

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, na->nc));

				ci->EraseAccess(i);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
		else if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessListCallback list(u, ci, nick);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && access->nc && !Anope::Match(access->nc->display, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					u->SendMessage(ChanServ, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessListCallback::DoList(u, ci, i, access);
			}

			if (SentHeader)
				u->SendMessage(ChanServ, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
			else
				u->SendMessage(ChanServ, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			u->SendMessage(ChanServ, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
		else if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessViewCallback list(u, ci, nick);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && access->nc && !Anope::Match(access->nc->display, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					u->SendMessage(ChanServ, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessViewCallback::DoList(u, ci, i, access);
			}

			if (SentHeader)
				u->SendMessage(ChanServ, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
			else
				u->SendMessage(ChanServ, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, ChannelInfo *ci)
	{
		if (!IsFounder(u, ci) && !u->Account()->HasPriv("chanserv/access/modify"))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

			u->SendMessage(ChanServ, CHAN_ACCESS_CLEAR, ci->name.c_str());

			bool override = !IsFounder(u, ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";
		}

		return MOD_CONT;
	}

 public:
	CommandCSAccess() : Command("ACCESS", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string nick = params.size() > 2 ? params[2] : "";
		Anope::string s = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = cs_findchan(chan);

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");
		bool is_clear = cmd.equals_ci("CLEAR");

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || is_clear ? 0 : (cmd.equals_ci("DEL") ? (nick.empty() || !s.empty()) : s.empty()))
			this->OnSyntaxError(u, cmd);
		/* We still allow LIST in xOP mode, but not others */
		else if (ci->HasFlag(CI_XOP) && !is_list && !is_clear)
		{
			if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
				u->SendMessage(ChanServ, CHAN_ACCESS_XOP_HOP, Config->s_ChanServ.c_str());
			else
				u->SendMessage(ChanServ, CHAN_ACCESS_XOP, Config->s_ChanServ.c_str());
		}
		else if ((is_list && !check_access(u, ci, CA_ACCESS_LIST) && !u->Account()->HasCommand("chanserv/access/list")) || (!is_list && !check_access(u, ci, CA_ACCESS_CHANGE) && !u->Account()->HasPriv("chanserv/access/modify")))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (readonly && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL") || cmd.equals_ci("CLEAR")))
			u->SendMessage(ChanServ, CHAN_ACCESS_DISABLED);
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(u, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(u, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(u, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(u, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(u, ci);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_ACCESS);
		u->SendMessage(ChanServ, CHAN_HELP_ACCESS_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
	}
};

class CommandCSLevels : public Command
{
	CommandReturn DoSet(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string what = params[2];
		Anope::string lev = params[3];

		Anope::string error;
		int level = (lev.is_number_only() ? convertTo<int>(lev, error, false) : 0);
		if (!lev.is_number_only())
			error = "1";

		if (lev.equals_ci("FOUNDER"))
		{
			level = ACCESS_FOUNDER;
			error.clear();
		}

		if (!error.empty())
			this->OnSyntaxError(u, "SET");
		else if (level <= ACCESS_INVALID || level > ACCESS_FOUNDER)
			u->SendMessage(ChanServ, CHAN_LEVELS_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
		{
			for (int i = 0; levelinfo[i].what >= 0; ++i)
			{
				if (what.equals_ci(levelinfo[i].name))
				{
					ci->levels[levelinfo[i].what] = level;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, level));

					bool override = !check_access(u, ci, CA_FOUNDER);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "SET " << levelinfo[i].name << " to " << level;

					if (level == ACCESS_FOUNDER)
						u->SendMessage(ChanServ, CHAN_LEVELS_CHANGED_FOUNDER, levelinfo[i].name.c_str(), ci->name.c_str());
					else
						u->SendMessage(ChanServ, CHAN_LEVELS_CHANGED, levelinfo[i].name.c_str(), ci->name.c_str(), level);
					return MOD_CONT;
				}
			}

			u->SendMessage(ChanServ, CHAN_LEVELS_UNKNOWN, what.c_str(), Config->s_ChanServ.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoDisable(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string what = params[2];

		/* Don't allow disabling of the founder level. It would be hard to change it back if you dont have access to use this command */
		if (what.equals_ci("FOUNDER"))
			for (int i = 0; levelinfo[i].what >= 0; ++i)
			{
				if (what.equals_ci(levelinfo[i].name))
				{
					ci->levels[levelinfo[i].what] = ACCESS_INVALID;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, levelinfo[i].what));

					bool override = !check_access(u, ci, CA_FOUNDER);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DISABLE " << levelinfo[i].name;

					u->SendMessage(ChanServ, CHAN_LEVELS_DISABLED, levelinfo[i].name.c_str(), ci->name.c_str());
					return MOD_CONT;
				}
			}

		u->SendMessage(ChanServ, CHAN_LEVELS_UNKNOWN, what.c_str(), Config->s_ChanServ.c_str());

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, ChannelInfo *ci)
	{
		u->SendMessage(ChanServ, CHAN_LEVELS_LIST_HEADER, ci->name.c_str());

		if (!levelinfo_maxwidth)
			for (int i = 0; levelinfo[i].what >= 0; ++i)
			{
				int len = levelinfo[i].name.length();
				if (len > levelinfo_maxwidth)
					levelinfo_maxwidth = len;
			}

		for (int i = 0; levelinfo[i].what >= 0; ++i)
		{
			int j = ci->levels[levelinfo[i].what];

			if (j == ACCESS_INVALID)
			{
				j = levelinfo[i].what;

				if (j == CA_AUTOOP || j == CA_AUTODEOP || j == CA_AUTOVOICE || j == CA_NOJOIN)
					u->SendMessage(ChanServ, CHAN_LEVELS_LIST_DISABLED, levelinfo_maxwidth, levelinfo[i].name.c_str());
				else
					u->SendMessage(ChanServ, CHAN_LEVELS_LIST_DISABLED, levelinfo_maxwidth, levelinfo[i].name.c_str());
			}
			else if (j == ACCESS_FOUNDER)
				u->SendMessage(ChanServ, CHAN_LEVELS_LIST_FOUNDER, levelinfo_maxwidth, levelinfo[i].name.c_str());
			else
				u->SendMessage(ChanServ, CHAN_LEVELS_LIST_NORMAL, levelinfo_maxwidth, levelinfo[i].name.c_str(), j);
		}

		return MOD_CONT;
	}

	CommandReturn DoReset(User *u, ChannelInfo *ci)
	{
		reset_levels(ci);
		FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, -1, 0));

		bool override = !check_access(u, ci, CA_FOUNDER);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "RESET";

		u->SendMessage(ChanServ, CHAN_LEVELS_RESET, ci->name.c_str());
		return MOD_CONT;
	}

 public:
	CommandCSLevels() : Command("LEVELS", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string what = params.size() > 2 ? params[2] : "";
		Anope::string s = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = cs_findchan(chan);

		/* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
		 * one; else, we want none.
		 */
		if (cmd.equals_ci("SET") ? s.empty() : (cmd.substr(0, 3).equals_ci("DIS") ? (what.empty() || !s.empty()) : !what.empty()))
			this->OnSyntaxError(u, cmd);
		else if (ci->HasFlag(CI_XOP))
			u->SendMessage(ChanServ, CHAN_LEVELS_XOP);
		else if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("chanserv/access/modify"))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (cmd.equals_ci("SET"))
			this->DoSet(u, ci, params);
		else if (cmd.equals_ci("DIS") || cmd.equals_ci("DISABLE"))
			this->DoDisable(u, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(u, ci);
		else if (cmd.equals_ci("RESET"))
			this->DoReset(u, ci);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.equals_ci("DESC"))
		{
			int i;
			u->SendMessage(ChanServ, CHAN_HELP_LEVELS_DESC);
			if (!levelinfo_maxwidth)
				for (i = 0; levelinfo[i].what >= 0; ++i)
				{
					int len = levelinfo[i].name.length();
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			for (i = 0; levelinfo[i].what >= 0; ++i)
				u->SendMessage(ChanServ, CHAN_HELP_LEVELS_DESC_FORMAT, levelinfo_maxwidth, levelinfo[i].name.c_str(), GetString(u, levelinfo[i].desc).c_str());
		}
		else
			u->SendMessage(ChanServ, CHAN_HELP_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_ACCESS);
		u->SendMessage(ChanServ, CHAN_HELP_CMD_LEVELS);
	}
};

class CSAccess : public Module
{
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;

 public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsaccess);
		this->AddCommand(ChanServ, &commandcslevels);
	}
};

MODULE_INIT(CSAccess)
