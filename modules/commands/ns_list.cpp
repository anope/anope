/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSList : public Command
{
 public:
	CommandNSList(Module *creator) : Command(creator, "nickserv/list", 1, 2)
	{
		this->SetDesc(_("List all registered nicknames that match a given pattern"));
		this->SetSyntax(_("\037pattern\037 [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;

		Anope::string pattern = params[0];
		const NickCore *mync;
		unsigned nnicks;
		bool is_servadmin = u->IsServicesOper();
		int count = 0, from = 0, to = 0;
		bool suspended, nsnoexpire, unconfirmed;

		suspended = nsnoexpire = unconfirmed = false;

		if (pattern[0] == '#')
		{
			Anope::string n1 = myStrGetToken(pattern.substr(1), '-', 0), /* Read FROM out */
					n2 = myStrGetToken(pattern, '-', 1);
			try
			{
				from = convertTo<int>(n1);
				to = convertTo<int>(n2);
			}
			catch (const ConvertException &)
			{
				source.Reply(LIST_INCORRECT_RANGE);
				return;
			}

			pattern = "*";
		}

		nnicks = 0;

		if (is_servadmin && params.size() > 1)
		{
			Anope::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetToken(keyword))
			{
				if (keyword.equals_ci("NOEXPIRE"))
					nsnoexpire = true;
				if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				if (keyword.equals_ci("UNCONFIRMED"))
					unconfirmed = true;
			}
		}

		mync = u->Account();
		ListFormatter list;

		list.addColumn("Nick").addColumn("Last usermask");

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;

			/* Don't show private nicks to non-services admins. */
			if (na->nc->HasFlag(NI_PRIVATE) && !is_servadmin && na->nc != mync)
				continue;
			else if (nsnoexpire && !na->HasFlag(NS_NO_EXPIRE))
				continue;
			else if (suspended && !na->nc->HasFlag(NI_SUSPENDED))
				continue;
			else if (unconfirmed && !na->nc->HasFlag(NI_UNCONFIRMED))
				continue;

			/* We no longer compare the pattern against the output buffer.
			 * Instead we build a nice nick!user@host buffer to compare.
			 * The output is then generated separately. -TheShadow */
			Anope::string buf = Anope::printf("%s!%s", na->nick.c_str(), !na->last_usermask.empty() ? na->last_usermask.c_str() : "*@*");
			if (na->nick.equals_ci(pattern) || Anope::Match(buf, pattern))
			{
				if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nnicks <= Config->NSListMax)
				{
					bool isnoexpire = false;
					if (is_servadmin && na->HasFlag(NS_NO_EXPIRE))
						isnoexpire = true;

					ListFormatter::ListEntry entry;
					entry["Nick"] = (isnoexpire ? "!" : "") + na->nick;
					if (na->nc->HasFlag(NI_HIDE_MASK) && !is_servadmin && na->nc != mync)
						entry["Last usermask"] = "[Hostname hidden]";
					else if (na->nc->HasFlag(NI_SUSPENDED))
						entry["Last usermask"] = "[Suspended]";
					else if (na->nc->HasFlag(NI_UNCONFIRMED))
						entry["Last usermask"] = "[Unconfirmed]";
					else
						entry["Last usermask"] = na->last_usermask;
					list.addEntry(entry);
				}
				++count;
			}
		}
			
		source.Reply(_("List of entries matching \002%s\002:"), pattern.c_str());

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of list - %d/%d matches shown."), nnicks > Config->NSListMax ? Config->NSListMax : nnicks, nnicks);
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all registered nicknames which match the given\n"
				"pattern, in \037nick!user@host\037 format.  Nicks with the \002PRIVATE\002\n"
				"option set will only be displayed to Services Operators.  Nicks\n"
				"with the \002NOEXPIRE\002 option set will have a \002!\002 appended to\n"
				"the nickname for Services Operators.\n"
				" \n"
				"If the SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only\n"
				"nicks which, respectively, are SUSPENDED, UNCONFIRMED or have the\n"
				"NOEXPIRE flag set will be displayed. If multiple options are\n"
				"given, all nicks matching at least one option will be displayed.\n"
				"These options are limited to \037Services Operators\037.  \n"
				"\n"
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
				"        Lists all registered nicks which have been set to not expire.\n"));

		return true;
	}
};

class NSList : public Module
{
	CommandNSList commandnslist;

 public:
	NSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnslist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSList)
