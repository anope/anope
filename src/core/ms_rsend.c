/* MemoServ core functions
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

int do_rsend(User * u);
void myMemoServHelp(User * u);

class MSRSend : public Module
{
 public:
	MSRSend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("RSEND", do_rsend, NULL, MEMO_HELP_RSEND, -1, -1, -1, -1);
		this->AddCommand(MEMOSERV, c, MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);

		if (!MSMemoReceipt)
			throw ModuleException("Don't like memo reciepts, or something.");
	}
};

/**
 * Unload the module
 **/
void AnopeFini()
{

}

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User * u)
{
	if (((MSMemoReceipt == 1) && (is_services_oper(u)))
		|| (MSMemoReceipt == 2)) {
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_RSEND);
	}
}

/**
 * The /ms rsend command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_rsend(User * u)
{
	char *name = strtok(NULL, " ");
	char *text = strtok(NULL, "");
	NickAlias *na = NULL;
	int z = 3;



	/* check if the variables are here */
	if (!name || !text) {
		notice_lang(s_MemoServ, u, MEMO_RSEND_SYNTAX);
		return MOD_CONT;
	}

	/* prevent user from rsend to themselves */
	if ((na = findnick(name))) {
		if (u->na) {
			if (stricmp(na->nc->display, u->na->nc->display) == 0) {
				notice_lang(s_MemoServ, u, MEMO_NO_RSEND_SELF);
				return MOD_CONT;
			}
		} else {
			notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, name);
			return MOD_CONT;
		}
	}

	if (MSMemoReceipt == 1) {
		/* Services opers and above can use rsend */
		if (is_services_oper(u)) {
			memo_send(u, name, text, z);
		} else {
			notice_lang(s_MemoServ, u, ACCESS_DENIED);
		}
	} else if (MSMemoReceipt == 2) {
		/* Everybody can use rsend */
		memo_send(u, name, text, z);
	} else {
		/* rsend has been disabled */
		if (debug) {
			alog("debug: MSMemoReceipt is set misconfigured to %d",
				 MSMemoReceipt);
		}
		notice_lang(s_MemoServ, u, MEMO_RSEND_DISABLED);
	}

	return MOD_CONT;
}

MODULE_INIT("ms_rsend", MSRSend)
