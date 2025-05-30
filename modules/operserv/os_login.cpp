/* OperServ core functions
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
#include "modules/encryption.h"

class CommandOSLogin final
	: public Command
{
private:
	bool ComparePassword(const Oper *o, const Anope::string &password)
	{
		if (o->password_hash.empty())
			return o->password.equals_cs(password); // Plaintext password.

		auto *service = Service::FindService("Encryption::Provider", o->password_hash);
		if (!service)
			return false; // Malformed hash.

		auto *hashprov = static_cast<Encryption::Provider *>(service);
		return hashprov->Compare(o->password, password);
	}

public:
	CommandOSLogin(Module *creator) : Command(creator, "operserv/login", 0, 1)
	{
		this->SetSyntax(_("\037password\037"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Oper *o = source.nc->o;
		if (o == NULL)
			source.Reply(_("No oper block for your nick."));
		else if (o->password.empty())
			source.Reply(_("Your oper block doesn't require logging in."));
		else if (u->HasExt("os_login"))
			source.Reply(_("You are already identified."));
		else if (params.empty() || !ComparePassword(o, params[0]))
		{
			source.Reply(PASSWORD_INCORRECT);
			u->BadPassword();
		}
		else
		{
			Log(LOG_ADMIN, source, this) << "and successfully identified to " << source.service->nick;
			u->Extend<bool>("os_login");
			source.Reply(_("Password accepted."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Logs you in to %s so you gain Services Operator privileges. "
				"This command may be unnecessary if your oper block is "
				"configured without a password."
			),
			source.service->nick.c_str());
		return true;
	}

	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Login to %s")), source.service->nick.c_str());
	}
};

class CommandOSLogout final
	: public Command
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
			source.Reply(_("No oper block for your nick."));
		else if (o->password.empty())
			source.Reply(_("Your oper block doesn't require logging in."));
		else if (!u->HasExt("os_login"))
			source.Reply(_("You are not identified."));
		else
		{
			Log(LOG_ADMIN, source, this);
			u->Shrink<bool>("os_login");
			source.Reply(_("You have been logged out."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Logs you out from %s so you lose Services Operator privileges. "
				"This command is only useful if your oper block is configured "
				"with a password."
			),
			source.service->nick.c_str());
		return true;
	}

	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Logout from %s")), source.service->nick.c_str());
	}
};

class OSLogin final
	: public Module
{
	CommandOSLogin commandoslogin;
	CommandOSLogout commandoslogout;
	ExtensibleItem<bool> os_login;

public:
	OSLogin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandoslogin(this), commandoslogout(this), os_login(this, "os_login")
	{

	}

	EventReturn IsServicesOper(User *u) override
	{
		if (!u->Account()->o->password.empty())
		{
			if (os_login.HasExt(u))
				return EVENT_ALLOW;
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSLogin)
