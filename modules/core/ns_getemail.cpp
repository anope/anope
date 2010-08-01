/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * A simple call to check for all emails that a user may have registered
 * with. It returns the nicks that match the email you provide. Wild
 * Cards are not excepted. Must use user@email-host.
 */

/*************************************************************************/

#include "module.h"

class CommandNSGetEMail : public Command
{
 public:
	CommandNSGetEMail() : Command("GETEMAIL", 1, 1, "nickserv/getemail")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string email = params[0];
		int j = 0;

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " used GETEMAIL on " << email;

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (!nc->email.empty() && nc->email.equals_ci(email))
			{
				++j;
				notice_lang(Config.s_NickServ, u, NICK_GETEMAIL_EMAILS_ARE, nc->display.c_str(), email.c_str());
			}
		}

		if (j <= 0)
		{
			notice_lang(Config.s_NickServ, u, NICK_GETEMAIL_NOT_USED, email.c_str());
			return MOD_CONT;
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_SERVADMIN_HELP_GETEMAIL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "GETMAIL", NICK_GETEMAIL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_GETEMAIL);
	}
};

class NSGetEMail : public Module
{
	CommandNSGetEMail commandnsgetemail;
 public:
	NSGetEMail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsgetemail);
	}
};

MODULE_INIT(NSGetEMail)
