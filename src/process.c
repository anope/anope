/* Main processing code for Services.
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

#include "services.h"
#include "messages.h"
#include "modules.h"

/*************************************************************************/

/* Use ignore code? */
int allow_ignore = 1;

/* Masks to ignore. */
IgnoreData *ignore;

/*************************************************************************/

/**
 * Add a mask/nick to the ignorelits for delta seconds.
 * @param nick Nick or (nick!)user@host to add to the ignorelist.
 * @param delta Seconds untill new entry is set to expire. 0 for permanent.
 */
void add_ignore(const char *nick, time_t delta)
{
    IgnoreData *ign;
    char tmp[BUFSIZE];
    char *mask, *user, *host;
    User *u;
    time_t now;

    if (!nick)
        return;
    now = time(NULL);

    /* If it s an existing user, we ignore the hostmask. */ 
    if ((u = finduser(nick))) {
        snprintf(tmp, sizeof(tmp), "*!*@%s", u->host);
        mask = sstrdup(tmp);

    /* Determine whether we get a nick or a mask. */ 
    } else if ((host = strchr(nick, '@'))) {
        /* Check whether we have a nick too.. */ 
        if ((user = strchr(nick, '!'))) {
            /* this should never happen */ 
            if (user > host)
                return;
            mask = sstrdup(nick);
        } else {
            /* We have user@host. Add nick wildcard. */
            snprintf(tmp, sizeof(tmp), "*!%s", nick);
            mask = sstrdup(tmp);
        }
    /* We only got a nick.. */
    } else {
        snprintf(tmp, sizeof(tmp), "%s!*@*", nick);
        mask = sstrdup(tmp);
    }

    /* Check if we already got an identical entry. */
    for (ign = ignore; ign; ign = ign->next)
        if (stricmp(ign->mask, mask) == 0)
            break;

    /* Found one.. */
    if (ign) {
        if (delta == 0)
            ign->time = 0;
        else if (ign->time < now + delta)
            ign->time = now + delta;

    /* Create new entry.. */ 
    } else {
        ign = scalloc(sizeof(*ign), 1);
        ign->mask = mask;
        ign->time = (delta == 0 ? 0 : now + delta);
        ign->prev = NULL;
        ign->next = ignore;
        if (ignore)
            ignore->prev = ign;
        ignore = ign;
        if (debug)
            alog("debug: Added new ignore entry for %s", mask);
    }
}

/*************************************************************************/

/**
 * Retrieve an ignorance record for a nick or mask.
 * If the nick isn't being ignored, we return NULL and if necesary 
 * flush the record from the ignore list (i.e. ignore timed out).
 * @param nick Nick or (nick!)user@host to look for on the ignorelist.
 * @return Pointer to the ignore record, NULL if none was found.
 */
IgnoreData *get_ignore(const char *nick)
{
    IgnoreData *ign;
    char tmp[BUFSIZE];
    char *user, *host;
    time_t now;
    User *u;

    if (!nick)
        return NULL;

    /* User has disabled the IGNORE system */
    if (!allow_ignore)
        return NULL;

    now = time(NULL);
    u = finduser(nick);

    /* If we find a real user, match his mask against the ignorelist. */
    if (u) {
        /* Opers are not ignored, even if a matching entry may be present. */ 
        if (is_oper(u))
            return NULL;
        for (ign = ignore; ign; ign = ign->next)
            if (match_usermask(ign->mask, u))
                break;
    } else {
        /* We didn't get a user.. generate a valid mask. */ 
        if ((host = strchr(nick, '@'))) {
            if ((user = strchr(nick, '!'))) {
                /* this should never happen */
                if (user > host)
                    return NULL;
                snprintf(tmp, sizeof(tmp), "%s", nick);
            } else {
                /* We have user@host. Add nick wildcard. */ 
                snprintf(tmp, sizeof(tmp), "*!%s", nick);
            }

        /* We only got a nick.. */ 
        } else
            snprintf(tmp, sizeof(tmp), "%s!*@*", nick);
        for (ign = ignore; ign; ign = ign->next)
            if (match_wild_nocase(ign->mask, tmp))
                break;
    }

    /* Check whether the entry has timed out */ 
    if (ign && ign->time != 0 && ign->time <= now) {
        if (debug)
            alog("debug: Expiring ignore entry %s", ign->mask);
        if (ign->prev)
            ign->prev->next = ign->next;
        else if (ignore == ign)
            ignore = ign->next;
        if (ign->next)
            ign->next->prev = ign->prev;
        free(ign->mask);
        free(ign);
        ign = NULL;
    }
    if (ign && debug)
        alog("debug: Found ignore entry (%s) for %s", ign->mask, nick);
    return ign;
}

/*************************************************************************/ 

/**
 * Deletes a given nick/mask from the ignorelist.
 * @param nick Nick or (nick!)user@host to delete from the ignorelist.
 * @return Returns 1 on success, 0 if no entry is found.
 */ 
int delete_ignore(const char *nick) 
{
    IgnoreData *ign;
    char tmp[BUFSIZE];
    char *user, *host;
    User *u;

    if (!nick)
        return 0;

    /* If it s an existing user, we ignore the hostmask. */ 
    if ((u = finduser(nick))) {
        snprintf(tmp, sizeof(tmp), "*!*@%s", u->host);

    /* Determine whether we get a nick or a mask. */ 
    } else if ((host = strchr(nick, '@'))) {
        /* Check whether we have a nick too.. */ 
        if ((user = strchr(nick, '!'))) {
            /* this should never happen */ 
            if (user > host)
                return 0;
            snprintf(tmp, sizeof(tmp), "%s", nick);
        } else {
            /* We have user@host. Add nick wildcard. */ 
            snprintf(tmp, sizeof(tmp), "*!%s", nick);
        }

    /* We only got a nick.. */
    } else
       snprintf(tmp, sizeof(tmp), "%s!*@*", nick);

    for (ign = ignore; ign; ign = ign->next)
        if (stricmp(ign->mask, tmp) == 0)
            break;

    /* No matching ignore found. */
    if (!ign)
        return 0;

    if (debug)
        alog("Deleting ignore entry %s", ign->mask);

    /* Delete the entry and all references to it. */ 
    if (ign->prev)
        ign->prev->next = ign->next;
    else if (ignore == ign)
        ignore = ign->next;
    if (ign->next)
        ign->next->prev = ign->prev;
    free(ign->mask);
    free(ign);
    ign = NULL;

    return 1;
}

/*************************************************************************/ 

/**
 * Clear the ignorelist.
 * @return The number of entries deleted.
 */ 
int clear_ignores() 
{
    IgnoreData *ign, *next;
    int i = 0;

    if (!ignore)
        return 0;

    for (ign = ignore; ign; ign = next) {
        next = ign->next;
        if (debug)
            alog("Deleting ignore entry %s", ign->mask);
        free(ign->mask);
        free(ign);
        i++;
    }
    ignore = NULL;
    return i;
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
    char buf[4096];             /* Most need only 512, InspIRCd 2.0 is seriously oversized though.. */
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
            mod_current_module = findModule(m->mod_name);
            retVal = m->func(source, ac, av);
            mod_current_module_name = NULL;
            mod_current_module = NULL;
            if (retVal == MOD_CONT) {
                current = m->next;
                while (current && current->func && retVal == MOD_CONT) {
                    mod_current_module_name = current->mod_name;
                    mod_current_module = findModule(current->mod_name);
                    retVal = current->func(source, ac, av);
                    mod_current_module_name = NULL;
                    mod_current_module = NULL;
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
