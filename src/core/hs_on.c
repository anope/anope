/* HostServ core functions
 *
 * (C) 2003-2009 Anope Team
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

int do_on(User * u);
void myHostServHelp(User * u);

class HSOn : public Module
{
 public:
	HSOn(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("ON", do_on, NULL, HOST_HELP_ON, -1, -1, -1, -1);
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
	notice_lang(s_HostServ, u, HOST_HELP_CMD_ON);
}

/**
 * The /hs on command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_on(User * u)
{
	NickAlias *na;
	char *vHost;
	char *vIdent = NULL;
	if ((na = findnick(u->nick))) {
		if (na->status & NS_IDENTIFIED) {
			vHost = getvHost(u->nick);
			vIdent = getvIdent(u->nick);
			if (vHost == NULL) {
				notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
			} else {
				if (vIdent) {
					notice_lang(s_HostServ, u, HOST_IDENT_ACTIVATED,
								vIdent, vHost);
				} else {
					notice_lang(s_HostServ, u, HOST_ACTIVATED, vHost);
				}
				ircdproto->SendVhost(u->nick, vIdent, vHost);
				if (ircd->vhost) {
					u->vhost = sstrdup(vHost);
				}
				if (ircd->vident) {
					if (vIdent)
						u->vident = sstrdup(vIdent);
				}
				set_lastmask(u);
			}
		} else {
			notice_lang(s_HostServ, u, HOST_ID);
		}
	} else {
		notice_lang(s_HostServ, u, HOST_NOT_REGED);
	}
	return MOD_CONT;
}

MODULE_INIT("hs_on", HSOn)
