// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandNSList final
	: public Command
{
public:
	CommandNSList(Module *creator) : Command(creator, "nickserv/list", 1, 2)
	{
		this->SetDesc(_("List all registered nicknames that match a given pattern"));
		this->SetSyntax(_("\037pattern\037 [DISPLAY] [NOEXPIRE] [SUSPENDED] [UNCONFIRMED]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		auto from = 0u, to = 0u;
		auto pattern = params[0];
		if (pattern[0] == '#')
		{
			Anope::string n1, n2;
			sepstream(pattern.substr(1), '-').GetToken(n1, 0);
			sepstream(pattern, '-').GetToken(n2, 1);

			auto num1 = Anope::TryConvert<int>(n1);
			auto num2 = Anope::TryConvert<int>(n2);
			if (!num1.has_value() || !num2.has_value())
			{
				source.Reply(LIST_INCORRECT_RANGE);
				return;
			}

			from = num1.value();
			to = num2.value();
			pattern = "*";
		}

		auto display = false, nsnoexpire = false, suspended = false, unconfirmed = false;
		const auto is_servadmin = source.HasCommand("nickserv/list");
		if (is_servadmin && params.size() > 1)
		{
			Anope::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetToken(keyword))
			{
				if (keyword.equals_ci("DISPLAY"))
					display = true;
				else if (keyword.equals_ci("NOEXPIRE"))
					nsnoexpire = true;
				else if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				else if (keyword.equals_ci("UNCONFIRMED"))
					unconfirmed = true;
			}
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Nick")).AddColumn(_("Last mask"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Last mask"].empty()
				? _("\002{nick}\002")
				: _("\002{nick}\002 (last mask: {last_mask})");
		});

		Anope::map<NickAlias *> ordered_map;
		for (const auto &[nick, na] : *NickAliasList)
			ordered_map[nick] = na;

		const auto listmax = Config->GetModule(this->owner).Get<unsigned>("listmax", "50");
		const auto *mync = source.GetAccount();
		auto count = 0u, nnicks = 0u;
		for (const auto &[_, na] : ordered_map)
		{
			/* Don't show private nicks to non-services admins. */
			if (na->nc->HasExt("NS_PRIVATE") && !is_servadmin && na->nc != mync)
				continue;
			else if (display && na->nc->na != na)
				continue;
			else if (nsnoexpire && !na->HasExt("NS_NO_EXPIRE"))
				continue;
			else if (suspended && !na->nc->HasExt("NS_SUSPENDED"))
				continue;
			else if (unconfirmed && !na->nc->HasExt("UNCONFIRMED"))
				continue;

			/* We no longer compare the pattern against the output buffer.
			 * Instead we build a nice nick!user@host buffer to compare.
			 * The output is then generated separately. -TheShadow */
			Anope::string buf = Anope::Format("%s!%s", na->nick.c_str(), !na->last_userhost.empty() ? na->last_userhost.c_str() : "*@*");
			if (na->nick.equals_ci(pattern) || Anope::Match(buf, pattern, false, true))
			{
				if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nnicks <= listmax)
				{
					bool isnoexpire = false;
					if (is_servadmin && na->HasExt("NS_NO_EXPIRE"))
						isnoexpire = true;

					ListFormatter::ListEntry entry;
					entry["Nick"] = (isnoexpire ? "!" : "") + na->nick;
					if (na->nc->HasExt("HIDE_MASK") && !is_servadmin && na->nc != mync)
						entry["Last mask"] = Language::Translate(source.GetAccount(), _("[Hostname hidden]"));
					else if (na->nc->HasExt("NS_SUSPENDED"))
						entry["Last mask"] = Language::Translate(source.GetAccount(), _("[Suspended]"));
					else if (na->nc->HasExt("UNCONFIRMED"))
						entry["Last mask"] = Language::Translate(source.GetAccount(), _("[Unconfirmed]"));
					else
						entry["Last mask"] = is_servadmin ? na->last_userhost_real : na->last_userhost;
					list.AddEntry(entry);
				}
				++count;
			}
		}

		source.Reply(_("List of entries matching \002%s\002:"), pattern.c_str());
		list.SendTo(source);
		source.Reply(_("End of list - %d/%d matches shown."), nnicks > listmax ? listmax : nnicks, nnicks);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Lists all registered nicknames which match the given "
			"pattern, in \037nick!user@host\037 format. Nicks with the \002PRIVATE\002 "
			"option set will only be displayed to Services Operators with the "
			"proper access. Nicks with the \002NOEXPIRE\002 option set will have "
			"a \002!\002 prefixed to the nickname for Services Operators to see."
			"\n\n"
			"Note that a preceding '#' specifies a range."
			"\n\n"
			"If the DISPLAY, NOEXPIRE, SUSPENDED, or UNCONFIRMED options are given "
			"only nicks which, respectively, are display nicks, will not expire, are "
			"suspended, or are unconfirmed will be shown. If multiple options are "
			"given, nicks must match every option to be shown. "
			"Note that these options are limited to \037Services Operators\037."
			"\n\n"
			"Examples:"
			"\n\n"
			"    \002LIST *!joeuser@foo.com\002\n"
			"        Lists all registered nicks owned by joeuser@foo.com."
			"\n\n"
			"    \002LIST *Bot*!*@*\002\n"
			"        Lists all registered nicks with \002Bot\002 in their\n"
			"        names (case insensitive)."
			"\n\n"
			"    \002LIST * NOEXPIRE\002\n"
			"        Lists all registered nicks which have been set to not expire."
			"\n\n"
			"    \002LIST #51-100\002\n"
			"        Lists all registered nicks within the given range (51-100)."
		));

		const Anope::string &regexengine = Config->GetBlock("options").Get<const Anope::string>("regexengine");
		if (!regexengine.empty())
		{
			source.Reply(" ");
			source.Reply(_(
					"Regex matches are also supported using the %s engine. "
					"Enclose your pattern in // if this is desired."
				),
				regexengine.c_str());
		}

		return true;
	}
};


class CommandNSSetPrivate
	: public Command
{
public:
	CommandNSSetPrivate(Module *creator, const Anope::string &sname = "nickserv/set/private", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Prevent the nickname from appearing in the LIST command"));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable private for " << nc->display;
			nc->Extend<bool>("NS_PRIVATE");
			source.Reply(_("Private option is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable private for " << nc->display;
			nc->Shrink<bool>("NS_PRIVATE");
			source.Reply(_("Private option is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Turns %s's privacy option on or off for your nick. "
				"With \002PRIVATE\002 set, your nickname will not appear in "
				"nickname lists generated with %s's \002LIST\002 command. "
				"(However, anyone who knows your nickname can still get "
				"information on it using the \002INFO\002 command.)"
			),
			source.service->nick.c_str(),
			source.service->nick.c_str());
		return true;
	}
};

class CommandNSSASetPrivate final
	: public CommandNSSetPrivate
{
public:
	CommandNSSASetPrivate(Module *creator) : CommandNSSetPrivate(creator, "nickserv/saset/private", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Turns %s's privacy option on or off for the nick. "
				"With \002PRIVATE\002 set, the nickname will not appear in "
				"nickname lists generated with %s's \002LIST\002 command. "
				"(However, anyone who knows the nickname can still get "
				"information on it using the \002INFO\002 command.)"
			),
			source.service->nick.c_str(),
			source.service->nick.c_str());
		return true;
	}
};


class NSList final
	: public Module
{
	CommandNSList commandnslist;

	CommandNSSetPrivate commandnssetprivate;
	CommandNSSASetPrivate commandnssasetprivate;

	SerializableExtensibleItem<bool> priv;

public:
	NSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnslist(this), commandnssetprivate(this), commandnssasetprivate(this),
		priv(this, "NS_PRIVATE")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		if (priv.HasExt(na->nc))
			info.AddOption(_("Private"));
	}
};

MODULE_INIT(NSList)
