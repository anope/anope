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

class CommandNSGetPass : public Command
{
 public:
	CommandNSGetPass() : Command("GETPASS", 1, 1, "nickserv/getpass")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string tmp_pass;
		NickAlias *na;
		NickRequest *nr = NULL;

		if (!(na = findnick(nick)))
		{
			if ((nr = findrequestnick(nick)))
			{
				Log(LOG_ADMIN, u, this) << "for " << nr->nick;
				if (Config->WallGetpass)
					ircdproto->SendGlobops(NickServ, "\2%s\2 used GETPASS on \2%s\2", u->nick.c_str(), nick.c_str());
				notice_lang(Config->s_NickServ, u, NICK_GETPASS_PASSCODE_IS, nick.c_str(), nr->passcode.c_str());
			}
			else
				notice_lang(Config->s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		}
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config->s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (Config->NSSecureAdmins && na->nc->IsServicesOper())
			notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
		else
		{
			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
			{
				Log(LOG_ADMIN, u, this) << "for " << nick;
				if (Config->WallGetpass)
					ircdproto->SendGlobops(NickServ, "\2%s\2 used GETPASS on \2%s\2", u->nick.c_str(), nick.c_str());
				notice_lang(Config->s_NickServ, u, NICK_GETPASS_PASSWORD_IS, nick.c_str(), tmp_pass.c_str());
			}
			else
				notice_lang(Config->s_NickServ, u, NICK_GETPASS_UNAVAILABLE);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_NickServ, u, NICK_SERVADMIN_HELP_GETPASS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_NickServ, u, "GETPASS", NICK_GETPASS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_GETPASS);
	}
};

class NSGetPass : public Module
{
	CommandNSGetPass commandnsgetpass;

 public:
	NSGetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		Anope::string tmp_pass = "plain:tmp";
		if (enc_decrypt(tmp_pass, tmp_pass) == -1)
			throw ModuleException("Incompatible with the encryption module being used");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsgetpass);
	}
};

MODULE_INIT(NSGetPass)
