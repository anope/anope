/* OperServ core functions
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

int do_chankill(User * u);
void myOperServHelp(User * u);

class OSChanKill : public Module
{
 public:
	OSChanKill(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("CHANKILL", do_chankill, is_services_admin, OPER_HELP_CHANKILL, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		moduleSetOperHelp(myOperServHelp);
	}
};



/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_admin(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_CHANKILL);
	}
}

/**
 * ChanKill - Akill an entire channel (got botnet?)
 *
 * /msg OperServ ChanKill +expire #channel reason
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 *
 **/
int do_chankill(User * u)
{
	char *expiry, *channel, *reason;
	time_t expires;
	char breason[BUFSIZE];
	char mask[USERMAX + HOSTMAX + 2];
	struct c_userlist *cu, *next;
	Channel *c;

	channel = strtok(NULL, " ");
	if (channel && *channel == '+') {
		expiry = channel;
		channel = strtok(NULL, " ");
	} else {
		expiry = NULL;
	}

	expires = expiry ? dotime(expiry) : ChankillExpiry;
	if (expiry && isdigit(expiry[strlen(expiry) - 1]))
		expires *= 86400;
	if (expires != 0 && expires < 60) {
		notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
		return MOD_CONT;
	} else if (expires > 0) {
		expires += time(NULL);
	}

	if (channel && (reason = strtok(NULL, ""))) {

		if (AddAkiller) {
			snprintf(breason, sizeof(breason), "[%s] %s", u->nick, reason);
			reason = sstrdup(breason);
		}

		if ((c = findchan(channel))) {
			for (cu = c->users; cu; cu = next) {
				next = cu->next;
				if (is_oper(cu->user)) {
					continue;
				}
				(void) strncpy(mask, "*@", 3); /* Use *@" for the akill's, */
				strncat(mask, cu->user->host, HOSTMAX);
				add_akill(NULL, mask, s_OperServ, expires, reason);
				check_akill(cu->user->nick, cu->user->username,
							cu->user->host, NULL, NULL);
			}
			if (WallOSAkill) {
				ircdproto->SendGlobops(s_OperServ, "%s used CHANKILL on %s (%s)",
								 u->nick, channel, reason);
			}
		} else {
			notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, channel);
		}
		if (AddAkiller) {
			delete [] reason;
		}
	} else {
		syntax_error(s_OperServ, u, "CHANKILL", OPER_CHANKILL_SYNTAX);
	}
	return MOD_CONT;
}

MODULE_INIT("os_chankill", OSChanKill)
