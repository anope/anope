/* ChanServ core functions
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

int do_list(User * u);
void myChanServHelp(User * u);

class CSList : public Module
{
 public:
	CSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("LIST", do_list, NULL, -1, CHAN_HELP_LIST, CHAN_SERVADMIN_HELP_LIST, CHAN_SERVADMIN_HELP_LIST, CHAN_SERVADMIN_HELP_LIST);

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
	if (!CSListOpersOnly || (is_oper(u))) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_LIST);
	}
}

/**
 * The /cs list command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_list(User * u)
{
	char *pattern = strtok(NULL, " ");
	int spattern_size;
	char *spattern;
	ChannelInfo *ci;
	unsigned nchans, i;
	char buf[BUFSIZE];
	int is_servadmin = is_services_admin(u);
	int count = 0, from = 0, to = 0, tofree = 0;
	char *tmp = NULL;
	char *s = NULL;
	char *keyword;
	int32 matchflags = 0;

	if (!(!CSListOpersOnly || (is_oper(u)))) {
		notice_lang(s_ChanServ, u, ACCESS_DENIED);
		return MOD_STOP;
	}

	if (!pattern) {
		syntax_error(s_ChanServ, u, "LIST",
					 is_servadmin ? CHAN_LIST_SERVADMIN_SYNTAX :
					 CHAN_LIST_SYNTAX);
	} else {

		if (pattern) {
			if (pattern[0] == '#') {
				tmp = myStrGetOnlyToken((pattern + 1), '-', 0); /* Read FROM out */
				if (!tmp) {
					notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
					notice_lang(s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
				for (s = tmp; *s; s++) {
					if (!isdigit(*s)) {
						delete [] tmp;
					   	notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
					   	notice_lang(s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
						return MOD_CONT;
					}
				}
				from = atoi(tmp);
				delete [] tmp;
				tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
				if (!tmp) {
					notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
					notice_lang(s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
				for (s = tmp; *s; s++) {
					if (!isdigit(*s)) {
						delete [] tmp;
						notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
						notice_lang(s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
						return MOD_CONT;
					}
				}
				to = atoi(tmp);
				delete [] tmp;
				pattern = sstrdup("*");
				tofree = 1;
			}
		}

		nchans = 0;

		while (is_servadmin && (keyword = strtok(NULL, " "))) {
			if (stricmp(keyword, "FORBIDDEN") == 0)
				matchflags |= CI_VERBOTEN;
			if (stricmp(keyword, "SUSPENDED") == 0)
				matchflags |= CI_SUSPENDED;
			if (stricmp(keyword, "NOEXPIRE") == 0)
				matchflags |= CI_NO_EXPIRE;
		}

		spattern_size = (strlen(pattern) + 2) * sizeof(char);
		spattern = new char[spattern_size];
		snprintf(spattern, spattern_size, "#%s", pattern);


		notice_lang(s_ChanServ, u, CHAN_LIST_HEADER, pattern);
		for (i = 0; i < 256; i++) {
			for (ci = chanlists[i]; ci; ci = ci->next) {
				if (!is_servadmin && ((ci->flags & CI_PRIVATE)
									  || (ci->flags & CI_VERBOTEN)))
					continue;
				if ((matchflags != 0) && !(ci->flags & matchflags))
					continue;

				if ((stricmp(pattern, ci->name) == 0)
					|| (stricmp(spattern, ci->name) == 0)
					|| match_wild_nocase(pattern, ci->name)
					|| match_wild_nocase(spattern, ci->name)) {
					if ((((count + 1 >= from) && (count + 1 <= to))
						 || ((from == 0) && (to == 0)))
						&& (++nchans <= CSListMax)) {
						char noexpire_char = ' ';
						if (is_servadmin && (ci->flags & CI_NO_EXPIRE))
							noexpire_char = '!';

						if (ci->flags & CI_VERBOTEN) {
							snprintf(buf, sizeof(buf),
									 "%-20s  [Forbidden]", ci->name);
						} else if (ci->flags & CI_SUSPENDED) {
							snprintf(buf, sizeof(buf),
									 "%-20s  [Suspended]", ci->name);
						} else {
							snprintf(buf, sizeof(buf), "%-20s  %s",
									 ci->name, ci->desc ? ci->desc : "");
						}

						notice_user(s_ChanServ, u, "  %c%s",
									noexpire_char, buf);
					}
					count++;
				}
			}
		}
		notice_lang(s_ChanServ, u, CHAN_LIST_END,
					nchans > CSListMax ? CSListMax : nchans, nchans);
		delete [] spattern;
	}
	if (tofree)
		delete [] pattern;
	return MOD_CONT;

}

MODULE_INIT("cs_list", CSList)
