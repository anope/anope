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
#include "encrypt.h"

int do_saset(User * u);
int do_saset_display(User * u, NickCore * nc, char *param);
int do_saset_password(User * u, NickCore * nc, char *param);
int do_saset_url(User * u, NickCore * nc, char *param);
int do_saset_email(User * u, NickCore * nc, char *param);
int do_saset_greet(User * u, NickCore * nc, char *param);
int do_saset_icq(User * u, NickCore * nc, char *param);
int do_saset_kill(User * u, NickCore * nc, char *param);
int do_saset_secure(User * u, NickCore * nc, char *param);
int do_saset_private(User * u, NickCore * nc, char *param);
int do_saset_msg(User * u, NickCore * nc, char *param);
int do_saset_hide(User * u, NickCore * nc, char *param);
int do_saset_noexpire(User * u, NickAlias * nc, char *param);
int do_saset_autoop(User * u, NickCore * nc, char *param);
int do_saset_language(User * u, NickCore * nc, char *param);
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

    c = createCommand("SASET", do_saset, is_services_oper, -1, -1,
                      NICK_HELP_SASET, NICK_HELP_SASET, NICK_HELP_SASET);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET DISPLAY", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_DISPLAY, NICK_HELP_SASET_DISPLAY, NICK_HELP_SASET_DISPLAY);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET PASSWORD", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_PASSWORD, NICK_HELP_SASET_PASSWORD, NICK_HELP_SASET_PASSWORD);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET URL", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_URL, NICK_HELP_SASET_URL, NICK_HELP_SASET_URL);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET EMAIL", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_EMAIL, NICK_HELP_SASET_EMAIL, NICK_HELP_SASET_EMAIL);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET ICQ", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_ICQ, NICK_HELP_SASET_ICQ, NICK_HELP_SASET_ICQ);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET GREET", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_GREET, NICK_HELP_SASET_GREET, NICK_HELP_SASET_GREET);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET KILL", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_KILL, NICK_HELP_SASET_KILL, NICK_HELP_SASET_KILL);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET SECURE", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_SECURE, NICK_HELP_SASET_SECURE, NICK_HELP_SASET_SECURE);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET PRIVATE", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_PRIVATE, NICK_HELP_SASET_PRIVATE, NICK_HELP_SASET_PRIVATE);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET MSG", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_MSG, NICK_HELP_SASET_MSG, NICK_HELP_SASET_MSG);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET HIDE", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_HIDE, NICK_HELP_SASET_HIDE, NICK_HELP_SASET_HIDE);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET NOEXPIRE", NULL, is_services_oper, -1, -1,
                      NICK_HELP_SASET_NOEXPIRE, NICK_HELP_SASET_NOEXPIRE,
                      NICK_HELP_SASET_NOEXPIRE);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET AUTOOP", NULL, is_services_oper, -1, -1,
                      NICK_HELP_SASET_AUTOOP, NICK_HELP_SASET_AUTOOP, NICK_HELP_SASET_AUTOOP);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SASET LANGUAGE", NULL, is_services_oper,
                      -1, -1, NICK_HELP_SASET_LANGUAGE, NICK_HELP_SASET_LANGUAGE, NICK_HELP_SASET_LANGUAGE);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);

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
    if (is_services_oper(u))
        notice_lang(s_NickServ, u, NICK_HELP_CMD_SASET);
}

/**
 * The /ns saset command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_saset(User * u)
{
    char *nick = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *param = strtok(NULL, " ");

    NickAlias *na;

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_SASET_DISABLED);
        return MOD_CONT;
    }
	if (!nick) {
		syntax_error(s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
		return MOD_CONT;
	}
    if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_SASET_BAD_NICK, nick);
        return MOD_CONT;
    }

    if (!param
        && (!cmd
            || (stricmp(cmd, "URL") != 0 && stricmp(cmd, "EMAIL") != 0
                && stricmp(cmd, "GREET") != 0
                && stricmp(cmd, "ICQ") != 0))) {
        syntax_error(s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
    } else if (!na) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
    } else if (stricmp(cmd, "DISPLAY") == 0) {
        do_saset_display(u, na->nc, param);
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        do_saset_password(u, na->nc, param);
    } else if (stricmp(cmd, "URL") == 0) {
        do_saset_url(u, na->nc, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        do_saset_email(u, na->nc, param);
    } else if (stricmp(cmd, "ICQ") == 0) {
        do_saset_icq(u, na->nc, param);
    } else if (stricmp(cmd, "GREET") == 0) {
        do_saset_greet(u, na->nc, param);
    } else if (stricmp(cmd, "KILL") == 0) {
        do_saset_kill(u, na->nc, param);
    } else if (stricmp(cmd, "SECURE") == 0) {
        do_saset_secure(u, na->nc, param);
    } else if (stricmp(cmd, "PRIVATE") == 0) {
        do_saset_private(u, na->nc, param);
    } else if (stricmp(cmd, "MSG") == 0) {
        do_saset_msg(u, na->nc, param);
    } else if (stricmp(cmd, "HIDE") == 0) {
        do_saset_hide(u, na->nc, param);
    } else if (stricmp(cmd, "NOEXPIRE") == 0) {
        do_saset_noexpire(u, na, param);
    } else if (stricmp(cmd, "AUTOOP") == 0) {
        do_saset_autoop(u, na->nc, param); 
    } else if (stricmp(cmd, "LANGUAGE") == 0) {
        do_saset_language(u, na->nc, param);
    } else {
        notice_lang(s_NickServ, u, NICK_SASET_UNKNOWN_OPTION, cmd);
    }
    return MOD_CONT;
}

int do_saset_display(User * u, NickCore * nc, char *param)
{
    int i;
    NickAlias *na;

    /* First check whether param is a valid nick of the group */
    for (i = 0; i < nc->aliases.count; i++) {
        na = nc->aliases.list[i];
        if (stricmp(na->nick, param) == 0) {
            param = na->nick;   /* Because case may differ */
            break;
        }
    }

    if (i == nc->aliases.count) {
        notice_lang(s_NickServ, u, NICK_SASET_DISPLAY_INVALID,
                    nc->display);
        return MOD_CONT;
    }

    alog("%s: %s!%s@%s set the display of %s to: %s",
         s_NickServ, u->nick, u->username, u->host, nc->display, param);
    change_core_display(nc, param);
    notice_lang(s_NickServ, u, NICK_SASET_DISPLAY_CHANGED, nc->display);

    if (NSNickTracking) {
        for (i = 0; i < nc->aliases.count; ++i) {
            na = nc->aliases.list[i];
            if (na->u && nick_identified(na->u))
                nsStartNickTracking(na->u);
        }
    }

    return MOD_CONT;
}

int do_saset_password(User * u, NickCore * nc, char *param)
{
    int len = strlen(param);
    char tmp_pass[PASSMAX];

    if (NSSecureAdmins && u->na->nc != nc && nick_is_services_admin(nc)
        && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    } else if (stricmp(nc->display, param) == 0
               || (StrictPasswords && len < 5)) {
        notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
        return MOD_CONT;
    } else if (enc_encrypt_check_len(len ,PASSMAX - 1)) {
        notice_lang(s_NickServ, u, PASSWORD_TOO_LONG);
        return MOD_CONT;
    }

    if (enc_encrypt(param, len, nc->pass, PASSMAX - 1) < 0) {
        memset(param, 0, len);
        alog("%s: Failed to encrypt password for %s (set)", s_NickServ,
             nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_FAILED,
                    nc->display);
        return MOD_CONT;
    }
    memset(param, 0, len);

    if(enc_decrypt(nc->pass,tmp_pass,PASSMAX - 1)==1) {
        notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_CHANGED_TO, nc->display,
                    tmp_pass);
    } else {
        notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_CHANGED, nc->display);
    }

    alog("%s: %s!%s@%s used SASET PASSWORD on %s (e-mail: %s)", s_NickServ,
         u->nick, u->username, u->host, nc->display,
         (nc->email ? nc->email : "none"));
    if (WallSetpass)
        anope_cmd_global(s_NickServ,
                         "\2%s\2 used SASET PASSWORD on \2%s\2",
                         u->nick, nc->display);
    return MOD_CONT;
}

int do_saset_url(User * u, NickCore * nc, char *param)
{
    if (nc->url)
        free(nc->url);

    if (param) {
        nc->url = sstrdup(param);
        alog("%s: %s!%s@%s set the url of %s to: %s",
             s_NickServ, u->nick, u->username, u->host, nc->display, nc->url);
        notice_lang(s_NickServ, u, NICK_SASET_URL_CHANGED, nc->display,
                    param);
    } else {
        nc->url = NULL;
        alog("%s: %s!%s@%s unset the url of %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_URL_UNSET, nc->display);
    }
    return MOD_CONT;
}

int do_saset_email(User * u, NickCore * nc, char *param)
{
    if (!param && NSForceEmail) {
        notice_lang(s_NickServ, u, NICK_SASET_EMAIL_UNSET_IMPOSSIBLE);
        return MOD_CONT;
    } else if (NSSecureAdmins && u->na->nc != nc
               && nick_is_services_admin(nc)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    } else if (param && !MailValidate(param)) {
        notice_lang(s_NickServ, u, MAIL_X_INVALID, param);
        return MOD_CONT;
    }

    alog("%s: %s!%s@%s used SASET EMAIL on %s (e-mail: %s)", s_NickServ,
         u->nick, u->username, u->host, nc->display,
         (nc->email ? nc->email : "none"));

    if (nc->email)
        free(nc->email);

    if (param) {
        nc->email = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SASET_EMAIL_CHANGED, nc->display,
                    param);
    } else {
        nc->email = NULL;
        notice_lang(s_NickServ, u, NICK_SASET_EMAIL_UNSET, nc->display);
    }
    return MOD_CONT;
}

int do_saset_icq(User * u, NickCore * nc, char *param)
{
    if (param) {
        int32 tmp = atol(param);
        if (tmp == 0) {
            notice_lang(s_NickServ, u, NICK_SASET_ICQ_INVALID, param);
        } else {
            nc->icq = tmp;
            alog("%s: %s!%s@%s set the icq of %s to: %d",
                 s_NickServ, u->nick, u->username, u->host, nc->display, nc->icq);
            notice_lang(s_NickServ, u, NICK_SASET_ICQ_CHANGED, nc->display,
                        param);
        }
    } else {
        nc->icq = 0;
        alog("%s: %s!%s@%s unset the icq of %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_ICQ_UNSET, nc->display);
    }
    return MOD_CONT;
}

int do_saset_greet(User * u, NickCore * nc, char *param)
{
    if (nc->greet)
        free(nc->greet);

    if (param) {
        char buf[BUFSIZE];
        char *end = strtok(NULL, "");

        snprintf(buf, sizeof(buf), "%s%s%s", param, (end ? " " : ""),
                 (end ? end : ""));

        nc->greet = sstrdup(buf);
        alog("%s: %s!%s@%s set the greet of %s to: %s",
             s_NickServ, u->nick, u->username, u->host, nc->display, nc->greet);
        notice_lang(s_NickServ, u, NICK_SASET_GREET_CHANGED, nc->display,
                    buf);
    } else {
        nc->greet = NULL;
        alog("%s: %s!%s@%s unset the greet of %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_GREET_UNSET, nc->display);
    }
    return MOD_CONT;
}

int do_saset_kill(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_KILLPROTECT;
        nc->flags &= ~(NI_KILL_QUICK | NI_KILL_IMMED);
        alog("%s: %s!%s@%s set kill ON for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_KILL_ON, nc->display);
    } else if (stricmp(param, "QUICK") == 0) {
        nc->flags |= NI_KILLPROTECT | NI_KILL_QUICK;
        nc->flags &= ~NI_KILL_IMMED;
        alog("%s: %s!%s@%s set kill QUICK for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_KILL_QUICK, nc->display);
    } else if (stricmp(param, "IMMED") == 0) {
        if (NSAllowKillImmed) {
            nc->flags |= NI_KILLPROTECT | NI_KILL_IMMED;
            nc->flags &= ~NI_KILL_QUICK;
            alog("%s: %s!%s@%s set kill IMMED for %s",
                 s_NickServ, u->nick, u->username, u->host, nc->display);
            notice_lang(s_NickServ, u, NICK_SASET_KILL_IMMED, nc->display);
        } else {
            notice_lang(s_NickServ, u, NICK_SASET_KILL_IMMED_DISABLED);
        }
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
        alog("%s: %s!%s@%s set kill OFF for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_KILL_OFF, nc->display);
    } else {
        syntax_error(s_NickServ, u, "SASET KILL",
                     NSAllowKillImmed ? NICK_SASET_KILL_IMMED_SYNTAX :
                     NICK_SASET_KILL_SYNTAX);
    }
    return MOD_CONT;
}

int do_saset_secure(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_SECURE;
        alog("%s: %s!%s@%s set secure ON for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_SECURE_ON, nc->display);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_SECURE;
        alog("%s: %s!%s@%s set secure OFF for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_SECURE_OFF, nc->display);
    } else {
        syntax_error(s_NickServ, u, "SASET SECURE",
                     NICK_SASET_SECURE_SYNTAX);
    }
    return MOD_CONT;
}

int do_saset_private(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_PRIVATE;
        alog("%s: %s!%s@%s set private ON for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_PRIVATE_ON, nc->display);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_PRIVATE;
        alog("%s: %s!%s@%s set private OFF for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_PRIVATE_OFF, nc->display);
    } else {
        syntax_error(s_NickServ, u, "SASET PRIVATE",
                     NICK_SASET_PRIVATE_SYNTAX);
    }
    return MOD_CONT;
}

int do_saset_msg(User * u, NickCore * nc, char *param)
{
    if (!UsePrivmsg) {
        notice_lang(s_NickServ, u, NICK_SASET_OPTION_DISABLED, "MSG");
        return MOD_CONT;
    }

    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_MSG;
        alog("%s: %s!%s@%s set msg ON for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_MSG_ON, nc->display);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_MSG;
        alog("%s: %s!%s@%s set msg OFF for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_MSG_OFF, nc->display);
    } else {
        syntax_error(s_NickServ, u, "SASET MSG", NICK_SASET_MSG_SYNTAX);
    }
    return MOD_CONT;
}

int do_saset_hide(User * u, NickCore * nc, char *param)
{
    int flag, onmsg, offmsg;
    char *param2;

    if (stricmp(param, "EMAIL") == 0) {
        flag = NI_HIDE_EMAIL;
        onmsg = NICK_SASET_HIDE_EMAIL_ON;
        offmsg = NICK_SASET_HIDE_EMAIL_OFF;
    } else if (stricmp(param, "USERMASK") == 0) {
        flag = NI_HIDE_MASK;
        onmsg = NICK_SASET_HIDE_MASK_ON;
        offmsg = NICK_SASET_HIDE_MASK_OFF;
    } else if (stricmp(param, "STATUS") == 0) {
        flag = NI_HIDE_STATUS;
        onmsg = NICK_SASET_HIDE_STATUS_ON;
        offmsg = NICK_SASET_HIDE_STATUS_OFF;
    } else if (stricmp(param, "QUIT") == 0) {
        flag = NI_HIDE_QUIT;
        onmsg = NICK_SASET_HIDE_QUIT_ON;
        offmsg = NICK_SASET_HIDE_QUIT_OFF;
    } else {
        syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
        return MOD_CONT;
    }

    param2 = strtok(NULL, " ");
    if (!param2) {
        syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
    } else if (stricmp(param2, "ON") == 0) {
        nc->flags |= flag;
        alog("%s: %s!%s@%s set hide %s ON for %s",
             s_NickServ, u->nick, u->username, u->host, param, nc->display);
        notice_lang(s_NickServ, u, onmsg, nc->display, s_NickServ);
    } else if (stricmp(param2, "OFF") == 0) {
        nc->flags &= ~flag;
        alog("%s: %s!%s@%s set hide %s OFF for %s",
             s_NickServ, u->nick, u->username, u->host, param, nc->display);
        notice_lang(s_NickServ, u, offmsg, nc->display, s_NickServ);
    } else {
        syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
    }

    return MOD_CONT;
}

int do_saset_noexpire(User * u, NickAlias * na, char *param)
{
    if (!param) {
        syntax_error(s_NickServ, u, "SASET NOEXPIRE",
                     NICK_SASET_NOEXPIRE_SYNTAX);
        return MOD_CONT;
    }
    if (stricmp(param, "ON") == 0) {
        na->status |= NS_NO_EXPIRE;
        alog("%s: %s!%s@%s set noexpire ON for %s",
             s_NickServ, u->nick, u->username, u->host, na->nick);
        notice_lang(s_NickServ, u, NICK_SASET_NOEXPIRE_ON, na->nick);
    } else if (stricmp(param, "OFF") == 0) {
        na->status &= ~NS_NO_EXPIRE;
        alog("%s: %s!%s@%s set noexpire OFF for %s",
             s_NickServ, u->nick, u->username, u->host, na->nick);
        notice_lang(s_NickServ, u, NICK_SASET_NOEXPIRE_OFF, na->nick);
    } else {
        syntax_error(s_NickServ, u, "SASET NOEXPIRE",
                     NICK_SASET_NOEXPIRE_SYNTAX);
    }
    return MOD_CONT;
}

int do_saset_autoop(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags &= ~NI_AUTOOP;
        alog("%s: %s!%s@%s set autoop ON for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_AUTOOP_ON, nc->display);
    } else if (stricmp(param, "OFF") == 0) {
	nc->flags |= NI_AUTOOP;
        alog("%s: %s!%s@%s set autoop OFF for %s",
             s_NickServ, u->nick, u->username, u->host, nc->display);
        notice_lang(s_NickServ, u, NICK_SASET_AUTOOP_OFF, nc->display);
    } else {
        syntax_error(s_NickServ, u, "SET AUTOOP", NICK_SASET_AUTOOP_SYNTAX);
    }

    return MOD_CONT;
}

int do_saset_language(User * u, NickCore * nc, char *param)
{
    int langnum;

    if (param[strspn(param, "0123456789")] != 0) {      /* i.e. not a number */
        syntax_error(s_NickServ, u, "SASET LANGUAGE",
                     NICK_SASET_LANGUAGE_SYNTAX);
        return MOD_CONT;
    }
    langnum = atoi(param) - 1;
    if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0) {
        notice_lang(s_NickServ, u, NICK_SASET_LANGUAGE_UNKNOWN, langnum + 1,
                    s_NickServ);
        return MOD_CONT;
    }
    nc->language = langlist[langnum];
    alog("%s: %s!%s@%s set the language of %s to %s",
         s_NickServ, u->nick, u->username, u->host, nc->display, getstring2(nc, LANG_NAME));
    notice_lang(s_NickServ, u, NICK_SASET_LANGUAGE_CHANGED, nc->display, getstring2(nc, LANG_NAME));

    return MOD_CONT;
}

/* EOF */
