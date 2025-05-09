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

class CommandNSSetMessage
	: public Command
{
public:
	CommandNSSetMessage(Module *creator, const Anope::string &sname = "nickserv/set/message", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Change the communication method of services"));
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
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable " << source.command << " for " << nc->display;
			nc->Extend<bool>("MSG");
			source.Reply(_("Services will now reply to \002%s\002 with \002messages\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable " << source.command << " for " << nc->display;
			nc->Shrink<bool>("MSG");
			source.Reply(_("Services will now reply to \002%s\002 with \002notices\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "MSG");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		Anope::string cmd = source.command;
		size_t i = cmd.find_last_of(' ');
		if (i != Anope::string::npos)
			cmd = cmd.substr(i + 1);

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Allows you to choose the way services are communicating with "
				"you. With \002%s\002 set, services will use messages, else they'll "
				"use notices."
			),
			cmd.upper().c_str());
		return true;
	}
};

class CommandNSSASetMessage final
	: public CommandNSSetMessage
{
public:
	CommandNSSASetMessage(Module *creator) : CommandNSSetMessage(creator, "nickserv/saset/message", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Allows you to choose the way services are communicating with "
			"the given user. With \002MSG\002 set, services will use messages, "
			"else they'll use notices."
		));
		return true;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}
};


class NSSetMessage final
	: public Module
{
private:
	CommandNSSetMessage commandnssetmessage;
	CommandNSSASetMessage commandnssasetmessage;
	SerializableExtensibleItem<bool> message;

public:
	NSSetMessage(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetmessage(this)
		, commandnssasetmessage(this)
		, message(this, "MSG")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;
		if (message.HasExt(na->nc))
			info.AddOption(_("Message mode"));
	}
};

MODULE_INIT(NSSetMessage)
