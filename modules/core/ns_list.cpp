/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
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
		Anope::string pattern = params[0];
		const NickCore *mync;
		unsigned nnicks;
		char buf[BUFSIZE];
		bool is_servadmin = u->Account()->IsServicesOper();
		char noexpire_char = ' ';
		int count = 0, from = 0, to = 0;
		bool suspended, nsnoexpire, forbidden, unconfirmed;

		suspended = nsnoexpire = forbidden = unconfirmed = false;

		if (Config->NSListOpersOnly && !is_oper(u)) /* reverse the help logic */
		{
			notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
			return MOD_STOP;
		}

		if (pattern[0] == '#')
		{
			Anope::string tmp = myStrGetToken(pattern.substr(1), '-', 0); /* Read FROM out */
			if (tmp.empty())
			{
				notice_lang(Config->s_NickServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			if (!tmp.is_number_only())
			{
				notice_lang(Config->s_NickServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			from = convertTo<int>(tmp);
			tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
			if (tmp.empty())
			{
				notice_lang(Config->s_NickServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			if (!tmp.is_number_only())
			{
				notice_lang(Config->s_NickServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			to = convertTo<int>(tmp);
			pattern = "*";
		}

		nnicks = 0;

		if (is_servadmin && params.size() > 1)
		{
			Anope::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetToken(keyword))
			{
				if (keyword.equals_ci("FORBIDDEN"))
					forbidden = true;
				if (keyword.equals_ci("NOEXPIRE"))
					nsnoexpire = true;
				if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				if (keyword.equals_ci("UNCONFIRMED"))
					unconfirmed = true;
			}
		}

		mync = u->Account();

		notice_lang(Config->s_NickServ, u, NICK_LIST_HEADER, pattern.c_str());
		if (!unconfirmed)
		{
			for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
			{
				NickAlias *na = it->second;

				/* Don't show private and forbidden nicks to non-services admins. */
				if (na->HasFlag(NS_FORBIDDEN) && !is_servadmin)
					continue;
				if (na->nc->HasFlag(NI_PRIVATE) && !is_servadmin && na->nc != mync)
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
				snprintf(buf, sizeof(buf), "%s!%s", na->nick.c_str(), !na->last_usermask.empty() && !na->HasFlag(NS_FORBIDDEN) ? na->last_usermask.c_str() : "*@*");
				if (na->nick.equals_ci(pattern) || Anope::Match(buf, pattern))
				{
					if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nnicks <= Config->NSListMax)
					{
						if (is_servadmin && na->HasFlag(NS_NO_EXPIRE))
							noexpire_char = '!';
						else
							noexpire_char = ' ';
						if (na->nc->HasFlag(NI_HIDE_MASK) && !is_servadmin && na->nc != mync)
							snprintf(buf, sizeof(buf), "%-20s  [Hostname Hidden]", na->nick.c_str());
						else if (na->HasFlag(NS_FORBIDDEN))
							snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", na->nick.c_str());
						else if (na->nc->HasFlag(NI_SUSPENDED))
							snprintf(buf, sizeof(buf), "%-20s  [Suspended]", na->nick.c_str());
						else
							snprintf(buf, sizeof(buf), "%-20s  %s", na->nick.c_str(), na->last_usermask.c_str());
						u->SendMessage(Config->s_NickServ, "   %c%s", noexpire_char, buf);
					}
					++count;
				}
			}
		}

		if (unconfirmed || is_servadmin)
		{
			noexpire_char = ' ';

			for (nickrequest_map::const_iterator it = NickRequestList.begin(), it_end = NickRequestList.end(); it != it_end; ++it)
			{
				NickRequest *nr = it->second;

				snprintf(buf, sizeof(buf), "%s!*@*", nr->nick.c_str());
				if ((nr->nick.equals_ci(pattern) || Anope::Match(buf, pattern)) && ++nnicks <= Config->NSListMax)
				{
					snprintf(buf, sizeof(buf), "%-20s  [UNCONFIRMED]", nr->nick.c_str());
					u->SendMessage(Config->s_NickServ, "   %c%s", noexpire_char, buf);
				}
			}
		}
		notice_lang(Config->s_NickServ, u, NICK_LIST_RESULTS, nnicks > Config->NSListMax ? Config->NSListMax : nnicks, nnicks);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config->s_NickServ, u, NICK_SERVADMIN_HELP_LIST);
		else
			notice_help(Config->s_NickServ, u, NICK_HELP_LIST);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		if (u->Account()->IsServicesOper())
			syntax_error(Config->s_NickServ, u, "LIST", NICK_LIST_SERVADMIN_SYNTAX);
		else
			syntax_error(Config->s_NickServ, u, "LIST", NICK_LIST_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_LIST);
	}
};

class NSList : public Module
{
	CommandNSList commandnslist;

 public:
	NSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnslist);
	}
};

MODULE_INIT(NSList)
