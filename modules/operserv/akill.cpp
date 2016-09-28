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

class CommandOSAKill : public Command
{
	ServiceReference<XLineManager> akills;
	
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string expiry, mask;

		if (params.size() < 2)
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		spacesepstream sep(params[1]);
		sep.GetToken(mask);

		if (mask[0] == '+')
		{
			expiry = mask;
			sep.GetToken(mask);
		}

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : Config->GetModule("operserv")->Get<time_t>("autokillexpiry", "30d");
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

		if (sep.StreamEnd())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		Anope::string reason;
		if (mask.find('#') != Anope::string::npos)
		{
			Anope::string remaining = sep.GetRemaining();

			size_t co = remaining[0] == ':' ? 0 : remaining.rfind(" :");
			if (co == Anope::string::npos)
			{
				this->OnSyntaxError(source, "ADD");
				return;
			}

			if (co != 0)
				++co;

			reason = remaining.substr(co + 1);
			mask += " " + remaining.substr(0, co);
			mask.trim();
		}
		else
			reason = sep.GetRemaining();

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
				source.Reply("%s", ex.what());
				return;
			}
		}

		User *targ = User::Find(mask, true);
		if (targ)
			mask = "*@" + targ->host;

		if (Config->GetModule("operserv")->Get<bool>("addakiller", "yes") && !source.GetNick().empty())
			reason = "[" + source.GetNick() + "] " + reason;

		if (mask.find_first_not_of("/~@.*?") == Anope::string::npos)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; Please use a more specific mask."), mask);
			return;
		}

		if (mask.find('@') == Anope::string::npos)
		{
			source.Reply(_("Mask must be in the form \037user\037@\037host\037."));
			return;
		}

		if (!akills->CanAdd(source, mask, expires, reason))
			return;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(source.GetNick());
		x->SetExpires(expires);
		x->SetReason(reason);

		if (Config->GetModule("operserv")->Get<bool>("akillids"))
			x->SetID(XLineManager::GenerateUID());

		unsigned int affected = 0;
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			if (akills->Check(it->second, x))
				++affected;
		float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

		if (percent > 95)
		{
			source.Reply(_("\002{0}\002 coverage is too wide; Please use a more specific mask."), mask);
			Log(LOG_ADMIN, source, this) << "tried to akill " << percent << "% of the network (" << affected << " users)";
			delete x;
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::AddXLine::OnAddXLine, source, x, akills);
		if (MOD_RESULT == EVENT_STOP)
		{
			delete x;
			return;
		}

		akills->AddXLine(x);
		if (Config->GetModule("operserv")->Get<bool>("akillonadd"))
			akills->Send(NULL, x);

		source.Reply(_("\002{0}\002 added to the akill list."), mask);

		Log(LOG_ADMIN, source, this) << "on " << mask << " (" << x->GetReason() << "), expires in " << (expires ? Anope::Duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (akills->GetXLines().empty())
		{
			source.Reply(_("The akill list is empty."));
			return;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			unsigned int deleted = 0;

			NumberList(mask, true,
				[&](unsigned int number)
				{
					XLine *x = akills->GetEntry(number - 1);

					if (!x)
						return;

					Log(LOG_ADMIN, source, this) << "to remove " << x->GetMask() << " from the list";

					++deleted;
					x->Delete();
				},
				[&]()
				{
					if (!deleted)
						source.Reply(_("No matching entries on the akill list."));
					else if (deleted == 1)
						source.Reply(_("Deleted \0021\002 entry from the akill list."));
					else
						source.Reply(_("Deleted \002{0}\002 entries from the akill list."), deleted);
				});
		}
		else
		{
			XLine *x = akills->HasEntry(mask);

			if (!x)
			{
				source.Reply(_("\002{0}\002 was not found on the akill list."), mask);
				return;
			}

			do
			{
				EventManager::Get()->Dispatch(&Event::DelXLine::OnDelXLine, source, x, akills);

				Log(LOG_ADMIN, source, this) << "to remove " << x->GetMask() << " from the list";
				source.Reply(_("\002{0}\002 deleted from the akill list."), x->GetMask());
				x->Delete();
			}
			while ((x = akills->HasEntry(mask)));

		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

	void ProcessList(CommandSource &source, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(mask, false,
				[&](unsigned int number)
				{
					XLine *x = akills->GetEntry(number - 1);

					if (!x)
						return;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Mask"] = x->GetMask();
					entry["Creator"] = x->GetBy();
					entry["Created"] = Anope::strftime(x->GetCreated(), NULL, true);
					entry["Expires"] = Anope::Expires(x->GetExpires(), source.nc);
					entry["Reason"] = x->GetReason();
					list.AddEntry(entry);
				},
				[&]{});
		}
		else
		{
			unsigned int i = 0;
			for (XLine *x : akills->GetXLines())
			{
				if (mask.empty() || mask.equals_ci(x->GetMask()) || mask == x->GetID() || Anope::Match(x->GetMask(), mask, false, true))
				{
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(++i);
					entry["Mask"] = x->GetMask();
					entry["Creator"] = x->GetBy();
					entry["Created"] = Anope::strftime(x->GetCreated(), NULL, true);
					entry["Expires"] = Anope::Expires(x->GetExpires(), source.nc);
					entry["Reason"] = x->GetReason();
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("There are no matching entries on the akill list."));
			return;
		}

		source.Reply(_("Current akill list:"));

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of akill list."));
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (akills->GetXLines().empty())
		{
			source.Reply(_("The akill list is empty."));
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Reason"));

		this->ProcessList(source, params, list);
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (akills->GetXLines().empty())
		{
			source.Reply(_("The akill list is empty."));
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Expires")).AddColumn(_("Reason"));

		this->ProcessList(source, params, list);
	}

	void DoClear(CommandSource &source)
	{
		for (XLine *x : akills->GetXLines())
		{
			EventManager::Get()->Dispatch(&Event::DelXLine::OnDelXLine, source, x, akills);
			x->Delete();
		}

		Log(LOG_ADMIN, source, this) << "to CLEAR the list";
		source.Reply(_("The akill list has been cleared."));

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}
 public:
	CommandOSAKill(Module *creator) : Command(creator, "operserv/akill", 1, 2)
		, akills("xlinemanager/sgline")
	{
		this->SetDesc(_("Manipulate the AKILL list"));
		this->SetSyntax(_("ADD [+\037expiry\037] \037mask\037 \037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037entry-num\037 | \037list\037 | \037id\037}"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037 | \037id\037]"));
		this->SetSyntax("CLEAR");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		if (!akills)
			return;

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("The \002{0} ADD\002 command adds \037mask\037 to the auto kill list with the given \037reason\037."
			               "\037mask\037 can be in the format of nickname!username@hostname#realname."
			               " If a real name is specified, the reason must be prefixed with a :."
			               " \037expiry\037 is optional, and if specified must be an integer followed by one of \037d\037 (days), \037h\037 (hours), or \037m\037 (minutes)."
			               " If a unit specifier is not included, the default is days, so \037+30\037 means 30 days."
			               " To add an auto kill which does not expire, use \037+0\037. "
			               " The default auto kill expiry time is \002{1}\002"),
			               source.command, Anope::Duration(Config->GetModule("operserv")->Get<time_t>("autokillexpiry", "30d"), source.GetAccount()));

			const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
			if (!regexengine.empty())
			{
				source.Reply(" ");
				source.Reply(_("Regex matches are also supported using the %s engine. Enclose your mask in // if this is desired."), regexengine);
			}
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes the given \037mask\037 from the auto kill list, if present."
			               " If a list of entry numbers is given, those entries are deleted."),
			               source.command);
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 commands display the auto kill list."
			               " If a wildcard \037mask\037 is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the auto kill, when it was created, and when it expires.\n"
			               "\n"
			               "Example:\n"
			               "\n"
			               "         {0} LIST 2-5,7-9\n"
			               "         Lists auto kill entries numbered 2 through 5 and 7 through 9."),
			               source.command);
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command removes all auto kills from the auto kill list."),
			               source.command);
		else
		{
			CommandInfo *help = source.service->FindCommand("generic/help");
			source.Reply(_("Allows manipulating the AKILL list."
			               " If a user matching an AKILL mask connects, services will issue a kill the user and prevent them from reconnecting.\n"
			               "\n"
			               "The \002ADD\002 command adds \037mask\037 to the auto kill list.\n"
			               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
			               "\n"
			               "The \002DEL\002 command removes \037mask\037 from the auto kill list.\n"
			               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
			               "\n"
			               "The \002LIST\002 and \002VIEW\002 commands both show the auto kill list, but \002VIEW\002 also shows who created the auto kill entry, when it was created, and when it expires.\n"
			               "\002{msg}{service} {help} {command} [LIST | VIEW]\002 for more information.\n"
			               "\n"
			               "The \002CLEAR\002 command clears th auto kill list."
			               "\002{msg}{service} {help} {command} CLEAR\002 for more information.\n"),
			               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "help"_kw = help->cname, "command"_kw = source.command);
		}
		return true;
	}
};

class OSAKill : public Module
{
	CommandOSAKill commandosakill;

 public:
	OSAKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosakill(this)
	{

	}
};

MODULE_INIT(OSAKill)
