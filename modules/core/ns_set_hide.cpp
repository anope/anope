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
	CommandNSSetHide(const Anope::string &spermission = "") : Command("HIDE", 2, 3, spermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSetHide");

		int onmsg, offmsg;
		NickCoreFlag flag;

		Anope::string param = params[1];
		Anope::string arg = params.size() > 2 ? params[2] : "";

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

		if (arg.equals_ci("ON"))
		{
			nc->SetFlag(flag);
			notice_lang(Config.s_NickServ, u, onmsg, nc->display.c_str(), Config.s_NickServ.c_str());
		}
		else if (arg.equals_ci("OFF"))
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

class CommandNSSASetHide : public CommandNSSetHide
{
 public:
	CommandNSSASetHide() : CommandNSSetHide("nickserv/saset/command")
	{
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
	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

 public:
	NSSetHide(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssethide);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasethide);
	}

	~NSSetHide()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssethide);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasethide);
	}
};

MODULE_INIT(NSSetHide)
