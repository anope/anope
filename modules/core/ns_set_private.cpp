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

class CommandNSSetPrivate : public Command
{
 public:
	CommandNSSetPrivate(const Anope::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("ON"))
		{
			u->Account()->SetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SET_PRIVATE_ON);
		}
		else if (params[0].equals_ci("OFF"))
		{
			u->Account()->UnsetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SET_PRIVATE_OFF);
		}
		else
			this->OnSyntaxError(u, "PRIVATE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_PRIVATE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET PRIVATE", NICK_SET_PRIVATE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_CMD_SET_PRIVATE);
	}
};

class CommandNSSASetPrivate : public Command
{
 public:
	CommandNSSASetPrivate(const Anope::string &cname) : Command(cname, 2, 2, "nickserv/saset/private")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSASetPrivate");

		Anope::string param = params[1];

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_PRIVATE_ON, nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_PRIVATE_OFF, nc->display.c_str());
		}
		else
			this->OnSyntaxError(u, "PRIVATE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_PRIVATE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET PRIVATE", NICK_SASET_PRIVATE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_CMD_SASET_PRIVATE);
	}
};

class NSSetPrivate : public Module
{
 public:
	NSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetPrivate("PRIVATE"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetPrivate("PRIVATE"));
	}

	~NSSetPrivate()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("PRIVATE");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("PRIVATE");
	}
};

MODULE_INIT(NSSetPrivate)
