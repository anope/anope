/* NickServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSUpdate : public Command
{
 public:
	CommandNSUpdate() : Command("UPDATE", 0, 0)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		NickAlias *na = findnick(u->nick);

		if (!na)
			return MOD_CONT;

		do_setmodes(u);
		check_memos(u);

		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(u->realname);
		na->status |= NS_IDENTIFIED;
		na->last_seen = time(NULL);
		if (ircd->vhost)
			do_on_id(u);
		notice_lang(s_NickServ, u, NICK_UPDATE_SUCCESS, s_NickServ);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(s_NickServ, u, NICK_HELP_UPDATE);
		return true;
	}
};

class NSUpdate : public Module
{
 public:
	NSUpdate(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(NICKSERV, new CommandNSUpdate(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_UPDATE);
	}
};

MODULE_INIT("ns_update", NSUpdate)
