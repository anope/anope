/* Various routines to perform simple actions.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"

/*************************************************************************/

/* Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.  
 */

void bad_password(User * u)
{
    time_t now = time(NULL);

    if (!BadPassLimit)
        return;

    if (BadPassTimeout > 0 && u->invalid_pw_time > 0
        && u->invalid_pw_time < now - BadPassTimeout)
        u->invalid_pw_count = 0;
    u->invalid_pw_count++;
    u->invalid_pw_time = now;
    if (u->invalid_pw_count >= BadPassLimit)
        kill_user(NULL, u->nick, "Too many invalid passwords");
}

/*************************************************************************/

void change_user_mode(User * u, char *modes, char *arg)
{
#ifndef IRC_HYBRID
    int ac = 1;
    char *av[2];

    av[0] = modes;
    if (arg) {
        av[1] = arg;
        ac++;
    }
#ifdef IRC_BAHAMUT
    send_cmd(ServerName, "SVSMODE %s %ld %s%s%s", u->nick, u->timestamp,
             av[0], (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
#else
    send_cmd(ServerName, "SVSMODE %s %s%s%s", u->nick, av[0],
             (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
#endif
    set_umode(u, ac, av);
#endif
}

/*************************************************************************/

/* Remove a user from the IRC network.  `source' is the nick which should
 * generate the kill, or NULL for a server-generated kill.
 */

void kill_user(const char *source, const char *user, const char *reason)
{
#ifdef IRC_BAHAMUT
    /* Bahamut uses SVSKILL as a better way to kill users. It sends back
     * a QUIT message that Anope uses to clean up after the kill is done.
     */
    send_cmd(NULL, "SVSKILL %s :%s", user, reason);
#else
    char *av[2];
    char buf[BUFSIZE];

    if (!user || !*user)
        return;
    if (!source || !*source)
        source = ServerName;
    if (!reason)
        reason = "";
    snprintf(buf, sizeof(buf), "%s (%s)", source, reason);
    av[0] = sstrdup(user);
    av[1] = buf;
    send_cmd(source, "KILL %s :%s", user, av[1]);
    do_kill(source, 2, av);
    free(av[0]);
#endif
}

/*************************************************************************/
