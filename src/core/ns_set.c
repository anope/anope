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

int do_set(User * u);
int do_set_display(User * u, NickCore * nc, char *param);
int do_set_password(User * u, NickCore * nc, char *param);
int do_set_language(User * u, NickCore * nc, char *param);
int do_set_url(User * u, NickCore * nc, char *param);
int do_set_email(User * u, NickCore * nc, char *param);
int do_set_greet(User * u, NickCore * nc, char *param);
int do_set_icq(User * u, NickCore * nc, char *param);
int do_set_kill(User * u, NickCore * nc, char *param);
int do_set_secure(User * u, NickCore * nc, char *param);
int do_set_private(User * u, NickCore * nc, char *param);
int do_set_msg(User * u, NickCore * nc, char *param);
int do_set_hide(User * u, NickCore * nc, char *param);
int do_set_autoop(User *u, NickCore *nc, char *param);
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

    c = createCommand("SET", do_set, NULL, NICK_HELP_SET, -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET DISPLAY", NULL, NULL, NICK_HELP_SET_DISPLAY, -1,
                      -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET PASSWORD", NULL, NULL, NICK_HELP_SET_PASSWORD,
                      -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET URL", NULL, NULL, NICK_HELP_SET_URL, -1, -1, -1,
                      -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET EMAIL", NULL, NULL, NICK_HELP_SET_EMAIL, -1, -1,
                      -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET ICQ", NULL, NULL, NICK_HELP_SET_ICQ, -1, -1, -1,
                      -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET GREET", NULL, NULL, NICK_HELP_SET_GREET, -1, -1,
                      -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET KILL", NULL, NULL, NICK_HELP_SET_KILL, -1, -1,
                      -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET SECURE", NULL, NULL, NICK_HELP_SET_SECURE, -1,
                      -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET PRIVATE", NULL, NULL, NICK_HELP_SET_PRIVATE, -1,
                      -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET MSG", NULL, NULL, NICK_HELP_SET_MSG, -1, -1, -1,
                      -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET HIDE", NULL, NULL, NICK_HELP_SET_HIDE, -1, -1,
                      -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("SET AUTOOP", NULL, NULL, NICK_HELP_SET_AUTOOP, -1, -1,
                      -1, -1);
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
    notice_lang(s_NickServ, u, NICK_HELP_CMD_SET);
}

/**
 * The /ns set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    NickAlias *na = u->na;

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_SET_DISABLED);
        return MOD_CONT;
    }

    if (!param
        && (!cmd
            || (stricmp(cmd, "URL") != 0 && stricmp(cmd, "EMAIL") != 0
                && stricmp(cmd, "GREET") != 0
                && stricmp(cmd, "ICQ") != 0))) {
        syntax_error(s_NickServ, u, "SET", NICK_SET_SYNTAX);
    } else if (!na) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
    } else if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (stricmp(cmd, "DISPLAY") == 0) {
        do_set_display(u, na->nc, param);
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        do_set_password(u, na->nc, param);
    } else if (stricmp(cmd, "LANGUAGE") == 0) {
        do_set_language(u, na->nc, param);
    } else if (stricmp(cmd, "URL") == 0) {
        do_set_url(u, na->nc, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        do_set_email(u, na->nc, param);
    } else if (stricmp(cmd, "ICQ") == 0) {
        do_set_icq(u, na->nc, param);
    } else if (stricmp(cmd, "GREET") == 0) {
        do_set_greet(u, na->nc, param);
    } else if (stricmp(cmd, "KILL") == 0) {
        do_set_kill(u, na->nc, param);
    } else if (stricmp(cmd, "SECURE") == 0) {
        do_set_secure(u, na->nc, param);
    } else if (stricmp(cmd, "PRIVATE") == 0) {
        do_set_private(u, na->nc, param);
    } else if (stricmp(cmd, "MSG") == 0) {
        do_set_msg(u, na->nc, param);
    } else if (stricmp(cmd, "HIDE") == 0) {
        do_set_hide(u, na->nc, param);
    } else if (stricmp(cmd, "AUTOOP") == 0) {
        do_set_autoop(u, na->nc, param);
    } else {
        notice_lang(s_NickServ, u, NICK_SET_UNKNOWN_OPTION, cmd);
    }
    return MOD_CONT;
}

int do_set_display(User * u, NickCore * nc, char *param)
{
    int i;
    NickAlias *na;

    /* First check whether param is a valid nick of the group */
    for (i = 0; i < nc->aliases.count; i++) {
        na = nc->aliases.list[i];
        if (!stricmp(na->nick, param)) {
            param = na->nick;   /* Because case may differ */
            break;
        }
    }

    if (i == nc->aliases.count) {
        notice_lang(s_NickServ, u, NICK_SET_DISPLAY_INVALID);
        return MOD_CONT;
    }

    change_core_display(nc, param);
    alog("%s: %s!%s@%s set their display nick to %s",
         s_NickServ, u->nick, u->username, u->host, nc->display);
    notice_lang(s_NickServ, u, NICK_SET_DISPLAY_CHANGED, nc->display);

    /* Enable nick tracking if enabled */
    if (NSNickTracking)
        nsStartNickTracking(u);

    return MOD_CONT;
}

int do_set_password(User * u, NickCore * nc, char *param)
{
    int len = strlen(param);
    char tmp_pass[PASSMAX];

    if (stricmp(nc->display, param) == 0 || (StrictPasswords && len < 5)) {
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
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_FAILED);
        return MOD_CONT;
    }
    memset(param, 0, len);

    if(enc_decrypt(nc->pass,tmp_pass,PASSMAX - 1)==1) {
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, tmp_pass);
    } else {
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED);
    }

    alog("%s: %s!%s@%s (e-mail: %s) changed its password.", s_NickServ,
         u->nick, u->username, u->host, (nc->email ? nc->email : "none"));

    return MOD_CONT;
}

int do_set_language(User * u, NickCore * nc, char *param)
{
    int langnum;

    if (param[strspn(param, "0123456789")] != 0) {      /* i.e. not a number */
        syntax_error(s_NickServ, u, "SET LANGUAGE",
                     NICK_SET_LANGUAGE_SYNTAX);
        return MOD_CONT;
    }
    langnum = atoi(param) - 1;
    if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0) {
        notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN, langnum + 1,
                    s_NickServ);
        return MOD_CONT;
    }
    nc->language = langlist[langnum];
    alog("%s: %s!%s@%s set their language to %s",
         s_NickServ, u->nick, u->username, u->host, getstring2(nc, LANG_NAME));
    notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_CHANGED);
    return MOD_CONT;
}

int do_set_url(User * u, NickCore * nc, char *param)
{
    if (nc->url)
        free(nc->url);

    if (param) {
        nc->url = sstrdup(param);
        alog("%s: %s!%s@%s set their url to %s",
             s_NickServ, u->nick, u->username, u->host, nc->url);
        notice_lang(s_NickServ, u, NICK_SET_URL_CHANGED, param);
    } else {
        nc->url = NULL;
        alog("%s: %s!%s@%s unset their url",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_URL_UNSET);
    }
    return MOD_CONT;
}

int do_set_email(User * u, NickCore * nc, char *param)
{
    if (!param && NSForceEmail) {
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
        return MOD_CONT;
    } else if (param && !MailValidate(param)) {
        notice_lang(s_NickServ, u, MAIL_X_INVALID, param);
        return MOD_CONT;
    }

    alog("%s: %s!%s@%s (e-mail: %s) changed its e-mail to %s.",
         s_NickServ, u->nick, u->username, u->host,
         (nc->email ? nc->email : "none"), (param ? param : "none"));

    if (nc->email)
        free(nc->email);

    if (param) {
        nc->email = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_CHANGED, param);
    } else {
        nc->email = NULL;
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET);
    }
    return MOD_CONT;
}

int do_set_icq(User * u, NickCore * nc, char *param)
{
    if (param) {
        int32 tmp = atol(param);
        if (!tmp) {
            notice_lang(s_NickServ, u, NICK_SET_ICQ_INVALID, param);
        } else {
            nc->icq = tmp;
            alog("%s: %s!%s@%s set their icq to %d",
                 s_NickServ, u->nick, u->username, u->host, nc->icq);
            notice_lang(s_NickServ, u, NICK_SET_ICQ_CHANGED, param);
        }
    } else {
        nc->icq = 0;
        alog("%s: %s!%s@%s unset their icq",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_ICQ_UNSET);
    }
    return MOD_CONT;
}

int do_set_greet(User * u, NickCore * nc, char *param)
{
    if (nc->greet)
        free(nc->greet);

    if (param) {
        char buf[BUFSIZE];
        char *end = strtok(NULL, "");

        snprintf(buf, sizeof(buf), "%s%s%s", param, (end ? " " : ""),
                 (end ? end : ""));

        nc->greet = sstrdup(buf);
        alog("%s: %s!%s@%s set their greet to %s",
             s_NickServ, u->nick, u->username, u->host, nc->greet);
        notice_lang(s_NickServ, u, NICK_SET_GREET_CHANGED, buf);
    } else {
        nc->greet = NULL;
        notice_lang(s_NickServ, u, NICK_SET_GREET_UNSET);
    }
    return MOD_CONT;
}

int do_set_kill(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_KILLPROTECT;
        nc->flags &= ~(NI_KILL_QUICK | NI_KILL_IMMED);
        alog("%s: %s!%s@%s set kill ON",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_KILL_ON);
    } else if (stricmp(param, "QUICK") == 0) {
        nc->flags |= NI_KILLPROTECT | NI_KILL_QUICK;
        nc->flags &= ~NI_KILL_IMMED;
        alog("%s: %s!%s@%s set kill QUICK",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_KILL_QUICK);
    } else if (stricmp(param, "IMMED") == 0) {
        if (NSAllowKillImmed) {
            nc->flags |= NI_KILLPROTECT | NI_KILL_IMMED;
            nc->flags &= ~NI_KILL_QUICK;
            alog("%s: %s!%s@%s set kill IMMED",
                 s_NickServ, u->nick, u->username, u->host);
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED);
        } else {
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED_DISABLED);
        }
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
        alog("%s: %s!%s@%s set kill OFF",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_KILL_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET KILL",
                     NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX :
                     NICK_SET_KILL_SYNTAX);
    }
    return MOD_CONT;
}

int do_set_secure(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_SECURE;
        alog("%s: %s!%s@%s set secure ON",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_SECURE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_SECURE;
        alog("%s: %s!%s@%s set secure OFF",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_SECURE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
    }
    return MOD_CONT;
}

int do_set_private(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_PRIVATE;
        alog("%s: %s!%s@%s set private ON",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_PRIVATE;
        alog("%s: %s!%s@%s set private OFF",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET PRIVATE",
                     NICK_SET_PRIVATE_SYNTAX);
    }
    return MOD_CONT;
}

int do_set_msg(User * u, NickCore * nc, char *param)
{
    if (!UsePrivmsg) {
        notice_lang(s_NickServ, u, NICK_SET_OPTION_DISABLED, "MSG");
        return MOD_CONT;
    }

    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_MSG;
        alog("%s: %s!%s@%s set msg ON",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_MSG_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_MSG;
        alog("%s: %s!%s@%s set msg OFF",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_MSG_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
    }
    return MOD_CONT;
}

int do_set_hide(User * u, NickCore * nc, char *param)
{
    int flag, onmsg, offmsg;
    char *param2;

    if (stricmp(param, "EMAIL") == 0) {
        flag = NI_HIDE_EMAIL;
        onmsg = NICK_SET_HIDE_EMAIL_ON;
        offmsg = NICK_SET_HIDE_EMAIL_OFF;
    } else if (stricmp(param, "USERMASK") == 0) {
        flag = NI_HIDE_MASK;
        onmsg = NICK_SET_HIDE_MASK_ON;
        offmsg = NICK_SET_HIDE_MASK_OFF;
    } else if (stricmp(param, "STATUS") == 0) {
        flag = NI_HIDE_STATUS;
        onmsg = NICK_SET_HIDE_STATUS_ON;
        offmsg = NICK_SET_HIDE_STATUS_OFF;
    } else if (stricmp(param, "QUIT") == 0) {
        flag = NI_HIDE_QUIT;
        onmsg = NICK_SET_HIDE_QUIT_ON;
        offmsg = NICK_SET_HIDE_QUIT_OFF;
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
        return MOD_CONT;
    }

    param2 = strtok(NULL, " ");
    if (!param2) {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
    } else if (stricmp(param2, "ON") == 0) {
        nc->flags |= flag;
        alog("%s: %s!%s@%s set hide %s ON",
             s_NickServ, u->nick, u->username, u->host, param);
        notice_lang(s_NickServ, u, onmsg, s_NickServ);
    } else if (stricmp(param2, "OFF") == 0) {
        nc->flags &= ~flag;
        alog("%s: %s!%s@%s set hide %s OFF",
             s_NickServ, u->nick, u->username, u->host, param);
        notice_lang(s_NickServ, u, offmsg, s_NickServ);
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
    }

    return MOD_CONT;
}

int do_set_autoop(User *u, NickCore *nc, char *param) {

    /**
     * This works the other way around, the absence of this flag denotes ON
     * This is so when people upgrade, and dont have the flag
     * the default is on
     **/
    if (stricmp(param, "ON") == 0) {
        nc->flags &= ~NI_AUTOOP;
        alog("%s: %s!%s@%s set autoop ON",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_AUTOOP_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags |= NI_AUTOOP;
        alog("%s: %s!%s@%s set autoop OFF",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_SET_AUTOOP_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET AUTOOP", NICK_SET_AUTOOP_SYNTAX);
    }

    return MOD_CONT;
}


/* EOF */
