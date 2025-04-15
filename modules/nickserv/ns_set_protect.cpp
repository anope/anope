/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSSetProtect
	: public Command
{
public:
	CommandNSSetProtect(Module *creator, const Anope::string &sname = "nickserv/set/protect", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn protection on or off"));
		this->SetSyntax(_("{ON | \037delay\037 | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (Config->GetModule("nickserv").Get<bool>("nonicknameownership"))
		{
			source.Reply(_("This command may not be used on this network because nickname ownership is disabled."));
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

		if (param.equals_ci("ON"))
		{
			nc->Extend<bool>("PROTECT");
			nc->Shrink<time_t>("PROTECT_AFTER");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable protection for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->Shrink<bool>("PROTECT");
			nc->Shrink<time_t>("PROTECT_AFTER");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable protection for " << nc->display;
			source.Reply(_("Protection is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
		{
			auto iparam = Anope::TryConvert<time_t>(param);
			if (!iparam)
			{
				this->OnSyntaxError(source, "PROTECT");
				return;
			}

			auto &block = Config->GetModule("nickserv");
			auto minprotect = block.Get<time_t>("minprotect", "10s");
			auto maxprotect = block.Get<time_t>("maxprotect", "10m");
			if (*iparam < minprotect || *iparam > maxprotect)
			{
				source.Reply(_("Protection delay must be between %s and %s."),
					Anope::Duration(minprotect, source.GetAccount()).c_str(),
					Anope::Duration(maxprotect, source.GetAccount()).c_str());
				return;
			}

			nc->Extend<bool>("PROTECT");
			nc->Extend<time_t>("PROTECT_AFTER", *iparam);
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable protection after " << *iparam << " seconds for " << nc->display;
			source.Reply(_("Protection is now \002on\002 after \002%lu seconds\002 for \002%s\002."), *iparam, nc->display.c_str());
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
				"Turns automatic protection for your account on or off. With "
				"protection on if another user tries to use a nickname from "
				"your group they will be given some time to change their nick "
				"after which %s will forcibly change their nick."
			),
			source.service->nick.c_str());
		return true;
	}
};

class CommandNSSASetProtect final
	: public CommandNSSetProtect
{
public:
	CommandNSSASetProtect(Module *creator)
		: CommandNSSetProtect(creator, "nickserv/saset/protect", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | \037delay\037 | OFF}"));
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
				"Turns automatic protection for the nick on or off. With "
				"protection on if a user tries to use a nickname from the "
				"nick's group they will be given some time to change their "
				"nick after which %s will forcibly change their nick."
			),
			source.service->nick.c_str());
		return true;
	}
};

class NSSetProtect final
	: public Module
{
private:
	CommandNSSetProtect commandnssetprotect;
	CommandNSSASetProtect commandnssasetprotect;

	SerializableExtensibleItem<bool> protect;
	SerializableExtensibleItem<time_t> protectafter;

public:
	NSSetProtect(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetprotect(this)
		, commandnssasetprotect(this)
		, protect(this, "PROTECT")
		, protectafter(this, "PROTECT_AFTER")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!protect.HasExt(na->nc))
			return;

		info.AddOption(_("Protection"));
		if (show_hidden)
		{
			auto protectafter = na->nc->GetExt<time_t>("PROTECT_AFTER");
			auto protect = protectafter ? *protectafter : Config->GetModule("nickserv").Get<time_t>("defaultprotect", "1m");
			info[_("Protection after")] = Anope::Duration(protect, source.GetAccount());
		}
	}
};

MODULE_INIT(NSSetProtect)
