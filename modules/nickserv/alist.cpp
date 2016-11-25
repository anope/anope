/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class CommandNSAList : public Command
{
	static bool ChannelSort(ChanServ::Channel *ci1, ChanServ::Channel *ci2)
	{
		return ci::less()(ci1->GetName(), ci2->GetName());
	}

 public:
	CommandNSAList(Module *creator) : Command(creator, "nickserv/alist", 0, 2)
	{
		this->SetDesc(_("List channels you have access on"));
		this->SetSyntax(_("[\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string nick = source.GetNick();
		NickServ::Account *nc = source.nc;

		if (params.size() && source.HasPriv("nickserv/alist"))
		{
			nick = params[0];
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}
			nc = na->GetAccount();
		}

		ListFormatter list(source.GetAccount());
		int chan_count = 0;

		list.AddColumn(_("Number")).AddColumn(_("Channel")).AddColumn(_("Access")).AddColumn(_("Description"));

		std::vector<ChanServ::Channel *> chans = nc->GetRefs<ChanServ::Channel *>();
		std::sort(chans.begin(), chans.end(), ChannelSort);
		for (ChanServ::Channel *ci : chans)
		{
			ListFormatter::ListEntry entry;

			if (ci->GetFounder() == nc)
			{
				++chan_count;
				entry["Number"] = stringify(chan_count);
				entry["Channel"] = (ci->IsNoExpire() ? "!" : "") + ci->GetName();
				entry["Access"] = Language::Translate(source.GetAccount(), _("Founder"));
				entry["Description"] = ci->GetDesc();
				list.AddEntry(entry);
				continue;
			}

			if (ci->GetSuccessor() == nc)
			{
				++chan_count;
				entry["Number"] = stringify(chan_count);
				entry["Channel"] = (ci->IsNoExpire() ? "!" : "") + ci->GetName();
				entry["Access"] = Language::Translate(source.GetAccount(), _("Successor"));
				entry["Description"] = ci->GetDesc();
				list.AddEntry(entry);
				continue;
			}

			ChanServ::AccessGroup access = ci->AccessFor(nc, false);
			if (access.empty())
				continue;

			++chan_count;

			entry["Number"] = stringify(chan_count);
			entry["Channel"] = (ci->IsNoExpire() ? "!" : "") + ci->GetName();
			for (unsigned j = 0; j < access.size(); ++j)
				entry["Access"] = entry["Access"] + ", " + access[j]->AccessSerialize();
			entry["Access"] = entry["Access"].substr(2);
			entry["Description"] = ci->GetDesc();
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		if (!chan_count)
		{
			source.Reply(_("\002{0}\002 has no access in any channels."), nc->GetDisplay());
		}
		else
		{
			source.Reply(_("Channels that \002{0}\002 has access on:"), nc->GetDisplay());

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("End of list - \002{0}\002 channels shown."), chan_count);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists all channels you have access on.\n"
		               " \n"
		               "Channels that have the \037NOEXPIRE\037 option set will be prefixed by an exclamation mark. The nickname parameter is limited to Services Operators"));

		return true;
	}
};

class NSAList : public Module
{
	CommandNSAList commandnsalist;

 public:
	NSAList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsalist(this)
	{

	}
};

MODULE_INIT(NSAList)
