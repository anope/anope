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
	ACCESS_INVALID = -10000,
	ACCESS_FOUNDER = 10001
};

static struct AccessLevels
{
	ChannelAccess priv;
	int default_level;
	Anope::string config_name;
	Anope::string name;
	Anope::string desc;
} defaultLevels[] = {
	{ CA_ACCESS_CHANGE, 10, "level_change", "ACC-CHANGE", _("Allowed to modify the access list") },
	{ CA_ACCESS_LIST, 1, "level_list", "ACC-LIST", _("Allowed to view the access list") },
	{ CA_AKICK, 10, "level_akick", "AKICK", _("Allowed to use AKICK command") },
	{ CA_ASSIGN, ACCESS_FOUNDER, "level_assign", "ASSIGN", _("Allowed to assign/unassign a bot") },
	{ CA_AUTOHALFOP, 4, "level_autohalfop", "AUTOHALFOP", _("Automatic mode +h") },
	{ CA_AUTOOP, 5, "level_autoop", "AUTOOP", _("Automatic channel operator status") },
	{ CA_AUTOOWNER,	10000, "level_autoowner", "AUTOOWNER", _("Automatic mode +q") },
	{ CA_AUTOPROTECT, 10, "level_autoprotect", "AUTOPROTECT", _("Automatic mode +a") },
	{ CA_AUTOVOICE, 3, "level_autovoice", "AUTOVOICE", _("Automatic mode +v") },
	{ CA_BADWORDS, 10, "level_badwords", "BADWORDS", _("Allowed to modify channel badwords list") },
	{ CA_BAN, 4, "level_ban", "BAN", _("Allowed to use ban users") },
	{ CA_FANTASIA, 3, "level_fantasia", "FANTASIA", _("Allowed to use fantaisist commands") },
	{ CA_FOUNDER, ACCESS_FOUNDER, "level_founder", "FOUNDER", _("Allowed to issue commands restricted to channel founders") },
	{ CA_GETKEY, 5, "level_getkey", "GETKEY", _("Allowed to use GETKEY command") },
	{ CA_GREET, 5, "level_greet", "GREET", _("Greet message displayed") },
	{ CA_HALFOP, 5, "level_halfop", "HALFOP", _("Allowed to (de)halfop users") },
	{ CA_HALFOPME, 4, "level_halfopme", "HALFOPME", _("Allowed to (de)halfop him/herself") },
	{ CA_INFO, 10000, "level_info", "INFO", _("Allowed to use INFO command with ALL option") },
	{ CA_INVITE, 5, "level_invite", "INVITE", _("Allowed to use the INVITE command") },
	{ CA_KICK, 4, "level_kick", "KICK", _("Allowed to use the KICK command") },
	{ CA_MEMO, 10, "level_memo", "MEMO", _("Allowed to read channel memos") },
	{ CA_MODE, 5, "level_mode", "MODE", _("Allowed to change channel modes") },
	{ CA_NOKICK, 1, "level_nokick", "NOKICK", _("Never kicked by the bot's kickers") },
	{ CA_OPDEOP, 5, "level_opdeop", "OPDEOP", _("Allowed to (de)op users") },
	{ CA_OPDEOPME, 5, "level_opdeopme", "OPDEOPME", _("Allowed to (de)op him/herself") },
	{ CA_OWNER, ACCESS_FOUNDER, "level_owner", "OWNER", _("Allowed to use (de)owner users") },
	{ CA_OWNERME, 10000, "level_ownerme", "OWNERME", _("Allowed to (de)owner him/herself") },
	{ CA_PROTECT, 10000, "level_protect", "PROTECT", _("Allowed to (de)protect users") },
	{ CA_PROTECTME, 10, "level_protectme", "PROTECTME", _("Allowed to (de)protect him/herself"), },
	{ CA_SAY, 5, "level_say", "SAY", _("Allowed to use SAY and ACT commands") },
	{ CA_SIGNKICK, ACCESS_FOUNDER, "level_signkick", "SIGNKICK", _("No signed kick when SIGNKICK LEVEL is used") },
	{ CA_SET, 10000, "level_set", "SET", _("Allowed to set channel settings") },
	{ CA_TOPIC, 5, "level_topic", "TOPIC", _("Allowed to change channel topics") },
	{ CA_UNBAN, 4, "level_unban", "UNBAN", _("Allowed to unban users") },
	{ CA_VOICE, 4, "level_voice", "VOICE", _("Allowed to (de)voice users") },
	{ CA_VOICEME, 3, "level_voiceme", "VOICEME", _("Allowed to (de)voice him/herself") },
	{ CA_SIZE, -1, "", "", "" }
};

static void reset_levels(ChannelInfo *ci)
{
	for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
		ci->levels[defaultLevels[i].priv] = defaultLevels[i].default_level;
}

class AccessChanAccess : public ChanAccess
{
 public:
	int level;

 	AccessChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool Matches(User *u, NickCore *nc)
	{
		if (u && (Anope::Match(u->nick, this->mask) || Anope::Match(u->GetMask(), this->mask)))
			return true;
		else if (nc && Anope::Match(nc->display, this->mask))
			return true;
		return false;
	}

	bool HasPriv(ChannelAccess priv)
	{
		for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
			if (defaultLevels[i].priv == priv)
				return DetermineLevel(this) >= this->ci->levels[priv];
		return false;
	}

	Anope::string Serialize()
	{
		return stringify(this->level);
	}

	void Unserialize(const Anope::string &data)
	{
		this->level = convertTo<int>(data);
	}

	static int DetermineLevel(ChanAccess *access)
	{
		if (access->provider->name == "access/access")
		{
			AccessChanAccess *aaccess = debug_cast<AccessChanAccess *>(access);
			return aaccess->level;
		}
		else
		{
			int highest = 1;
			for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
				if (access->ci->levels[defaultLevels[i].priv] > highest && access->HasPriv(defaultLevels[i].priv))
					highest = access->ci->levels[defaultLevels[i].priv];
			return highest;
		}
	}
};

class AccessAccessProvider : public AccessProvider
{
 public:
	AccessAccessProvider(Module *o) : AccessProvider(o, "access/access")
	{
	}

	ChanAccess *Create()
	{
		return new AccessChanAccess(this);
	}
};

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
			source.Reply(_("End of access list."));
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
		source.Reply(_("  %3d  %4d  %s"), Number + 1, AccessChanAccess::DetermineLevel(access), access->mask.c_str());
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
		if (ci->c)
			for (CUserList::const_iterator cit = ci->c->users.begin(), cit_end = ci->c->users.end(); cit != cit_end; ++cit)
				if (access->Matches((*cit)->user, (*cit)->user->Account()))
					timebuf = "Now";
		if (timebuf.empty())
		{
			if (access->last_seen == 0)
				timebuf = "Never";
			else
				timebuf = do_strftime(access->last_seen);
		}

		source.Reply(CHAN_ACCESS_VIEW_AXS_FORMAT, Number + 1, AccessChanAccess::DetermineLevel(access), access->mask.c_str(), access->creator.c_str(), timebuf.c_str());
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
		if (!ci->HasPriv(source.u, CA_ACCESS_CHANGE) && source.u->HasPriv("chanserv/access/modify"))
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

		AccessGroup u_access = ci->AccessFor(u);
		ChanAccess *u_highest = u_access.Highest();

		if (u_highest ? AccessChanAccess::DetermineLevel(u_highest) : 0 <= AccessChanAccess::DetermineLevel(access) && !u->HasPriv("chanserv/access/modify"))
		{
			Denied = true;
			return;
		}

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->mask;
		else
			Nicks = access->mask;

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

		if (!level)
		{
			source.Reply(_("Access level must be non-zero."));
			return;
		}

		AccessGroup u_access = ci->AccessFor(u);
		ChanAccess *highest = u_access.Highest();
		int u_level = (highest ? AccessChanAccess::DetermineLevel(highest) : 0);
		if (!ci->HasPriv(u, CA_FOUNDER) && level >= u_level && !u->HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}
		else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
		{
			source.Reply(CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
			return;
		}

		bool override = !ci->HasPriv(u, CA_ACCESS_CHANGE) || level >= u_level;

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanAccess *access = ci->GetAccess(i - 1);
			if (mask.equals_ci(access->mask))
			{
				/* Don't allow lowering from a level >= u_level */
				if (AccessChanAccess::DetermineLevel(access) >= u_level && !u->HasPriv("chanserv/access/modify"))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
				ci->EraseAccess(i - 1);
				break;
			}
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel."), Config->CSAccessMax);
			return;
		}

		if (mask.find_first_of("!*@") == Anope::string::npos && findnick(mask) == NULL)
			mask += "!*@*";

		service_reference<AccessProvider> provider("access/access");
		if (!provider)
			return;
		AccessChanAccess *access = debug_cast<AccessChanAccess *>(provider->Create());
		access->ci = ci;
		access->mask = mask;
		access->creator = u->nick;
		access->level = level;
		access->last_seen = 0;
		access->created = Anope::CurTime;
		ci->AddAccess(access);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, access));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << " (level: " << level << ") as level " << u_level;
		source.Reply(_("\002%s\002 added to %s access list at level \002%d\002."), access->mask.c_str(), ci->name.c_str(), level);

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
			AccessGroup u_access = ci->AccessFor(u);
			ChanAccess *highest = u_access.Highest();
			int u_level = (highest ? AccessChanAccess::DetermineLevel(highest) : 0);

			for (unsigned i = ci->GetAccessCount(); i > 0; --i)
			{
				ChanAccess *access = ci->GetAccess(i - 1);
				if (mask.equals_ci(access->mask))
				{
					int access_level = AccessChanAccess::DetermineLevel(access);
					if (!access->mask.equals_ci(u->Account()->display) && u_level <= access_level && !u->HasPriv("chanserv/access/modify"))
						source.Reply(ACCESS_DENIED);
					else
					{
						source.Reply(_("\002%s\002 deleted from %s access list."), access->mask.c_str(), ci->name.c_str());
						bool override = !ci->HasPriv(u, CA_ACCESS_CHANGE) && !access->mask.equals_ci(u->Account()->display);
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << access->mask;

						FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access));
						ci->EraseAccess(access);
					}
					return;
				}
			}

			source.Reply(_("\002%s\002 not found on %s access list."), mask.c_str(), ci->name.c_str());
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

				if (!nick.empty() && !Anope::Match(access->mask, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessListCallback::DoList(source, ci, i, access);
			}

			if (SentHeader)
				source.Reply(_("End of access list."));
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

				if (!nick.empty() && !Anope::Match(access->mask, nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_ACCESS_LIST_HEADER, ci->name.c_str());
				}

				AccessViewCallback::DoList(source, ci, i, access);
			}

			if (SentHeader)
				source.Reply(_("End of access list."));
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
		else if (is_list && ci->HasPriv(u, CA_ACCESS_LIST))
			has_access = true;
		else if (ci->HasPriv(u, CA_ACCESS_CHANGE))
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
				"a user level of 0."), source.owner->nick.c_str());
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
				"access list."));
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
	int levelinfo_maxwidth;

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
			for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
			{
				AccessLevels &l = defaultLevels[i];

				if (what.equals_ci(l.name))
				{
					ci->levels[l.priv] = level;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, level));

					bool override = !ci->HasPriv(u, CA_FOUNDER);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "SET " << l.name << " to " << level;

					if (level == ACCESS_FOUNDER)
						source.Reply(_("Level for %s on channel %s changed to founder only."), l.name.c_str(), ci->name.c_str());
					else
						source.Reply(_("Level for \002%s\002 on channel %s changed to \002%d\002."), l.name.c_str(), ci->name.c_str(), level);
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
			for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
			{
				AccessLevels &l = defaultLevels[i];

				if (what.equals_ci(l.name))
				{
					ci->levels[l.priv] = ACCESS_INVALID;
					FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, i, l.priv));

					bool override = !ci->HasPriv(u, CA_FOUNDER);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DISABLE " << l.name;

					source.Reply(_("\002%s\002 disabled on channel %s."), l.name.c_str(), ci->name.c_str());
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
			for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
			{
				AccessLevels &l = defaultLevels[i];

				int len = l.name.length();
				if (len > levelinfo_maxwidth)
					levelinfo_maxwidth = len;
			}

		for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
		{
			AccessLevels &l = defaultLevels[i];
			int j = ci->levels[l.priv];

			if (j == ACCESS_INVALID)
				source.Reply(_("    %-*s  (disabled)"), levelinfo_maxwidth, l.name.c_str());
			else if (j == ACCESS_FOUNDER)
				source.Reply(_("    %-*s  (founder only)"), levelinfo_maxwidth, l.name.c_str());
			else
				source.Reply(_("    %-*s  %d"), levelinfo_maxwidth, l.name.c_str(), j);
		}

		return;
	}

	void DoReset(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;

		reset_levels(ci);
		FOREACH_MOD(I_OnLevelChange, OnLevelChange(u, ci, -1, 0));

		bool override = !ci->HasPriv(u, CA_FOUNDER);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "RESET";

		source.Reply(_("Access levels for \002%s\002 reset to defaults."), ci->name.c_str());
		return;
	}

 public:
	CommandCSLevels(Module *creator) : Command(creator, "chanserv/levels", 2, 4), levelinfo_maxwidth(0)
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
		else if (!ci->HasPriv(u, CA_FOUNDER) && !u->HasPriv("chanserv/access/modify"))
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
			source.Reply(_("The following feature/function names are understood."));
			if (!levelinfo_maxwidth)
				for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
				{
					AccessLevels &l = defaultLevels[i];

					int len = l.name.length();
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
			{
				AccessLevels &l = defaultLevels[i];
				source.Reply(_("    %-*s  %s"), levelinfo_maxwidth, l.name.c_str(), translate(source.u, l.desc.c_str()));
			}
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
	AccessAccessProvider accessprovider;
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;

 public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		accessprovider(this), commandcsaccess(this), commandcslevels(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&accessprovider);
		ModuleManager::RegisterService(&commandcsaccess);
		ModuleManager::RegisterService(&commandcslevels);

		Implementation i[] = { I_OnReload, I_OnChanRegistered };
		ModuleManager::Attach(i, this, 2);

		this->OnReload();
	}

	void OnReload()
	{
		ConfigReader config;

		for (int i = 0; defaultLevels[i].priv != CA_SIZE; ++i)
		{
			AccessLevels &l = defaultLevels[i];

			const Anope::string &value = config.ReadValue("chanserv", l.config_name, "", 0);
			if (value.equals_ci("founder"))
				l.default_level = ACCESS_FOUNDER;
			else
				l.default_level = config.ReadInteger("chanserv", l.config_name, 0, false);
		}
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		reset_levels(ci);
	}
};

MODULE_INIT(CSAccess)
