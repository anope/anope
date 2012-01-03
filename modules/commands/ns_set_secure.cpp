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

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(Module *creator, const Anope::string &sname = "nickserv/set/secure", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn nickname security on or off"));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_SECURE);
			source.Reply(_("Secure option is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_SECURE);
			source.Reply(_("Secure option is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, source.u->Account()->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetSecure : public CommandNSSetSecure
{
 public:
	CommandNSSASetSecure(Module *creator) : CommandNSSetSecure(creator, "nickserv/saset/secure", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class NSSetSecure : public Module
{
	CommandNSSetSecure commandnssetsecure;
	CommandNSSASetSecure commandnssasetsecure;

 public:
	NSSetSecure(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetsecure(this), commandnssasetsecure(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetSecure)
