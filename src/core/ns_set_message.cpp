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

class CommandNSSetMessage : public Command
{
 public:
	CommandNSSetMessage(const ci::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (!Config.UsePrivmsg)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (params[0] == "ON")
		{
			u->Account()->SetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SET_MSG_ON);
		}
		else if (params[0] == "OFF")
		{
			u->Account()->UnsetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SET_MSG_OFF);
		}
		else
			this->OnSyntaxError(u, "MSG");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_MSG);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_MSG);
	}
};

class CommandNSSASetMessage : public Command
{
 public:
	CommandNSSASetMessage(const ci::string &cname) : Command(cname, 2, 2, "nickserv/saset/message")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		ci::string param = params[1];

		if (!Config.UsePrivmsg)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SASET_MSG_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SASET_MSG_OFF, nc->display);
		}
		else
			this->OnSyntaxError(u, "MSG");

		return MOD_CONT;
	}

	bool OnHelp(User *u)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_MSG);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET MSG", NICK_SASET_MSG_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_MSG);
	}
};

class NSSetMessage : public Module
{
 public:
	NSSetMessage(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetMessage("MSG"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetMessage("MSG"));
	}

	~NSSetMessage()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("MSG");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("MSG");
	}
};

MODULE_INIT(NSSetMessage)
