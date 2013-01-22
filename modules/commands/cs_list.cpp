/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSList : public Command
{
 public:
	CommandCSList(Module *creator) : Command(creator, "chanserv/list", 1, 2)
	{
		this->SetDesc(_("Lists all registered channels matching the given pattern"));
		this->SetSyntax(_("\037pattern\037 [SUSPENDED] [NOEXPIRE]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string pattern = params[0];
		unsigned nchans;
		bool is_servadmin = source.HasCommand("chanserv/list");
		int count = 0, from = 0, to = 0;
		bool suspended = false, channoexpire = false;

		if (pattern[0] == '#')
		{
			Anope::string n1, n2;
			sepstream(pattern.substr(1), '-').GetToken(n1, 0);
			sepstream(pattern, '-').GetToken(n2, 1);

			try
			{
				from = convertTo<int>(n1);
				to = convertTo<int>(n2);
			}
			catch (const ConvertException &)
			{
				source.Reply(LIST_INCORRECT_RANGE);
				source.Reply(_("To search for channels starting with #, search for the channel\n"
					"name without the #-sign prepended (\002anope\002 instead of \002#anope\002)."));
				return;
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
				if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				if (keyword.equals_ci("NOEXPIRE"))
					channoexpire = true;
			}
		}

		Anope::string spattern = "#" + pattern;

		source.Reply(_("List of entries matching \002%s\002:"), pattern.c_str());

		ListFormatter list;
		list.AddColumn("Name").AddColumn("Description");

		for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
		{
			const ChannelInfo *ci = it->second;

			if (!is_servadmin && (ci->HasExt("PRIVATE") || ci->HasExt("SUSPENDED")))
				continue;
			else if (suspended && !ci->HasExt("SUSPENDED"))
				continue;
			else if (channoexpire && !ci->HasExt("NO_EXPIRE"))
				continue;

			if (pattern.equals_ci(ci->name) || ci->name.equals_ci(spattern) || Anope::Match(ci->name, pattern, false, true) || Anope::Match(ci->name, spattern, false, true))
			{
				if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nchans <= Config->CSListMax)
				{
					bool isnoexpire = false;
					if (is_servadmin && (ci->HasExt("NO_EXPIRE")))
						isnoexpire = true;

					ListFormatter::ListEntry entry;
					entry["Name"] = (isnoexpire ? "!" : "") + ci->name;
					if (ci->HasExt("SUSPENDED"))
						entry["Description"] = "[Suspended]";
					else
						entry["Description"] = ci->desc;
					list.AddEntry(entry);
				}
				++count;
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of list - %d/%d matches shown."), nchans > Config->CSListMax ? Config->CSListMax : nchans, nchans);
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all registered channels matching the given pattern.\n"
				"Channels with the \002PRIVATE\002 option set will only be\n"
				"displayed to Services Operators with the proper access.\n"
				"Channels with the \002NOEXPIRE\002 option set will have\n"
				"a \002!\002 prefixed to the channel for Services Operators to see.\n"
				" \n"
				"Note that a preceding '#' specifies a range, channel names\n"
				"are to be written without '#'.\n"
				" \n"
				"If the SUSPENDED or NOEXPIRE options are given, only channels\n"
				"which, respectively, are SUSPENDED or have the NOEXPIRE\n"
				"flag set will be displayed. If multiple options are given,\n"
				"all channels matching at least one option will be displayed.\n"
				"Note that these options are limited to \037Services Operators\037.\n"
				" \n"
				"Examples:\n"
				" \n"
				"    \002LIST *anope*\002\n"
				"        Lists all registered channels with \002anope\002 in their\n"
				"        names (case insensitive).\n"
				" \n"
				"    \002LIST * NOEXPIRE\002\n"
				"        Lists all registered channels which have been set to not expire.\n"
				" \n"
				"    \002LIST #51-100\002\n"
				"        Lists all registered channels within the given range (51-100).\n"));
		if (!Config->RegexEngine.empty())
			source.Reply(" \n"
					"Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this desired.", Config->RegexEngine.c_str());
		return true;
	}
};

class CSList : public Module
{
	CommandCSList commandcslist;

 public:
	CSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcslist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSList)
