/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(const ci::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (params[0] == "ON")
		{
			u->Account()->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_ON);
		}
		else if (params[0] == "OFF")
		{
			u->Account()->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_OFF);
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_SECURE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
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
	CommandNSSASetSecure(const ci::string &cname) : Command(cname, 2, 2, "nickserv/saset/secure")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		ci::string param = params[1];

		if (param == "ON")
		{
			nc->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_OFF, nc->display);
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_SECURE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
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
	NSSetSecure(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
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
