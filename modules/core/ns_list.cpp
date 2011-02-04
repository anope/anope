/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
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
		User *u = source.u;

		Anope::string pattern = params[0];
		const NickCore *mync;
		unsigned nnicks;
		char buf[BUFSIZE];
		bool is_servadmin = u->Account()->IsServicesOper();
		char noexpire_char = ' ';
		int count = 0, from = 0, to = 0;
		bool suspended, nsnoexpire, forbidden, unconfirmed;

		suspended = nsnoexpire = forbidden = unconfirmed = false;

		if (Config->NSListOpersOnly && !u->HasMode(UMODE_OPER)) /* reverse the help logic */
		{
			source.Reply(LanguageString::ACCESS_DENIED);
			return MOD_STOP;
		}

		if (pattern[0] == '#')
		{
			Anope::string tmp = myStrGetToken(pattern.substr(1), '-', 0); /* Read FROM out */
			if (tmp.empty())
			{
				source.Reply(LanguageString::LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			if (!tmp.is_number_only())
			{
				source.Reply(LanguageString::LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			from = convertTo<int>(tmp);
			tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
			if (tmp.empty())
			{
				source.Reply(LanguageString::LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			if (!tmp.is_number_only())
			{
				source.Reply(LanguageString::LIST_INCORRECT_RANGE);
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

		source.Reply(LanguageString::LIST_HEADER, pattern.c_str());
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
						source.Reply("   %c%s", noexpire_char, buf);
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
					source.Reply("   %c%s", noexpire_char, buf);
				}
			}
		}
		source.Reply(_("End of list - %d/%d matches shown."), nnicks > Config->NSListMax ? Config->NSListMax : nnicks, nnicks);
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(_("Syntax: \002LIST \037pattern\037 [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]\002\n"
					" \n"
					"Lists all registered nicknames which match the given\n"
					"pattern, in \037nick!user@host\037 format.  Nicks with the \002PRIVATE\002\n"
					"option set will only be displayed to Services Operators.  Nicks\n"
					"with the \002NOEXPIRE\002 option set will have a \002!\002 appended to\n"
					"the nickname for Services Operators.\n"
					" \n"
					"If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only\n"
					"nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the\n"
					"NOEXPIRE flag set will be displayed. If multiple options are\n"
					"given, all nicks matching at least one option will be displayed.\n"
					"These options are limited to \037Services Operators\037.  \n"
					"Examples:\n"
					" \n"
					"    \002LIST *!joeuser@foo.com\002\n"
					"        Lists all registered nicks owned by joeuser@foo.com.\n"
					" \n"
					"    \002LIST *Bot*!*@*\002\n"
					"        Lists all registered nicks with \002Bot\002 in their\n"
					"        names (case insensitive).\n"
					" \n"
					"    \002LIST * NOEXPIRE\002\n"
					"        Lists all registered nicks which have been set to\n"));
		else
			source.Reply(_("Syntax: \002LIST \037pattern\037\002\n"
					" \n"
					"Lists all registered nicknames which match the given\n"
					"pattern, in \037nick!user@host\037 format.  Nicks with the\n"
					"\002PRIVATE\002 option set will not be displayed.\n"
					" \n"
					"Examples:\n"
					" \n"
					"    \002LIST *!joeuser@foo.com\002\n"
					"        Lists all nicks owned by joeuser@foo.com.\n"
					" \n"
					"    \002LIST *Bot*!*@*\002\n"
					"        Lists all registered nicks with \002Bot\002 in their\n"
					"        names (case insensitive).\n"
					" \n"
					"    \002LIST *!*@*.bar.org\002\n"
					"        Lists all nicks owned by users in the \002bar.org\002\n"
					"        domain."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account()->IsServicesOper())
			SyntaxError(source, "LIST", _("LIST \037pattern\037 [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]"));
		else
			SyntaxError(source, "LIST", LanguageString::NICK_LIST_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    LIST       List all registered nicknames that match a given pattern"));
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
