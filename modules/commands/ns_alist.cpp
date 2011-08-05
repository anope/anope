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
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else
		{
			int chan_count = 0;

			source.Reply(_("Channels that \002%s\002 has access on:\n"
					"  Num  Channel              Level"), na->nick.c_str());

			for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
			{
				ChannelInfo *ci = it->second;

				AccessGroup access = ci->AccessFor(na->nc);
				if (access.empty())
					continue;
				
				++chan_count;

				if (access.size() > 1)
				{
					source.Reply(_("  %3d You match %d access entries on %c%s, they are"), chan_count, access.size(), ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str());
					for (unsigned i = 0; i < access.size(); ++i)
						source.Reply(_("  %3d %-8s"), i + 1, access[i]->Serialize().c_str());
				}
				else
					source.Reply(_("  %3d %c%-20s %-8s"), chan_count, ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str(), access[0]->Serialize().c_str());
			}

			source.Reply(_("End of list - %d channels shown."), chan_count);
		}
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

		ModuleManager::RegisterService(&commandnsalist);
	}
};

MODULE_INIT(NSAList)
