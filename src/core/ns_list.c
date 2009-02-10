/* NickServ core functions
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

void myNickServHelp(User *u);

class CommandNSList : public Command
{
 public:
	CommandNSList() : Command("LIST", 1, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		/* SADMINS can search for nicks based on their NS_FORBIDDEN and NS_NO_EXPIRE
		 * status. The keywords FORBIDDEN and NOEXPIRE represent these two states
		 * respectively. These keywords should be included after the search pattern.
		 * Multiple keywords are accepted and should be separated by spaces. Only one
		 * of the keywords needs to match a nick's state for the nick to be displayed.
		 * Forbidden nicks can be identified by "[Forbidden]" appearing in the last
		 * seen address field. Nicks with NOEXPIRE set are preceeded by a "!". Only
		 * SADMINS will be shown forbidden nicks and the "!" indicator.
		 * Syntax for sadmins: LIST pattern [FORBIDDEN] [NOEXPIRE]
		 * -TheShadow
		 *
		 * UPDATE: SUSPENDED keyword is now accepted as well.
		 */
		char *pattern = params[0].c_str();
		NickAlias *na;
		NickCore *mync;
		unsigned nnicks, i;
		char buf[BUFSIZE];
		int is_servadmin = is_services_admin(u);
		int16 matchflags = 0;
		NickRequest *nr = NULL;
		int nronly = 0;
		int susp_keyword = 0;
		char noexpire_char = ' ';
		int count = 0, from = 0, to = 0, tofree = 0;
		char *tmp = NULL;
		char *s = NULL;

		if (NSListOpersOnly && !is_oper(u)) /* reverse the help logic */
		{
			notice_lang(s_NickServ, u, ACCESS_DENIED);
			return MOD_STOP;
		}

		if (pattern[0] == '#')
		{
			tmp = myStrGetOnlyToken((pattern + 1), '-', 0); /* Read FROM out */
			if (!tmp)
			{
				notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (s = tmp; *s; ++s)
			{
				if (!isdigit(*s))
				{
					delete [] tmp;
					notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
			}
			from = atoi(tmp);
			delete [] tmp;
			tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
			if (!tmp)
			{
				notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (s = tmp; *s; ++s)
			{
				if (!isdigit(*s))
				{
					delete [] tmp;
					notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
			}
			to = atoi(tmp);
			delete [] tmp;
			pattern = sstrdup("*");
			tofree = 1;
		}

		nnicks = 0;

		if (is_servadmin && params.size() > 1)
		{
			std::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetString(keyword))
			{
				if (!stricmp(keyword.c_str(), "FORBIDDEN"))
					matchflags |= NS_FORBIDDEN;
				if (!stricmp(keyword.c_str(), "NOEXPIRE"))
					matchflags |= NS_NO_EXPIRE;
				if (!stricmp(keyword.c_str(), "SUSPENDED"))
					susp_keyword = 1;
				if (!stricmp(keyword.c_str(), "UNCONFIRMED"))
					nronly = 1;
			}
		}

		mync = nick_identified(u) ? u->na->nc : NULL;

		notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
		if (!nronly)
		{
			for (i = 0; i < 1024; ++i)
			{
				for (na = nalists[i]; na; na = na->next)
				{
					/* Don't show private and forbidden nicks to non-services admins. */
					if ((na->status & NS_FORBIDDEN) && !is_servadmin)
						continue;
					if ((na->nc->flags & NI_PRIVATE) && !is_servadmin && na->nc != mync)
						continue;
					if (matchflags && !(na->status & matchflags) && !susp_keyword)
						continue;
					else if (susp_keyword && !(na->nc->flags & NI_SUSPENDED))
						continue;

					/* We no longer compare the pattern against the output buffer.
					 * Instead we build a nice nick!user@host buffer to compare.
					 * The output is then generated separately. -TheShadow */
					snprintf(buf, sizeof(buf), "%s!%s", na->nick, na->last_usermask && !(na->status & NS_FORBIDDEN) ? na->last_usermask : "*@*");
					if (!stricmp(pattern, na->nick) || match_wild_nocase(pattern, buf))
					{
						if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nnicks <= NSListMax)
						{
							if (is_servadmin && (na->status & NS_NO_EXPIRE))
								noexpire_char = '!';
							else
								noexpire_char = ' ';
							if ((na->nc->flags & NI_HIDE_MASK) && !is_servadmin && na->nc != mync)
								snprintf(buf, sizeof(buf), "%-20s  [Hostname Hidden]", na->nick);
							else if (na->status & NS_FORBIDDEN)
								snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", na->nick);
							else if (na->nc->flags & NI_SUSPENDED)
								snprintf(buf, sizeof(buf), "%-20s  [Suspended]", na->nick);
							else
								snprintf(buf, sizeof(buf), "%-20s  %s", na->nick, na->last_usermask);
							notice_user(s_NickServ, u, "   %c%s", noexpire_char, buf);
						}
						++count;
					}
				}
			}
		}

		if (nronly || (is_servadmin && !matchflags))
		{
			noexpire_char = ' ';
			for (i = 0; i < 1024; ++i)
			{
				for (nr = nrlists[i]; nr; nr = nr->next)
				{
					snprintf(buf, sizeof(buf), "%s!*@*", nr->nick);
					if (!stricmp(pattern, nr->nick) || match_wild_nocase(pattern, buf))
					{
						if (++nnicks <= NSListMax)
						{
							snprintf(buf, sizeof(buf), "%-20s  [UNCONFIRMED]", nr->nick);
							notice_user(s_NickServ, u, "   %c%s", noexpire_char, buf);
						}
					}
				}
			}
		}
		notice_lang(s_NickServ, u, NICK_LIST_RESULTS, nnicks > NSListMax ? NSListMax : nnicks, nnicks);
		if (tofree)
			delete [] pattern;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (is_services_admin(u))
			notice_lang(s_NickServ, u, NICK_SERVADMIN_HELP_LIST);
		else
			notice_lang(s_NickServ, u, NICK_HELP_LIST);
	}

	void OnSyntaxError(User *u)
	{
		if (is_services_admin(u))
			syntax_error(s_NickServ, u, "LIST", NICK_LIST_SERVADMIN_SYNTAX);
		else
			syntax_error(s_NickServ, u, "LIST", NICK_LIST_SYNTAX);
	}
};

class NSList : public Module
{
 public:
	NSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSList(), MOD_UNIQUE);

		this->SetNickHelp(myNickServHelp);
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User *u)
{
	if (!NSListOpersOnly || is_oper(u))
		notice_lang(s_NickServ, u, NICK_HELP_CMD_LIST);
}

MODULE_INIT("ns_list", NSList)
