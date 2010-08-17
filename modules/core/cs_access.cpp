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
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
		else
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned Number, ChanAccess *access)
	{
		if (ci->HasFlag(CI_XOP))
		{
			Anope::string xop = get_xop_level(access->level);
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_XOP_FORMAT, Number + 1, xop.c_str(), access->nc->display.c_str());
		}
		else
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_AXS_FORMAT, Number + 1, access->level, access->nc->display.c_str());
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
		if (Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned Number, ChanAccess *access)
	{
		char timebuf[64] = "";
		struct tm tm;

		if (ci->c && u->Account() && nc_on_chan(ci->c, u->Account()))
			snprintf(timebuf, sizeof(timebuf), "Now");
		else if (access->last_seen == 0)
			snprintf(timebuf, sizeof(timebuf), "Never");
		else
		{
			tm = *localtime(&access->last_seen);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
		}

		if (ci->HasFlag(CI_XOP))
		{
			Anope::string xop = get_xop_level(access->level);
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_VIEW_XOP_FORMAT, Number + 1, xop.c_str(), access->nc->display.c_str(), access->creator.c_str(), timebuf);
		}
		else
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_VIEW_AXS_FORMAT, Number + 1, access->level, access->nc->display.c_str(), access->creator.c_str(), timebuf);
	}
};

class AccessDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	unsigned Deleted;
	Anope::string Nicks;
	bool Denied;
 public:
	AccessDelCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, true), u(_u), ci(_ci), Deleted(0), Denied(false)
	{
	}

	~AccessDelCallback()
	{
		if (Denied && !Deleted)
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
		else if (!Deleted)
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		else
		{
			Alog() << Config->s_ChanServ << ": " << u->GetMask() << " (level " << get_access(u, ci) << ") deleted access of user" << (Deleted == 1 ? " " : "s ") << Nicks << " on " << ci->name;

			if (Deleted == 1)
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_DELETED_ONE, ci->name.c_str());
			else
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_DELETED_SEVERAL, Deleted, ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAccessCount())
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
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!level)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LEVEL_NONZERO);
			return MOD_CONT;
		}
		else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NICKS_ONLY);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config->s_ChanServ, u, NICK_X_FORBIDDEN, nick.c_str());
			return MOD_CONT;
		}

		NickCore *nc = na->nc;
		ChanAccess *access = ci->GetAccess(nc);
		if (access)
		{
			/* Don't allow lowering from a level >= ulev */
			if (access->level >= ulev && !u->Account()->HasPriv("chanserv/access/modify"))
			{
				notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			if (access->level == level)
			{
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LEVEL_UNCHANGED, access->nc->display.c_str(), ci->name.c_str(), level);
				return MOD_CONT;
			}
			access->level = level;

			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, na->nc, level));

			Alog() << Config->s_ChanServ << ": " << u->GetMask() << " (level " << ulev << ") set access level " << access->level << " to " << na->nick << " (group " << nc->display << ") on channel " << ci->name;
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LEVEL_CHANGED, nc->display.c_str(), ci->name.c_str(), level);
			return MOD_CONT;
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_REACHED_LIMIT, Config->CSAccessMax);
			return MOD_CONT;
		}

		ci->AddAccess(nc, level, u->nick);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, nc, level));

		Alog() << Config->s_ChanServ << ": " << u->GetMask() << " (level " << ulev << ") set access level " << level << " to " << na->nick << " (group " << nc->display << ") on channel " << ci->name;
		notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_ADDED, nc->display.c_str(), ci->name.c_str(), level);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[2];

		if (!ci->GetAccessCount())
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
		else if (isdigit(nick[0]) && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessDelCallback list(u, ci, nick);
			list.Process();
		}
		else
		{
			NickAlias *na = findnick(nick);
			if (!na)
			{
				notice_lang(Config->s_ChanServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
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
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NOT_FOUND, nick.c_str(), ci->name.c_str());
			else if (get_access(u, ci) <= access->level && !u->Account()->HasPriv("chanserv/access/modify"))
				notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
			else
			{
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_DELETED, access->nc->display.c_str(), ci->name.c_str());
				Alog() << Config->s_ChanServ << ": " << u->GetMask() << " (level " << get_access(u, ci) << ") deleted access of " << na->nick << " (group " << access->nc->display << ") on " << ci->name;
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
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
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
					notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessListCallback::DoList(u, ci, i, access);
			}

			if (SentHeader)
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
			else
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, ci->name.c_str());
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
					notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessViewCallback::DoList(u, ci, i, access);
			}

			if (SentHeader)
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name.c_str());
			else
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, ChannelInfo *ci)
	{
		if (!IsFounder(u, ci) && !u->Account()->HasPriv("chanserv/access/modify"))
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_CLEAR, ci->name.c_str());
			Alog() << Config->s_ChanServ << ": " << u->GetMask() << " (level " << get_access(u, ci) << " cleared access list on " << ci->name;
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
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_XOP_HOP, Config->s_ChanServ.c_str());
			else
				notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_XOP, Config->s_ChanServ.c_str());
		}
		else if ((is_list && !check_access(u, ci, CA_ACCESS_LIST) && !u->Account()->HasCommand("chanserv/access/list")) || (!is_list && !check_access(u, ci, CA_ACCESS_CHANGE) && !u->Account()->HasPriv("chanserv/access/modify")))
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
		else if (readonly && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL") || cmd.equals_ci("CLEAR")))
			notice_lang(Config->s_ChanServ, u, CHAN_ACCESS_DISABLED);
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
		notice_help(Config->s_ChanServ, u, CHAN_HELP_ACCESS);
		notice_help(Config->s_ChanServ, u, CHAN_HELP_ACCESS_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
	}
};

class CommandCSLevels : public Command
{
	CommandReturn DoSet(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string what = params[2];
		Anope::string lev = params[3];

		Anope::string error;
		int level = convertTo<int>(lev, error, false);

		if (lev.equals_ci("FOUNDER"))
		{
			level = ACCESS_FOUNDER;
			error.clear();
		}

		if (!error.empty())
			this->OnSyntaxError(u, "SET");
		else if (level <= ACCESS_INVALID || level > ACCESS_FOUNDER)
			notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
			for (int i = 0; levelinfo[i].what >= 0; ++i)
			{
				if (what.equals_ci(levelinfo[i].name))
				{
					ci->levels[levelinfo[i].what] = level;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, level));
					Alog() << Config->s_ChanServ << ": " << u->GetMask() << " set level " << levelinfo[i].name << " on channel " << ci->name << " to " << level;
					if (level == ACCESS_FOUNDER)
						notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_CHANGED_FOUNDER, levelinfo[i].name.c_str(), ci->name.c_str());
					else
						notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_CHANGED, levelinfo[i].name.c_str(), ci->name.c_str(), level);
					return MOD_CONT;
				}
			}

		notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what.c_str(), Config->s_ChanServ.c_str());

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

					Alog() << Config->s_ChanServ << ": " << u->GetMask() << " disabled level " << levelinfo[i].name << " on channel " << ci->name;
					notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_DISABLED, levelinfo[i].name.c_str(), ci->name.c_str());
					return MOD_CONT;
				}
			}

		notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what.c_str(), Config->s_ChanServ.c_str());

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, ChannelInfo *ci)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_LIST_HEADER, ci->name.c_str());

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
					notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED, levelinfo_maxwidth, levelinfo[i].name.c_str());
				else
					notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED, levelinfo_maxwidth, levelinfo[i].name.c_str());
			}
			else if (j == ACCESS_FOUNDER)
				notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_LIST_FOUNDER, levelinfo_maxwidth, levelinfo[i].name.c_str());
			else
				notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_LIST_NORMAL, levelinfo_maxwidth, levelinfo[i].name.c_str(), j);
		}

		return MOD_CONT;
	}

	CommandReturn DoReset(User *u, ChannelInfo *ci)
	{
		reset_levels(ci);
		FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, -1, 0));
		Alog() << Config->s_ChanServ << ": " << u->GetMask() << " reset levels definitions on channel " << ci->name;
		notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_RESET, ci->name.c_str());
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
			notice_lang(Config->s_ChanServ, u, CHAN_LEVELS_XOP);
		else if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("chanserv/access/modify"))
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
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
		notice_help(Config->s_ChanServ, u, CHAN_HELP_LEVELS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_ACCESS);
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_LEVELS);
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
