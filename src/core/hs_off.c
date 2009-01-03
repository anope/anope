/* HostServ core functions
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

int do_off(User * u);
void myHostServHelp(User * u);

class HSOff : public Module
{
 public:
	HSOff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("OFF", do_off, NULL, HOST_HELP_OFF, -1, -1, -1, -1);
		this->AddCommand(HOSTSERV, c, MOD_UNIQUE);

		this->SetHostHelp(myHostServHelp);
	}
};



/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User * u)
{
	notice_lang(s_HostServ, u, HOST_HELP_CMD_OFF);
}

/**
 * The /hs off command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_off(User * u)
{
	NickAlias *na;
	char *vhost;
	char *vident = NULL;
	if ((na = findnick(u->nick))) {
		if (na->status & NS_IDENTIFIED) {
			vhost = getvHost(u->nick);
			vident = getvIdent(u->nick);
			if (vhost == NULL && vident == NULL)
				notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
			else
				ircdproto->SendVhostDel(u);
		} else {
			notice_lang(s_HostServ, u, HOST_ID);
		}
	} else {
		notice_lang(s_HostServ, u, HOST_NOT_REGED);
	}
	return MOD_CONT;
}

MODULE_INIT("hs_off", HSOff)
