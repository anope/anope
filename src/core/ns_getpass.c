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

class CommandNSGetPass : public Command
{
 public:
	CommandNSGetPass() : Command("GETPASS", 1, 1, "nickserv/getpass")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		char tmp_pass[PASSMAX];
		NickAlias *na;
		NickRequest *nr = NULL;

		if (!(na = findnick(nick)))
		{
			if ((nr = findrequestnick(nick)))
			{
				alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nick);
				if (WallGetpass)
					ircdproto->SendGlobops(s_NickServ, "\2%s\2 used GETPASS on \2%s\2", u->nick, nick);
				notice_lang(s_NickServ, u, NICK_GETPASS_PASSCODE_IS, nick, nr->passcode);
			}
			else
				notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		}
		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (NSSecureAdmins && na->nc->IsServicesOper())
			notice_lang(s_NickServ, u, ACCESS_DENIED);
		else
		{
			if (enc_decrypt(na->nc->pass, tmp_pass, PASSMAX - 1) == 1)
			{
				alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nick);
				if (WallGetpass)
					ircdproto->SendGlobops(s_NickServ, "\2%s\2 used GETPASS on \2%s\2", u->nick, nick);
				notice_lang(s_NickServ, u, NICK_GETPASS_PASSWORD_IS, nick, tmp_pass);
			}
			else
				notice_lang(s_NickServ, u, NICK_GETPASS_UNAVAILABLE);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_GETPASS);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "GETPASS", NICK_GETPASS_SYNTAX);
	}
};

class NSGetPass : public Module
{
 public:
	NSGetPass(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSGetPass());

		char tmp_pass[PASSMAX];
		if (!enc_decrypt("tmp", tmp_pass, PASSMAX - 1))
			throw ModuleException("Incompatible with the encryption module being used");
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_GETPASS);
	}
};

MODULE_INIT(NSGetPass)
