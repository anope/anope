/* HostServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandHSList : public Command
{
 public:
	CommandHSList(Module *creator) : Command(creator, "hostserv/list", 0, 1)
	{
		this->SetDesc(_("Displays one or more vhost entries"));
		this->SetSyntax(_("[\037key\037|\037#X-Y\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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
				source.Reply(_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002."));
				return;
			}
			for (unsigned i = 1, end = key.length(); i < end; ++i)
			{
				if (!isdigit(key[i]) && i != tmp)
				{
					source.Reply(_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002."));
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

		unsigned display_counter = 0, listmax = Config->GetModule(this->owner)->Get<unsigned>("listmax", "50");
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Nick")).AddColumn(_("Vhost")).AddColumn(_("Creator")).AddColumn(_("Created"));

		for (NickServ::Nick *na : NickServ::service->GetNickList())
		{
			if (!na->HasVhost())
				continue;

			if (!key.empty() && key[0] != '#')
			{
				if ((Anope::Match(na->GetNick(), key) || Anope::Match(na->GetVhostHost(), key)) && display_counter < listmax)
				{
					++display_counter;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(display_counter);
					entry["Nick"] = na->GetNick();
					if (!na->GetVhostIdent().empty())
						entry["Vhost"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
					else
						entry["Vhost"] = na->GetVhostHost();
					entry["Creator"] = na->GetVhostCreator();
					entry["Created"] = Anope::strftime(na->GetVhostCreated(), NULL, true);
					list.AddEntry(entry);
				}
			}
			else
			{
				/**
				 * List the host if its in the display range, and not more
				 * than NSListMax records have been displayed...
				 **/
				if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < listmax)
				{
					++display_counter;
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(display_counter);
					entry["Nick"] = na->GetNick();
					if (!na->GetVhostIdent().empty())
						entry["Vhost"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
					else
						entry["Vhost"] = na->GetVhostHost();
					entry["Creator"] = na->GetVhostCreator();
					entry["Created"] = Anope::strftime(na->GetVhostCreated(), NULL, true);
					list.AddEntry(entry);
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
			source.Reply(_("Displayed records matching key \002{0}\002 (count: \002{1}\002)."), key, display_counter);
		else
		{
			if (from)
				source.Reply(_("Displayed records from \002{0}\002 to \002{1}\002."), from, to);
			else
				source.Reply(_("Displayed all records (count: \002{0}\002)."), display_counter);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists all vhosts. If \037key\037 is specified, only entries whose nick or vhost match the pattern given in \037key\037 are displayed."
		               "If a \037#X-Y\037 style is used, only entries between the range of \002X\002 and \002Y\002 will be displayed.\n"
		               "\n"
		               "Examples:\n"
		               "         {0} Rob*\n"
		               "         Lists all entries with the nick or vhost beginning with \"Rob\"\n"
		               "\n"
		               "         {0} #1-3\n"
		               "         Lists the first three entries."),
		               source.command);
		return true;
	}
};

class HSList : public Module
{
	CommandHSList commandhslist;

 public:
	HSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhslist(this)
	{
	}
};

MODULE_INIT(HSList)
