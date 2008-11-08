/* NickServ core functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

int do_sendpass(User * u);

void myNickServHelp(User * u);

class NSSendPass : public Module
{
 public:
	NSSendPass(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("SENDPASS", do_sendpass, NULL, NICK_HELP_SENDPASS, -1, -1, -1, -1);
		moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

		moduleSetNickHelp(myNickServHelp);
		if (!UseMail)
		{
			throw ModuleException("Not using mail, whut.");
		}
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_SENDPASS);
}

/**
 * The /ns sendpass command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_sendpass(User * u)
{

    char *nick = strtok(NULL, " ");
    NickAlias *na;

    if (!nick) {
        syntax_error(s_NickServ, u, "SENDPASS", NICK_SENDPASS_SYNTAX);
    } else if (RestrictMail && !is_services_oper(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        char buf[BUFSIZE];
        char tmp_pass[PASSMAX];
        if(enc_decrypt(na->nc->pass,tmp_pass,PASSMAX - 1)==1) {
            MailInfo *mail;

            snprintf(buf, sizeof(buf), getstring(na, NICK_SENDPASS_SUBJECT),
                     na->nick);
            mail = MailBegin(u, na->nc, buf, s_NickServ);
            if (!mail)
                return MOD_CONT;

            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_HEAD));
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_1), na->nick);
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_2),
                    tmp_pass);
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_3));
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_4));
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_5),
                    NetworkName);
            fprintf(mail->pipe, "\n.\n");

            MailEnd(mail);

            alog("%s: %s!%s@%s used SENDPASS on %s", s_NickServ, u->nick,
                 u->username, u->host, nick);
            notice_lang(s_NickServ, u, NICK_SENDPASS_OK, nick);
        } else {
            notice_lang(s_NickServ, u, NICK_SENDPASS_UNAVAILABLE);
        }
    }

    return MOD_CONT;
}

MODULE_INIT("ns_sendpass", NSSendPass)
