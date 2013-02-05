/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

enum XOPType
{
	XOP_QOP,
	XOP_SOP,
	XOP_AOP,
	XOP_HOP,
	XOP_VOP,
	XOP_UNKNOWN
};

static struct XOPAccess
{
	XOPType type;
	Anope::string name;
	Anope::string access[10];
} xopAccess[] = {
	{ XOP_QOP, "QOP",
		{
			"SIGNKICK",
			"SET",
			"MODE",
			"AUTOOWNER",
			"OWNERME",
			"PROTECT",
			"INFO",
			"ASSIGN",
			""
		}
	},
	{ XOP_SOP, "SOP",
		{
			"AUTOPROTECT",
			"AKICK",
			"BADWORDS",
			"MEMO",
			"ACCESS_CHANGE",
			"PROTECTME",
			"OPDEOP",
			""
		}
	},
	{ XOP_AOP, "AOP",
		{
			"TOPIC",
			"GETKEY",
			"INVITE",
			"AUTOOP",
			"OPDEOPME",
			"HALFOP",
			"SAY",
			"GREET",
			""
		}
	},
	{ XOP_HOP, "HOP",
		{
			"AUTOHALFOP",
			"HALFOPME",
			"VOICE",
			"KICK",
			"BAN",
			"UNBAN",
			""
		}
	},
	{ XOP_VOP, "VOP",
		{
			"AUTOVOICE",
			"VOICEME",
			"ACCESS_LIST",
			"FANTASIA",
			"NOKICK",
			""
		}
	},
	{ XOP_UNKNOWN, "", { }
	}
};

class XOPChanAccess : public ChanAccess
{
 public:
	XOPType type;

 	XOPChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool HasPriv(const Anope::string &priv) const anope_override
	{
		for (int i = 0; xopAccess[i].type != XOP_UNKNOWN; ++i)
		{
			XOPAccess &x = xopAccess[i];

			if (this->type > x.type)
				continue;

			for (int j = 0; !x.access[j].empty(); ++j)
				if (x.access[j] == priv)
					return true;
		}

		return false;
	}

	Anope::string AccessSerialize() const
	{
		for (int i = 0; xopAccess[i].type != XOP_UNKNOWN; ++i)
		{
			XOPAccess &x = xopAccess[i];

			if (this->type == x.type)
				return x.name;
		}

		return "";
	}

	void AccessUnserialize(const Anope::string &data) anope_override
	{
		for (int i = 0; xopAccess[i].type != XOP_UNKNOWN; ++i)
		{
			XOPAccess &x = xopAccess[i];

			if (data == x.name)
			{
				this->type = x.type;
				return;
			}
		}

		this->type = XOP_UNKNOWN;
	}

	static XOPType DetermineLevel(const ChanAccess *access)
	{
		if (access->provider->name == "access/xop")
		{
			const XOPChanAccess *xaccess = anope_dynamic_static_cast<const XOPChanAccess *>(access);
			return xaccess->type;
		}
		else
		{
			int count[XOP_UNKNOWN];
			for (int i = 0; i < XOP_UNKNOWN; ++i)
				count[i] = 0;

			for (int i = 0; xopAccess[i].type != XOP_UNKNOWN; ++i)
			{
				XOPAccess &x = xopAccess[i];

				for (int j = 0; !x.access[j].empty(); ++j)
					if (access->HasPriv(x.access[j]))
						++count[x.type];
			}

			XOPType max = XOP_UNKNOWN;
			int maxn = 0;
			for (int i = 0; i < XOP_UNKNOWN; ++i)
				if (count[i] > maxn)
				{
					max = static_cast<XOPType>(i);
					maxn = count[i];
				}

			return max;
		}
	}
};

class XOPAccessProvider : public AccessProvider
{
 public:
	XOPAccessProvider(Module *o) : AccessProvider(o, "access/xop")
	{
	}

	ChanAccess *Create() anope_override
	{
		return new XOPChanAccess(this);
	}
};

class XOPBase : public Command
{
 private:
	void DoAdd(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params, XOPType level)
	{

		Anope::string mask = params.size() > 2 ? params[2] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		XOPChanAccess tmp_access(NULL);
		tmp_access.ci = ci;
		tmp_access.type = level;

		AccessGroup access = source.AccessFor(ci);
		const ChanAccess *highest = access.Highest();
		bool override = false;

		if ((!access.founder && !access.HasPriv("ACCESS_CHANGE")) || ((!highest || *highest <= tmp_access) && !access.founder))
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}

		if (mask.find_first_of("!*@") == Anope::string::npos && !NickAlias::Find(mask))
		{
			User *targ = User::Find(mask, true);
			if (targ != NULL)
				mask = "*!*@" + targ->GetDisplayedHost();
			else
			{
				source.Reply(NICK_X_NOT_REGISTERED, mask.c_str());
				return;
			}
		}

		for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
		{
			const ChanAccess *a = ci->GetAccess(i);

			if (a->mask.equals_ci(mask))
			{
				if ((!highest || *a >= *highest) && !access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}

				ci->EraseAccess(i);
				break;
			}
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d %s entries on a channel."), Config->CSAccessMax, source.command.c_str());
			return;
		}

		ServiceReference<AccessProvider> provider("AccessProvider", "access/xop");
		if (!provider)
			return;
		XOPChanAccess *acc = anope_dynamic_static_cast<XOPChanAccess *>(provider->Create());
		acc->ci = ci;
		acc->mask = mask;
		acc->creator = source.GetNick();
		acc->type = level;
		acc->last_seen = 0;
		acc->created = Anope::CurTime;
		ci->AddAccess(acc);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask;

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, source, acc));
		source.Reply(_("\002%s\002 added to %s %s list."), acc->mask.c_str(), ci->name.c_str(), source.command.c_str());
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params, XOPType level)
	{
		NickCore *nc = source.nc;
		Anope::string mask = params.size() > 2 ? params[2] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		XOPChanAccess tmp_access(NULL);
		tmp_access.ci = ci;
		tmp_access.type = level;

		AccessGroup access = source.AccessFor(ci);
		const ChanAccess *highest = access.Highest();
		bool override = false;

		if (!isdigit(mask[0]) && mask.find_first_of("!*@") == Anope::string::npos && !NickAlias::Find(mask))
		{
			User *targ = User::Find(mask, true);
			if (targ != NULL)
				mask = "*!*@" + targ->GetDisplayedHost();
			else
			{
				source.Reply(NICK_X_NOT_REGISTERED, mask.c_str());
				return;
			}
		}

		if ((!mask.equals_ci(nc->display) && !access.HasPriv("ACCESS_CHANGE") && !access.founder) || ((!highest || tmp_access >= *highest) && !access.founder))
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class XOPDelCallback : public NumberList
			{
				CommandSource &source;
				ChannelInfo *ci;
				Command *c;
				unsigned deleted;
				Anope::string nicks;
				bool override;
				XOPType type;
			 public:
				XOPDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, bool _override, XOPType _type, const Anope::string &numlist) : NumberList(numlist, true), source(_source), ci(_ci), c(_c), deleted(0), override(_override), type(_type)
				{
				}

				~XOPDelCallback()
				{
					if (!deleted)
						 source.Reply(_("No matching entries on %s %s list."), ci->name.c_str(), source.command.c_str());
					else
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << nicks;

						if (deleted == 1)
							source.Reply(_("Deleted one entry from %s %s list."), ci->name.c_str(), source.command.c_str());
						else
							source.Reply(_("Deleted %d entries from %s %s list."), deleted, ci->name.c_str(), source.command.c_str());
					}
				}

				void HandleNumber(unsigned number) anope_override
				{
					if (!number || number > ci->GetAccessCount())
						return;

					ChanAccess *caccess = ci->GetAccess(number - 1);

					if (this->type != XOPChanAccess::DetermineLevel(caccess))
						return;

					++deleted;
					if (!nicks.empty())
						nicks += ", " + caccess->mask;
					else
						nicks = caccess->mask;

					FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, source, caccess));

					ci->EraseAccess(number - 1);
				}
			}
			delcallback(source, ci, this, override, level, mask);
			delcallback.Process();
		}
		else
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanAccess *a = ci->GetAccess(i);

				if (a->mask.equals_ci(mask) && XOPChanAccess::DetermineLevel(a) == level)
				{
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << a->mask;

					source.Reply(_("\002%s\002 deleted from %s %s list."), a->mask.c_str(), ci->name.c_str(), source.command.c_str());

					FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, source, a));
					a->Destroy();

					return;
				}
			}
			
			source.Reply(_("\002%s\002 not found on %s %s list."), mask.c_str(), ci->name.c_str(), source.command.c_str());
		}
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params, XOPType level)
	{

		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		AccessGroup access = source.AccessFor(ci);

		if (!access.HasPriv("ACCESS_LIST") && !source.HasCommand("chanserv/access/list"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		ListFormatter list;
		list.AddColumn("Number").AddColumn("Mask");

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class XOPListCallback : public NumberList
			{
				ListFormatter &list;
				ChannelInfo *ci;
				XOPType type;
			 public:
				XOPListCallback(ListFormatter &_list, ChannelInfo *_ci, const Anope::string &numlist, XOPType _type) : NumberList(numlist, false), list(_list), ci(_ci), type(_type)
				{
				}

				void HandleNumber(unsigned Number) anope_override
				{
					if (!Number || Number > ci->GetAccessCount())
						return;

					const ChanAccess *a = ci->GetAccess(Number - 1);

					if (this->type != XOPChanAccess::DetermineLevel(a))
						return;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Mask"] = a->mask;
					this->list.AddEntry(entry);
				}
			} nl_list(list, ci, nick, level);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				const ChanAccess *a = ci->GetAccess(i);

				if (XOPChanAccess::DetermineLevel(a) != level)
					continue;
				else if (!nick.empty() && !Anope::Match(a->mask, nick))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Mask"] = a->mask;
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("%s list for %s"), source.command.c_str(), ci->name.c_str());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoClear(CommandSource &source, ChannelInfo *ci, XOPType level)
	{

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("FOUNDER") && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			const ChanAccess *access = ci->GetAccess(i - 1);
			if (XOPChanAccess::DetermineLevel(access) == level)
				ci->EraseAccess(i - 1);
		}

		FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, source));

		source.Reply(_("Channel %s %s list has been cleared."), ci->name.c_str(), source.command.c_str());

		return;
	}

 protected:
	void DoXop(CommandSource &source, const std::vector<Anope::string> &params, XOPType level)
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		const Anope::string &cmd = params[1];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, ci, params, level);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, ci, params, level);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, ci, params, level);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, ci, level);
		else
			this->OnSyntaxError(source, "");

		return;
	}
 public:
	XOPBase(Module *modname, const Anope::string &command) : Command(modname, command, 2, 4)
	{
		this->SetSyntax("\037channel\037 ADD \037mask\037");
		this->SetSyntax("\037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}");
		this->SetSyntax("\037channel\037 LIST [\037mask\037 | \037list\037]");
		this->SetSyntax("\037channel\037 CLEAR");
	}

	virtual ~XOPBase()
	{
	}

	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) = 0;
};

class CommandCSQOP : public XOPBase
{
 public:
	CommandCSQOP(Module *creator) : XOPBase(creator, "chanserv/qop")
	{
		this->SetDesc(_("Modify the list of QOP users"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoXop(source, params, XOP_QOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002QOP\002 (AutoOwner) \002list\002 for a channel. The QOP\n"
				"list gives users the right to be auto-owner on your channel,\n"
				"which gives them almost (or potentially, total) access.\n"
				" \n"
				"The \002QOP ADD\002 command adds the given nickname to the\n"
				"QOP list.\n"
				" \n"
				"The \002QOP DEL\002 command removes the given nick from the\n"
				"QOP list. If a list of entry numbers is given, those\n"
				"entries are deleted. (See the example for LIST below.)\n"
				" \n"
				"The \002QOP LIST\002 command displays the QOP list. If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002QOP #channel LIST 2-5,7-9\002\n"
				"      Lists QOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002QOP CLEAR\002 command clears all entries of the\n"
				"QOP list."));
		source.Reply(_(" \n"
				"The \002QOP\002 commands are limited to founders\n"
				"(unless SECUREOPS is off). However, any user on the\n"
				"VOP list or above may use the \002QOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("Alternative methods of modifying channel access lists are\n"
				"available. See \002%s%s HELP ACCESS\002 for information\n"
				"about the access list, and \002%s%s HELP FLAGS\002 for\n"
				"information about the flags based system."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSAOP : public XOPBase
{
 public:
	CommandCSAOP(Module *creator) : XOPBase(creator, "chanserv/aop")
	{
		this->SetDesc(_("Modify the list of AOP users"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoXop(source, params, XOP_AOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002AOP\002 (AutoOP) \002list\002 for a channel. The AOP\n"
				"list gives users the right to be auto-opped on your channel,\n"
				"to unban or invite themselves if needed, to have their\n"
				"greet message showed on join, and so on.\n"
				" \n"
				"The \002AOP ADD\002 command adds the given nickname to the\n"
				"AOP list.\n"
				" \n"
				"The \002AOP DEL\002 command removes the given nick from the\n"
				"AOP list. If a list of entry numbers is given, those\n"
				"entries are deleted. (See the example for LIST below.)\n"
				" \n"
				"The \002AOP LIST\002 command displays the AOP list. If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002AOP #channel LIST 2-5,7-9\002\n"
				"      Lists AOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002AOP CLEAR\002 command clears all entries of the\n"
				"AOP list."));
		source.Reply(_(" \n"
				"The \002AOP ADD\002 and \002AOP DEL\002 commands are limited to\n"
				"SOPs or above, while the \002AOP CLEAR\002 command can only\n"
				"be used by the channel founder. However, any user on the\n"
				"VOP list or above may use the \002AOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("Alternative methods of modifying channel access lists are\n"
				"available. See \002%s%s HELP ACCESS\002 for information\n"
				"about the access list, and \002%s%s HELP FLAGS\002 for\n"
				"information about the flags based system."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(), 
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSHOP : public XOPBase
{
 public:
	CommandCSHOP(Module *creator) : XOPBase(creator, "chanserv/hop")
	{
		this->SetDesc(_("Maintains the HOP (HalfOP) list for a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoXop(source, params, XOP_HOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002HOP\002 (HalfOP) \002list\002 for a channel. The HOP\n"
				"list gives users the right to be auto-halfopped on your\n"
				"channel.\n"
				" \n"
				"The \002HOP ADD\002 command adds the given nickname to the\n"
				"HOP list.\n"
				" \n"
				"The \002HOP DEL\002 command removes the given nick from the\n"
				"HOP list. If a list of entry numbers is given, those\n"
				"entries are deleted. (See the example for LIST below.)\n"
				" \n"
				"The \002HOP LIST\002 command displays the HOP list. If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002HOP #channel LIST 2-5,7-9\002\n"
				"      Lists HOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002HOP CLEAR\002 command clears all entries of the\n"
				"HOP list."));
		source.Reply(_(" \n"
				"The \002HOP ADD\002 and \002HOP DEL\002 commands are limited\n"
				"to SOPs or above, while \002HOP LIST\002 is available to VOPs\n"
				"and above. The \002HOP CLEAR\002 command can only be used by the\n"
				"channel founder.\n"
				" \n"));
		source.Reply(_("Alternative methods of modifying channel access lists are\n"
				"available. See \002%s%s HELP ACCESS\002 for information\n"
				"about the access list, and \002%s%s HELP FLAGS\002 for\n"
				"information about the flags based system."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSOP : public XOPBase
{
 public:
	CommandCSSOP(Module *creator) : XOPBase(creator, "chanserv/sop")
	{
		this->SetDesc(_("Modify the list of SOP users"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoXop(source, params, XOP_SOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002SOP\002 (SuperOP) \002list\002 for a channel. The SOP\n"
				"list gives users all rights given by the AOP list, and adds\n"
				"those needed to use the AutoKick and the BadWords lists,\n"
				"to send and read channel memos, and so on.\n"
				" \n"
				"The \002SOP ADD\002 command adds the given nickname to the\n"
				"SOP list.\n"
				" \n"
				"The \002SOP DEL\002 command removes the given nick from the\n"
				"SOP list. If a list of entry numbers is given, those\n"
				"entries are deleted. (See the example for LIST below.)\n"
				" \n"
				"The \002SOP LIST\002 command displays the SOP list. If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002SOP #channel LIST 2-5,7-9\002\n"
				"      Lists SOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002SOP CLEAR\002 command clears all entries of the\n"
				"SOP list."));
		source.Reply(_(" \n"
				"The \002SOP ADD\002, \002SOP DEL\002 and \002SOP CLEAR\002 commands are\n"
				"limited to the channel founder. However, any user on the\n"
				"VOP list or above may use the \002SOP LIST\002 command.\n"
				" \n"));
		source.Reply(_("Alternative methods of modifying channel access lists are\n"
				"available. See \002%s%s HELP ACCESS\002 for information\n"
				"about the access list, and \002%s%s HELP FLAGS\002 for\n"
				"information about the flags based system."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSVOP : public XOPBase
{
 public:
	CommandCSVOP(Module *creator) : XOPBase(creator, "chanserv/vop")
	{
		this->SetDesc(_("Maintains the VOP (VOicePeople) list for a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoXop(source, params, XOP_VOP);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002VOP\002 (VOicePeople) \002list\002 for a channel.\n"
				"The VOP list allows users to be auto-voiced and to voice\n"
				"themselves if they aren't.\n"
				" \n"
				"The \002VOP ADD\002 command adds the given nickname to the\n"
				"VOP list.\n"
				" \n"
				"The \002VOP DEL\002 command removes the given nick from the\n"
				"VOP list. If a list of entry numbers is given, those\n"
				"entries are deleted. (See the example for LIST below.)\n"
				" \n"
				"The \002VOP LIST\002 command displays the VOP list. If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002VOP #channel LIST 2-5,7-9\002\n"
				"      Lists VOP entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				"      \n"
				"The \002VOP CLEAR\002 command clears all entries of the\n"
				"VOP list."));
		source.Reply(_(" \n"
				"The \002VOP ADD\002 and \002VOP DEL\002 commands are limited\n"
				"to SOPs or above, while \002VOP LIST\002 is available to VOPs\n"
				"and above. The \002VOP CLEAR\002 command can only be used by the\n"
				"channel founder.\n"
				" \n"));
		source.Reply(_("Alternative methods of modifying channel access lists are\n"
				"available. See \002%s%s HELP ACCESS\002 for information\n"
				"about the access list, and \002%s%s HELP FLAGS\002 for\n"
				"information about the flags based system."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CSXOP : public Module
{
	XOPAccessProvider accessprovider;
	CommandCSQOP commandcsqop;
	CommandCSSOP commandcssop;
	CommandCSAOP commandcsaop;
	CommandCSHOP commandcshop;
	CommandCSVOP commandcsvop;

 public:
	CSXOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		accessprovider(this), commandcsqop(this), commandcssop(this), commandcsaop(this), commandcshop(this), commandcsvop(this)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);
	}
};

MODULE_INIT(CSXOP)
