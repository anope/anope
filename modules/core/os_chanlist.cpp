/* OperServ core functions
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
#include "operserv.h"

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList() : Command("CHANLIST", 0, 2)
	{
		this->SetDesc(_("Lists all channel records"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		std::list<ChannelModeName> Modes;
		User *u2;

		if (!opt.empty() && opt.equals_ci("SECRET"))
		{
			Modes.push_back(CMODE_SECRET);
			Modes.push_back(CMODE_PRIVATE);
		}

		if (!pattern.empty() && (u2 = finduser(pattern)))
		{
			source.Reply(_("\002%s\002 channel list:\n"
					"Name                 Users Modes   Topic"), u2->nick.c_str());

			for (UChannelList::iterator uit = u2->chans.begin(), uit_end = u2->chans.end(); uit != uit_end; ++uit)
			{
				ChannelContainer *cc = *uit;

				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!cc->chan->HasMode(*it))
							continue;

				source.Reply(_("%-20s  %4d +%-6s %s"), cc->chan->name.c_str(), cc->chan->users.size(), cc->chan->GetModes(true, true).c_str(), !cc->chan->topic.empty() ? cc->chan->topic.c_str() : "");
			}
		}
		else
		{
			source.Reply(_("Channel list:\n"
					"Name                 Users Modes   Topic"));

			for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
			{
				Channel *c = cit->second;

				if (!pattern.empty() && !Anope::Match(c->name, pattern))
					continue;
				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!c->HasMode(*it))
							continue;

				source.Reply(_("%-20s  %4d +%-6s %s"), c->name.c_str(), c->users.size(), c->GetModes(true, true).c_str(), !c->topic.empty() ? c->topic.c_str() : "");
			}
		}

		source.Reply(_("End of channel list."));
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002CHANLIST [{\037pattern\037 | \037nick\037} [\037SECRET\037]]\002\n"
				" \n"
				"Lists all channels currently in use on the IRC network, whether they\n"
				"are registered or not.\n"
				"If \002pattern\002 is given, lists only channels that match it. If a nickname\n"
				"is given, lists only the channels the user using it is on. If SECRET is\n"
				"specified, lists only channels matching \002pattern\002 that have the +s or\n"
				"+p mode."));
		return true;
	}
};

class OSChanList : public Module
{
	CommandOSChanList commandoschanlist;

 public:
	OSChanList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
	 	this->SetType(CORE);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandoschanlist);
	}
};

MODULE_INIT(OSChanList)
