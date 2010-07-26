/* NickServ core functions
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

class CommandNSUpdate : public Command
{
 public:
	CommandNSUpdate() : Command("UPDATE", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(u->nick);

		if (!na)
			return MOD_CONT;

		do_setmodes(u);
		check_memos(u);

		na->last_realname = u->realname;
		na->last_seen = time(NULL);
		if (ircd->vhost)
			do_on_id(u);
		notice_lang(Config.s_NickServ, u, NICK_UPDATE_SUCCESS, Config.s_NickServ.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_UPDATE);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_UPDATE);
	}
};

class NSUpdate : public Module
{
 public:
	NSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSUpdate());
	}
};

MODULE_INIT(NSUpdate)
