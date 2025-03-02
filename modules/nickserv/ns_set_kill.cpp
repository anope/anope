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

class CommandNSSetKill
	: public Command
{
public:
	CommandNSSetKill(Module *creator, const Anope::string &sname = "nickserv/set/kill", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn protection on or off"));
		this->SetSyntax("{ON | QUICK | IMMED | OFF}");
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
			nc->Extend<bool>("KILLPROTECT");
			nc->Shrink<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill on for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->Extend<bool>("KILLPROTECT");
			nc->Extend<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill quick for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002, with a reduced delay."), nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->GetModule(this->owner).Get<bool>("allowkillimmed"))
			{
				nc->Extend<bool>("KILLPROTECT");
				nc->Shrink<bool>("KILL_QUICK");
				nc->Extend<bool>("KILL_IMMED");
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill immed for " << nc->display;
				source.Reply(_("Protection is now \002on\002 for \002%s\002, with no delay."), nc->display.c_str());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->Shrink<bool>("KILLPROTECT");
			nc->Shrink<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable kill for " << nc->display;
			source.Reply(_("Protection is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "KILL");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for your nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take your nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), source.service->nick.c_str());
		return true;
	}
};

class CommandNSSASetKill final
	: public CommandNSSetKill
{
public:
	CommandNSSASetKill(Module *creator)
		: CommandNSSetKill(creator, "nickserv/saset/kill", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | QUICK | IMMED | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for the nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take the nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), source.service->nick.c_str());
		return true;
	}
};

class NSSetKill final
	: public Module
{
private:
	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;
	SerializableExtensibleItem<bool> killprotect, kill_quick, kill_immed;

public:
	NSSetKill(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetkill(this)
		, commandnssasetkill(this)
		, killprotect(this, "KILLPROTECT")
		, kill_quick(this, "KILL_QUICK")
		, kill_immed(this, "KILL_IMMED")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (kill_immed.HasExt(na->nc))
			info.AddOption(_("Immediate protection"));
		else if (kill_quick.HasExt(na->nc))
			info.AddOption(_("Quick protection"));
		else if (killprotect.HasExt(na->nc))
			info.AddOption(_("Protection"));
	}
};

MODULE_INIT(NSSetKill)
