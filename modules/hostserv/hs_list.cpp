/* HostServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandHSList final
	: public Command
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

				from = Anope::Convert<int>(key.substr(1, tmp - 1), 0);
				to = Anope::Convert<int>(key.substr(tmp + 1), 0);
			}
		}

		unsigned display_counter = 0, listmax = Config->GetModule(this->owner).Get<unsigned>("listmax", "50");
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Nick")).AddColumn(_("VHost")).AddColumn(_("Creator")).AddColumn(_("Created"));

		for (const auto &[_, na] : *NickAliasList)
		{
			if (!na->HasVHost())
				continue;

			if (!key.empty() && key[0] != '#')
			{
				if ((Anope::Match(na->nick, key) || Anope::Match(na->GetVHostHost(), key)) && display_counter < listmax)
				{
					++display_counter;

					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(display_counter);
					entry["Nick"] = na->nick;
					entry["VHost"] = na->GetVHostMask();
					entry["Creator"] = na->GetVHostCreator();
					entry["Created"] = Anope::strftime(na->GetVHostCreated(), NULL, true);
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
					entry["Number"] = Anope::ToString(display_counter);
					entry["Nick"] = na->nick;
					entry["VHost"] = na->GetVHostMask();
					entry["Creator"] = na->GetVHostCreator();
					entry["Created"] = Anope::strftime(na->GetVHostCreated(), NULL, true);
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
			source.Reply(_("Displayed records matching key \002%s\002 (count: \002%d\002)."), key.c_str(), display_counter);
		else
		{
			if (from)
				source.Reply(_("Displayed records from \002%d\002 to \002%d\002."), from, to);
			else
				source.Reply(_("Displayed all records (count: \002%d\002)."), display_counter);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const auto &reply : replies)
			source.Reply(reply);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command lists registered vhosts to the operator. "
			"If a \037key\037 is specified, only entries whose nick or vhost match "
			"the pattern given in \037key\037 are displayed e.g. Rob* for all "
			"entries beginning with \"Rob\". "
			"If a \037#X-Y\037 style is used, only entries between the range of \002X\002 "
			"and \002Y\002 will be displayed, e.g. \002#1-3\002 will display the first 3 "
			"nick/vhost entries."
		));
		return true;
	}
};

class HSList final
	: public Module
{
	CommandHSList commandhslist;

public:
	HSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhslist(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSList)
