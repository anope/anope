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
int do_list(User * u);
int list_memo_callback(User * u, int num, va_list args);
int list_memo(User * u, int index, MemoInfo * mi, int *sent_header, int newi, const char *chan);
void myMemoServHelp(User * u);

class MSList : public Module
{
 public:
	MSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("LIST", do_list, NULL, MEMO_HELP_LIST, -1, -1, -1, -1);
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
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_LIST);
}

/**
 * List the memos (if any) for the source nick or given channel.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_list(User * u)
{
	char *param = strtok(NULL, " "), *chan = NULL;
	ChannelInfo *ci;
	MemoInfo *mi;
	Memo *m;
	int i;

	if (param && *param == '#') {
		chan = param;
		param = strtok(NULL, " ");
		if (!(ci = cs_findchan(chan))) {
			notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		} else if (ci->flags & CI_VERBOTEN) {
			notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		} else if (!check_access(u, ci, CA_MEMO)) {
			notice_lang(s_MemoServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		mi = &ci->memos;
	} else {
		if (!nick_identified(u)) {
			notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
			return MOD_CONT;
		}
		mi = &u->na->nc->memos;
	}
	if (param && !isdigit(*param) && stricmp(param, "NEW") != 0) {
		syntax_error(s_MemoServ, u, "LIST", MEMO_LIST_SYNTAX);
	} else if (mi->memos.size() == 0) {
		if (chan)
			notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan);
		else
			notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
	} else {
		int sent_header = 0;
		if (param && isdigit(*param)) {
			process_numlist(param, NULL, list_memo_callback, u,
							mi, &sent_header, chan);
		} else {
			if (param) {
				for (i = 0; i < mi->memos.size(); i++) {
					if (mi->memos[i]->flags & MF_UNREAD)
						break;
				}
				if (i == mi->memos.size()) {
					if (chan)
						notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS,
									chan);
					else
						notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
					return MOD_CONT;
				}
			}
			for (i = 0; i < mi->memos.size(); i++) {
				if (param && !(mi->memos[i]->flags & MF_UNREAD))
					continue;
				list_memo(u, i, mi, &sent_header, param != NULL, chan);
			}
		}
	}
	return MOD_CONT;
}

/**
 * list memno callback function
 * @param u User Struct
 * @param int Memo number
 * @param va_list List of arguements
 * @return result form list_memo()
 */
int list_memo_callback(User * u, int num, va_list args)
{
	MemoInfo *mi = va_arg(args, MemoInfo *);
	int *sent_header = va_arg(args, int *);
	const char *chan = va_arg(args, const char *);
	int i;

	for (i = 0; i < mi->memos.size(); i++) {
		if (mi->memos[i]->number == num)
			break;
	}
	/* Range checking done by list_memo() */
	return list_memo(u, i, mi, sent_header, 0, chan);
}

/**
 * Display a single memo entry, possibly printing the header first.
 * @param u User Struct
 * @param int Memo index
 * @param mi MemoInfo Struct
 * @param send_header If we are to send the headers
 * @param newi If we are listing new memos
 * @param chan Channel name
 * @return MOD_CONT
 */
int list_memo(User * u, int index, MemoInfo * mi, int *sent_header, int newi, const char *chan)
{
	Memo *m;
	char timebuf[64];
	struct tm tm;

	if (index < 0 || index >= mi->memos.size())
		return 0;
	if (!*sent_header) {
		if (chan) {
			notice_lang(s_MemoServ, u,
						newi ? MEMO_LIST_CHAN_NEW_MEMOS :
						MEMO_LIST_CHAN_MEMOS, chan, s_MemoServ, chan);
		} else {
			notice_lang(s_MemoServ, u,
						newi ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS,
						u->nick, s_MemoServ);
		}
		notice_lang(s_MemoServ, u, MEMO_LIST_HEADER);
		*sent_header = 1;
	}
	m = mi->memos[index];
	tm = *localtime(&m->time);
	strftime_lang(timebuf, sizeof(timebuf),
				  u, STRFTIME_DATE_TIME_FORMAT, &tm);
	timebuf[sizeof(timebuf) - 1] = 0;   /* just in case */
	notice_lang(s_MemoServ, u, MEMO_LIST_FORMAT,
				(m->flags & MF_UNREAD) ? '*' : ' ',
				m->number, m->sender, timebuf);
	return 1;
}

MODULE_INIT("ms_list", MSList)
