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

int do_list(User * u);
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

    c = createCommand("LIST", do_list, NULL, -1, NICK_HELP_LIST, -1,
                      NICK_SERVADMIN_HELP_LIST, NICK_SERVADMIN_HELP_LIST);

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
    if (!NSListOpersOnly || (is_oper(u))) {
        notice_lang(s_NickServ, u, NICK_HELP_CMD_LIST);
    }
}

/**
 * The /ns list command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_list(User * u)
{

/* SADMINS can search for nicks based on their NS_VERBOTEN and NS_NO_EXPIRE
 * status. The keywords FORBIDDEN and NOEXPIRE represent these two states
 * respectively. These keywords should be included after the search pattern.
 * Multiple keywords are accepted and should be separated by spaces. Only one
 * of the keywords needs to match a nick's state for the nick to be displayed.
 * Forbidden nicks can be identified by "[Forbidden]" appearing in the last
 * seen address field. Nicks with NOEXPIRE set are preceeded by a "!". Only
 * SADMINS will be shown forbidden nicks and the "!" indicator.
 * Syntax for sadmins: LIST pattern [FORBIDDEN] [NOEXPIRE]
 * -TheShadow
 *
 * UPDATE: SUSPENDED keyword is now accepted as well.
 */


    char *pattern = strtok(NULL, " ");
    char *keyword;
    NickAlias *na;
    NickCore *mync;
    int nnicks, i;
    char buf[BUFSIZE];
    int is_servadmin = is_services_admin(u);
    int16 matchflags = 0;
    NickRequest *nr = NULL;
    int nronly = 0;
    int susp_keyword = 0;
    char noexpire_char = ' ';
    int count = 0, from = 0, to = 0, tofree = 0;
    char *tmp = NULL;
    char *s = NULL;

    if (!(!NSListOpersOnly || (is_oper(u)))) {  /* reverse the help logic */
        notice_lang(s_NickServ, u, ACCESS_DENIED);
        return MOD_STOP;
    }

    if (!pattern) {
        syntax_error(s_NickServ, u, "LIST",
                     is_servadmin ? NICK_LIST_SERVADMIN_SYNTAX :
                     NICK_LIST_SYNTAX);
    } else {

        if (pattern) {
            if (pattern[0] == '#') {
                tmp = myStrGetOnlyToken((pattern + 1), '-', 0); /* Read FROM out */
                if (!tmp) {
                	notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        free(tmp);
	                	notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
                        return MOD_CONT;
                    }
                }
                from = atoi(tmp);
                free(tmp);
                tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
                if (!tmp) {
                	notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        free(tmp);
	                	notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
                        return MOD_CONT;
                    }
                }
                to = atoi(tmp);
                free(tmp);
                pattern = sstrdup("*");
                tofree = 1;
            }
        }

        nnicks = 0;

        while (is_servadmin && (keyword = strtok(NULL, " "))) {
            if (stricmp(keyword, "FORBIDDEN") == 0)
                matchflags |= NS_VERBOTEN;
            if (stricmp(keyword, "NOEXPIRE") == 0)
                matchflags |= NS_NO_EXPIRE;
            if (stricmp(keyword, "SUSPENDED") == 0)
                susp_keyword = 1;
            if (stricmp(keyword, "UNCONFIRMED") == 0)
                nronly = 1;
        }

        mync = (nick_identified(u) ? u->na->nc : NULL);

        notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
        if (nronly != 1) {
            for (i = 0; i < 1024; i++) {
                for (na = nalists[i]; na; na = na->next) {
                    /* Don't show private and forbidden nicks to non-services admins. */
                    if ((na->status & NS_VERBOTEN) && !is_servadmin)
                        continue;
                    if ((na->nc->flags & NI_PRIVATE) && !is_servadmin
                        && na->nc != mync)
                        continue;
                    if ((matchflags != 0) && !(na->status & matchflags) && (susp_keyword == 0))
                        continue;
		    else if ((susp_keyword == 1) && !(na->nc->flags & NI_SUSPENDED))
                        continue;

                    /* We no longer compare the pattern against the output buffer.
                     * Instead we build a nice nick!user@host buffer to compare.
                     * The output is then generated separately. -TheShadow */
                    snprintf(buf, sizeof(buf), "%s!%s", na->nick,
                             (na->last_usermask
                              && !(na->status & NS_VERBOTEN)) ? na->
                             last_usermask : "*@*");
                    if (stricmp(pattern, na->nick) == 0
                        || match_wild_nocase(pattern, buf)) {

                        if ((((count + 1 >= from) && (count + 1 <= to))
                             || ((from == 0) && (to == 0)))
                            && (++nnicks <= NSListMax)) {
                            if (is_servadmin
                                && (na->status & NS_NO_EXPIRE))
                                noexpire_char = '!';
                            else {
                                noexpire_char = ' ';
                            }
                            if ((na->nc->flags & NI_HIDE_MASK)
                                && !is_servadmin && na->nc != mync) {
                                snprintf(buf, sizeof(buf),
                                         "%-20s  [Hostname Hidden]",
                                         na->nick);
                            } else if (na->status & NS_VERBOTEN) {
                                snprintf(buf, sizeof(buf),
                                         "%-20s  [Forbidden]", na->nick);
                            } else if (na->nc->flags & NI_SUSPENDED) {
                                snprintf(buf, sizeof(buf),
                                         "%-20s  [Suspended]", na->nick);
                            } else {
                                snprintf(buf, sizeof(buf), "%-20s  %s",
                                         na->nick, na->last_usermask);
                            }
                            notice_user(s_NickServ, u, "   %c%s",
                                        noexpire_char, buf);
                        }
                        count++;
                    }
                }
            }
        }

        if (nronly == 1 || (is_servadmin && matchflags == 0 && susp_keyword == 0)) {
            noexpire_char = ' ';
            for (i = 0; i < 1024; i++) {
                for (nr = nrlists[i]; nr; nr = nr->next) {
                    snprintf(buf, sizeof(buf), "%s!*@*", nr->nick);
                    if (stricmp(pattern, nr->nick) == 0
                        || match_wild_nocase(pattern, buf)) {
                        if (++nnicks <= NSListMax) {
                            snprintf(buf, sizeof(buf),
                                     "%-20s  [UNCONFIRMED]", nr->nick);
                            notice_user(s_NickServ, u, "   %c%s",
                                        noexpire_char, buf);
                        }
                    }
                }
            }
        }
        notice_lang(s_NickServ, u, NICK_LIST_RESULTS,
                    nnicks > NSListMax ? NSListMax : nnicks, nnicks);
    }
    if (tofree)
        free(pattern);
    return MOD_CONT;
}
