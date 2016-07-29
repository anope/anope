/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
#include "module.h"

class CommandOSLogin : public Command
{
 public:
	CommandOSLogin(Module *creator) : Command(creator, "operserv/login", 1, 1)
	{
		this->SetSyntax(_("\037password\037"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &password = params[0];

		User *u = source.GetUser();
		Oper *o = source.nc->o;
		if (o == NULL)
		{
			source.Reply(_("No oper block for your nickname."));
			return;
		}

		if (o->GetPassword().empty())
		{
			source.Reply(_("Your oper block doesn't require logging in."));
			return;
		}

		if (u->HasExtOK("os_login"))
		{
			source.Reply(_("You are already logged in."));
			return;
		}

		if (o->GetPassword() != password)
		{
			source.Reply(_("Password incorrect."));
			u->BadPassword();
			return;
		}

		Log(LOG_ADMIN, source, this) << "and successfully identified to " << source.service->nick;
		u->Extend<bool>("os_login", true);
		source.Reply(_("Password accepted."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Logs you in to {0} so you gain Services Operator privileges. This command is unnecessary if your oper block is configured without a password."), source.service->nick);
		return true;
	}

 	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Login to %s")), source.service->nick.c_str());
	}
};

class CommandOSLogout : public Command
{
 public:
	CommandOSLogout(Module *creator) : Command(creator, "operserv/logout", 0, 0)
	{
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Oper *o = source.nc->o;
		if (o == NULL)
		{
			source.Reply(_("No oper block for your nick."));
			return;
		}

		if (o->GetPassword().empty())
		{
			source.Reply(_("Your oper block doesn't require logging in."));
			return;
		}

		if (!u->HasExtOK("os_login"))
		{
			source.Reply(_("You are not identified."));
			return;
		}

		Log(LOG_ADMIN, source, this);
		u->Shrink<bool>("os_login");
		source.Reply(_("You have been logged out."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Logs you out from %s so you lose Services Operator privileges. This command is only useful if your oper block is configured with a password."), source.service->nick);
		return true;
	}

 	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Logout from %s")), source.service->nick.c_str());
	}
};

class OSLogin : public Module
	, public EventHook<Event::IsServicesOperEvent>
{
	CommandOSLogin commandoslogin;
	CommandOSLogout commandoslogout;
	ExtensibleItem<bool> os_login;

 public:
	OSLogin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::IsServicesOperEvent>(this)
		, commandoslogin(this)
		, commandoslogout(this)
		, os_login(this, "os_login")
	{

	}

	EventReturn IsServicesOper(User *u) override
	{
		if (!u->Account()->o->GetPassword().empty())
		{
			if (os_login.HasExt(u))
				return EVENT_ALLOW;
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSLogin)
