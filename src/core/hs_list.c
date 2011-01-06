/* HostServ core functions
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

int listOut(User * u);
void myHostServHelp(User * u);

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

    c = createCommand("LIST", listOut, is_services_oper, -1, -1,
                      HOST_HELP_LIST, HOST_HELP_LIST, HOST_HELP_LIST);
    moduleAddCommand(HOSTSERV, c, MOD_UNIQUE);
    moduleSetHostHelp(myHostServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User * u)
{
    if (is_services_oper(u)) {
        notice_lang(s_HostServ, u, HOST_HELP_CMD_LIST);
    }
}

/**
 * The /hs list command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int listOut(User * u)
{
    char *key = strtok(NULL, "");
    struct tm *tm;
    char buf[BUFSIZE];
    int counter = 1;
    int from = 0, to = 0;
    char *tmp = NULL;
    char *s = NULL;
    int display_counter = 0;
    HostCore *head = NULL;
    HostCore *current;

    head = hostCoreListHead();

    current = head;
    if (current == NULL)
        notice_lang(s_HostServ, u, HOST_EMPTY);
    else {
                /**
		 * Do a check for a range here, then in the next loop
		 * we'll only display what has been requested..
		 **/
        if (key) {
            if (key[0] == '#') {
                tmp = myStrGetOnlyToken((key + 1), '-', 0);     /* Read FROM out */
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
                tmp = myStrGetTokenRemainder(key, '-', 1);      /* Read TO out */
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
                key = NULL;
            }
        }

        while (current != NULL) {
            if (key) {
                if (((match_wild_nocase(key, current->nick))
                     || (match_wild_nocase(key, current->vHost)))
                    && (display_counter < NSListMax)) {
                    display_counter++;
                    tm = localtime(&current->time);
                    strftime_lang(buf, sizeof(buf), u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    if (current->vIdent) {
                        notice_lang(s_HostServ, u, HOST_IDENT_ENTRY,
                                    counter, current->nick,
                                    current->vIdent, current->vHost,
                                    current->creator, buf);
                    } else {
                        notice_lang(s_HostServ, u, HOST_ENTRY, counter,
                                    current->nick, current->vHost,
                                    current->creator, buf);
                    }
                }
            } else {
                        /**
			 * List the host if its in the display range, and not more
			 * than NSListMax records have been displayed...
			 **/
                if ((((counter >= from) && (counter <= to))
                     || ((from == 0) && (to == 0)))
                    && (display_counter < NSListMax)) {
                    display_counter++;
                    tm = localtime(&current->time);
                    strftime_lang(buf, sizeof(buf), u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    if (current->vIdent) {
                        notice_lang(s_HostServ, u, HOST_IDENT_ENTRY,
                                    counter, current->nick,
                                    current->vIdent, current->vHost,
                                    current->creator, buf);
                    } else {
                        notice_lang(s_HostServ, u, HOST_ENTRY, counter,
                                    current->nick, current->vHost,
                                    current->creator, buf);
                    }
                }
            }
            counter++;
            current = current->next;
        }
        if (key) {
            notice_lang(s_HostServ, u, HOST_LIST_KEY_FOOTER, key,
                        display_counter);
        } else {
            if (from != 0) {
                notice_lang(s_HostServ, u, HOST_LIST_RANGE_FOOTER, from,
                            to);
            } else {
                notice_lang(s_HostServ, u, HOST_LIST_FOOTER,
                            display_counter);
            }
        }
    }
    return MOD_CONT;
}
