/* ChanServ core functions
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

class CommandCSList : public Command
{
 public:
	CommandCSList() : Command("LIST", 1, 2)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Lists all registered channels matching the given pattern"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string pattern = params[0];
		unsigned nchans;
		char buf[BUFSIZE];
		bool is_servadmin = u->Account()->HasCommand("chanserv/list");
		int count = 0, from = 0, to = 0;
		bool forbidden = false, suspended = false, channoexpire = false;

		if (Config->CSListOpersOnly && !u->HasMode(UMODE_OPER))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (pattern[0] == '#')
		{
			Anope::string n1 = myStrGetToken(pattern.substr(1), '-', 0), /* Read FROM out */
					n2 = myStrGetTokenRemainder(pattern, '-', 1);

			try
			{
				from = convertTo<int>(n1);
				to = convertTo<int>(n2);
			}
			catch (const ConvertException &)
			{
				source.Reply(_(LIST_INCORRECT_RANGE));
				source.Reply(_("To search for channels starting with #, search for the channel\n"
					"name without the #-sign prepended (\002anope\002 instead of \002#anope\002)."));
				return MOD_CONT;
			}

			pattern = "*";
		}

		nchans = 0;

		if (is_servadmin && params.size() > 1)
		{
			Anope::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetToken(keyword))
			{
				if (keyword.equals_ci("FORBIDDEN"))
					forbidden = true;
				if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				if (keyword.equals_ci("NOEXPIRE"))
					channoexpire = true;
			}
		}

		Anope::string spattern = "#" + pattern;

		source.Reply(_(LIST_HEADER), pattern.c_str());

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			ChannelInfo *ci = it->second;

			if (!is_servadmin && (ci->HasFlag(CI_PRIVATE) || ci->HasFlag(CI_FORBIDDEN) || ci->HasFlag(CI_SUSPENDED)))
				continue;
			if (forbidden && !ci->HasFlag(CI_FORBIDDEN))
				continue;
			else if (suspended && !ci->HasFlag(CI_SUSPENDED))
				continue;
			else if (channoexpire && !ci->HasFlag(CI_NO_EXPIRE))
				continue;

			if (pattern.equals_ci(ci->name) || ci->name.equals_ci(spattern) || Anope::Match(ci->name, pattern) || Anope::Match(ci->name, spattern))
			{
				if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nchans <= Config->CSListMax)
				{
					char noexpire_char = ' ';
					if (is_servadmin && (ci->HasFlag(CI_NO_EXPIRE)))
						noexpire_char = '!';

					if (ci->HasFlag(CI_FORBIDDEN))
						snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", ci->name.c_str());
					else if (ci->HasFlag(CI_SUSPENDED))
						snprintf(buf, sizeof(buf), "%-20s  [Suspended]", ci->name.c_str());
					else
						snprintf(buf, sizeof(buf), "%-20s  %s", ci->name.c_str(), !ci->desc.empty() ? ci->desc.c_str() : "");

					source.Reply("  %c%s", noexpire_char, buf);
				}
				++count;
			}
		}

		source.Reply(_("End of list - %d/%d matches shown."), nchans > Config->CSListMax ? Config->CSListMax : nchans, nchans);
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002LIST \037pattern\037\002\n"
				" \n"
				"Lists all registered channels matching the given pattern.\n"
				"(Channels with the \002PRIVATE\002 option set are not listed.)\n"
				"Note that a preceding '#' specifies a range, channel names\n"
				"are to be written without '#'."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LIST", _(NICK_LIST_SYNTAX));
	}
};

class CSList : public Module
{
	CommandCSList commandcslist;

 public:
	CSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcslist);
	}
};

MODULE_INIT(CSList)
