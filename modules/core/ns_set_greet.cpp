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

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(const Anope::string &spermission = "") : Command("GREET", 1, 2, spermission)
	{
		this->SetDesc(_("Associate a greet message with your nickname"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetGreet");
		NickCore *nc = na->nc;

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (!param.empty())
		{
			nc->greet = param;
			source.Reply(_("Greet message for \002%s\002 changed to \002%s\002."), nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			nc->greet.clear();
			source.Reply(_("Greet message for \002%s\002 unset."), nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET GREET \037message\037\002\n"
				" \n"
				"Makes the given message the greet of your nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that you have the necessary \n"
				"access on it."));
		return true;
	}
};

class CommandNSSASetGreet : public CommandNSSetGreet
{
 public:
	CommandNSSASetGreet() : CommandNSSetGreet("nickserv/saset/greet")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 GREET \037message\037\002\n"
				" \n"
				"Makes the given message the greet of the nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that the user has the necessary \n"
				"access on it."));
		return true;
	}
};

class NSSetGreet : public Module
{
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

 public:
	NSSetGreet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetgreet);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetgreet);
	}

	~NSSetGreet()
	{
		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->DelSubcommand(&commandnssetgreet);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetgreet);
	}
};

MODULE_INIT(NSSetGreet)
