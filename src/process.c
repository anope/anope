/* Main processing code for Services.
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

#include "services.h"
#include "messages.h"
#include "modules.h"

/*************************************************************************/

/* Use ignore code? */
int allow_ignore = 1;

/* People to ignore (hashed by first character of nick). */
IgnoreData *ignore[256];

/*************************************************************************/

/* add_ignore: Add someone to the ignorance list for the next `delta'
 *             seconds.
 */

void add_ignore(const char *nick, time_t delta)
{
    IgnoreData *ign;
    char who[NICKMAX];
    time_t now = time(NULL);
    IgnoreData **whichlist = &ignore[tolower(nick[0])];

    strscpy(who, nick, NICKMAX);
    for (ign = *whichlist; ign; ign = ign->next) {
        if (stricmp(ign->who, who) == 0)
            break;
    }
    if (ign) {
        if (ign->time > now)
            ign->time += delta;
        else
            ign->time = now + delta;
    } else {
        ign = scalloc(sizeof(*ign), 1);
        strscpy(ign->who, who, sizeof(ign->who));
        ign->time = now + delta;
        ign->next = *whichlist;
        *whichlist = ign;
    }
}

/*************************************************************************/

/* get_ignore: Retrieve an ignorance record for a nick.  If the nick isn't
 *             being ignored, return NULL and flush the record from the
 *             in-core list if it exists (i.e. ignore timed out).
 */

IgnoreData *get_ignore(const char *nick)
{
    IgnoreData *ign, *prev;
    time_t now = time(NULL);
    IgnoreData **whichlist = &ignore[tolower(nick[0])];
    User *u = finduser(nick);
    IgnoreData **whichlist2 = NULL;
    /* Bleah, this doesn't work. I need a way to get the first char of u->username.
       /if (u) whichlist2 = &ignore[tolower(u->username[0])]; */
    IgnoreData **whichlistast = &ignore[42];    /* * */
    IgnoreData **whichlistqst = &ignore[63];    /* ? */
    int finished = 0;


    /* User has disabled the IGNORE system */
    if (!allow_ignore) {
        return NULL;
    }

    /* if an oper gets on the ignore list we let them privmsg, this will allow
       them in places we call get_ignore to get by */
    if (u && is_oper(u)) {
        return NULL;
    }

    for (ign = *whichlist, prev = NULL; ign; prev = ign, ign = ign->next) {
        if (stricmp(ign->who, nick) == 0) {
            finished = 1;
            break;
        }
    }
    /* We can only do the next checks if we have an actual user -GD */
    if (u) {
        if (!finished && whichlist2) {
            for (ign = *whichlist2, prev = NULL; ign;
                 prev = ign, ign = ign->next) {
                if (match_usermask(ign->who, u)) {
                    finished = 1;
                    break;
                }
            }
        }
        if (!finished) {
            for (ign = *whichlistast, prev = NULL; ign;
                 prev = ign, ign = ign->next) {
                if (match_usermask(ign->who, u)) {
                    finished = 1;
                    break;
                }
            }
        }
        if (!finished) {
            for (ign = *whichlistqst, prev = NULL; ign;
                 prev = ign, ign = ign->next) {
                if (match_usermask(ign->who, u)) {
                    finished = 1;
                    break;
                }
            }
        }
    }
    if (ign && ign->time <= now) {
        if (prev)
            prev->next = ign->next;
        else
            *whichlist = ign->next;
        free(ign);
        ign = NULL;
    }
    return ign;
}

/*************************************************************************/

/* split_buf:  Split a buffer into arguments and store the arguments in an
 *             argument vector pointed to by argv (which will be malloc'd
 *             as necessary); return the argument count.  If colon_special
 *             is non-zero, then treat a parameter with a leading ':' as
 *             the last parameter of the line, per the IRC RFC.  Destroys
 *             the buffer by side effect.
 */

int split_buf(char *buf, char ***argv, int colon_special)
{
    int argvsize = 8;
    int argc;
    char *s;

    *argv = scalloc(sizeof(char *) * argvsize, 1);
    argc = 0;
    while (*buf) {
        if (argc == argvsize) {
            argvsize += 8;
            *argv = srealloc(*argv, sizeof(char *) * argvsize);
        }
        if (*buf == ':') {
            (*argv)[argc++] = buf + 1;
            buf = "";
        } else {
            s = strpbrk(buf, " ");
            if (s) {
                *s++ = 0;
                while (*s == ' ')
                    s++;
            } else {
                s = buf + strlen(buf);
            }
            (*argv)[argc++] = buf;
            buf = s;
        }
    }
    return argc;
}

/*************************************************************************/

/* process:  Main processing routine.  Takes the string in inbuf (global
 *           variable) and does something appropriate with it. */

void process()
{
    int retVal = 0;
    Message *current = NULL;
    char source[64];
    char cmd[64];
    char buf[512];              /* Longest legal IRC command line */
    char *s;
    int ac;                     /* Parameters for the command */
    char **av;
    Message *m;

    /* zero out the buffers before we do much else */
    *buf = '\0';
    *source = '\0';
    *cmd = '\0';

    /* If debugging, log the buffer */
    if (debug) {
        alog("debug: Received: %s", inbuf);
    }

    /* First make a copy of the buffer so we have the original in case we
     * crash - in that case, we want to know what we crashed on. */
    strscpy(buf, inbuf, sizeof(buf));

    doCleanBuffer((char *) buf);

    /* Split the buffer into pieces. */
    if (*buf == ':') {
        s = strpbrk(buf, " ");
        if (!s)
            return;
        *s = 0;
        while (isspace(*++s));
        strscpy(source, buf + 1, sizeof(source));
        memmove(buf, s, strlen(s) + 1);
    } else {
        *source = 0;
    }
    if (!*buf)
        return;
    s = strpbrk(buf, " ");
    if (s) {
        *s = 0;
        while (isspace(*++s));
    } else
        s = buf + strlen(buf);
    strscpy(cmd, buf, sizeof(cmd));
    ac = split_buf(s, &av, 1);
    if (protocoldebug) {
        protocol_debug(source, cmd, ac, av);
    }
    if (mod_current_buffer) {
        free(mod_current_buffer);
    }
    /* fix to moduleGetLastBuffer() bug 296 */
    /* old logic was that since its meant for PRIVMSG that we would get
       the NICK as AV[0] and the rest would be in av[1], however on Bahamut
       based systems when you do /cs it assumes we will translate the command
       to the NICK and thus AV[0] is the message. The new logic is to check
       av[0] to see if its a service nick if so assign mod_current_buffer the
       value from AV[1] else just assign av[0] - TSL */
    /* First check if the ircd proto module overrides this -GD */
    /* fix to moduleGetLastBuffer() bug 476:
       fixed in part by adding {} to nickIsServices()
       however if you have a pseudo they could not use moduleGetLastBuffer()
       cause they are not part of nickIsServices, even those the ac count is 2
       that was ignored and only the first param was passed on which is fine for
       Bahmut ircd aliases but not for pseudo clients on. So additional logic is
       that if the ac is greater then 1 copy av[1] else copy av[0] 
       I also changed from if statments, cause attempting to access a array member
       that is not set can lead to odd things - TSL (3/12/06) */
    if (!anope_set_mod_current_buffer(ac, av)) {
        if (ac >= 1) {
            if (nickIsServices(av[0], 1)) {
                mod_current_buffer =
                    (ac > 1 ? sstrdup(av[1]) : sstrdup(av[0]));
            } else {
                mod_current_buffer =
                    (ac > 1 ? sstrdup(av[1]) : sstrdup(av[0]));
            }
        } else {
            mod_current_buffer = NULL;
        }
    }
    /* Do something with the message. */
    m = find_message(cmd);
    if (m) {
        if (m->func) {
            mod_current_module_name = m->mod_name;
            retVal = m->func(source, ac, av);
            mod_current_module_name = NULL;
            if (retVal == MOD_CONT) {
                current = m->next;
                while (current && current->func && retVal == MOD_CONT) {
                    mod_current_module_name = current->mod_name;
                    retVal = current->func(source, ac, av);
                    mod_current_module_name = NULL;
                    current = current->next;
                }
            }
        }
    } else {
        if (debug)
            alog("debug: unknown message from server (%s)", inbuf);
    }

    /* Load/unload modules if needed */
    handleModuleOperationQueue();

    /* Free argument list we created */
    free(av);
}

/*************************************************************************/
