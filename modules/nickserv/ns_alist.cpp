/* NickServ core functions
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

class CommandNSAList final
	: public Command
{
public:
	CommandNSAList(Module *creator) : Command(creator, "nickserv/alist", 0, 2)
	{
		this->SetDesc(_("List channels you have access on"));
		this->SetSyntax(_("[\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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

		ListFormatter list(source.GetAccount());
		int chan_count = 0;

		list.AddColumn(_("Number")).AddColumn(_("Channel")).AddColumn(_("Access")).AddColumn(_("Description"));

		std::deque<ChannelInfo *> queue;
		nc->GetChannelReferences(queue);
		std::sort(queue.begin(), queue.end(), [](auto *lhs, auto *rhs) {
			return ci::less()(lhs->name, rhs->name);
		});

		for (auto *ci : queue)
		{
			Anope::string privstr;
			if (ci->GetFounder() == nc)
			{
				privstr = Language::Translate(source.GetAccount(), _("Founder"));
			}
			else if (ci->GetSuccessor() == nc)
			{
				privstr += Language::Translate(source.GetAccount(), _("Successor"));
			}

			AccessGroup access = ci->AccessFor(nc, false);
			if (!access.empty())
			{
				for (auto &p : access.paths)
				{
					// not interested in indirect access
					if (p.size() != 1)
						continue;

					ChanAccess *a = p[0];
					privstr += access.empty() ? "" : ", ";
					privstr += a->AccessSerialize();
				}
			}

			if (privstr.empty())
				continue; // No privs for this channel???

			ListFormatter::ListEntry entry;
			entry["Number"] = Anope::ToString(++chan_count);
			entry["Channel"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
			entry["Access"] = privstr;
			entry["Description"] = ci->desc;
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		if (!chan_count)
		{
			source.Reply(_("\002%s\002 has no access in any channels."), nc->display.c_str());
		}
		else
		{
			source.Reply(_("Channels that \002%s\002 has access on:"), nc->display.c_str());

			for (const auto &reply : replies)
				source.Reply(reply);

			source.Reply(_("End of list - %d channels shown."), chan_count);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Lists all channels you have access on."
			"\n\n"
			"Channels that have the \037NOEXPIRE\037 option set will be "
			"prefixed by an exclamation mark. The nickname parameter is "
			"limited to Services Operators."
		));

		return true;
	}
};

class NSAList final
	: public Module
{
	CommandNSAList commandnsalist;

public:
	NSAList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsalist(this)
	{

	}
};

MODULE_INIT(NSAList)
