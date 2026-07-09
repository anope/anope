// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

class CommandNSSetLayout
	: public Command
{
public:
	CommandNSSetLayout(Module *creator, const Anope::string &sname = "nickserv/set/layout", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Configure the layout used for services messages"));
		this->SetSyntax("{FIXED | FLEXIBLE | MONOSPACE}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const auto *na = NickAlias::Find(user);
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

		if (param.equals_ci("FIXED"))
		{
			nc->Shrink<bool>("NS_FLEXIBLE");
			nc->Shrink<bool>("NS_MONOSPACE");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the layout to fixed for " << nc->display;
			source.Reply(_("Layout is now \002fixed\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("FLEXIBLE"))
		{
			nc->Extend<bool>("NS_FLEXIBLE");
			nc->Shrink<bool>("NS_MONOSPACE");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the layout to flexible for " << nc->display;
			source.Reply(_("Layout is now \002flexible\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("MONOSPACE"))
		{
			nc->Shrink<bool>("NS_FLEXIBLE");
			nc->Extend<bool>("NS_MONOSPACE");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the layout to monospace for " << nc->display;
			source.Reply(_("Layout is now \002monospace\002 for \002%s\002."), nc->display.c_str());
		}
		else
		{
			this->OnSyntaxError(source, "LAYOUT");
		}
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
			"Configures the layout used for services messages."
			"\n\n"
			"When the layout is set to \037FIXED\037 services will use tables and position "
			"text such that it looks good in a client that uses a fixed-width font."
			"\n\n"
			"When the layout is set to \037FLEXIBLE\037 services will use an alternate "
			"format for messages and avoid any positioning that might be broken by a "
			"variable-width font."
			"\n\n"
			"When the layout is set to \037MONOSPACE\037 services will format messages "
			"similar to \037FIXED\037 but will prefix all messages with a monospace "
			"formatting character to force it to display correctly. This requires client "
			"support for monospace text formatting."
		));
		return true;
	}
};

class CommandNSSASetLayout final
	: public CommandNSSetLayout
{
public:
	CommandNSSASetLayout(Module *creator)
		: CommandNSSetLayout(creator, "nickserv/saset/layout", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {FIXED | FLEXIBLE | MONOSPACE}"));
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
			"Configures the layout used by the account for services messages."
			"\n\n"
			"When the layout is set to \037FIXED\037 services will use tables and position "
			"text such that it looks good in a client that uses a fixed-width font."
			"\n\n"
			"When the layout is set to \037FLEXIBLE\037 services will use an alternate "
			"format for messages and avoid any positioning that might be broken by a "
			"variable-width font."
			"\n\n"
			"When the layout is set to \037MONOSPACE\037 services will format messages "
			"similar to \037FIXED\037 but will prefix all messages with a monospace "
			"formatting character to force it to display correctly. This requires client "
			"support for monospace text formatting."
		));
		return true;
	}
};

class NSSetLayout final
	: public Module
{
private:
	CommandNSSetLayout commandnssetlayout;
	CommandNSSASetLayout commandnssasetlayout;

	SerializableExtensibleItem<bool> nsflexible;
	SerializableExtensibleItem<bool> nsmonospace;

public:
	NSSetLayout(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetlayout(this)
		, commandnssasetlayout(this)
		, nsflexible(this, "NS_FLEXIBLE")
		, nsmonospace(this, "NS_MONOSPACE")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (nsflexible.HasExt(na->nc))
			info.AddOption(_("Flexible layout"));
		else if (nsmonospace.HasExt(na->nc))
			info.AddOption(_("Monospace layout"));
		else
			info.AddOption(_("Fixed layout"));
	}
};

MODULE_INIT(NSSetLayout)
