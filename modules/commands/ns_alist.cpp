/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSAList : public Command
{
 public:
	CommandNSAList(Module *creator) : Command(creator, "nickserv/alist", 0, 2)
	{
		this->SetDesc(_("List channels you have access on"));
		this->SetSyntax(_("[\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string nick = source.GetNick();
		NickCore *nc = source.nc;

		if (params.size() && source.HasPriv("nickserv/alist"))
		{
			nick = params[0];
			const NickAlias *na = NickAlias::Find(nick);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return;
			}
			nc = na->nc;
		}

		ListFormatter list;
		int chan_count = 0;

		list.AddColumn("Number").AddColumn("Channel").AddColumn("Access");

		source.Reply(_("Channels that \002%s\002 has access on:"), nc->display.c_str());

		std::deque<ChannelInfo *> queue;
		nc->GetChannelReferences(queue);

		if (queue.empty())
		{
			source.Reply(_("\2%s\2 has no access in any channels."), nc->display.c_str());
			return;
		}

		for (unsigned i = 0; i < queue.size(); ++i)
		{
			ChannelInfo *ci = queue[i];
			ListFormatter::ListEntry entry;

			if (ci->GetFounder() == nc)
			{
				++chan_count;
				entry["Number"] = stringify(chan_count);
				entry["Channel"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
				entry["Access"] = "Founder";
				list.AddEntry(entry);
				continue;
			}

			if (ci->GetSuccessor() == nc)
			{
				++chan_count;
				entry["Number"] = stringify(chan_count);
				entry["Channel"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
				entry["Access"] = "Successor";
				list.AddEntry(entry);
				continue;
			}

			AccessGroup access = ci->AccessFor(nc);
			if (access.empty())
				continue;
				
			++chan_count;

			entry["Number"] = stringify(chan_count);
			entry["Channel"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
			for (unsigned j = 0; j < access.size(); ++j)
				entry["Access"] = entry["Access"] + ", " + access[j]->AccessSerialize();
			entry["Access"] = entry["Access"].substr(2);
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of list - %d channels shown."), chan_count);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all channels you have access on.\n"
				" \n"
				"Channels that have the \037NOEXPIRE\037 option set will be\n"
				"prefixed by an exclamation mark. The nickname parameter is\n"
				"limited to Services Operators"));

		return true;
	}
};

class NSAList : public Module
{
	CommandNSAList commandnsalist;

 public:
	NSAList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsalist(this)
	{

	}
};

MODULE_INIT(NSAList)
