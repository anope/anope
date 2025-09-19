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

class CommandNSSetLayout
	: public Command
{
public:
	CommandNSSetLayout(Module *creator, const Anope::string &sname = "nickserv/set/layout", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Configures the layout used for services messages"));
		this->SetSyntax("{FIXED | FLEXIBLE}");
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
			nc->Shrink<time_t>("NS_FLEXIBLE");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the layout to fixed for " << nc->display;
			source.Reply(_("Layout is now \002fixed\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("FLEXIBLE"))
		{
			nc->Extend<bool>("NS_FLEXIBLE");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the layout to flexible for " << nc->display;
			source.Reply(_("Layout is now \002flexible\002 for \002%s\002."), nc->display.c_str());
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
			"Configures the layout used for services messages. When the layout is set to "
			"\037FIXED\037 services will use tables and position text such that it looks "
			"good in a client that uses a fixed-width font. When the layout is set to "
			"\037FLEXIBLE\037 services will use an alternate format for messages and avoid "
			"any positioning that might be broken by a variable-width font."
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
		this->SetSyntax(_("\037nickname\037 {FIXED | FLEXIBLE}"));
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
			"Configures the layout used by the account for services messages. When the "
			"layout is set to \037FIXED\037 services will use tables and position text such "
			"that it looks good in a client that uses a fixed-width font. When the layout is "
			"set to \037FLEXIBLE\037 services will use an alternate format for messages and "
			"avoid any positioning that might be broken by a variable-width font."
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

public:
	NSSetLayout(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetlayout(this)
		, commandnssasetlayout(this)
		, nsflexible(this, "NS_FLEXIBLE")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (nsflexible.HasExt(na->nc))
			info.AddOption(_("Flexible layout"));
		else
			info.AddOption(_("Fixed layout"));

		if (!show_hidden)
			return;
	}
};

MODULE_INIT(NSSetLayout)
