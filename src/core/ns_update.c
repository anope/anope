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

int do_nickupdate(User * u);
void myNickServHelp(User * u);

class NSUpdate : public Module
{
 public:
	NSUpdate(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("UPDATE", do_nickupdate, NULL, NICK_HELP_UPDATE, -1, -1, -1, -1);
		this->AddCommand(NICKSERV, c, MOD_UNIQUE);

		moduleSetNickHelp(myNickServHelp);
	}
};



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_UPDATE);
}

/**
 * The /ns update command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_nickupdate(User * u)
{
    NickAlias *na;

    if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else {
        na = u->na;
        if (NSModeOnID)
            do_setmodes(u);
        check_memos(u);
        if (na->last_realname)
            free(na->last_realname);
        na->last_realname = sstrdup(u->realname);
        na->status |= NS_IDENTIFIED;
        na->last_seen = time(NULL);
        if (ircd->vhost) {
            do_on_id(u);
        }
        notice_lang(s_NickServ, u, NICK_UPDATE_SUCCESS, s_NickServ);
    }
    return MOD_CONT;
}

MODULE_INIT("ns_update", NSUpdate)
