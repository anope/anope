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

class CommandHSGroup final
	: public Command
{
	bool setting = false;

public:
	void Sync(const NickAlias *na)
	{
		if (setting)
			return;

		if (!na || !na->HasVHost())
			return;

		setting = true;
		for (auto *nick : *na->nc->aliases)
		{
			if (nick && nick != na)
			{
				nick->SetVHost(na->GetVHostIdent(), na->GetVHostHost(), na->GetVHostCreator());
				FOREACH_MOD(OnSetVHost, (nick));
			}
		}
		setting = false;
	}

	CommandHSGroup(Module *creator) : Command(creator, "hostserv/group", 0, 0)
	{
		this->SetDesc(_("Sync the vhost for all nicks in an account"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = NickAlias::Find(source.GetNick());
		if (na && source.GetAccount() == na->nc && na->HasVHost())
		{
			this->Sync(na);
			source.Reply(_("All vhosts for the account \002%s\002 have been set to \002%s\002."),
				source.nc->display.c_str(), na->GetVHostMask().c_str());
			Log(LOG_COMMAND, source, this) << "to sync their vhost of " << na->GetVHostMask() << " across all nicks in their group";
		}
		else
			source.Reply(HOST_NOT_ASSIGNED);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command allows users to set the vhost of their "
			"CURRENT nick to be the vhost for all nicks in the same "
			"group."
		));
		return true;
	}
};

class HSGroup final
	: public Module
{
	CommandHSGroup commandhsgroup;
	bool syncongroup;
	bool synconset;

public:
	HSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhsgroup(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnSetVHost(NickAlias *na) override
	{
		if (!synconset)
			return;

		commandhsgroup.Sync(na);
	}

	void OnNickGroup(User *u, NickAlias *na) override
	{
		if (!syncongroup)
			return;

		commandhsgroup.Sync(na);
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		syncongroup = block.Get<bool>("syncongroup");
		synconset = block.Get<bool>("synconset");
	}
};

MODULE_INIT(HSGroup)
