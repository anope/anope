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

int do_cs_topic(User * u);
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

    c = createCommand("TOPIC", do_cs_topic, NULL, CHAN_HELP_TOPIC, -1, -1,
                      -1, -1);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_TOPIC);
}

/**
 * The /cs topic command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_cs_topic(User * u)
{
    char *chan = strtok(NULL, " ");
    char *topic = strtok(NULL, "");

    Channel *c;
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "TOPIC", CHAN_TOPIC_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (!is_services_admin(u) && !check_access(u, ci, CA_TOPIC)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        if (ci->last_topic)
            free(ci->last_topic);
        ci->last_topic = topic ? sstrdup(topic) : NULL;
        strscpy(ci->last_topic_setter, u->nick, NICKMAX);
        ci->last_topic_time = time(NULL);

        if (c->topic)
            free(c->topic);
        c->topic = topic ? sstrdup(topic) : NULL;
        strscpy(c->topic_setter, u->nick, NICKMAX);
        if (ircd->topictsbackward) {
            c->topic_time = c->topic_time - 1;
        } else {
            c->topic_time = ci->last_topic_time;
        }

        if (is_services_admin(u) && !check_access(u, ci, CA_TOPIC))
            alog("%s: %s!%s@%s changed topic of %s as services admin.",
                 s_ChanServ, u->nick, u->username, u->host, c->name);
        if (ircd->join2set) {
            if (whosends(ci) == s_ChanServ) {
                anope_cmd_join(s_ChanServ, c->name, c->creation_time);
                anope_cmd_mode(NULL, c->name, "+o %s", GET_BOT(s_ChanServ));
            }
        }
        anope_cmd_topic(whosends(ci), c->name, u->nick, topic ? topic : "",
                        c->topic_time);
        if (ircd->join2set) {
            if (whosends(ci) == s_ChanServ) {
                anope_cmd_part(s_ChanServ, c->name, NULL);
            }
        }
    }
    return MOD_CONT;
}
