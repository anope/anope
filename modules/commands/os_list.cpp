/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
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
		std::list<ChannelModeName> Modes;
		User *u2;

		if (!opt.empty() && opt.equals_ci("SECRET"))
		{
			Modes.push_back(CMODE_SECRET);
			Modes.push_back(CMODE_PRIVATE);
		}

		ListFormatter list;
		list.addColumn("Name").addColumn("Users").addColumn("Modes").addColumn("Topic");

		if (!pattern.empty() && (u2 = finduser(pattern)))
		{
			source.Reply(_("\002%s\002 channel list:"), u2->nick.c_str());

			for (UChannelList::iterator uit = u2->chans.begin(), uit_end = u2->chans.end(); uit != uit_end; ++uit)
			{
				ChannelContainer *cc = *uit;

				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!cc->chan->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = cc->chan->name;
				entry["Users"] = stringify(cc->chan->users.size());
				entry["Modes"] = cc->chan->GetModes(true, true);
				entry["Topic"] = cc->chan->topic;
				list.addEntry(entry);
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
				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!c->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = c->name;
				entry["Users"] = stringify(c->users.size());
				entry["Modes"] = c->GetModes(true, true);
				entry["Topic"] = c->topic;
				list.addEntry(entry);
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
			source.Reply(" \n"
					"Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this desired.", Config->RegexEngine.c_str());
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
		std::list<UserModeName> Modes;

		if (!opt.empty() && opt.equals_ci("INVISIBLE"))
			Modes.push_back(UMODE_INVIS);

		ListFormatter list;
		list.addColumn("Name").addColumn("Mask");

		if (!pattern.empty() && (c = findchan(pattern)))
		{
			source.Reply(_("\002%s\002 users list:"), pattern.c_str());

			for (CUserList::iterator cuit = c->users.begin(), cuit_end = c->users.end(); cuit != cuit_end; ++cuit)
			{
				UserContainer *uc = *cuit;

				if (!Modes.empty())
					for (std::list<UserModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!uc->user->HasMode(*it))
							continue;

				ListFormatter::ListEntry entry;
				entry["Name"] = uc->user->nick;
				entry["Mask"] = uc->user->GetIdent() + "@" + uc->user->GetDisplayedHost();
				list.addEntry(entry);
			}
		}
		else
		{
			source.Reply(_("Users list:"));

			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u2 = it->second;

				if (!pattern.empty())
				{
					Anope::string mask = u2->nick + "!" + u2->GetIdent() + "@" + u2->GetDisplayedHost(), mask2 = u2->nick + "!" + u2->GetIdent() + "@" + u2->host, mask3 = u2->nick + "!" + u2->GetIdent() + "@" + (!u2->ip.empty() ? u2->ip : u2->host);
					if (!Anope::Match(mask, pattern) && !Anope::Match(mask2, pattern) && !Anope::Match(mask3, pattern))
						continue;
					if (!Modes.empty())
						for (std::list<UserModeName>::iterator mit = Modes.begin(), mit_end = Modes.end(); mit != mit_end; ++mit)
							if (!u2->HasMode(*mit))
								continue;
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = u2->nick;
				entry["Mask"] = u2->GetIdent() + "@" + u2->GetDisplayedHost();
				list.addEntry(entry);
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
			source.Reply(" \n"
					"Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this desired.", Config->RegexEngine.c_str());
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
