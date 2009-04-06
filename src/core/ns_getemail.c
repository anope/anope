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
 *
 * A simple call to check for all emails that a user may have registered
 * with. It returns the nicks that match the email you provide. Wild
 * Cards are not excepted. Must use user@email-host.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSGetEMail : public Command
{
 public:
	CommandNSGetEMail() : Command("GETEMAIL", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *email = params[0].c_str();
		int i, j = 0;
		NickCore *nc;

		alog("%s: %s!%s@%s used GETEMAIL on %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, email);
		for (i = 0; i < 1024; ++i)
		{
			for (nc = nclists[i]; nc; nc = nc->next)
			{
				if (nc->email)
				{
					if (!stricmp(nc->email, email))
					{
						++j;
						notice_lang(s_NickServ, u, NICK_GETEMAIL_EMAILS_ARE, nc->display, email);
					}
				}
			}
		}
		if (j <= 0)
		{
			notice_lang(s_NickServ, u, NICK_GETEMAIL_NOT_USED, email);
			return MOD_CONT;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_GETEMAIL);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "GETMAIL", NICK_GETEMAIL_SYNTAX);
	}
};

class NSGetEMail : public Module
{
 public:
	NSGetEMail(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSGetEMail(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_GETEMAIL);
	}
};

MODULE_INIT("ns_getemail", NSGetEMail)
