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

class CommandNSSetHide : public Command
{
 public:
	CommandNSSetHide(const Anope::string &cname) : Command(cname, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		int onmsg, offmsg;
		NickCoreFlag flag;

		if (params[0].equals_ci("EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SET_HIDE_EMAIL_ON;
			offmsg = NICK_SET_HIDE_EMAIL_OFF;
		}
		else if (params[0].equals_ci("USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SET_HIDE_MASK_ON;
			offmsg = NICK_SET_HIDE_MASK_OFF;
		}
		else if (params[0].equals_ci("STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SET_HIDE_STATUS_ON;
			offmsg = NICK_SET_HIDE_STATUS_OFF;
		}
		else if (params[0].equals_ci("QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SET_HIDE_QUIT_ON;
			offmsg = NICK_SET_HIDE_QUIT_OFF;
		}
		else
		{
			this->OnSyntaxError(u, "HIDE");
			return MOD_CONT;
		}

		if (params[1].equals_ci("ON"))
		{
			u->Account()->SetFlag(flag);
			notice_lang(Config.s_NickServ, u, onmsg, Config.s_NickServ.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			u->Account()->UnsetFlag(flag);
			notice_lang(Config.s_NickServ, u, offmsg, Config.s_NickServ.c_str());
		}
		else
			this->OnSyntaxError(u, "HIDE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_HIDE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_HIDE);
	}
};

class CommandNSSASetHide : public Command
{
 public:
	CommandNSSASetHide(const Anope::string &cname) : Command(cname, 3, 3, "nickserv/saset/command")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		Anope::string param = params[1];

		int onmsg, offmsg;
		NickCoreFlag flag;

		if (param.equals_ci("EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SASET_HIDE_EMAIL_ON;
			offmsg = NICK_SASET_HIDE_EMAIL_OFF;
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SASET_HIDE_MASK_ON;
			offmsg = NICK_SASET_HIDE_MASK_OFF;
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SASET_HIDE_STATUS_ON;
			offmsg = NICK_SASET_HIDE_STATUS_OFF;
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SASET_HIDE_QUIT_ON;
			offmsg = NICK_SASET_HIDE_QUIT_OFF;
		}
		else
		{
			this->OnSyntaxError(u, "HIDE");
			return MOD_CONT;
		}

		param = params[2];
		if (param.empty())
			this->OnSyntaxError(u, "HIDE");
		else if (param.equals_ci("ON"))
		{
			nc->SetFlag(flag);
			notice_lang(Config.s_NickServ, u, onmsg, nc->display.c_str(), Config.s_NickServ.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(flag);
			notice_lang(Config.s_NickServ, u, offmsg, nc->display.c_str(), Config.s_NickServ.c_str());
		}
		else
			this->OnSyntaxError(u, "HIDE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_HIDE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_HIDE);
	}
};

class NSSetHide : public Module
{
 public:
	NSSetHide(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetHide("HIDE"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetHide("HIDE"));
	}

	~NSSetHide()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("HIDE");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("HIDE");
	}
};

MODULE_INIT(NSSetHide)
