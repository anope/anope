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

int do_staff(User * u);
void myOperServHelp(User * u);
int opers_list_callback(SList * slist, int number, void *item,
						va_list args);
int opers_list(int number, NickCore * nc, User * u, char *level);

class OSStaff : public Module
{
 public:
	OSStaff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("STAFF", do_staff, NULL, OPER_HELP_STAFF, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	notice_lang(s_OperServ, u, OPER_HELP_CMD_STAFF);
}

/**
 * The /os staff command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_staff(User * u)
{
	int idx = 0;
	User *au = NULL;
	NickCore *nc;
	NickAlias *na;
	int found;
	int i;

	notice_lang(s_OperServ, u, OPER_STAFF_LIST_HEADER);
	slist_enum(&servopers, NULL, &opers_list_callback, u, "OPER");
	slist_enum(&servadmins, NULL, &opers_list_callback, u, "ADMN");

	for (idx = 0; idx < RootNumber; idx++) {
		found = 0;
		if ((au = finduser(ServicesRoots[idx]))) {	  /* see if user is online */
			found = 1;
			notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, '*', "ROOT",
						ServicesRoots[idx]);
		} else if ((nc = findcore(ServicesRoots[idx]))) {
			for (i = 0; i < nc->aliases.count; i++) {   /* check all aliases */
				na = static_cast<NickAlias *>(nc->aliases.list[i]);
				if ((au = finduser(na->nick))) {		/* see if user is online */
					found = 1;
					notice_lang(s_OperServ, u, OPER_STAFF_AFORMAT,
								'*', "ROOT", ServicesRoots[idx], na->nick);
				}
			}
		}

		if (!found)
			notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, ' ', "ROOT",
						ServicesRoots[idx]);

	}
	notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Staff");
	return MOD_CONT;
}

/**
 * Function for the enumerator to call
 **/
int opers_list_callback(SList * slist, int number, void *item,
						va_list args)
{
	User *u = va_arg(args, User *);
	char *level = va_arg(args, char *);

	return opers_list(number, static_cast<NickCore *>(item), u, level);
}


/**
 * Display an Opers list Entry
 **/
int opers_list(int number, NickCore * nc, User * u, char *level)
{
	User *au = NULL;
	NickAlias *na;
	int found;
	int i;

	if (!nc)
		return 0;

	found = 0;
	if ((au = finduser(nc->display))) { /* see if user is online */
		found = 1;
		notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, '*', level,
					nc->display);
	} else {
		for (i = 0; i < nc->aliases.count; i++) {	   /* check all aliases */
			na = static_cast<NickAlias *>(nc->aliases.list[i]);
			if ((au = finduser(na->nick))) {	/* see if user is online */
				found = 1;
				notice_lang(s_OperServ, u, OPER_STAFF_AFORMAT, '*', level,
							nc->display, na->nick);
			}
		}
	}

	if (!found)
		notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, ' ', level,
					nc->display);

	return 1;
}

MODULE_INIT("os_staff", OSStaff)
