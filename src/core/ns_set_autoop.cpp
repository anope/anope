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

class CommandNSSetAutoOp : public Command
{
 public:
	CommandNSSetAutoOp(const ci::string &cname) : Command(cname, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (params[0] == "ON")
		{
			u->Account()->SetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SET_AUTOOP_ON);
		}
		else if (params[0] == "OFF")
		{
			u->Account()->UnsetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SET_AUTOOP_OFF);
		}
		else
			this->OnSyntaxError(u, "AUTOOP");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_AUTOOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET AUTOOP", NICK_SET_AUTOOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_AUTOOP);
	}
};

class CommandNSSASetAutoOp : public Command
{
 public:
	CommandNSSASetAutoOp(const ci::string &cname) : Command(cname, 2, 2, "nickserv/saset/autoop")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		ci::string param = params[1];

		if (param == "ON")
		{
			nc->SetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SASET_AUTOOP_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SASET_AUTOOP_OFF, nc->display);
		}
		else
			this->OnSyntaxError(u, "AUTOOP");

		return MOD_CONT;
	}

	bool Help(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_AUTOOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET AUTOOP", NICK_SASET_AUTOOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_AUTOOP);
	}
};

class NSSetAutoOp : public Module
{
 public:
	NSSetAutoOp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		Command *set = FindCommand(NickServ, "SET");

		if (set)
			set->AddSubcommand(new CommandNSSetAutoOp("AUTOOP"));
	}

	~NSSetAutoOp()
	{
		Command *set = FindCommand(NickServ, "SET");

		if (set)
			set->DelSubcommand("AUTOOP");
	}
};

MODULE_INIT(NSSetAutoOp)
