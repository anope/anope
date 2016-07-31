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

class CommandOSSXLineBase : public Command
{
 private:
 	virtual XLineManager* xlm() anope_abstract;

	virtual void OnAdd(CommandSource &source, const std::vector<Anope::string> &params) anope_abstract;

	void OnDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!this->xlm() || this->xlm()->GetXLines().empty())
		{
			source.Reply(_("{0} list is empty."), source.command);
			return;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			unsigned int deleted = 0;

			NumberList(mask, true,
				[&](unsigned int number)
				{
					XLine *x = this->xlm()->GetEntry(number - 1);

					if (!x)
						return;

					Log(LOG_ADMIN, source, this) << "to remove " << x->GetMask() << " from the list";

					++deleted;
					x->Delete();
				},
				[&]()
				{
					if (!deleted)
						source.Reply(_("No matching entries on the {0} list."), source.command);
					else if (deleted == 1)
						source.Reply(_("Deleted \0021\002 entry from the {0} list."), source.command);
					else
						source.Reply(_("Deleted \002{0}\002 entries from the {1} list."), deleted, source.command);
				});
		}
		else
		{
			XLine *x = this->xlm()->HasEntry(mask);

			if (!x)
			{
				source.Reply(_("\002{0}\002 not found on the {1] list."), mask, source.command);
				return;
			}

			EventManager::Get()->Dispatch(&Event::DelXLine::OnDelXLine, source, x, this->xlm());

			x->Delete();
			source.Reply(_("\002{0}\002 deleted from the {1} list."), mask, source.command);
			Log(LOG_ADMIN, source, this) << "to remove " << mask << " from the list";
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

	void ProcessList(CommandSource &source, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		if (!this->xlm() || this->xlm()->GetXLines().empty())
		{
			source.Reply(_("{0} list is empty."), source.command);
			return;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(mask, false,
				[&](unsigned int number)
				{
					XLine *x = this->xlm()->GetEntry(number - 1);

					if (!x)
						return;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Mask"] = x->GetMask();
					entry["By"] = x->GetBy();
					entry["Created"] = Anope::strftime(x->GetCreated(), NULL, true);
					entry["Expires"] = Anope::Expires(x->GetExpires(), source.nc);
					entry["Reason"] = x->GetReason();
					list.AddEntry(entry);
				},
				[]{});
		}
		else
		{
			unsigned int i = 0;
			for (XLine *x : this->xlm()->GetXLines())
			{
				if (mask.empty() || mask.equals_ci(x->GetMask()) || mask == x->GetID() || Anope::Match(x->GetMask(), mask, false, true))
				{
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(i + 1);
					entry["Mask"] = x->GetMask();
					entry["By"] = x->GetBy();
					entry["Created"] = Anope::strftime(x->GetCreated(), NULL, true);
					entry["Expires"] = Anope::Expires(x->GetExpires(), source.nc);
					entry["Reason"] = x->GetReason();
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on the {0} list."), source.command);
		else
		{
			source.Reply(_("{0} list:"), source.command);

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void OnList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Reason"));

		this->ProcessList(source, params, list);
	}

	void OnView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("By")).AddColumn(_("Created")).AddColumn(_("Expires")).AddColumn(_("Reason"));
		this->ProcessList(source, params, list);
	}

	void OnClear(CommandSource &source)
	{
		EventManager::Get()->Dispatch(&Event::DelXLine::OnDelXLine, source, nullptr, this->xlm());

		for (XLine *x : this->xlm()->GetXLines())
			x->Delete();

		Log(LOG_ADMIN, source, this) << "to CLEAR the list";
		source.Reply(_("The {0} list has been cleared."), source.command);
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}
 public:
	CommandOSSXLineBase(Module *creator, const Anope::string &cmd) : Command(creator, cmd, 1, 4)
	{
	}

	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Manipulate the %s list")), source.command.upper().c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->OnAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->OnDel(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->OnList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->OnView(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->OnClear(source);
		else
			this->OnSyntaxError(source, "");
	}

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) override = 0;
};

class CommandOSSNLine : public CommandOSSXLineBase
{
	XLineManager *xlm() override
	{
		return this->snlines;
	}

	void OnAdd(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!this->xlm())
			return;

		unsigned last_param = 2;
		Anope::string param, expiry;

		param = params.size() > 1 ? params[1] : "";
		if (!param.empty() && param[0] == '+')
		{
			expiry = param;
			param = params.size() > 2 ? params[2] : "";
			last_param = 3;
		}

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : Config->GetModule("operserv")->Get<time_t>("snlineexpiry", "30d");
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
			return;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (param.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		Anope::string rest = param;
		if (params.size() > last_param)
			rest += " " + params[last_param];

		if (rest.find(':') == Anope::string::npos)
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		sepstream sep(rest, ':');
		Anope::string mask;
		sep.GetToken(mask);
		Anope::string reason = sep.GetRemaining();

		if (mask.empty() || reason.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (mask[0] == '/' && mask[mask.length() - 1] == '/')
		{
			if (!Config->regex_flags)
			{
				source.Reply(_("Regex is disabled."));
				return;
			}

			Anope::string stripped_mask = mask.substr(1, mask.length() - 2);
			try
			{
				std::regex(stripped_mask.str(), Config->regex_flags);
			}
			catch (const std::regex_error &ex)
			{
				source.Reply(ex.what());
				return;
			}
		}

		/* Clean up the last character of the mask if it is a space
		 * See bug #761
		 */
		unsigned masklen = mask.length();
		if (mask[masklen - 1] == ' ')
			mask.erase(masklen - 1);

		if (Config->GetModule("operserv")->Get<bool>("addakiller", "yes") && !source.GetNick().empty())
			reason = "[" + source.GetNick() + "] " + reason;

		if (!this->xlm()->CanAdd(source, mask, expires, reason))
			return;
		else if (mask.find_first_not_of("/.*?") == Anope::string::npos)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; please use a more specific mask."), mask);
			return;
		}

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(source.GetNick());
		x->SetExpires(expires);
		x->SetReason(reason);

		if (Config->GetModule("operserv")->Get<bool>("akillids"))
			x->SetID(XLineManager::GenerateUID());

		unsigned int affected = 0;
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			if (this->xlm()->Check(it->second, x))
				++affected;
		float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

		if (percent > 95)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; please use a more specific mask."), mask);
			Log(LOG_ADMIN, source, this) << "tried to " << source.command << " " << percent << "% of the network (" << affected << " users)";
			delete x;
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::AddXLine::OnAddXLine, source, x, this->xlm());
		if (MOD_RESULT == EVENT_STOP)
		{
			delete x;
			return;
		}

		this->xlm()->AddXLine(x);

		if (Config->GetModule("operserv")->Get<bool>("killonsnline", "yes"))
		{
			Anope::string rreason = "G-Lined: " + reason;

			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *user = it->second;

				if (!user->HasMode("OPER") && user->server != Me && this->xlm()->Check(user, x))
					user->Kill(Me, rreason);
			}

			this->xlm()->Send(NULL, x);
		}

		source.Reply(_("\002{0}\002 added to the {1} list."), mask, source.command);
		Log(LOG_ADMIN, source, this) << "on " << mask << " (" << reason << "), expires in " << (expires ? Anope::Duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

	ServiceReference<XLineManager> snlines;
 public:
 	CommandOSSNLine(Module *creator) : CommandOSSXLineBase(creator, "operserv/snline")
		, snlines("xlinemanager/snline")
	{
		this->SetSyntax(_("ADD [+\037expiry\037] \037mask\037:\037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037entry-num\037 | \037list\037 | \037id\037}"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax("CLEAR");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("\002{0} ADD\002 adds the given \037mask\037 to the {0} list for the given \037reason\037."
			               " \037expiry\037 is specified as an integer followed by one of \037d\037 (days), \037h\037 (hours), or \037m\037 (minutes)."
			               " If a unit specifier is not included, the default is days, so \037+30\037 by itself means 30 days."
			               " To add a {0} which does not expire, use \037+0\037."
			               " The default {0} expiry time is \002{1}\002."
			               " Because the real name may contain spaces, the separator between it and the reason is a \002colon\002."),
			               source.command, Anope::Duration(Config->GetModule("operserv")->Get<time_t>("snlineexpiry", "30d"), source.GetAccount()));

			const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
			if (!regexengine.empty())
			{
				source.Reply(" ");
				source.Reply(_("Regex matches are also supported using the {0} engine. Enclose your mask in // if this is desired."), regexengine);
			}
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes the given \037mask\037 from the {0} list if it is present."
			               " If a list of entry numbers is given, those entries are deleted."));
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 commands displays the {0} list.\n"
			               "If a wildcard \037mask\037 is given, only those entries matching the \037mask\037 are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the {0}, when it was created, and when it expires.\n"
			               "\n"
			               "Example:\n"
			               "\n"
			               "         {0} LIST 2-5,7-9\n"
			               "         Lists {0} entries numbered 2 through 5 and 7 through 9.\n"),
			               source.command);
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("\002{0} CLEAR\002 removes all entries from the {0} list."),
			               source.command);
		else
		{
			CommandInfo *help = source.service->FindCommand("generic/help");

			source.Reply(_("Allows you to manipulate the {0} list."
			               " If a user attempts to use a realname that matches a {0} mask, services will kill the user."
			               "\n"
			               "The \002ADD\002 command adds \037mask\037 to the {0} list.\n"
			               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
			               "\n"
			               "The \002DEL\002 command removes \037mask\037 from the {0} list.\n"
			               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
			               "\n"
			               "The \002LIST\002 and \002VIEW\002 commands both show the {0} list, but \002VIEW\002 also shows who created the {0} entry, when it was created, and when it expires.\n"
			               "\002{msg}{service} {help} {command} [LIST | VIEW]\002 for more information.\n"
			               "\n"
			               "The \002CLEAR\002 command clears th auto kill list."
			               "\002{msg}{service} {help} {command} CLEAR\002 for more information.\n"),
			               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "help"_kw = help->cname, "command"_kw = source.command);
		}
		return true;
	}
};

class CommandOSSQLine : public CommandOSSXLineBase
{
	XLineManager *xlm() override
	{
		return this->sqlines;
	}

	void OnAdd(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!this->xlm())
			return;

		unsigned last_param = 2;
		Anope::string expiry, mask;

		mask = params.size() > 1 ? params[1] : "";
		if (!mask.empty() && mask[0] == '+')
		{
			expiry = mask;
			mask = params.size() > 2 ? params[2] : "";
			last_param = 3;
		}

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : Config->GetModule("operserv")->Get<time_t>("sqlineexpiry", "30d");
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
			return;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		Anope::string reason = params[last_param];
		if (last_param == 2 && params.size() > 3)
			reason += " " + params[3];

		if (mask.empty() || reason.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (mask[0] == '/' && mask[mask.length() - 1] == '/')
		{
			if (!Config->regex_flags)
			{
				source.Reply(_("Regex is disabled."));
				return;
			}

			Anope::string stripped_mask = mask.substr(1, mask.length() - 2);
			try
			{
				std::regex(stripped_mask.str(), Config->regex_flags);
			}
			catch (const std::regex_error &ex)
			{
				source.Reply(ex.what());
				return;
			}
		}

		if (Config->GetModule("operserv")->Get<bool>("addakiller", "yes") && !source.GetNick().empty())
			reason = "[" + source.GetNick() + "] " + reason;

		if (!this->sqlines->CanAdd(source, mask, expires, reason))
			return;
		else if (mask.find_first_not_of("./?*") == Anope::string::npos)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; please use a more specific mask."), mask);
			return;
		}

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(source.GetNick());
		x->SetExpires(expires);
		x->SetReason(reason);

		if (Config->GetModule("operserv")->Get<bool>("akillids"))
			x->SetID(XLineManager::GenerateUID());

		unsigned int affected = 0;
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			if (this->xlm()->Check(it->second, x))
				++affected;
		float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

		if (percent > 95)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; please use a more specific mask."), mask);
			Log(LOG_ADMIN, source, this) << "tried to SQLine " << percent << "% of the network (" << affected << " users)";
			delete x;
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::AddXLine::OnAddXLine, source, x, this->xlm());
		if (MOD_RESULT == EVENT_STOP)
		{
			delete x;
			return;
		}

		this->xlm()->AddXLine(x);

		if (Config->GetModule("operserv")->Get<bool>("killonsqline", "yes"))
		{
			Anope::string rreason = "Q-Lined: " + reason;

			if (mask[0] == '#')
			{
				for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
				{
					Channel *c = cit->second;

					if (!Anope::Match(c->name, mask, false, true))
						continue;

					std::vector<User *> users;
					for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
					{
						ChanUserContainer *uc = it->second;
						User *user = uc->user;

						if (!user->HasMode("OPER") && user->server != Me)
							users.push_back(user);
					}

					for (unsigned i = 0; i < users.size(); ++i)
						c->Kick(NULL, users[i], "%s", reason.c_str());
				}
			}
			else
			{
				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
				{
					User *user = it->second;

					if (!user->HasMode("OPER") && user->server != Me && this->xlm()->Check(user, x))
						user->Kill(Me, rreason);
				}
			}

			this->xlm()->Send(NULL, x);
		}

		source.Reply(_("\002{0}\002 added to the {1} list."), mask, source.command);
		Log(LOG_ADMIN, source, this) << "on " << mask << " (" << reason << "), expires in " << (expires ? Anope::Duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

 	ServiceReference<XLineManager> sqlines;
 public:
	CommandOSSQLine(Module *creator) : CommandOSSXLineBase(creator, "operserv/sqline")
		, sqlines("xlinemanager/sqline")
	{
		this->SetSyntax(_("ADD [+\037expiry\037] \037mask\037 \037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037entry-num\037 | \037list\037 | \037id\037}"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax("CLEAR");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("\002{0} ADD\002 adds the given \037mask\037 to the {0} list for the given \037reason\037."
			               " \037expiry\037 is specified as an integer followed by one of \037d\037 (days), \037h\037 (hours), or \037m\037 (minutes)."
			               " If a unit specifier is not included, the default is days, so \037+30\037 by itself means 30 days."
			               " To add a {0} which does not expire, use \037+0\037."
			               " The default {0} expiry time is \002{1}\002."),
			               source.command, Anope::Duration(Config->GetModule("operserv")->Get<time_t>("sqlineexpiry", "30d"), source.GetAccount()));

			const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
			if (!regexengine.empty())
			{
				source.Reply(" ");
				source.Reply(_("Regex matches are also supported using the {0} engine. Enclose your mask in // if this is desired."), regexengine);
			}
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes the given \037mask\037 from the {0} list if it is present."
			               " If a list of entry numbers is given, those entries are deleted."));
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 commands displays the {0} list.\n"
			               "If a wildcard \037mask\037 is given, only those entries matching the \037mask\037 are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the {0}, when it was created, and when it expires.\n"
			               "\n"
			               "Example:\n"
			               "\n"
			               "         {0} LIST 2-5,7-9\n"
			               "         Lists {0} entries numbered 2 through 5 and 7 through 9.\n"),
			               source.command);
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("\002{0} CLEAR\002 removes all entries from the {0} list."),
			               source.command);
		else
		{
			CommandInfo *help = source.service->FindCommand("generic/help");

			source.Reply(_("Allows you to manipulate the {0} list."
			               " If a user attempts to use a nickname that matches a {0} mask, services will force the user off of the nickname."
			               " If the first character of the mask is a #, services will prevent the use of matching channels."
			               "\n"
			               "The \002ADD\002 command adds \037mask\037 to the {0} list.\n"
			               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
			               "\n"
			               "The \002DEL\002 command removes \037mask\037 from the {0} list.\n"
			               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
			               "\n"
			               "The \002LIST\002 and \002VIEW\002 commands both show the {0} list, but \002VIEW\002 also shows who created the {0} entry, when it was created, and when it expires.\n"
			               "\002{msg}{service} {help} {command} [LIST | VIEW]\002 for more information.\n"
			               "\n"
			               "The \002CLEAR\002 command clears th auto kill list."
			               "\002{msg}{service} {help} {command} CLEAR\002 for more information.\n"),
			               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "help"_kw = help->cname, "command"_kw = source.command);
		}
		return true;
	}
};

class OSSXLine : public Module
{
	CommandOSSNLine commandossnline;
	CommandOSSQLine commandossqline;

 public:
	OSSXLine(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandossnline(this), commandossqline(this)
	{
	}
};

MODULE_INIT(OSSXLine)
