/* MemoServ core functions
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

int do_cancel(User * u);
void myMemoServHelp(User * u);

class MSCancel : public Module
{
 public:
	MSCancel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("CANCEL", do_cancel, NULL, MEMO_HELP_CANCEL, -1, -1, -1, -1);
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
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_CANCEL);
}

/**
 * The /ms cancel command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_cancel(User * u)
{
	int ischan;
	int isforbid;
	char *name = strtok(NULL, " ");
	MemoInfo *mi;

	if (!name) {
		syntax_error(s_MemoServ, u, "CANCEL", MEMO_CANCEL_SYNTAX);

	} else if (!nick_recognized(u)) {
		notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

	} else if (!(mi = getmemoinfo(name, &ischan, &isforbid))) {
		if (isforbid) {
			notice_lang(s_MemoServ, u,
						ischan ? CHAN_X_FORBIDDEN :
						NICK_X_FORBIDDEN, name);
		} else {
			notice_lang(s_MemoServ, u,
						ischan ? CHAN_X_NOT_REGISTERED :
						NICK_X_NOT_REGISTERED, name);
		}
	} else {
		int i;

		for (i = mi->memos.size() - 1; i >= 0; i--) {
			if ((mi->memos[i]->flags & MF_UNREAD)
				&& !stricmp(mi->memos[i]->sender, u->na->nc->display)
				&& (!(mi->memos[i]->flags & MF_NOTIFYS))) {
				delmemo(mi, mi->memos[i]->number);
				notice_lang(s_MemoServ, u, MEMO_CANCELLED, name);
				return MOD_CONT;
			}
		}

		notice_lang(s_MemoServ, u, MEMO_CANCEL_NONE);
	}
	return MOD_CONT;
}

MODULE_INIT("ms_cancel", MSCancel)
