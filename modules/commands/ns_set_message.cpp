/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSSetMessage : public Command
{
 public:
	CommandNSSetMessage(Module *creator, const Anope::string &sname = "nickserv/set/message", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Change the communication method of Services"));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (!Config->UsePrivmsg)
		{
			source.Reply(_("You cannot %s on this network."), source.command.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_MSG);
			source.Reply(_("Services will now reply to \002%s\002 with \002messages\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_MSG);
			source.Reply(_("Services will now reply to \002%s\002 with \002notices\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "MSG");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to choose the way Services are communicating with \n"
				"you. With \002MSG\002 set, Services will use messages, else they'll \n"
				"use notices."));
		return true;
	}

	void OnServHelp(CommandSource &source) anope_override
	{
		if (Config->UsePrivmsg)
			Command::OnServHelp(source);
	}
};

class CommandNSSASetMessage : public CommandNSSetMessage
{
 public:
	CommandNSSASetMessage(Module *creator) : CommandNSSetMessage(creator, "nickserv/saset/message", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to choose the way Services are communicating with \n"
				"the given user. With \002MSG\002 set, Services will use messages,\n"
				"else they'll use notices."));
		return true;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}
};

class NSSetMessage : public Module
{
	CommandNSSetMessage commandnssetmessage;
	CommandNSSASetMessage commandnssasetmessage;

 public:
	NSSetMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetmessage(this), commandnssasetmessage(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetMessage)
