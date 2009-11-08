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
#include "hashcomp.h"

class CommandNSList : public Command
{
 public:
	CommandNSList() : Command("LIST", 1, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
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
		const char *pattern = params[0].c_str();
		NickAlias *na;
		NickCore *mync;
		unsigned nnicks, i;
		char buf[BUFSIZE];
		bool is_servadmin = u->nc->IsServicesOper();
		NickRequest *nr = NULL;
		char noexpire_char = ' ';
		int count = 0, from = 0, to = 0, tofree = 0;
		char *tmp = NULL;
		char *s = NULL;
		bool suspended, nsnoexpire, forbidden, unconfirmed;

		suspended = nsnoexpire = forbidden = unconfirmed = false;

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
			spacesepstream keywords(params[1].c_str());
			while (keywords.GetToken(keyword))
			{
				ci::string keyword_ci = keyword.c_str();
				if (keyword_ci == "FORBIDDEN")
					forbidden = true;
				if (keyword_ci == "NOEXPIRE")
					nsnoexpire = true;
				if (keyword_ci == "SUSPENDED")
					suspended = true;
				if (keyword_ci == "UNCONFIRMED")
					unconfirmed = true;
			}
		}

		mync = u->nc;

		notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
		if (!unconfirmed)
		{
			for (i = 0; i < 1024; ++i)
			{
				for (na = nalists[i]; na; na = na->next)
				{
					/* Don't show private and forbidden nicks to non-services admins. */
					if ((na->HasFlag(NS_FORBIDDEN)) && !is_servadmin)
						continue;
					if ((na->nc->HasFlag(NI_PRIVATE)) && !is_servadmin && na->nc != mync)
						continue;
					if (forbidden && !na->HasFlag(NS_FORBIDDEN))
						continue;
					else if (nsnoexpire && !na->HasFlag(NS_NO_EXPIRE))
						continue;
					else if (suspended && !na->nc->HasFlag(NI_SUSPENDED))
						continue;

					/* We no longer compare the pattern against the output buffer.
					 * Instead we build a nice nick!user@host buffer to compare.
					 * The output is then generated separately. -TheShadow */
					snprintf(buf, sizeof(buf), "%s!%s", na->nick, na->last_usermask && !(na->HasFlag(NS_FORBIDDEN)) ? na->last_usermask : "*@*");
					if (!stricmp(pattern, na->nick) || Anope::Match(buf, pattern, false))
					{
						if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nnicks <= NSListMax)
						{
							if (is_servadmin && (na->HasFlag(NS_NO_EXPIRE)))
								noexpire_char = '!';
							else
								noexpire_char = ' ';
							if ((na->nc->HasFlag(NI_HIDE_MASK)) && !is_servadmin && na->nc != mync)
								snprintf(buf, sizeof(buf), "%-20s  [Hostname Hidden]", na->nick);
							else if (na->HasFlag(NS_FORBIDDEN))
								snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", na->nick);
							else if (na->nc->HasFlag(NI_SUSPENDED))
								snprintf(buf, sizeof(buf), "%-20s  [Suspended]", na->nick);
							else
								snprintf(buf, sizeof(buf), "%-20s  %s", na->nick, na->last_usermask);
							u->SendMessage(s_NickServ, "   %c%s", noexpire_char, buf);
						}
						++count;
					}
				}
			}
		}

		if (unconfirmed || is_servadmin)
		{
			noexpire_char = ' ';
			for (i = 0; i < 1024; ++i)
			{
				for (nr = nrlists[i]; nr; nr = nr->next)
				{
					snprintf(buf, sizeof(buf), "%s!*@*", nr->nick);
					if (!stricmp(pattern, nr->nick) || Anope::Match(buf, pattern, false))
					{
						if (++nnicks <= NSListMax)
						{
							snprintf(buf, sizeof(buf), "%-20s  [UNCONFIRMED]", nr->nick);
							u->SendMessage(s_NickServ, "   %c%s", noexpire_char, buf);
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

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_LIST);
		else
			notice_help(s_NickServ, u, NICK_HELP_LIST);

		return true;
	}

	void OnSyntaxError(User *u)
	{
		if (u->nc->IsServicesOper())
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

		this->AddCommand(NICKSERV, new CommandNSList());
		
		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_LIST);
	}
};

MODULE_INIT(NSList)
