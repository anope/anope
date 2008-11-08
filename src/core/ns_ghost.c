/* NickServ core functions
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
/*************************************************************************/

#include "module.h"

int do_ghost(User * u);
void myNickServHelp(User * u);

class NSGhost : public Module
{
 public:
	NSGhost(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("GHOST", do_ghost, NULL, NICK_HELP_GHOST, -1, -1, -1, -1);
		moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

		moduleSetNickHelp(myNickServHelp);
	}
};



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_GHOST);
}

/**
 * The /ns ghost command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_ghost(User * u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickAlias *na;
    User *u2;

    if (!nick) {
        syntax_error(s_NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
    } else if (!(u2 = finduser(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(na = u2->na)) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
    } else if (stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_NO_GHOST_SELF);
    } else if (pass) {
        int res = enc_check_password(pass, na->nc->pass);
        if (res == 1) {
            char buf[NICKMAX + 32];
            snprintf(buf, sizeof(buf), "GHOST command used by %s",
                     u->nick);
            if (LimitSessions) {
                del_session(u2->host);
            }
            kill_user(s_NickServ, nick, buf);
            notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            if (res == 0) {
                alog("%s: GHOST: invalid password for %s by %s!%s@%s",
                     s_NickServ, nick, u->nick, u->username, u->host);
                bad_password(u);
            }
        }
    } else {
        if (group_identified(u, na->nc)
            || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc))) {
            char buf[NICKMAX + 32];
            snprintf(buf, sizeof(buf), "GHOST command used by %s",
                     u->nick);
            if (LimitSessions) {
                del_session(u2->host);
            }
            kill_user(s_NickServ, nick, buf);
            notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
        }
    }
    return MOD_CONT;
}

MODULE_INIT("ns_ghost", NSGhost)
