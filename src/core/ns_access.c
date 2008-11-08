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

int do_access(User * u);
void myNickServHelp(User * u);

class NSAccess : public Module
{
 public:
	NSAccess(const std::string &modname, const std::string &creator) : Module(creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(CORE);

		c = createCommand("ACCESS", do_access, NULL, NICK_HELP_ACCESS, -1, -1, -1, -1);
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
    notice_lang(s_NickServ, u, NICK_HELP_CMD_ACCESS);
}

/**
 * The /ns access command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_access(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    NickAlias *na;
    int i;
    char **access;

    if (cmd && stricmp(cmd, "LIST") == 0 && mask && is_services_admin(u)
        && (na = findnick(mask))) {

        if (na->nc->accesscount == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X_EMPTY, na->nick);
            return MOD_CONT;
        }

        if (na->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
            return MOD_CONT;
        }

        if (na->nc->flags & NI_SUSPENDED) {
            notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
            return MOD_CONT;
        }


        notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X, mask);
        mask = strtok(NULL, " ");
        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (mask && !match_wild(mask, *access))
                continue;
            notice_user(s_NickServ, u, "    %s", *access);
        }

    } else if (!cmd || ((stricmp(cmd, "LIST") == 0) ? !!mask : !mask)) {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    } else if (mask && !strchr(mask, '@')) {
        notice_lang(s_NickServ, u, BAD_USERHOST_MASK);
        notice_lang(s_NickServ, u, MORE_INFO, s_NickServ, "ACCESS");

    } else if (!(na = u->na)) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);

    } else if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);

    } else if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (na->nc->accesscount >= NSAccessMax) {
            notice_lang(s_NickServ, u, NICK_ACCESS_REACHED_LIMIT,
                        NSAccessMax);
            return MOD_CONT;
        }

        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (strcmp(*access, mask) == 0) {
                notice_lang(s_NickServ, u, NICK_ACCESS_ALREADY_PRESENT,
                            *access);
                return MOD_CONT;
            }
        }

        na->nc->accesscount++;
        na->nc->access =
            (char **)srealloc(na->nc->access, sizeof(char *) * na->nc->accesscount);
        na->nc->access[na->nc->accesscount - 1] = sstrdup(mask);
        notice_lang(s_NickServ, u, NICK_ACCESS_ADDED, mask);

    } else if (stricmp(cmd, "DEL") == 0) {

        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (stricmp(*access, mask) == 0)
                break;
        }
        if (i == na->nc->accesscount) {
            notice_lang(s_NickServ, u, NICK_ACCESS_NOT_FOUND, mask);
            return MOD_CONT;
        }

        notice_lang(s_NickServ, u, NICK_ACCESS_DELETED, *access);
        free(*access);
        na->nc->accesscount--;
        if (i < na->nc->accesscount)    /* if it wasn't the last entry... */
            memmove(access, access + 1,
                    (na->nc->accesscount - i) * sizeof(char *));
        if (na->nc->accesscount)        /* if there are any entries left... */
            na->nc->access =
                (char **)srealloc(na->nc->access,
                         na->nc->accesscount * sizeof(char *));
        else {
            free(na->nc->access);
            na->nc->access = NULL;
        }
    } else if (stricmp(cmd, "LIST") == 0) {
        if (na->nc->accesscount == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_EMPTY, u->nick);
            return MOD_CONT;
        }

        notice_lang(s_NickServ, u, NICK_ACCESS_LIST);
        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (mask && !match_wild(mask, *access))
                continue;
            notice_user(s_NickServ, u, "    %s", *access);
        }
    } else {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    }
    return MOD_CONT;
}

MODULE_INIT("ns_access", NSAccess)
