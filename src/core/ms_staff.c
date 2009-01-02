/* MemoServ core functions
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

int do_staff(User * u);
void myMemoServHelp(User * u);

class MSStaff : public Module
{
 public:
	MSStaff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("STAFF", do_staff, is_services_oper, -1, -1, MEMO_HELP_STAFF, MEMO_HELP_STAFF, MEMO_HELP_STAFF);
		this->AddCommand(MEMOSERV, c, MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};



/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User * u)
{
	if (is_services_oper(u)) {
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_STAFF);
	}
}

/**
 * The /ms staff command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_staff(User * u)
{
	NickCore *nc;
	int i, z = 0;
	char *text = strtok(NULL, "");

	if (readonly) {
		notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);
		return MOD_CONT;
	} else if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
		notice_lang(s_MemoServ, u, OPER_DEFCON_DENIED);
		return MOD_CONT;
	} else if (text == NULL) {
		syntax_error(s_MemoServ, u, "SEND", MEMO_SEND_SYNTAX);
		return MOD_CONT;
	}

	for (i = 0; i < 1024; i++) {
		for (nc = nclists[i]; nc; nc = nc->next) {
			if (nick_is_services_oper(nc))
				memo_send(u, nc->display, text, z);
		}
	}
	return MOD_CONT;
}

MODULE_INIT("ms_staff", MSStaff)
