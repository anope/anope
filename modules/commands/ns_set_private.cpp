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

class CommandNSSetPrivate : public Command
{
 public:
	CommandNSSetPrivate(Module *creator, const Anope::string &sname = "nickserv/set/private", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(Anope::printf(_("Prevent the nickname from appearing in a \002%s%s LIST\002"), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str()));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_PRIVATE);
			source.Reply(_("Private option is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_PRIVATE);
			source.Reply(_("Private option is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

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
		source.Reply(_("Turns %s's privacy option on or off for your nick.\n"
				"With \002PRIVATE\002 set, your nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows your nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetPrivate : public CommandNSSetPrivate
{
 public:
	CommandNSSASetPrivate(Module *creator) : CommandNSSetPrivate(creator, "nickserv/saset/private", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's privacy option on or off for the nick.\n"
				"With \002PRIVATE\002 set, the nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows the nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class NSSetPrivate : public Module
{
	CommandNSSetPrivate commandnssetprivate;
	CommandNSSASetPrivate commandnssasetprivate;

 public:
	NSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetprivate(this), commandnssasetprivate(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetPrivate)
