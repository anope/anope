/* OperServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList(Module *creator) : Command(creator, "operserv/chanlist", 0, 2)
	{
		this->SetDesc(_("Lists all channel records"));
		this->SetSyntax(_("[{\037pattern\037 | \037nick\037} [\037SECRET\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		std::set<Anope::string> modes;
		User *u2;

		if (!opt.empty() && opt.equals_ci("SECRET"))
		{
			modes.insert("SECRET");
			modes.insert("PRIVATE");
		}

		ListFormatter list;
		list.AddColumn("Name").AddColumn("Users").AddColumn("Modes").AddColumn("Topic");

		if (!pattern.empty() && (u2 = User::Find(pattern, true)))
		{
			source.Reply(_("\002%s\002 channel list:"), u2->nick.c_str());

			for (User::ChanUserList::iterator uit = u2->chans.begin(), uit_end = u2->chans.end(); uit != uit_end; ++uit)
			{
				ChanUserContainer *cc = *uit;

				if (!modes.empty())
					for (std::set<Anope::string>::iterator it = modes.begin(), it_end = modes.end(); it != it_end; ++it)
						if (!cc->chan->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = cc->chan->name;
				entry["Users"] = stringify(cc->chan->users.size());
				entry["Modes"] = cc->chan->GetModes(true, true);
				entry["Topic"] = cc->chan->topic;
				list.AddEntry(entry);
			}
		}
		else
		{
			source.Reply(_("Channel list:"));

			for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
			{
				Channel *c = cit->second;

				if (!pattern.empty() && !Anope::Match(c->name, pattern, false, true))
					continue;
				if (!modes.empty())
					for (std::set<Anope::string>::iterator it = modes.begin(), it_end = modes.end(); it != it_end; ++it)
						if (!c->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = c->name;
				entry["Users"] = stringify(c->users.size());
				entry["Modes"] = c->GetModes(true, true);
				entry["Topic"] = c->topic;
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of channel list."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all channels currently in use on the IRC network, whether they\n"
				"are registered or not.\n"
				"If \002pattern\002 is given, lists only channels that match it. If a nickname\n"
				"is given, lists only the channels the user using it is on. If SECRET is\n"
				"specified, lists only channels matching \002pattern\002 that have the +s or\n"
				"+p mode."));

		if (!Config->RegexEngine.empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this is desired."), Config->RegexEngine.c_str());
		}

		return true;
	}
};

class CommandOSUserList : public Command
{
 public:
	CommandOSUserList(Module *creator) : Command(creator, "operserv/userlist", 0, 2)
	{
		this->SetDesc(_("Lists all user records"));
		this->SetSyntax(_("[{\037pattern\037 | \037channel\037} [\037INVISIBLE\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		Channel *c;
		std::set<Anope::string> modes;

		if (!opt.empty() && opt.equals_ci("INVISIBLE"))
			modes.insert("INVIS");

		ListFormatter list;
		list.AddColumn("Name").AddColumn("Mask");

		if (!pattern.empty() && (c = Channel::Find(pattern)))
		{
			source.Reply(_("\002%s\002 users list:"), pattern.c_str());

			for (Channel::ChanUserList::iterator cuit = c->users.begin(), cuit_end = c->users.end(); cuit != cuit_end; ++cuit)
			{
				ChanUserContainer *uc = *cuit;

				if (!modes.empty())
					for (std::set<Anope::string>::iterator it = modes.begin(), it_end = modes.end(); it != it_end; ++it)
						if (!uc->user->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = uc->user->nick;
				entry["Mask"] = uc->user->GetIdent() + "@" + uc->user->GetDisplayedHost();
				list.AddEntry(entry);
			}
		}
		else
		{
			/* Historically this has been ordered, so... */
			Anope::map<User *> ordered_map;
			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
				ordered_map[it->first] = it->second;

			source.Reply(_("Users list:"));

			for (Anope::map<User *>::const_iterator it = ordered_map.begin(); it != ordered_map.end(); ++it)
			{
				User *u2 = it->second;

				if (!pattern.empty())
				{
					Anope::string mask = u2->nick + "!" + u2->GetIdent() + "@" + u2->GetDisplayedHost(), mask2 = u2->nick + "!" + u2->GetIdent() + "@" + u2->host, mask3 = u2->nick + "!" + u2->GetIdent() + "@" + (!u2->ip.empty() ? u2->ip : u2->host);
					if (!Anope::Match(mask, pattern) && !Anope::Match(mask2, pattern) && !Anope::Match(mask3, pattern))
						continue;
					if (!modes.empty())
						for (std::set<Anope::string>::iterator mit = modes.begin(), mit_end = modes.end(); mit != mit_end; ++mit)
							if (!u2->HasMode(*mit))
								continue;
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = u2->nick;
				entry["Mask"] = u2->GetIdent() + "@" + u2->GetDisplayedHost();
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of users list."));
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all users currently online on the IRC network, whether their\n"
				"nick is registered or not.\n"
				" \n"
				"If \002pattern\002 is given, lists only users that match it (it must be in\n"
				"the format nick!user@host). If \002channel\002 is given, lists only users\n"
				"that are on the given channel. If INVISIBLE is specified, only users\n"
				"with the +i flag will be listed."));

		if (!Config->RegexEngine.empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this is desired."), Config->RegexEngine.c_str());
		}

		return true;
	}
};

class OSList : public Module
{
	CommandOSChanList commandoschanlist;
	CommandOSUserList commandosuserlist;

 public:
	OSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandoschanlist(this), commandosuserlist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(OSList)
