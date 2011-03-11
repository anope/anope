/* NickServ core functions
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

class CommandNSUpdate : public Command
{
 public:
	CommandNSUpdate() : Command("UPDATE", 0, 0)
	{
		this->SetDesc(_("Updates your current status, i.e. it checks for new memos"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(u->nick);

		if (!na)
			return MOD_CONT;

		do_setmodes(u);
		check_memos(u);

		na->last_realname = u->realname;
		na->last_seen = Anope::CurTime;
		if (ircd->vhost)
			do_on_id(u);
		source.Reply(_("Status updated (memos, vhost, chmodes, flags)."), Config->s_NickServ.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: UPDATE\n"
				"Updates your current status, i.e. it checks for new memos,\n"
				"sets needed chanmodes (ModeonID) and updates your vhost and\n"
				"your userflags (lastseentime, etc)."));
		return true;
	}
};

class NSUpdate : public Module
{
	CommandNSUpdate commandnsupdate;

 public:
	NSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsupdate);
	}
};

MODULE_INIT(NSUpdate)
