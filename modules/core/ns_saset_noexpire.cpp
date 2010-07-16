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

class CommandNSSASetNoexpire : public Command
{
 public:
	CommandNSSASetNoexpire(const ci::string &cname) : Command(cname, 1, 2, "nickserv/saset/noexpire")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		assert(na);

		ci::string param = params.size() > 1 ? params[1] : "";

		if (param == "ON")
		{
			na->SetFlag(NS_NO_EXPIRE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_NOEXPIRE_ON, na->nick);
		}
		else if (param == "OFF")
		{
			na->UnsetFlag(NS_NO_EXPIRE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_NOEXPIRE_OFF, na->nick);
		}
		else
			this->OnSyntaxError(u, "NOEXPIRE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_NOEXPIRE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET NOEXPIRE", NICK_SASET_NOEXPIRE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_NOEXPIRE);
	}
};

class NSSASetNoexpire : public Module
{
 public:
	NSSASetNoexpire(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetNoexpire("NOEXPIRE"));
	}

	~NSSASetNoexpire()
	{
		Command *c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("NOEXPIRE");
	}
};

MODULE_INIT(NSSASetNoexpire)
