/* ChanServ core functions
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
int do_set_founder(User * u, ChannelInfo * ci, char *param);
int do_set_successor(User * u, ChannelInfo * ci, char *param);
int do_set_password(User * u, ChannelInfo * ci, char *param);
int do_set_desc(User * u, ChannelInfo * ci, char *param);
int do_set_url(User * u, ChannelInfo * ci, char *param);
int do_set_email(User * u, ChannelInfo * ci, char *param);
int do_set_entrymsg(User * u, ChannelInfo * ci, char *param);
int do_set_bantype(User * u, ChannelInfo * ci, char *param);
int do_set_mlock(User * u, ChannelInfo * ci, char *param);
int do_set_keeptopic(User * u, ChannelInfo * ci, char *param);
int do_set_topiclock(User * u, ChannelInfo * ci, char *param);
int do_set_private(User * u, ChannelInfo * ci, char *param);
int do_set_secureops(User * u, ChannelInfo * ci, char *param);
int do_set_securefounder(User * u, ChannelInfo * ci, char *param);
int do_set_restricted(User * u, ChannelInfo * ci, char *param);
int do_set_secure(User * u, ChannelInfo * ci, char *param);
int do_set_signkick(User * u, ChannelInfo * ci, char *param);
int do_set_opnotice(User * u, ChannelInfo * ci, char *param);
int do_set_xop(User * u, ChannelInfo * ci, char *param);
int do_set_peace(User * u, ChannelInfo * ci, char *param);
int do_set_noexpire(User * u, ChannelInfo * ci, char *param);
void myChanServHelp(User * u);

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

    c = createCommand("SET", do_set, NULL, CHAN_HELP_SET, -1, -1,
                      CHAN_SERVADMIN_HELP_SET, CHAN_SERVADMIN_HELP_SET);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET FOUNDER", NULL, NULL, CHAN_HELP_SET_FOUNDER, -1,
                      -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET SUCCESSOR", NULL, NULL, CHAN_HELP_SET_SUCCESSOR,
                      -1, -1, -1, -1);
    c->help_param1 = (char *) (long) CSMaxReg;
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET PASSWORD", NULL, NULL, CHAN_HELP_SET_PASSWORD,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET DESC", NULL, NULL, CHAN_HELP_SET_DESC, -1, -1,
                      -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET URL", NULL, NULL, CHAN_HELP_SET_URL, -1, -1, -1,
                      -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET EMAIL", NULL, NULL, CHAN_HELP_SET_EMAIL, -1, -1,
                      -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET ENTRYMSG", NULL, NULL, CHAN_HELP_SET_ENTRYMSG,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET BANTYPE", NULL, NULL, CHAN_HELP_SET_BANTYPE, -1,
                      -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET PRIVATE", NULL, NULL, CHAN_HELP_SET_PRIVATE, -1,
                      -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET KEEPTOPIC", NULL, NULL, CHAN_HELP_SET_KEEPTOPIC,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET TOPICLOCK", NULL, NULL, CHAN_HELP_SET_TOPICLOCK,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET MLOCK", NULL, NULL, CHAN_HELP_SET_MLOCK, -1, -1,
                      -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET RESTRICTED", NULL, NULL,
                      CHAN_HELP_SET_RESTRICTED, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET SECURE", NULL, NULL, CHAN_HELP_SET_SECURE, -1,
                      -1, -1, -1);
    c->help_param1 = s_NickServ;
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET SECUREOPS", NULL, NULL, CHAN_HELP_SET_SECUREOPS,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET SECUREFOUNDER", NULL, NULL,
                      CHAN_HELP_SET_SECUREFOUNDER, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET SIGNKICK", NULL, NULL, CHAN_HELP_SET_SIGNKICK,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET OPNOTICE", NULL, NULL, CHAN_HELP_SET_OPNOTICE,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET XOP", NULL, NULL, CHAN_HELP_SET_XOP, -1, -1, -1,
                      -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET PEACE", NULL, NULL, CHAN_HELP_SET_PEACE, -1, -1,
                      -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SET NOEXPIRE", NULL, NULL, -1, -1, -1,
                      CHAN_SERVADMIN_HELP_SET_NOEXPIRE,
                      CHAN_SERVADMIN_HELP_SET_NOEXPIRE);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    moduleSetChanHelp(myChanServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SET);
}

/**
 * The /cs set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/

/* Main SET routine.  Calls other routines as follows:
 *	do_set_command(User *command_sender, ChannelInfo *ci, char *param);
 * The parameter passed is the first space-delimited parameter after the
 * option name, except in the case of DESC, TOPIC, and ENTRYMSG, in which
 * it is the entire remainder of the line.  Additional parameters beyond
 * the first passed in the function call can be retrieved using
 * strtok(NULL, toks).
 */
int do_set(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *param;
    ChannelInfo *ci;
    int is_servadmin = is_services_admin(u);

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_SET_DISABLED);
        return MOD_CONT;
    }

    if (cmd) {
        if (stricmp(cmd, "DESC") == 0 || stricmp(cmd, "ENTRYMSG") == 0)
            param = strtok(NULL, "");
        else
            param = strtok(NULL, " ");
    } else {
        param = NULL;
    }

    if (!param && (!cmd || (stricmp(cmd, "SUCCESSOR") != 0 &&
                            stricmp(cmd, "URL") != 0 &&
                            stricmp(cmd, "EMAIL") != 0 &&
                            stricmp(cmd, "ENTRYMSG") != 0 &&
                            stricmp(cmd, "MLOCK") != 0))) {
        syntax_error(s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_servadmin && !check_access(u, ci, CA_SET)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "FOUNDER") == 0) {
        if (!is_servadmin
            && (ci->
                flags & CI_SECUREFOUNDER ? !is_real_founder(u,
                                                            ci) :
                !is_founder(u, ci))) {
            notice_lang(s_ChanServ, u, ACCESS_DENIED);
        } else {
            do_set_founder(u, ci, param);
        }
    } else if (stricmp(cmd, "SUCCESSOR") == 0) {
        if (!is_servadmin
            && (ci->
                flags & CI_SECUREFOUNDER ? !is_real_founder(u,
                                                            ci) :
                !is_founder(u, ci))) {
            notice_lang(s_ChanServ, u, ACCESS_DENIED);
        } else {
            do_set_successor(u, ci, param);
        }
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        if (!is_servadmin
            && (ci->
                flags & CI_SECUREFOUNDER ? !is_real_founder(u,
                                                            ci) :
                !is_founder(u, ci))) {
            notice_lang(s_ChanServ, u, ACCESS_DENIED);
        } else {
            do_set_password(u, ci, param);
        }
    } else if (stricmp(cmd, "DESC") == 0) {
        do_set_desc(u, ci, param);
    } else if (stricmp(cmd, "URL") == 0) {
        do_set_url(u, ci, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        do_set_email(u, ci, param);
    } else if (stricmp(cmd, "ENTRYMSG") == 0) {
        do_set_entrymsg(u, ci, param);
    } else if (stricmp(cmd, "TOPIC") == 0) {
        notice_lang(s_ChanServ, u, OBSOLETE_COMMAND, "TOPIC");
    } else if (stricmp(cmd, "BANTYPE") == 0) {
        do_set_bantype(u, ci, param);
    } else if (stricmp(cmd, "MLOCK") == 0) {
        do_set_mlock(u, ci, param);
    } else if (stricmp(cmd, "KEEPTOPIC") == 0) {
        do_set_keeptopic(u, ci, param);
    } else if (stricmp(cmd, "TOPICLOCK") == 0) {
        do_set_topiclock(u, ci, param);
    } else if (stricmp(cmd, "PRIVATE") == 0) {
        do_set_private(u, ci, param);
    } else if (stricmp(cmd, "SECUREOPS") == 0) {
        do_set_secureops(u, ci, param);
    } else if (stricmp(cmd, "SECUREFOUNDER") == 0) {
        if (!is_servadmin
            && (ci->
                flags & CI_SECUREFOUNDER ? !is_real_founder(u,
                                                            ci) :
                !is_founder(u, ci))) {
            notice_lang(s_ChanServ, u, ACCESS_DENIED);
        } else {
            do_set_securefounder(u, ci, param);
        }
    } else if (stricmp(cmd, "RESTRICTED") == 0) {
        do_set_restricted(u, ci, param);
    } else if (stricmp(cmd, "SECURE") == 0) {
        do_set_secure(u, ci, param);
    } else if (stricmp(cmd, "SIGNKICK") == 0) {
        do_set_signkick(u, ci, param);
    } else if (stricmp(cmd, "OPNOTICE") == 0) {
        do_set_opnotice(u, ci, param);
    } else if (stricmp(cmd, "XOP") == 0) {
        if (!(findModule("cs_xop"))) {
            notice_lang(s_ChanServ, u, CHAN_XOP_NOT_AVAILABLE, cmd);
        } else {
            do_set_xop(u, ci, param);
        }
    } else if (stricmp(cmd, "PEACE") == 0) {
        do_set_peace(u, ci, param);
    } else if (stricmp(cmd, "NOEXPIRE") == 0) {
        do_set_noexpire(u, ci, param);
    } else {
        notice_lang(s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, cmd);
        notice_lang(s_ChanServ, u, MORE_INFO, s_ChanServ, "SET");
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_founder(User * u, ChannelInfo * ci, char *param)
{
    NickAlias *na = findnick(param);
    NickCore *nc, *nc0 = ci->founder;

    if (!na) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
        return MOD_CONT;
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
        return MOD_CONT;
    }

    nc = na->nc;
    if (nc->channelmax > 0 && nc->channelcount >= nc->channelmax
        && !is_services_admin(u)) {
        notice_lang(s_ChanServ, u, CHAN_SET_FOUNDER_TOO_MANY_CHANS, param);
        return MOD_CONT;
    }

    alog("%s: Changing founder of %s from %s to %s by %s!%s@%s",
         s_ChanServ, ci->name, ci->founder->display, nc->display, u->nick,
         u->username, u->host);

    /* Founder and successor must not be the same group */
    if (nc == ci->successor)
        ci->successor = NULL;

    nc0->channelcount--;
    ci->founder = nc;
    nc->channelcount++;

    notice_lang(s_ChanServ, u, CHAN_FOUNDER_CHANGED, ci->name, param);
    return MOD_CONT;
}

/*************************************************************************/

int do_set_successor(User * u, ChannelInfo * ci, char *param)
{
    NickAlias *na;
    NickCore *nc;

    if (param) {
        na = findnick(param);

        if (!na) {
            notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
            return MOD_CONT;
        }
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
            return MOD_CONT;
        }
        if (na->nc == ci->founder) {
            notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_IS_FOUNDER, param,
                        ci->name);
            return MOD_CONT;
        }
        nc = na->nc;

    } else {
        nc = NULL;
    }

    alog("%s: Changing successor of %s from %s to %s by %s!%s@%s",
         s_ChanServ, ci->name,
         (ci->successor ? ci->successor->display : "none"),
         (nc ? nc->display : "none"), u->nick, u->username, u->host);

    ci->successor = nc;

    if (nc)
        notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_CHANGED, ci->name,
                    param);
    else
        notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_UNSET, ci->name);
    return MOD_CONT;
}

/*************************************************************************/

int do_set_password(User * u, ChannelInfo * ci, char *param)
{
    int len = strlen(param);

    if (stricmp(u->nick, param) == 0 || (StrictPasswords && len < 5)) {
        notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
        return MOD_CONT;
    }

    if (enc_encrypt_check_len(len ,PASSMAX - 1)) {
        notice_lang(s_ChanServ, u, PASSWORD_TOO_LONG);
        return MOD_CONT;
    }

    if (enc_encrypt(param, len, ci->founderpass, PASSMAX -1) < 0) {
        memset(param, 0, strlen(param));
        alog("%s: Failed to encrypt password for %s (set)", s_ChanServ,
             ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PASSWORD_FAILED);
        return MOD_CONT;
    }

    memset(param, 0, strlen(param));
    notice_lang(s_ChanServ, u, CHAN_PASSWORD_CHANGED, ci->name);

    if (get_access(u, ci) < ACCESS_FOUNDER) {
        alog("%s: %s!%s@%s set password as Services admin for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        if (WallSetpass)
            anope_cmd_global(s_ChanServ,
                             "\2%s\2 set password as Services admin for channel \2%s\2",
                             u->nick, ci->name);
    } else {
        alog("%s: %s!%s@%s changed password of %s (founder: %s)",
             s_ChanServ, u->nick, u->username, u->host,
             ci->name, ci->founder->display);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_desc(User * u, ChannelInfo * ci, char *param)
{
    if (ci->desc)
        free(ci->desc);
    ci->desc = sstrdup(param);
    alog("%s: %s!%s@%s set description of %s to: %s",
         s_ChanServ, u->nick, u->username, u->host, ci->name, ci->desc);
    notice_lang(s_ChanServ, u, CHAN_DESC_CHANGED, ci->name, param);
    return MOD_CONT;
}

/*************************************************************************/

int do_set_url(User * u, ChannelInfo * ci, char *param)
{
    if (ci->url)
        free(ci->url);
    if (param) {
        ci->url = sstrdup(param);
        alog("%s: %s!%s@%s set url for %s to: %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name, ci->url);
        notice_lang(s_ChanServ, u, CHAN_URL_CHANGED, ci->name, param);
    } else {
        ci->url = NULL;
        alog("%s: %s!%s@%s unset url for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_URL_UNSET, ci->name);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_email(User * u, ChannelInfo * ci, char *param)
{
    if (ci->email)
        free(ci->email);
    if (param) {
        ci->email = sstrdup(param);
        alog("%s: %s!%s@%s set email for %s to: %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name, ci->email);
        notice_lang(s_ChanServ, u, CHAN_EMAIL_CHANGED, ci->name, param);
    } else {
        ci->email = NULL;
        alog("%s: %s!%s@%s unset email for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_EMAIL_UNSET, ci->name);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_entrymsg(User * u, ChannelInfo * ci, char *param)
{
    if (ci->entry_message)
        free(ci->entry_message);
    if (param) {
        ci->entry_message = sstrdup(param);
        alog("%s: %s!%s@%s set entrymsg for %s to: %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name, ci->entry_message);
        notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name,
                    param);
    } else {
        ci->entry_message = NULL;
        alog("%s: %s!%s@%s unset entrymsg for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_mlock(User * u, ChannelInfo * ci, char *param)
{
    int add = -1;               /* 1 if adding, 0 if deleting, -1 if neither */
    unsigned char mode;
    CBMode *cbm;

    if (checkDefCon(DEFCON_NO_MLOCK_CHANGE)) {
        notice_lang(s_ChanServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    }

    /* Reinitialize everything */
    if (ircd->chanreg) {
        ci->mlock_on = ircd->regmode;
    } else {
        ci->mlock_on = 0;
    }
    ci->mlock_off = ci->mlock_limit = 0;
    ci->mlock_key = NULL;
    if (ircd->fmode) {
        ci->mlock_flood = NULL;
    }
    if (ircd->Lmode) {
        ci->mlock_redirect = NULL;
    }

    while (param && (mode = *param++)) {
        switch (mode) {
        case '+':
            add = 1;
            continue;
        case '-':
            add = 0;
            continue;
        default:
            if (add < 0)
                continue;
        }

        if ((int) mode < 128 && (cbm = &cbmodes[(int) mode])->flag != 0) {
            if ((cbm->flags & CBM_NO_MLOCK)
                || ((cbm->flags & CBM_NO_USER_MLOCK) && !is_oper(u))) {
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_IMPOSSIBLE_CHAR,
                            mode);
            } else if (add) {
                ci->mlock_on |= cbm->flag;
                ci->mlock_off &= ~cbm->flag;
                if (cbm->cssetvalue)
                    cbm->cssetvalue(ci, strtok(NULL, " "));
            } else {
                ci->mlock_off |= cbm->flag;
                if (ci->mlock_on & cbm->flag) {
                    ci->mlock_on &= ~cbm->flag;
                    if (cbm->cssetvalue)
                        cbm->cssetvalue(ci, NULL);
                }
            }
        } else {
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_UNKNOWN_CHAR, mode);
        }
    }                           /* while (*param) */

    if (ircd->Lmode) {
        /* We can't mlock +L if +l is not mlocked as well. */
        if ((ci->mlock_on & ircd->chan_lmode)
            && !(ci->mlock_on & anope_get_limit_mode())) {
            ci->mlock_on &= ~ircd->chan_lmode;
            free(ci->mlock_redirect);
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_L_REQUIRED);
        }
    }

    /* Some ircd we can't set NOKNOCK without INVITE */
    /* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
    if (ircd->noknock && ircd->knock_needs_i) {
        if ((ci->mlock_on & ircd->noknock)
            && !(ci->mlock_on & anope_get_invite_mode())) {
            ci->mlock_on &= ~ircd->noknock;
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_K_REQUIRED);
        }
    }

    /* Since we always enforce mode r there is no way to have no
     * mode lock at all.
     */
    if (get_mlock_modes(ci, 0)) {
        alog("%s: %s!%s@%s set mlock for %s to: %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name, get_mlock_modes(ci, 0));
        notice_lang(s_ChanServ, u, CHAN_MLOCK_CHANGED, ci->name,
                    get_mlock_modes(ci, 0));
    }

    /* Implement the new lock. */
    if (ci->c)
        check_modes(ci->c);
    return MOD_CONT;
}

/*************************************************************************/

int do_set_bantype(User * u, ChannelInfo * ci, char *param)
{
    char *endptr;

    int16 bantype = strtol(param, &endptr, 10);

    if (*endptr != 0 || bantype < 0 || bantype > 3) {
        notice_lang(s_ChanServ, u, CHAN_SET_BANTYPE_INVALID, param);
    } else {
        ci->bantype = bantype;
        alog("%s: %s!%s@%s set ban type for %s to: %d",
             s_ChanServ, u->nick, u->username, u->host, ci->name, ci->bantype);
        notice_lang(s_ChanServ, u, CHAN_SET_BANTYPE_CHANGED, ci->name,
                    ci->bantype);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_keeptopic(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_KEEPTOPIC;
        alog("%s: %s!%s@%s set keeptopic ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_KEEPTOPIC_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_KEEPTOPIC;
        alog("%s: %s!%s@%s set keeptopic OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_KEEPTOPIC_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET KEEPTOPIC",
                     CHAN_SET_KEEPTOPIC_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_topiclock(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_TOPICLOCK;
        alog("%s: %s!%s@%s set topiclock ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_TOPICLOCK_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_TOPICLOCK;
        alog("%s: %s!%s@%s set topiclock OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_TOPICLOCK_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET TOPICLOCK",
                     CHAN_SET_TOPICLOCK_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_private(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_PRIVATE;
        alog("%s: %s!%s@%s set private ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PRIVATE_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_PRIVATE;
        alog("%s: %s!%s@%s set private OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PRIVATE_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET PRIVATE",
                     CHAN_SET_PRIVATE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_secureops(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_SECUREOPS;
        alog("%s: %s!%s@%s set secureops ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECUREOPS_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_SECUREOPS;
        alog("%s: %s!%s@%s set secureops OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECUREOPS_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET SECUREOPS",
                     CHAN_SET_SECUREOPS_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_securefounder(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_SECUREFOUNDER;
        alog("%s: %s!%s@%s set securefounder ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECUREFOUNDER_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_SECUREFOUNDER;
        alog("%s: %s!%s@%s set securefounder OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECUREFOUNDER_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET SECUREFOUNDER",
                     CHAN_SET_SECUREFOUNDER_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_restricted(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_RESTRICTED;
        if (ci->levels[CA_NOJOIN] < 0)
            ci->levels[CA_NOJOIN] = 0;
        alog("%s: %s!%s@%s set restricted ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_RESTRICTED_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_RESTRICTED;
        if (ci->levels[CA_NOJOIN] >= 0)
            ci->levels[CA_NOJOIN] = -2;
        alog("%s: %s!%s@%s set restricted OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_RESTRICTED_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET RESTRICTED",
                     CHAN_SET_RESTRICTED_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_secure(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_SECURE;
        alog("%s: %s!%s@%s set secure ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECURE_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_SECURE;
        alog("%s: %s!%s@%s set secure OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SECURE_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET SECURE", CHAN_SET_SECURE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_signkick(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_SIGNKICK;
        ci->flags &= ~CI_SIGNKICK_LEVEL;
        alog("%s: %s!%s@%s set signkick ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_ON, ci->name);
    } else if (stricmp(param, "LEVEL") == 0) {
        ci->flags |= CI_SIGNKICK_LEVEL;
        ci->flags &= ~CI_SIGNKICK;
        alog("%s: %s!%s@%s set signkick LEVEL for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_LEVEL, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~(CI_SIGNKICK | CI_SIGNKICK_LEVEL);
        alog("%s: %s!%s@%s set signkick OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET SIGNKICK",
                     CHAN_SET_SIGNKICK_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_opnotice(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_OPNOTICE;
        alog("%s: %s!%s@%s set opnotice ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_OPNOTICE_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_OPNOTICE;
        alog("%s: %s!%s@%s set opnotice OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_OPNOTICE_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET OPNOTICE",
                     CHAN_SET_OPNOTICE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

#define CHECKLEV(lev) ((ci->levels[(lev)] != ACCESS_INVALID) && (access->level >= ci->levels[(lev)]))

int do_set_xop(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        if (!(ci->flags & CI_XOP)) {
            int i;
            ChanAccess *access;

            for (access = ci->access, i = 0; i < ci->accesscount;
                 access++, i++) {
                if (!access->in_use)
                    continue;
                /* This will probably cause wrong levels to be set, but hey,
                 * it's better than losing it altogether.
                 */
                if (CHECKLEV(CA_AKICK) || CHECKLEV(CA_SET)) {
                    access->level = ACCESS_SOP;
                } else if (CHECKLEV(CA_AUTOOP) || CHECKLEV(CA_OPDEOP)
                           || CHECKLEV(CA_OPDEOPME)) {
                    access->level = ACCESS_AOP;
                } else if (ircd->halfop && (CHECKLEV(CA_AUTOHALFOP) || CHECKLEV(CA_HALFOP)
                        || CHECKLEV(CA_HALFOPME))) {
                        access->level = ACCESS_HOP;
                } else if (CHECKLEV(CA_AUTOVOICE) || CHECKLEV(CA_VOICE)
                           || CHECKLEV(CA_VOICEME)) {
                    access->level = ACCESS_VOP;
                } else {
                    access->in_use = 0;
                    access->nc = NULL;
                }
            }
            CleanAccess(ci);

            reset_levels(ci);
            ci->flags |= CI_XOP;
        }

        alog("%s: %s!%s@%s enabled XOP for %s", s_ChanServ, u->nick,
             u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_XOP_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_XOP;

        alog("%s: %s!%s@%s disabled XOP for %s", s_ChanServ, u->nick,
             u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_XOP_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET XOP", CHAN_SET_XOP_SYNTAX);
    }
    return MOD_CONT;
}

#undef CHECKLEV

/*************************************************************************/

int do_set_peace(User * u, ChannelInfo * ci, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_PEACE;
        alog("%s: %s!%s@%s set peace ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PEACE_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_PEACE;
        alog("%s: %s!%s@%s set peace OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PEACE_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET PEACE", CHAN_SET_PEACE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_set_noexpire(User * u, ChannelInfo * ci, char *param)
{
    if (!is_services_admin(u)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    }
    if (stricmp(param, "ON") == 0) {
        ci->flags |= CI_NO_EXPIRE;
        alog("%s: %s!%s@%s set noexpire ON for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_NOEXPIRE_ON, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~CI_NO_EXPIRE;
        alog("%s: %s!%s@%s set noexpire OFF for %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_NOEXPIRE_OFF, ci->name);
    } else {
        syntax_error(s_ChanServ, u, "SET NOEXPIRE",
                     CHAN_SET_NOEXPIRE_SYNTAX);
    }
    return MOD_CONT;
}
