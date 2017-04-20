/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class CommandHSList : public Command
{
	bool VHostMatches(NickServ::Account *acc, const Anope::string &mask)
	{
		for (HostServ::VHost *vhost : acc->GetRefs<HostServ::VHost *>())
			if (Anope::Match(vhost->GetHost(), mask))
				return true;
		return false;
	}

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

		unsigned display_counter = 0, listmax = Config->GetModule(this->GetOwner())->Get<unsigned>("listmax", "50");
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Account")).AddColumn(_("Vhost")).AddColumn(_("Creator")).AddColumn(_("Created"));

		for (NickServ::Account *acc : NickServ::service->GetAccountList())
		{
			std::vector<HostServ::VHost *> vhosts = acc->GetRefs<HostServ::VHost *>();

			if (vhosts.empty())
				continue;

			++counter;

			if (display_counter >= listmax)
				continue;

			if (from && to && (counter < from || counter > to))
				continue;

			if (!key.empty() && !Anope::Match(acc->GetDisplay(), key) && !VHostMatches(acc, key))
				continue;

			++display_counter;

			bool first = true;
			for (HostServ::VHost *vhost : vhosts)
			{
				ListFormatter::ListEntry entry;

				if (first)
				{
					entry["Number"] = stringify(display_counter);
					entry["Account"] = acc->GetDisplay();
				}

				entry["Vhost"] = vhost->Mask();
				entry["Creator"] = vhost->GetCreator();
				entry["Created"] = Anope::strftime(vhost->GetCreated(), NULL, true);
				list.AddEntry(entry);

				first = false;
			}
		}

		if (!display_counter)
		{
			source.Reply(_("No records to display."));
			return;
		}

		if (!key.empty())
		{
			source.Reply(_("Displayed records matching key \002{0}\002 (count: \002{1}\002)."), key, display_counter);
		}
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
		source.Reply(_("Lists all vhosts. If \037key\037 is specified, only entries whose account or vhost match the pattern given in \037key\037 are displayed."
		               "If a \037#X-Y\037 style is used, only entries between the range of \002X\002 and \002Y\002 will be displayed.\n"
		               "\n"
		               "Examples:\n"
		               "         {0} Rob*\n"
		               "         Lists all entries with the account or vhost beginning with \"Rob\"\n"
		               "\n"
		               "         {0} #1-3\n"
		               "         Lists the first three entries."),
		               source.GetCommand());
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
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSList)
