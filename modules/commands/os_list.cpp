/* OperServ core functions
 *
 * (C) 2003-2023 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList(Module *creator) : Command(creator, "operserv/chanlist", 0, 2)
	{
		this->SetDesc(_("Lists all channel records"));
		this->SetSyntax(_("[{\037pattern\037 | \037nick\037} [\037SECRET\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		std::set<Anope::string> modes;
		User *u2;
		unsigned int count = 0;

		if (!pattern.empty())
			Log(LOG_ADMIN, source, this) << "for " << pattern;
		else
			Log(LOG_ADMIN, source, this);

		if (!opt.empty() && opt.equals_ci("SECRET"))
		{
			modes.insert("SECRET");
			modes.insert("PRIVATE");
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Name")).AddColumn(_("Users")).AddColumn(_("Modes")).AddColumn(_("Topic"));

		if (!pattern.empty() && (u2 = User::Find(pattern, true)))
		{
			source.Reply(_("\002%s\002 channel list:"), u2->nick.c_str());

			for (const auto &[_, cc]: u2->chans)
			{
				if (!modes.empty())
				{
					for (const auto &mode : modes)
					{
						if (!cc->chan->HasMode(mode))
							continue;
					}
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = cc->chan->name;
				entry["Users"] = stringify(cc->chan->users.size());
				entry["Modes"] = cc->chan->GetModes(true, true);
				entry["Topic"] = cc->chan->topic;
				list.AddEntry(entry);

				++count;
			}
		}
		else
		{
			source.Reply(_("Channel list:"));

			for (const auto &[_, c] : ChannelList)
			{
				if (!pattern.empty() && !Anope::Match(c->name, pattern, false, true))
					continue;
				if (!modes.empty())
				{
					for (const auto &mode : modes)
					{
						if (!c->HasMode(mode))
							continue;
					}
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = c->name;
				entry["Users"] = stringify(c->users.size());
				entry["Modes"] = c->GetModes(true, true);
				entry["Topic"] = c->topic;
				list.AddEntry(entry);

				++count;
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const auto &reply : replies)
			source.Reply(reply);

		source.Reply(_("End of channel list. \002%u\002 channels shown."), count);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all channels currently in use on the IRC network, whether they\n"
				"are registered or not.\n"
				" \n"
				"If \002pattern\002 is given, lists only channels that match it. If a nickname\n"
				"is given, lists only the channels the user using it is on. If SECRET is\n"
				"specified, lists only channels matching \002pattern\002 that have the +s or\n"
				"+p mode."));

		const Anope::string &regexengine = Config->GetBlock("options")->Get<const Anope::string>("regexengine");
		if (!regexengine.empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this is desired."), regexengine.c_str());
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		Channel *c;
		std::set<Anope::string> modes;
		unsigned int count = 0;

		if (!pattern.empty())
			Log(LOG_ADMIN, source, this) << "for " << pattern;
		else
			Log(LOG_ADMIN, source, this);

		if (!opt.empty() && opt.equals_ci("INVISIBLE"))
			modes.insert("INVIS");

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Name")).AddColumn(_("Mask")).AddColumn(_("Realname"));

		if (!pattern.empty() && (c = Channel::Find(pattern)))
		{
			source.Reply(_("\002%s\002 users list:"), pattern.c_str());

			for (const auto &[_, uc] : c->users)
			{
				if (!modes.empty())
				{
					for (const auto &mode : modes)
					{
						if (!uc->user->HasMode(mode))
							continue;
					}
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = uc->user->nick;
				entry["Mask"] = uc->user->GetIdent() + "@" + uc->user->GetDisplayedHost();
				entry["Realname"] = uc->user->realname;
				list.AddEntry(entry);

				++count;
			}
		}
		else
		{
			/* Historically this has been ordered, so... */
			Anope::map<User *> ordered_map;
			for (const auto &[nick, user] : UserListByNick)
				ordered_map[nick] = user;

			source.Reply(_("Users list:"));

			for (Anope::map<User *>::const_iterator it = ordered_map.begin(); it != ordered_map.end(); ++it)
			{
				User *u2 = it->second;

				if (!pattern.empty())
				{
					/* check displayed host, host, and ip */
					Anope::string masks[] = {
						u2->nick + "!" + u2->GetIdent() + "@" + u2->GetDisplayedHost(),
						u2->nick + "!" + u2->GetIdent() + "@" + u2->host,
						u2->nick + "!" + u2->GetIdent() + "@" + u2->ip.addr()
					};

					bool match = false;
					for (const auto &mask : masks)
					{
						/* Check mask with realname included, too */
						if (Anope::Match(mask, pattern, false, true) || Anope::Match(mask + "#" + u2->realname, pattern, false, true))
						{
							match = true;
							break;
						}
					}

					if (!match)
						continue;

					if (!modes.empty())
					{
						for (const auto &mode : modes)
						{
							if (!u2->HasMode(mode))
								continue;
						}
					}
				}

				ListFormatter::ListEntry entry;
				entry["Name"] = u2->nick;
				entry["Mask"] = u2->GetIdent() + "@" + u2->GetDisplayedHost();
				entry["Realname"] = u2->realname;
				list.AddEntry(entry);

				++count;
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const auto &reply : replies)
			source.Reply(reply);

		source.Reply(_("End of users list. \002%u\002 users shown."), count);
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all users currently online on the IRC network, whether their\n"
				"nick is registered or not.\n"
				" \n"
				"If \002pattern\002 is given, lists only users that match it (it must be in\n"
				"the format nick!user@host[#realname]). If \002channel\002 is given, lists\n"
				"only users that are on the given channel. If INVISIBLE is specified, only users\n"
				"with the +i flag will be listed."));

		const Anope::string &regexengine = Config->GetBlock("options")->Get<const Anope::string>("regexengine");
		if (!regexengine.empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this is desired."), regexengine.c_str());
		}

		return true;
	}
};

class OSList : public Module
{
	CommandOSChanList commandoschanlist;
	CommandOSUserList commandosuserlist;

 public:
	OSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandoschanlist(this), commandosuserlist(this)
	{

	}
};

MODULE_INIT(OSList)
