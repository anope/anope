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

class AccessListCallback : public NumberList
{
 protected:
	CommandSource &source;
	ChannelInfo *ci;
	bool SentHeader;
 public:
	AccessListCallback(CommandSource &_source, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), source(_source), ci(_ci), SentHeader(false)
	{
	}

	~AccessListCallback()
	{
		if (SentHeader)
			source.Reply(_("End of access list."), ci->name.c_str());
		else
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(source, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(CommandSource &source, ChannelInfo *ci, unsigned Number, ChanAccess *access)
	{
		if (ci->HasFlag(CI_XOP))
		{
			Anope::string xop = get_xop_level(access->level);
			source.Reply(_("  %3d   %s  %s"), Number + 1, xop.c_str(), access->GetMask().c_str());
		}
		else
			source.Reply(_("  %3d  %4d  %s"), Number + 1, access->level, access->GetMask().c_str());
	}
};

class AccessViewCallback : public AccessListCallback
{
 public:
	AccessViewCallback(CommandSource &_source, ChannelInfo *_ci, const Anope::string &numlist) : AccessListCallback(_source, _ci, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
		}

		DoList(source, ci, Number - 1, ci->GetAccess(Number - 1));
	}

	static void DoList(CommandSource &source, ChannelInfo *ci, unsigned Number, ChanAccess *access)
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
			source.Reply(CHAN_ACCESS_VIEW_XOP_FORMAT, Number + 1, xop.c_str(), access->GetMask().c_str(), access->creator.c_str(), timebuf.c_str());
		}
		else
			source.Reply(CHAN_ACCESS_VIEW_AXS_FORMAT, Number + 1, access->level, access->GetMask().c_str(), access->creator.c_str(), timebuf.c_str());
	}
};

class AccessDelCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	Command *c;
	unsigned Deleted;
	Anope::string Nicks;
	bool Denied;
	bool override;
 public:
	AccessDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &numlist) : NumberList(numlist, true), source(_source), ci(_ci), c(_c), Deleted(0), Denied(false)
	{
		if (!check_access(source.u, ci, CA_ACCESS_CHANGE) && source.u->HasPriv("chanserv/access/modify"))
			this->override = true;
	}

	~AccessDelCallback()
	{
		if (Denied && !Deleted)
			source.Reply(ACCESS_DENIED);
		else if (!Deleted)
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, c, ci) << "for user" << (Deleted == 1 ? " " : "s ") << Nicks;

			if (Deleted == 1)
				source.Reply(_("Deleted 1 entry from %s access list."), ci->name.c_str());
			else
				source.Reply(_("Deleted %d entries from %s access list."), Deleted, ci->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAccessCount())
			return;

		User *u = source.u;

		ChanAccess *access = ci->GetAccess(Number - 1);

		ChanAccess *u_access = ci->GetAccess(u);
		int16 u_level = u_access ? u_access->level : 0;
		if (u_level <= access->level && !u->HasPriv("chanserv/access/modify"))
		{
			Denied = true;
			return;
		}

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->GetMask();
		else
			Nicks = access->GetMask();

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access));

		ci->EraseAccess(Number - 1);
	}
};

class CommandCSAccess : public Command
{
	void DoAdd(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string mask = params[2];
		int level = ACCESS_INVALID;
		
		try
		{
			level = convertTo<int>(params[3]);
		}
		catch (const ConvertException &) { }

		ChanAccess *u_access = ci->GetAccess(u);
		int16 u_level = u_access ? u_access->level : 0;
		if (level >= u_level && !u->HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!level)
		{
			source.Reply(_("Access level must be non-zero."));
			return;
		}
		else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
		{
			source.Reply(CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
			return;
		}

		bool override = !check_access(u, ci, CA_ACCESS_CHANGE) || level >= u_level;

		ChanAccess *access = ci->GetAccess(mask, 0, false);
		if (access)
		{
			/* Don't allow lowering from a level >= u_level */
			if (access->level >= u_level && !u->HasPriv("chanserv/access/modify"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			if (access->level == level)
			{
				source.Reply(_("Access level for \002%s\002 on %s unchanged from \002%d\002."), access->GetMask().c_str(), ci->name.c_str(), level);
				return;
			}
			access->level = level;

			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, access));

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << " (level: " << level << ") as level " << u_level;
			source.Reply(_("Access level for \002%s\002 on %s changed to \002%d\002."), access->GetMask().c_str(), ci->name.c_str(), level);
			return;
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel."), Config->CSAccessMax);
			return;
		}

		access = ci->AddAccess(mask, level, u->nick);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, access));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << " (level: " << level << ") as level " << u_level;
		source.Reply(_("\002%s\002 added to %s access list at level \002%d\002."), access->GetMask().c_str(), ci->name.c_str(), level);

		return;
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &mask = params[2];

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessDelCallback list(source, ci, this, mask);
			list.Process();
		}
		else
		{
			ChanAccess *access = ci->GetAccess(mask, 0, false);
			ChanAccess *u_access = ci->GetAccess(u);
			int16 u_level = u_access ? u_access->level : 0;
			if (!access)
				source.Reply(_("\002%s\002 not found on %s access list."), mask.c_str(), ci->name.c_str());
			else if (access->nc != u->Account() && check_access(u, ci, CA_NOJOIN) && u_level <= access->level && !u->HasPriv("chanserv/access/modify"))
				source.Reply(ACCESS_DENIED);
			else
			{
				source.Reply(_("\002%s\002 deleted from %s access list."), access->GetMask().c_str(), ci->name.c_str());
				bool override = !check_access(u, ci, CA_ACCESS_CHANGE) && access->nc != u->Account();
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << access->GetMask() << " from level " << access->level;

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access));

				ci->EraseAccess(access);
			}
		}

		return;
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessListCallback list(source, ci, nick);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && !Anope::Match(access->GetMask(), nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessListCallback::DoList(source, ci, i, access);
			}

			if (SentHeader)
				source.Reply(_("End of access list."), ci->name.c_str());
			else
				source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		}

		return;
	}

	void DoView(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AccessViewCallback list(source, ci, nick);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && !Anope::Match(access->GetMask(), nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessViewCallback::DoList(source, ci, i, access);
			}

			if (SentHeader)
				source.Reply(_("End of access list."), ci->name.c_str());
			else
				source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		}

		return;
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;

		if (!IsFounder(u, ci) && !u->HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

			source.Reply(_("Channel %s access list has been cleared."), ci->name.c_str());

			bool override = !IsFounder(u, ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";
		}

		return;
	}

 public:
	CommandCSAccess(Module *creator) : Command(creator, "chanserv/access", 2, 4)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 ADD \037mask\037 \037level\037"));
		this->SetSyntax(_("\037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[1];
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");
		bool is_clear = cmd.equals_ci("CLEAR");
		bool is_del = cmd.equals_ci("DEL");

		bool has_access = false;
		if (u->HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && check_access(u, ci, CA_ACCESS_LIST))
			has_access = true;
		else if (check_access(u, ci, CA_ACCESS_CHANGE))
			has_access = true;
		else if (is_del)
		{
			NickAlias *na = findnick(nick);
			if (na && na->nc == u->Account())
				has_access = true;
		}

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || is_clear ? 0 : (cmd.equals_ci("DEL") ? (nick.empty() || !s.empty()) : s.empty()))
			this->OnSyntaxError(source, cmd);
		else if (!has_access)
			source.Reply(ACCESS_DENIED);
		/* We still allow LIST and CLEAR in xOP mode, but not others */
		else if (ci->HasFlag(CI_XOP) && !is_list && !is_clear)
		{
			if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
				source.Reply(_("You can't use this command. \n"
						"Use the AOP, SOP, HOP and VOP commands instead.\n"
						"Type \002%s%s HELP \037command\037\002 for more information."), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
			else
				source.Reply(_("You can't use this command. \n"
						"Use the AOP, SOP and VOP commands instead.\n"
						"Type \002%s%s HELP \037command\037\002 for more information."), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		}
		else if (readonly && !is_list)
			source.Reply(_("Sorry, channel access list modification is temporarily disabled."));
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002access list\002 for a channel.  The access\n"
				"list specifies which users are allowed chanop status or\n"
				"access to %s commands on the channel.  Different\n"
				"user levels allow for access to different subsets of\n"
				"privileges. Any nick not on the access list has\n"
				"a user level of 0."));
		source.Reply(" ");
		source.Reply(_("The \002ACCESS ADD\002 command adds the given mask to the\n"
				"access list with the given user level; if the mask is\n"
				"already present on the list, its access level is changed to\n"
				"the level specified in the command.  The \037level\037 specified\n"
				"must be less than that of the user giving the command, and\n"
				"if the \037mask\037 is already on the access list, the current\n"
				"access level of that nick must be less than the access level\n"
				"of the user giving the command. When a user joins the channel\n"
				"the access they receive is from the highest level entry in the\n"
				"access list."));
		source.Reply(" ");
		source.Reply(_("The \002ACCESS DEL\002 command removes the given nick from the\n"
				"access list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				"You may remove yourself from an access list, even if you\n"
				"do not have access to modify that list otherwise."));
		source.Reply(" ");
		source.Reply(_("The \002ACCESS LIST\002 command displays the access list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002ACCESS #channel LIST 2-5,7-9\002\n"
				"      Lists access entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				" \n"
				"The \002ACCESS VIEW\002 command displays the access list similar\n"
				"to \002ACCESS LIST\002 but shows the creator and last used time.\n"
				" \n"
				"The \002ACCESS CLEAR\002 command clears all entries of the\n"
				"access list."),
				source.owner->nick.c_str());
		source.Reply(_("\002User access levels\002\n"
				" \n"
				"By default, the following access levels are defined:\n"
				" \n"
				"   \002Founder\002   Full access to %s functions; automatic\n"
				"                     opping upon entering channel.  Note\n"
				"                     that only one person may have founder\n"
				"                     status (it cannot be given using the\n"
				"                     \002ACCESS\002 command).\n"
				"   \002     10\002   Access to AKICK command; automatic opping.\n"
				"   \002      5\002   Automatic opping.\n"
				"   \002      3\002   Automatic voicing.\n"
				"   \002      0\002   No special privileges; can be opped by other\n"
				"                     ops (unless \002secure-ops\002 is set).\n"
				"   \002     <0\002   May not be opped.\n"
				" \n"
				"These levels may be changed, or new ones added, using the\n"
				"\002LEVELS\002 command; type \002%s%s HELP LEVELS\002 for\n"
				"information."), source.owner->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		return true;
	}
};

class CommandCSLevels : public Command
{
	void DoSet(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &what = params[2];
		const Anope::string &lev = params[3];

		int level;

		if (lev.equals_ci("FOUNDER"))
			level = ACCESS_FOUNDER;
		else
		{
			level = 1;
			try
			{
				level = convertTo<int>(lev);
			}
			catch (const ConvertException &)
			{
				this->OnSyntaxError(source, "SET");
				return;
			}
		}

		if (level <= ACCESS_INVALID || level > ACCESS_FOUNDER)
			source.Reply(_("Level must be between %d and %d inclusive."), ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
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
						source.Reply(_("Level for %s on channel %s changed to founder only."), levelinfo[i].name.c_str(), ci->name.c_str());
					else
						source.Reply(_("Level for \002%s\002 on channel %s changed to \002%d\002."), levelinfo[i].name.c_str(), ci->name.c_str(), level);
					return;
				}
			}

			source.Reply(_("Setting \002%s\002 not known.  Type \002%s%s HELP LEVELS \002 for a list of valid settings."), what.c_str(), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		}

		return;
	}

	void DoDisable(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &what = params[2];

		/* Don't allow disabling of the founder level. It would be hard to change it back if you dont have access to use this command */
		if (!what.equals_ci("FOUNDER"))
			for (int i = 0; levelinfo[i].what >= 0; ++i)
			{
				if (what.equals_ci(levelinfo[i].name))
				{
					ci->levels[levelinfo[i].what] = ACCESS_INVALID;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, levelinfo[i].what));

					bool override = !check_access(u, ci, CA_FOUNDER);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DISABLE " << levelinfo[i].name;

					source.Reply(_("\002%s\002 disabled on channel %s."), levelinfo[i].name.c_str(), ci->name.c_str());
					return;
				}
			}

		source.Reply(_("Setting \002%s\002 not known.  Type \002%s%s HELP LEVELS \002 for a list of valid settings."), what.c_str(), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());

		return;
	}

	void DoList(CommandSource &source, ChannelInfo *ci)
	{
		source.Reply(_("Access level settings for channel %s:"), ci->name.c_str());

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

				source.Reply(_("    %-*s  (disabled)"), levelinfo_maxwidth, levelinfo[i].name.c_str());
			}
			else if (j == ACCESS_FOUNDER)
				source.Reply(_("    %-*s  (founder only)"), levelinfo_maxwidth, levelinfo[i].name.c_str());
			else
				source.Reply(_("    %-*s  %d"), levelinfo_maxwidth, levelinfo[i].name.c_str(), j);
		}

		return;
	}

	void DoReset(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;

		reset_levels(ci);
		FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, -1, 0));

		bool override = !check_access(u, ci, CA_FOUNDER);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "RESET";

		source.Reply(_("Access levels for \002%s\002 reset to defaults."), ci->name.c_str());
		return;
	}

 public:
	CommandCSLevels(Module *creator) : Command(creator, "chanserv/levels", 2, 4)
	{
		this->SetDesc(_("Redefine the meanings of access levels"));
		this->SetSyntax(_("\037channel\037 SET \037type\037 \037level\037"));
		this->SetSyntax(_("\037channel\037 {DIS | DISABLE} \037type\037"));
		this->SetSyntax(_("\037channel\037 LIST"));
		this->SetSyntax(_("\037channel\037 RESET"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[1];
		const Anope::string &what = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		/* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
		 * one; else, we want none.
		 */
		if (cmd.equals_ci("SET") ? s.empty() : (cmd.substr(0, 3).equals_ci("DIS") ? (what.empty() || !s.empty()) : !what.empty()))
			this->OnSyntaxError(source, cmd);
		else if (ci->HasFlag(CI_XOP))
			source.Reply(_("Levels are not available as xOP is enabled on this channel."));
		else if (!check_access(u, ci, CA_FOUNDER) && !u->HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else if (cmd.equals_ci("SET"))
			this->DoSet(source, ci, params);
		else if (cmd.equals_ci("DIS") || cmd.equals_ci("DISABLE"))
			this->DoDisable(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci);
		else if (cmd.equals_ci("RESET"))
			this->DoReset(source, ci);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.equals_ci("DESC"))
		{
			int i;
			source.Reply(_("The following feature/function names are understood. Note\n"
					"that the leves for NOJOIN is the maximum level,\n"
					"while all others are minimum levels."));
			if (!levelinfo_maxwidth)
				for (i = 0; levelinfo[i].what >= 0; ++i)
				{
					int len = levelinfo[i].name.length();
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			for (i = 0; levelinfo[i].what >= 0; ++i)
				source.Reply(_("    %-*s  %s"), levelinfo_maxwidth, levelinfo[i].name.c_str(), translate(source.u, levelinfo[i].desc));
		}
		else
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_("The \002LEVELS\002 command allows fine control over the meaning of\n"
					"the numeric access levels used for channels.  With this\n"
					"command, you can define the access level required for most\n"
					"of %s's functions. (The \002SET FOUNDER\002 and this command\n"
					"are always restricted to the channel founder.)\n"
					" \n"
					"\002LEVELS SET\002 allows the access level for a function or group of\n"
					"functions to be changed. \002LEVELS DISABLE\002 (or \002DIS\002 for short)\n"
					"disables an automatic feature or disallows access to a\n"
					"function by anyone, INCLUDING the founder (although, the founder\n"
					"can always reenable it).\n"
					" \n"
					"\002LEVELS LIST\002 shows the current levels for each function or\n"
					"group of functions. \002LEVELS RESET\002 resets the levels to the\n"
					"default levels of a newly-created channel (see\n"
					"\002HELP ACCESS LEVELS\002).\n"
					" \n"
					"For a list of the features and functions whose levels can be\n"
					"set, see \002HELP LEVELS DESC\002."), source.owner->nick.c_str());
		}
		return true;
	}
};

class CSAccess : public Module
{
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;

 public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsaccess(this), commandcslevels(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcsaccess);
		ModuleManager::RegisterService(&commandcslevels);
	}
};

MODULE_INIT(CSAccess)
