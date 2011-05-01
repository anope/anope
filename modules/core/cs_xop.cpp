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

class XOPListCallback : public NumberList
{
	CommandSource &source;
	int level;
	Command *c;
	bool SentHeader;
 public:
	XOPListCallback(CommandSource &_source, const Anope::string &numlist, int _level, Command *_c) : NumberList(numlist, false), source(_source), level(_level), c(_c), SentHeader(false)
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
			source.Reply(_("%s list for %s:\n  Num  Nick"), this->c->name.c_str(), source.ci->name.c_str());
		}

		DoList(source, access, Number - 1, level);
	}

	static void DoList(CommandSource &source, ChanAccess *access, unsigned index, int level)
	{
		source.Reply(_("  %3d  %s"), index, access->GetMask().c_str());
	}
};

class XOPDelCallback : public NumberList
{
	CommandSource &source;
	Command *c;
	unsigned Deleted;
	Anope::string Nicks;
	bool override;
 public:
	XOPDelCallback(CommandSource &_source, Command *_c, bool _override, const Anope::string &numlist) : NumberList(numlist, true), source(_source), c(_c), Deleted(0), override(_override)
	{
	}

	~XOPDelCallback()
	{
		if (!Deleted)
			 source.Reply(_("No matching entries on %s %s list."), source.ci->name.c_str(), this->c->name.c_str());
		else
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, c, source.ci) << "deleted access of users " << Nicks;

			if (Deleted == 1)
				source.Reply(_("Deleted one entry from %s %s list."), source.ci->name.c_str(), this->c->name.c_str());
			else
				source.Reply(_("Deleted %d entries from %s %s list."), Deleted, source.ci->name.c_str(), this->c->name.c_str());
		}
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAccessCount())
			return;

		ChanAccess *access = source.ci->GetAccess(Number - 1);

		++Deleted;
		if (!Nicks.empty())
			Nicks += ", " + access->GetMask();
		else
			Nicks = access->GetMask();

		FOREACH_MOD(I_OnAccessDel, OnAccessDel(source.ci, source.u, access));

		source.ci->EraseAccess(Number - 1);
	}
};

class XOPBase : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params, int level)
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
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), this->name.c_str());
			return MOD_CONT;
		}

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if ((level >= ulev || ulev < ACCESS_AOP) && !u->HasPriv("chanserv/access/modify"))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		NickAlias *na = findnick(mask);
		if (!na && mask.find_first_of("!@*") == Anope::string::npos)
			mask += "!*@*";
		else if (na && na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
			return MOD_CONT;
		}

		access = ci->GetAccess(mask, 0, false);
		if (access)
		{
			/**
			 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
			 **/
			if (access->level >= ulev && !u->HasPriv("chanserv/access/modify"))
			{
				source.Reply(_(ACCESS_DENIED));
				return MOD_CONT;
			}
			++change;
		}

		if (!change && ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d %s entries on a channel."), Config->CSAccessMax, this->name.c_str());
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
			source.Reply(("\002%s\002 added to %s %s list."), access->GetMask().c_str(), ci->name.c_str(), this->name.c_str());
		}
		else
		{
			FOREACH_MOD(I_OnAccessChange, OnAccessChange(ci, u, access));
			source.Reply(_("\002%s\002 moved to %s %s list."), access->GetMask().c_str(), ci->name.c_str(), this->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params, int level)
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
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), this->name.c_str());
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), this->name.c_str());
			return MOD_CONT;
		}

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if ((!access || access->nc != u->Account()) && (level >= ulev || ulev < ACCESS_AOP) && !u->HasPriv("chanserv/access/modify"))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		access = ci->GetAccess(mask, 0, false);

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			bool override = level >= ulev || ulev < ACCESS_AOP;
			XOPDelCallback list(source, this, override, mask);
			list.Process();
		}
		else if (!access || access->level != level)
		{
			source.Reply(_("\002%s\002 not found on %s %s list."), mask.c_str(), ci->name.c_str(), this->name.c_str());
			return MOD_CONT;
		}
		else
		{
			if (access->nc != u->Account() && ulev <= access->level && !u->HasPriv("chanserv/access/modify"))
				source.Reply(_(ACCESS_DENIED));
			else
			{
				bool override = ulev <= access->level;
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << access->GetMask();

				source.Reply(_("\002%s\002 deleted from %s %s list."), access->GetMask().c_str(), ci->name.c_str(), this->name.c_str());

				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, access));

				ci->EraseAccess(access);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params, int level)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		ChanAccess *access = ci->GetAccess(u);
		uint16 ulev = access ? access->level : 0;

		if (!ulev && !u->HasCommand("chanserv/access/list"))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		bool override = !ulev;
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), this->name.c_str());
			return MOD_CONT;
		}

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			XOPListCallback list(source, nick, level, this);
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
				else if (!nick.empty() && !Anope::Match(access->GetMask(), nick))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(_("%s list for %s:\n  Num  Nick"), this->name.c_str(), ci->name.c_str());
				}

				XOPListCallback::DoList(source, access, i + 1, level);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s %s list."), ci->name.c_str(), this->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source, int level)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), this->name.c_str());
			return MOD_CONT;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), this->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_FOUNDER) && !u->HasPriv("chanserv/access/modify"))
		{
			source.Reply(_(ACCESS_DENIED));
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

		source.Reply(_("Channel %s %s list has been cleared."), ci->name.c_str(), this->name.c_str());

		return MOD_CONT;
	}

 protected:
	CommandReturn DoXop(CommandSource &source, const std::vector<Anope::string> &params, int level)
	{
		ChannelInfo *ci = source.ci;

		const Anope::string &cmd = params[1];

		if (!ci->HasFlag(CI_XOP))
			source.Reply(_("You can't use this command. Use the ACCESS command instead.\n"
					"Type \002%s%s HELP ACCESS\002 for more information."), Config->UseStrictPrivMsgString.c_str(), Config->s_ChanServ.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params, level);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params, level);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params, level);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, level);
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
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP() : XOPBase("QOP")
	{
		this->SetDesc(_("Modify the list of QOP users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_QOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002QOP \037channel\037 ADD \037mask\037\002\n"
				"        \002QOP \037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002QOP \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002QOP \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002QOP\002 (AutoOwner) \002list\002 for a channel. The QOP \n"
				"list gives users the right to be auto-owner on your channel,\n"
				"which gives them almost (or potentially, total) access.\n"
				" \n"
				"The \002QOP ADD\002 command adds the given nickname to the\n"
				"QOP list.\n"
				" \n"
				"The \002QOP DEL\002 command removes the given nick from the\n"
				"QOP list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002QOP LIST\002 command displays the QOP list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002QOP #channel LIST 2-5,7-9\002\n"
				"      Lists QOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002QOP CLEAR\002 command clears all entries of the\n"
				"QOP list.\n"));
		source.Reply(_(" \n"
				"The \002QOP\002 commands are limited to\n"
				"founders (unless SECUREOPS is off). However, any user on the\n"
				"QOP list may use the \002QOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("This command may have been disabled for your channel, and\n"
				"in that case you need to use the access list. See \n"
				"\002%s%s HELP ACCESS\002 for information about the access list,\n"
				"and \002%s%s HELP SET XOP\002 to know how to toggle between \n"
				"the access list and xOP list systems."),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "QOP", _("QOP \037channel\037 {ADD|DEL|LIST|CLEAR} [\037nick\037 | \037entry-list\037]"));
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP() : XOPBase("AOP")
	{
		this->SetDesc(_("Modify the list of AOP users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_AOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002AOP \037channel\037 ADD \037mask\037\002\n"
				"        \002AOP \037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002AOP \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002AOP \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002AOP\002 (AutoOP) \002list\002 for a channel. The AOP \n"
				"list gives users the right to be auto-opped on your channel,\n"
				"to unban or invite themselves if needed, to have their\n"
				"greet message showed on join, and so on.\n"
				" \n"
				"The \002AOP ADD\002 command adds the given nickname to the\n"
				"AOP list.\n"
				" \n"
				"The \002AOP DEL\002 command removes the given nick from the\n"
				"AOP list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002AOP LIST\002 command displays the AOP list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002AOP #channel LIST 2-5,7-9\002\n"
				"      Lists AOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002AOP CLEAR\002 command clears all entries of the\n"
				"AOP list.\n"));
		source.Reply(_(" \n"
				"The \002AOP ADD\002 and \002AOP DEL\002 commands are limited to\n"
				"SOPs or above, while the \002AOP CLEAR\002 command can only\n"
				"be used by the channel founder. However, any user on the\n"
				"AOP list may use the \002AOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("This command may have been disabled for your channel, and\n"
				"in that case you need to use the access list. See \n"
				"\002%s%s HELP ACCESS\002 for information about the access list,\n"
				"and \002%s%s HELP SET XOP\002 to know how to toggle between \n"
				"the access list and xOP list systems."),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str(), 
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AOP", _("AOP \037channel\037 {ADD|DEL|LIST|CLEAR} [\037nick\037 | \037entry-list\037]"));
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP() : XOPBase("HOP")
	{
		this->SetDesc(_("Maintains the HOP (HalfOP) list for a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_HOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002HOP \037channel\037 ADD \037mask\037\002\n"
				"        \002HOP \037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002HOP \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002HOP \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002HOP\002 (HalfOP) \002list\002 for a channel. The HOP \n"
				"list gives users the right to be auto-halfopped on your \n"
				"channel.\n"
				" \n"
				"The \002HOP ADD\002 command adds the given nickname to the\n"
				"HOP list.\n"
				" \n"
				"The \002HOP DEL\002 command removes the given nick from the\n"
				"HOP list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002HOP LIST\002 command displays the HOP list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002HOP #channel LIST 2-5,7-9\002\n"
				"      Lists HOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002HOP CLEAR\002 command clears all entries of the\n"
				"HOP list.\n"));
		source.Reply(_(" \n"
				"The \002HOP ADD\002, \002HOP DEL\002 and \002HOP LIST\002 commands are \n"
				"limited to AOPs or above, while the \002HOP CLEAR\002 command \n"
				"can only be used by the channel founder.\n"
				" \n"));
		source.Reply(_("This command may have been disabled for your channel, and\n"
				"in that case you need to use the access list. See \n"
				"\002%s%s HELP ACCESS\002 for information about the access list,\n"
				"and \002%s%s HELP SET XOP\002 to know how to toggle between \n"
				"the access list and xOP list systems."),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "HOP", _("HOP \037channel\037 {ADD|DEL|LIST|CLEAR} [\037nick\037 | \037entry-list\037]"));
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP() : XOPBase("SOP")
	{
		this->SetDesc(_("Modify the list of SOP users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_SOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SOP \037channel\037 ADD \037mask\037\002\n"
				"        \002SOP \037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002SOP \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002SOP \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002SOP\002 (SuperOP) \002list\002 for a channel. The SOP \n"
				"list gives users all rights given by the AOP list, and adds\n"
				"those needed to use the AutoKick and the BadWords lists, \n"
				"to send and read channel memos, and so on.\n"
				" \n"
				"The \002SOP ADD\002 command adds the given nickname to the\n"
				"SOP list.\n"
				" \n"
				"The \002SOP DEL\002 command removes the given nick from the\n"
				"SOP list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002SOP LIST\002 command displays the SOP list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002SOP #channel LIST 2-5,7-9\002\n"
				"      Lists AOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002SOP CLEAR\002 command clears all entries of the\n"
				"SOP list.\n"));
		source.Reply(_(
				" \n"
				"The \002SOP ADD\002, \002SOP DEL\002 and \002SOP CLEAR\002 commands are \n"
				"limited to the channel founder. However, any user on the\n"
				"AOP list may use the \002SOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("This command may have been disabled for your channel, and\n"
				"in that case you need to use the access list. See \n"
				"\002%s%s HELP ACCESS\002 for information about the access list,\n"
				"and \002%s%s HELP SET XOP\002 to know how to toggle between \n"
				"the access list and xOP list systems."),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SOP", _("SOP \037channel\037 {ADD|DEL|LIST|CLEAR} [\037nick\037 | \037entry-list\037]"));
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP() : XOPBase("VOP")
	{
		this->SetDesc(_("Maintains the VOP (VOicePeople) list for a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoXop(source, params, ACCESS_VOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002VOP \037channel\037 ADD \037mask\037\002\n"
				"        \002VOP \037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002VOP \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002VOP \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002VOP\002 (VOicePeople) \002list\002 for a channel.  \n"
				"The VOP list allows users to be auto-voiced and to voice \n"
				"themselves if they aren't.\n"
				" \n"
				"The \002VOP ADD\002 command adds the given nickname to the\n"
				"VOP list.\n"
				" \n"
				"The \002VOP DEL\002 command removes the given nick from the\n"
				"VOP list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002VOP LIST\002 command displays the VOP list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002VOP #channel LIST 2-5,7-9\002\n"
				"      Lists VOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002VOP CLEAR\002 command clears all entries of the\n"
				"VOP list.\n"));
		source.Reply(_(
				" \n"
				"The \002VOP ADD\002, \002VOP DEL\002 and \002VOP LIST\002 commands are \n"
				"limited to AOPs or above, while the \002VOP CLEAR\002 command \n"
				"can only be used by the channel founder.\n"
				" \n"));
		source.Reply(_("This command may have been disabled for your channel, and\n"
				"in that case you need to use the access list. See \n"
				"\002%s%s HELP ACCESS\002 for information about the access list,\n"
				"and \002%s%s HELP SET XOP\002 to know how to toggle between \n"
				"the access list and xOP list systems."),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "VOP", _("VOP \037channel\037 {ADD|DEL|LIST|CLEAR} [\037nick\037 | \037entry-list\037]"));
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
