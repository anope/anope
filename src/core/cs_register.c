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

int do_register(User * u);
void myChanServHelp(User * u);

/**
 * Create the off command, and tell anope about it.
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

    c = createCommand("REGISTER", do_register, NULL, CHAN_HELP_REGISTER,
                      -1, -1, -1, -1);
    c->help_param1 = s_NickServ;
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_REGISTER);
}

/**
 * The /cs register command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_register(User * u)
{
    char *chan = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    char *desc = strtok(NULL, "");
    NickCore *nc;
    Channel *c;
    ChannelInfo *ci;
    struct u_chaninfolist *uc;
    int is_servadmin = is_services_admin(u);
    char tmp_pass[PASSMAX];

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_DISABLED);
        return MOD_CONT;
    }

    if (checkDefCon(DEFCON_NO_NEW_CHANNELS)) {
        notice_lang(s_ChanServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    }

    if (!desc) {
        syntax_error(s_ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
    } else if (*chan == '&') {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_NOT_LOCAL);
    } else if (*chan != '#') {
        notice_lang(s_ChanServ, u, CHAN_SYMBOL_REQUIRED);
    } else if (!anope_valid_chan(chan)) {
        notice_lang(s_ChanServ, u, CHAN_X_INVALID, chan);
    } else if (!u->na || !(nc = u->na->nc)) {
        notice_lang(s_ChanServ, u, CHAN_MUST_REGISTER_NICK, s_NickServ);
    } else if (!nick_recognized(u)) {
        notice_lang(s_ChanServ, u, CHAN_MUST_IDENTIFY_NICK, s_NickServ,
                    s_NickServ);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_NONE_CHANNEL, chan);
    } else if ((ci = cs_findchan(chan)) != NULL) {
        if (ci->flags & CI_VERBOTEN) {
            alog("%s: Attempt to register FORBIDden channel %s by %s!%s@%s", s_ChanServ, ci->name, u->nick, u->username, u->host);
            notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
        }
    } else if (!stricmp(chan, "#")) {
        notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
    } else if (!chan_has_user_status(c, u, CUS_OP)) {
        notice_lang(s_ChanServ, u, CHAN_MUST_BE_CHANOP);

    } else if (!is_servadmin && nc->channelmax > 0
               && nc->channelcount >= nc->channelmax) {
        notice_lang(s_ChanServ, u, nc->channelcount >
                    nc->channelmax ? CHAN_EXCEEDED_CHANNEL_LIMIT :
                    CHAN_REACHED_CHANNEL_LIMIT, nc->channelmax);
    } else if (stricmp(u->nick, pass) == 0
               || (StrictPasswords && strlen(pass) < 5)) {
        notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
    } else if(enc_encrypt_check_len(strlen(pass), PASSMAX - 1)) {
        notice_lang(s_ChanServ, u, PASSWORD_TOO_LONG);
    } else if (!(ci = makechan(chan))) {
        alog("%s: makechan() failed for REGISTER %s", s_ChanServ, chan);
        notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);

    } else if (enc_encrypt(pass, strlen(pass), ci->founderpass, PASSMAX - 1) < 0) {
        alog("%s: Couldn't encrypt password for %s (REGISTER)",
             s_ChanServ, chan);
        notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);
        delchan(ci);
    } else {
        c->ci = ci;
        ci->c = c;
        ci->bantype = CSDefBantype;
        ci->flags = CSDefFlags;
        ci->mlock_on = ircd->defmlock;
        ci->memos.memomax = MSMaxMemos;
        ci->last_used = ci->time_registered;
        ci->founder = nc;

        ci->desc = sstrdup(desc);
        if (c->topic) {
            ci->last_topic = sstrdup(c->topic);
            strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
            ci->last_topic_time = c->topic_time;
        } else {
            /* Set this to something, otherwise it will maliform the topic */
            strscpy(ci->last_topic_setter, s_ChanServ, NICKMAX);
        }
        ci->bi = NULL;
        ci->botflags = BSDefFlags;
        ci->founder->channelcount++;
        alog("%s: Channel '%s' registered by %s!%s@%s", s_ChanServ, chan,
             u->nick, u->username, u->host);
        notice_lang(s_ChanServ, u, CHAN_REGISTERED, chan, u->nick);

        if(enc_decrypt(ci->founderpass,tmp_pass,PASSMAX - 1) == 1) {
            notice_lang(s_ChanServ, u, CHAN_PASSWORD_IS, tmp_pass);
        }

        uc = scalloc(sizeof(*uc), 1);
        uc->next = u->founder_chans;
        uc->prev = NULL;
        if (u->founder_chans)
            u->founder_chans->prev = uc;
        u->founder_chans = uc;
        uc->chan = ci;
        /* Implement new mode lock */
        check_modes(c);
        /* On most ircds you do not receive the admin/owner mode till its registered */
        if (ircd->admin) {
            anope_cmd_mode(s_ChanServ, chan, "%s %s", ircd->adminset,
                           GET_USER(u));
        }
        if (ircd->owner && ircd->ownerset) {
            anope_cmd_mode(s_ChanServ, chan, "%s %s", ircd->ownerset,
                           GET_USER(u));
        }
        send_event(EVENT_CHAN_REGISTERED, 1, chan);
    }
    return MOD_CONT;
}
