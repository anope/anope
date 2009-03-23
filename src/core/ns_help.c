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

class CommandNSHelp : public Command
{
 public:
	CommandNSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (!stricmp(cmd, "SET LANGUAGE"))
		{
			int i;
			notice_help(s_NickServ, u, NICK_HELP_SET_LANGUAGE);
			for (i = 0; i < NUM_LANGS && langlist[i] >= 0; ++i)
				u->SendMessage(s_NickServ, "    %2d) %s", i + 1, langnames[langlist[i]]);
		}
		else
			mod_help_cmd(s_NickServ, u, NICKSERV, cmd);

		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_NickServ, u, NICK_HELP);
		moduleDisplayHelp(1, u);
		if (is_services_admin(u))
			notice_help(s_NickServ, u, NICK_SERVADMIN_HELP);
		if (NSExpire >= 86400)
			notice_help(s_NickServ, u, NICK_HELP_EXPIRES, NSExpire / 86400);
		notice_help(s_NickServ, u, NICK_HELP_FOOTER);
	}
};

class NSHelp : public Module
{
 public:
	NSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSHelp(), MOD_UNIQUE);
	}
};

MODULE_INIT("ns_help", NSHelp)
