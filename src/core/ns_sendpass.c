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

class CommandNSSendPass : public Command
{
 public:
	CommandNSSendPass() : Command("SENDPASS", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		NickAlias *na;

		if (RestrictMail && !u->nc->HasCommand("nickserv/sendpass"))
			notice_lang(s_NickServ, u, ACCESS_DENIED);
		else if (!(na = findnick(nick)))
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else
		{
			char buf[BUFSIZE];
			char tmp_pass[PASSMAX];
			if (enc_decrypt(na->nc->pass,tmp_pass,PASSMAX - 1) == 1)
			{
				MailInfo *mail;

				snprintf(buf, sizeof(buf), getstring(na, NICK_SENDPASS_SUBJECT), na->nick);
				mail = MailBegin(u, na->nc, buf, s_NickServ);
				if (!mail)
					return MOD_CONT;

				fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_HEAD));
				fprintf(mail->pipe, "\n\n");
				fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_1), na->nick);
				fprintf(mail->pipe, "\n\n");
				fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_2), tmp_pass);
				fprintf(mail->pipe, "\n\n");
				fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_LINE_3));
				fprintf(mail->pipe, "\n\n");
				fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_LINE_4));
				fprintf(mail->pipe, "\n\n");
				fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_5), NetworkName);
				fprintf(mail->pipe, "\n.\n");

				MailEnd(mail);

				alog("%s: %s!%s@%s used SENDPASS on %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nick);
				notice_lang(s_NickServ, u, NICK_SENDPASS_OK, nick);
			}
			else
				notice_lang(s_NickServ, u, NICK_SENDPASS_UNAVAILABLE);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_SENDPASS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_NickServ, u, "SENDPASS", NICK_SENDPASS_SYNTAX);
	}
};

class NSSendPass : public Module
{
 public:
	NSSendPass(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSSendPass());

		if (!UseMail)
			throw ModuleException("Not using mail, whut.");

		char tmp_pass[PASSMAX];
		if (!enc_decrypt("tmp", tmp_pass, PASSMAX - 1))
			throw ModuleException("Incompatible with the encryption module being used");

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_SENDPASS);
	}
};

MODULE_INIT(NSSendPass)
