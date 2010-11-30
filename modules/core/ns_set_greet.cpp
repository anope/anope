/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(const Anope::string &spermission = "") : Command("GREET", 1, 2, spermission)
	{
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
			source.Reply(NICK_SASET_GREET_CHANGED, nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			nc->greet.clear();
			source.Reply(NICK_SASET_GREET_UNSET, nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SET_GREET);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET_GREET);
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
		source.Reply(NICK_HELP_SASET_GREET);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SASET_GREET);
	}
};

class NSSetGreet : public Module
{
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

 public:
	NSSetGreet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetgreet);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetgreet);
	}

	~NSSetGreet()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetgreet);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetgreet);
	}
};

MODULE_INIT(NSSetGreet)
