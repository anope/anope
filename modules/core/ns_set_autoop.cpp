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

class CommandNSSetAutoOp : public Command
{
 public:
	CommandNSSetAutoOp(const Anope::string &spermission = "") : Command("AUTOOP", 1, 2, spermission)
	{
		this->SetDesc(_("Should services op you automatically."));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetAutoOp");
		NickCore *nc = na->nc;
		
		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_AUTOOP);
			source.Reply(_("Services will now autoop %s in channels."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_AUTOOP);
			source.Reply(_("Services will no longer autoop %s in channels."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET AUTOOP {ON | OFF}\002\n"
				" \n"
				"Sets whether you will be opped automatically. Set to ON to \n"
				"allow ChanServ to op you automatically when entering channels."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET AUTOOP", _("SET AUTOOP {ON | OFF}"));
	}
};

class CommandNSSASetAutoOp : public CommandNSSetAutoOp
{
 public:
	CommandNSSASetAutoOp() : CommandNSSetAutoOp("nickserv/saset/autoop")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 AUTOOP {ON | OFF}\002\n"
				" \n"
				"Sets whether the given nickname will be opped automatically.\n"
				"Set to \002ON\002 to allow ChanServ to op the given nickname \n"
				"omatically when joining channels."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET AUTOOP", _("SASET \037nickname\037 AUTOOP {ON | OFF}"));
	}
};

class NSSetAutoOp : public Module
{
	CommandNSSetAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

 public:
	NSSetAutoOp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetautoop);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetautoop);
	}

	~NSSetAutoOp()
	{
		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->DelSubcommand(&commandnssetautoop);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetautoop);
	}
};

MODULE_INIT(NSSetAutoOp)
