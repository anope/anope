/* HostServ core functions
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

class CommandHSList : public Command
{
 public:
	CommandHSList(Module *creator) : Command(creator, "hostserv/list", 0, 1)
	{
		this->SetDesc(_("Displays one or more vhost entries."));
		this->SetSyntax(_("\002[<key>|<#X-Y>]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &key = !params.empty() ? params[0] : "";
		int from = 0, to = 0, counter = 1;

		/**
		 * Do a check for a range here, then in the next loop
		 * we'll only display what has been requested..
		 **/
		if (!key.empty() && key[0] == '#')
		{
			size_t tmp = key.find('-');
			if (tmp == Anope::string::npos || tmp == key.length() || tmp == 1)
			{
				source.Reply(LIST_INCORRECT_RANGE);
				return;
			}
			for (unsigned i = 1, end = key.length(); i < end; ++i)
			{
				if (!isdigit(key[i]) && i != tmp)
				{
					source.Reply(LIST_INCORRECT_RANGE);
					return;
				}
				try
				{
					from = convertTo<int>(key.substr(1, tmp - 1));
					to = convertTo<int>(key.substr(tmp + 1));
				}
				catch (const ConvertException &) { }
			}
		}

		unsigned display_counter = 0;
		ListFormatter list;
		list.addColumn("Number").addColumn("Nick").addColumn("Vhost").addColumn("Creator").addColumn("Created");

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;

			if (!na->hostinfo.HasVhost())
				continue;

			if (!key.empty() && key[0] != '#')
			{
				if ((Anope::Match(na->nick, key) || Anope::Match(na->hostinfo.GetHost(), key)) && display_counter < Config->NSListMax)
				{
					++display_counter;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(display_counter);
					entry["Nick"] = na->nick;
					if (!na->hostinfo.GetIdent().empty())
						entry["Vhost"] = na->hostinfo.GetIdent() + "@" + na->hostinfo.GetHost();
					else
						entry["Vhost"] = na->hostinfo.GetHost();
					entry["Creator"] = na->hostinfo.GetCreator();
					entry["Created"] = do_strftime(na->hostinfo.GetTime());
					list.addEntry(entry);
				}
			}
			else
			{
				/**
				 * List the host if its in the display range, and not more
				 * than NSListMax records have been displayed...
				 **/
				if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config->NSListMax)
				{
					++display_counter;
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(display_counter);
					entry["Nick"] = na->nick;
					if (!na->hostinfo.GetIdent().empty())
						entry["Vhost"] = na->hostinfo.GetIdent() + "@" + na->hostinfo.GetHost();
					else
						entry["Vhost"] = na->hostinfo.GetHost();
					entry["Creator"] = na->hostinfo.GetCreator();
					entry["Created"] = do_strftime(na->hostinfo.GetTime());
					list.addEntry(entry);
				}
			}
			++counter;
		}

		if (!display_counter)
		{
			source.Reply(_("No records to display."));
			return;
		}

		if (!key.empty())
			source.Reply(_("Displayed records matching key \002%s\002 (Count: \002%d\002)"), key.c_str(), display_counter);
		else
		{
			if (from)
				source.Reply(_("Displayed records from \002%d\002 to \002%d\002"), from, to);
			else
				source.Reply(_("Displayed all records (Count: \002%d\002)"), display_counter);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command lists registered vhosts to the operator\n"
				"if a Key is specified, only entries whos nick or vhost match\n"
				"the pattern given in <key> are displayed e.g. Rob* for all\n"
				"entries beginning with \"Rob\"\n"
				"If a #X-Y style is used, only entries between the range of X\n"
				"and Y will be displayed, e.g. #1-3 will display the first 3\n"
				"nick/vhost entries.\n"
				"The list uses the value of NSListMax as a hard limit for the\n"
				"number of items to display to a operator at any 1 time."));
		return true;
	}
};

class HSList : public Module
{
	CommandHSList commandhslist;

 public:
	HSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandhslist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(HSList)
