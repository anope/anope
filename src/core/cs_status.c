/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
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

int do_status(User * u);
void myChanServHelp(User * u);

class CSStatus : public Module
{
 public:
	CSStatus(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("STATUS", do_status, is_services_admin, -1, -1, -1, CHAN_SERVADMIN_HELP_STATUS, CHAN_SERVADMIN_HELP_STATUS);
		this->AddCommand(CHANSERV, c, MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	if (is_services_admin(u)) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_STATUS);
	}
}

/**
 * The /cs status command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_status(User * u)
{
	ChannelInfo *ci;
	User *u2;
	char *nick, *chan;
	char *temp = NULL;

	chan = strtok(NULL, " ");
	nick = strtok(NULL, " ");
	if (!nick || strtok(NULL, " ")) {
		notice_lang(s_ChanServ, u, CHAN_STATUS_SYNTAX);
		return MOD_CONT;
	}
	if (!(ci = cs_findchan(chan))) {
		temp = chan;
		chan = nick;
		nick = temp;
		ci = cs_findchan(chan);
	}
	if (!ci) {
		notice_lang(s_ChanServ, u, CHAN_STATUS_NOT_REGGED, temp);
	} else if (ci->flags & CI_VERBOTEN) {
		notice_lang(s_ChanServ, u, CHAN_STATUS_FORBIDDEN, chan);
		return MOD_CONT;
	} else if ((u2 = finduser(nick)) != NULL) {
		notice_lang(s_ChanServ, u, CHAN_STATUS_INFO, chan, nick,
					get_access(u2, ci));
	} else {					/* !u2 */
		notice_lang(s_ChanServ, u, CHAN_STATUS_NOTONLINE, nick);
	}
	return MOD_CONT;
}

MODULE_INIT("cs_status", CSStatus)
