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

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(const Anope::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("ON"))
		{
			u->Account()->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_ON);
		}
		else if (params[0].equals_ci("OFF"))
		{
			u->Account()->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_OFF);
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_SECURE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_SECURE);
	}
};

class CommandNSSASetSecure : public Command
{
 public:
	CommandNSSASetSecure(const Anope::string &cname) : Command(cname, 2, 2, "nickserv/saset/secure")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		Anope::string param = params[1];

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_ON, nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_OFF, nc->display.c_str());
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_SECURE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET SECURE", NICK_SASET_SECURE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_SECURE);
	}
};

class NSSetSecure : public Module
{
 public:
	NSSetSecure(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetSecure("SECURE"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetSecure("SECURE"));
	}

	~NSSetSecure()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("SECURE");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("SECURE");
	}
};

MODULE_INIT(NSSetSecure)
