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

static bool SendPassMail(User *u, NickAlias *na, const Anope::string &pass);

class CommandNSSendPass : public Command
{
 public:
	CommandNSSendPass() : Command("SENDPASS", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		NickAlias *na;

		if (Config->RestrictMail && (!u->Account() || !u->Account()->HasCommand("nickserv/sendpass")))
			notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
		else if (!(na = findnick(nick)))
			notice_lang(Config->s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config->s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
		else
		{
			Anope::string tmp_pass;
			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
			{
				if (SendPassMail(u, na, tmp_pass))
				{
					Log(Config->RestrictMail ? LOG_ADMIN : LOG_COMMAND, u, this) << "for " << na->nick;
					notice_lang(Config->s_NickServ, u, NICK_SENDPASS_OK, nick.c_str());
				}
			}
			else
				notice_lang(Config->s_NickServ, u, NICK_SENDPASS_UNAVAILABLE);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_NickServ, u, NICK_HELP_SENDPASS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_NickServ, u, "SENDPASS", NICK_SENDPASS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_SENDPASS);
	}
};

class NSSendPass : public Module
{
	CommandNSSendPass commandnssendpass;

 public:
	NSSendPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!Config->UseMail)
			throw ModuleException("Not using mail, whut.");

		Anope::string tmp_pass = "plain:tmp";
		if (enc_decrypt(tmp_pass, tmp_pass) == -1)
			throw ModuleException("Incompatible with the encryption module being used");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnssendpass);
	}
};

static bool SendPassMail(User *u, NickAlias *na, const Anope::string &pass)
{
	char subject[BUFSIZE], message[BUFSIZE];

	snprintf(subject, sizeof(subject), getstring(na, NICK_SENDPASS_SUBJECT), na->nick.c_str());
	snprintf(message, sizeof(message), getstring(na, NICK_SENDPASS), na->nick.c_str(), pass.c_str(), Config->NetworkName.c_str());

	return Mail(u, na->nc, Config->s_NickServ, subject, message);
}

MODULE_INIT(NSSendPass)
