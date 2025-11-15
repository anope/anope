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


/// BEGIN CMAKE
/// target_compile_features(${SO} PRIVATE "cxx_std_20")
/// END CMAKE

#include <version>
#if __cpp_lib_chrono >= 201907L
# include <chrono>
# define HAS_CXX20_CHRONO
#endif

#include "module.h"

namespace
{
	Anope::map<std::vector<Anope::string>> timeregions;
	std::vector<Anope::string> timezones;
}

class CommandNSSetTimezone
	: public Command
{
protected:
	SerializableExtensibleItem<Anope::string> &timezone;

	bool SendZones(CommandSource &source, const Anope::string &subcommand)
	{
		auto timeregion = timeregions.find(subcommand);
		if (timeregion == timeregions.end())
			return false;

		source.Reply(_("Available timezones in the \002%s\002 region:"),
			timeregion->first.c_str());

		const auto max_length = Config->GetBlock("options").Get<size_t>("linelength", "100");
		Anope::string buffer;
		for (const auto &timezone : timeregion->second)
		{
			if (buffer.length() + 2 + timezone.length() >= max_length)
			{
				source.Reply(buffer);
				buffer.clear();
			}

			buffer.append(buffer.empty() ? "  " : ", ");
			buffer.append(timezone);
		}

		if (!buffer.empty())
			source.Reply(buffer);

		return true;
	}

public:
	CommandNSSetTimezone(Module *creator, SerializableExtensibleItem<Anope::string> &tz, const Anope::string &sname = "nickserv/set/timezone", size_t min = 0)
		: Command(creator, sname, min, min + 1)
		, timezone(tz)
	{
		this->SetDesc(_("Set the timezone services will use when messaging you"));
		this->SetSyntax(_("[\037timezone\037]"));
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

		Anope::string usertz;
		if (param.empty())
		{
			usertz = "UTC";
			timezone.Unset(nc);
		}
		else
		{
			for (const auto &timezone : timezones)
			{
				if (timezone.find_ci(param) != 0)
					continue; // Timezone does not match.

				if (!usertz.empty())
				{
					source.Reply(_("Multiple timezones matched \002%s\002. Please be more specific."), param.c_str());
					return;
				}

				usertz = timezone;
				if (usertz.equals_ci(param))
					break; // Exact match.
			}

			if (usertz.empty())
			{
				this->OnSyntaxError(source, "");
				return;
			}

			timezone.Set(nc, usertz);
		}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the timezone of " << nc->display << " to " << usertz;
		if (source.GetAccount() == nc)
			source.Reply(_("Timezone changed to \002%s\002."), usertz.c_str());
		else
			source.Reply(_("Timezone for \002%s\002 changed to \002%s\002."), nc->display.c_str(), usertz.c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params.empty() ? "" : params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.empty())
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_(
				"Changes the timezone services uses when sending messages to you (for example, "
				"when responding to a command you send). \037timezone\037 should be chosen from "
				"an entry in one of the supported timezone regions:"
			));

			for (const auto &[timeregion, timezone] : timeregions)
				source.Reply("    %s (%zu timezones)", timeregion.c_str(), timezone.size());

			source.Reply(_("Type \002%s\032\037region\037\002 to list timezones for a region."),
				source.service->GetQueryCommand("generic/help", source.command).c_str());
		}
		else if (!SendZones(source, subcommand))
			this->OnSyntaxError(source, subcommand);

		return true;
	}
};

class CommandNSSASetTimezone final
	: public CommandNSSetTimezone
{
public:
	CommandNSSASetTimezone(Module *creator, SerializableExtensibleItem<Anope::string> &tz)
		: CommandNSSetTimezone(creator, tz, "nickserv/saset/timezone", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 [\037timezone\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.empty())
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_(
				"Changes the timezone services uses when sending messages to the given user (for "
				"example, when responding to a command they send). \037timezone\037 should be "
				"chosen from an entry in one of the supported timezone regions:"
			));

			for (const auto &[timeregion, timezone] : timeregions)
				source.Reply("    %s (%zu timezones)", timeregion.c_str(), timezone.size());

			source.Reply(_("Type \002%s\032\037region\037\002 to list timezones for a region."),
				source.service->GetQueryCommand("generic/help", source.command).c_str());
		}
		else if (!SendZones(source, subcommand))
			this->OnSyntaxError(source, subcommand);

		return true;
	}
};

class NSSetTimezone final
	: public Module
{
private:
	SerializableExtensibleItem<Anope::string> timezone;
	CommandNSSetTimezone commandnssettimezone;
	CommandNSSASetTimezone commandnssasettimezone;

public:
	NSSetTimezone(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, timezone(this, "timezone")
		, commandnssettimezone(this, timezone)
		, commandnssasettimezone(this, timezone)
	{
#ifndef HAS_CXX20_CHRONO
		throw ModuleException("A compiler with C++20 support is required by this module");
#else
		// Build the zone list.
		const auto& tzdb = std::chrono::get_tzdb();
		for (const auto &tz : tzdb.zones)
			timezones.emplace_back(tz.name());
		for (const auto &tz : tzdb.links)
			timezones.emplace_back(tz.name());
		std::sort(timezones.begin(), timezones.end());

		// Build the region list.
		for (const auto &timezone : timezones)
		{
			auto tzsep = timezone.find('/');
			auto region = tzsep == Anope::string::npos ? "Misc" : timezone.substr(0, tzsep);
			timeregions[region].push_back(timezone);
		}
		for (auto &[_, timeregion] : timeregions)
			std::sort(timeregion.begin(), timeregion.end());
#endif
	}
};

MODULE_INIT(NSSetTimezone)
