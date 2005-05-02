/* NickServ core functions
 *
 * (C) 2003-2005 Anope Team
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
    moduleAddVersion("$Id$");
    moduleSetType(CORE);

    c = createCommand("SENDPASS", do_sendpass, NULL, NICK_HELP_SENDPASS,
                      -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);
    if (!UseMail) {
        return MOD_STOP;
    }
#ifdef USE_ENCRYPTION
    return MOD_STOP;
#else
    return MOD_CONT;
#endif
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
    } else if (RestrictMail && !is_oper(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        char buf[BUFSIZE];
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
                na->nc->pass);
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
    }

    return MOD_CONT;
}
