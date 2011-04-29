/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "nickserv.h"

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(const Anope::string &cpermission = "") : Command("SECURE", 2, 2, cpermission)
	{
		this->SetDesc(_("Turn nickname security on or off"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetSecure");
		NickCore *nc = na->nc;

		Anope::string param = params[1];

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

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET SECURE {ON | OFF}\002\n"
				" \n"
				"Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->s_NickServ.c_str(), Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SECURE", _("SET SECURE {ON | OFF}"));
	}
};

class CommandNSSASetSecure : public CommandNSSetSecure
{
 public:
	CommandNSSASetSecure() : CommandNSSetSecure("nickserv/saset/secure")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 SECURE {ON | OFF}\002\n"
				" \n"
				"Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->s_NickServ.c_str(), Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SECURE", _("SASET \037nickname\037 SECURE {ON | OFF}"));
	}
};

class NSSetSecure : public Module
{
	CommandNSSetSecure commandnssetsecure;
	CommandNSSASetSecure commandnssasetsecure;

 public:
	NSSetSecure(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetsecure);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetsecure);
	}

	~NSSetSecure()
	{
		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->DelSubcommand(&commandnssetsecure);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetsecure);
	}
};

MODULE_INIT(NSSetSecure)
