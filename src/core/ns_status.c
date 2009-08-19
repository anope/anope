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

class CommandNSStatus : public Command
{
 public:
	CommandNSStatus() : Command("STATUS", 0, 16)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		User *u2;
		NickAlias *na = NULL;
		unsigned i = 0;
		const char *nick = params.size() ? params[0].c_str() : NULL;

		/* If no nickname is given, we assume that the user
		 * is asking for himself */
		if (!nick)
			nick = u->nick;

		while (nick && i++ < 16)
		{
			if (!(u2 = finduser(nick))) /* Nick is not online */
				notice_lang(s_NickServ, u, NICK_STATUS_0, nick);
			else if (nick_identified(u2)) /* Nick is identified */
				notice_lang(s_NickServ, u, NICK_STATUS_3, nick);
			else if (nick_recognized(u2)) /* Nick is recognised, but NOT identified */
				notice_lang(s_NickServ, u, NICK_STATUS_2, nick);
			else if (!(na = findnick(nick))) /* Nick is online, but NOT a registered */
				notice_lang(s_NickServ, u, NICK_STATUS_0, nick);
			else
				notice_lang(s_NickServ, u, NICK_STATUS_1, nick);

			/* Get the next nickname */
			nick = params.size() > i ? params[i].c_str() : NULL;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_STATUS);
		return true;
	}
};

class NSStatus : public Module
{
 public:
	NSStatus(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSStatus(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_STATUS);
	}
};

MODULE_INIT("ns_status", NSStatus)
