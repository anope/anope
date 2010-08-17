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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSetGreet");

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (!param.empty())
		{
			nc->greet = param;
			notice_lang(Config->s_NickServ, u, NICK_SASET_GREET_CHANGED, nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			nc->greet.clear();
			notice_lang(Config->s_NickServ, u, NICK_SASET_GREET_UNSET, nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config->s_NickServ, u, NICK_HELP_SET_GREET);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_SET_GREET);
	}
};

class CommandNSSASetGreet : public CommandNSSetGreet
{
 public:
	CommandNSSASetGreet() : CommandNSSetGreet("nickserv/saset/greet")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config->s_NickServ, u, NICK_HELP_SASET_GREET);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_SASET_GREET);
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
			c->AddSubcommand(&commandnssetgreet);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasetgreet);
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
