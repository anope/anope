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

class CommandNSSetOpAutoOp
	: public Command
{
public:
	CommandNSSetOpAutoOp(Module *creator, const Anope::string &sname = "nickserv/set/autoop", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether services should set channel status modes on you automatically."));
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
		if (na == NULL)
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
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autoop for " << na->nc->display;
			nc->Extend<bool>("AUTOOP");
			source.Reply(_("Services will from now on set status modes on %s in channels."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autoop for " << na->nc->display;
			nc->Shrink<bool>("AUTOOP");
			source.Reply(_("Services will no longer set status modes on %s in channels."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		BotInfo *bi = Config->GetClient("ChanServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Sets whether you will be given your channel status modes automatically. "
				"Set to \002ON\002 to allow %s to set status modes on you automatically "
				"when entering channels. Note that depending on channel settings some modes "
				"may not get set automatically."
			),
			bi ? bi->nick.c_str() : "ChanServ");
		return true;
	}
};

class CommandNSSASetAutoOp final
	: public CommandNSSetOpAutoOp
{
public:
	CommandNSSASetAutoOp(Module *creator)
		: CommandNSSetOpAutoOp(creator, "nickserv/saset/autoop", 2)
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
		BotInfo *bi = Config->GetClient("ChanServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Sets whether the given nickname will be given its status modes "
				"in channels automatically. Set to \002ON\002 to allow %s "
				"to set status modes on the given nickname automatically when it "
				"is entering channels. Note that depending on channel settings "
				"some modes may not get set automatically."
			),
			bi ? bi->nick.c_str() : "ChanServ");
		return true;
	}
};

class CommandNSSetOpNeverOp
	: public Command
{
public:
	CommandNSSetOpNeverOp(Module *creator, const Anope::string &sname = "nickserv/set/neverop", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether you can be added to a channel access list."));
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
		if (na == NULL)
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
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable neverop for " << na->nc->display;
			nc->Extend<bool>("NEVEROP");
			source.Reply(_("%s can no longer be added to channel access lists."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable neverop for " << na->nc->display;
			nc->Shrink<bool>("NEVEROP");
			source.Reply(_("%s can now be added to channel access lists."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "NEVEROP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether you can be added to a channel access list."));
		return true;
	}
};

class CommandNSSASetNeverOp final
	: public CommandNSSetOpNeverOp
{
public:
	CommandNSSASetNeverOp(Module *creator)
		: CommandNSSetOpNeverOp(creator, "nickserv/saset/neverop", 2)
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
		source.Reply(_("Sets whether the given nickname can be added to a channel access list."));
		return true;
	}
};

class NSSetOp final
	: public Module
{
private:
	CommandNSSetOpAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

	CommandNSSetOpNeverOp commandnssetneverop;
	CommandNSSASetNeverOp commandnssasetneverop;

	SerializableExtensibleItem<bool> autoop;
	SerializableExtensibleItem<bool> neverop;

public:
	NSSetOp(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetautoop(this)
		, commandnssasetautoop(this)
		, commandnssetneverop(this)
		, commandnssasetneverop(this)
		, autoop(this, "AUTOOP")
		, neverop(this, "NEVEROP")
	{
	}

	void OnSetCorrectModes(User *user, Channel *chan, AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (!chan->ci)
			return;

		// Only give modes if autoop is set.
		give_modes &= !user->IsIdentified() || autoop.HasExt(user->Account());
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (autoop.HasExt(na->nc))
			info.AddOption(_("Auto op"));

		if (neverop.HasExt(na->nc))
			info.AddOption(_("Never op"));
	}
};

MODULE_INIT(NSSetOp)
