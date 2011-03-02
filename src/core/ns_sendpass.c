/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */
/*************************************************************************/

#include "module.h"

int do_sendpass(User * u);

void myNickServHelp(User * u);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("SENDPASS", do_sendpass, NULL, NICK_HELP_SENDPASS,
                      -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);
    if (!UseMail) {
        return MOD_STOP;
    }

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    if (!RestrictMail || is_services_oper(u))
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

            fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_HEAD));
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_1), na->nick);
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_2),
                    tmp_pass);
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_LINE_3));
            fprintf(mail->pipe, "\n\n");
            fprintf(mail->pipe, "%s", getstring(na, NICK_SENDPASS_LINE_4));
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
