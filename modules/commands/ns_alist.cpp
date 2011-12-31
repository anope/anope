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

class CommandNSAList : public Command
{
 public:
	CommandNSAList(Module *creator) : Command(creator, "nickserv/alist", 0, 2)
	{
		this->SetDesc(_("List channels you have access on"));
		this->SetSyntax(_("[\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string nick = u->Account()->display;

		if (params.size() && u->IsServicesOper())
			nick = params[0];

		NickAlias *na = findnick(nick);

		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}
		
		ListFormatter list;
		int chan_count = 0;

		list.addColumn("Number").addColumn("Channel").addColumn("Access");

		source.Reply(_("Channels that \002%s\002 has access on:"), na->nick.c_str());

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			ChannelInfo *ci = it->second;
			ListFormatter::ListEntry entry;

			if (ci->GetFounder() && ci->GetFounder() == na->nc)
			{
				++chan_count;
				entry["Number"] = stringify(chan_count);
				entry["Channel"] = (ci->HasFlag(CI_NO_EXPIRE) ? "!" : "") + ci->name;
				entry["Access"] = "Founder";
				list.addEntry(entry);
				continue;
			}

			AccessGroup access = ci->AccessFor(na->nc);
			if (access.empty())
				continue;
				
			++chan_count;

			entry["Number"] = stringify(chan_count);
			entry["Channel"] = (ci->HasFlag(CI_NO_EXPIRE) ? "!" : "") + ci->name;
			for (unsigned i = 0; i < access.size(); ++i)
				entry["Access"] = entry["Access"] + ", " + access[i]->Serialize();
			entry["Access"] = entry["Access"].substr(2);
			list.addEntry(entry);
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
	NSAList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsalist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSAList)
