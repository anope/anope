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

class CommandNSSASetNoexpire : public Command
{
 public:
	CommandNSSASetNoexpire() : Command("NOEXPIRE", 1, 2, "nickserv/saset/noexpire")
	{
		this->SetDesc("Prevent the nickname from expiring");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSASsetNoexpire");

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			na->SetFlag(NS_NO_EXPIRE);
			source.Reply(_("Nick %s \002will not\002 expire."), na->nick.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			na->UnsetFlag(NS_NO_EXPIRE);
			source.Reply(_("Nick %s \002will\002 expire."), na->nick.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 NOEXPIRE {ON | OFF}\002\n"
				" \n"
				"Sets whether the given nickname will expire.  Setting this\n"
				"to \002ON\002 prevents the nickname from expiring."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET NOEXPIRE", _("SASET \037nickname\037 NOEXPIRE {ON | OFF}"));
	}
};

class NSSASetNoexpire : public Module
{
	CommandNSSASetNoexpire commandnssasetnoexpire;

 public:
	NSSASetNoexpire(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetnoexpire);
	}

	~NSSASetNoexpire()
	{
		Command *c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetnoexpire);
	}
};

MODULE_INIT(NSSASetNoexpire)
