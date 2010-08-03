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

class CommandNSSetKill : public Command
{
 public:
	CommandNSSetKill(const Anope::string &spermission = "") : Command("KILL", 2, 3, spermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSetKill");

		Anope::string param = params[1];
		Anope::string arg = params.size() > 2 ? params[2] : "";

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_ON, nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->SetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_QUICK, nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config.NSAllowKillImmed)
			{
				nc->SetFlag(NI_KILLPROTECT);
				nc->SetFlag(NI_KILL_IMMED);
				nc->UnsetFlag(NI_KILL_QUICK);
				notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_IMMED, nc->display.c_str());
			}
			else
				notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_IMMED_DISABLED);
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_OFF, nc->display.c_str());
		}
		else
			this->OnSyntaxError(u, "KILL");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_KILL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET KILL", Config.NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX : NICK_SET_KILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_KILL);
	}
};

class CommandNSSASetKill : public CommandNSSetKill
{
 public:
	CommandNSSASetKill() : CommandNSSetKill("nickserv/saset/kill")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_KILL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET KILL", Config.NSAllowKillImmed ? NICK_SASET_KILL_IMMED_SYNTAX : NICK_SASET_KILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_KILL);
	}
};

class NSSetKill : public Module
{
	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;

 public:
	NSSetKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssetkill);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasetkill);
	}

	~NSSetKill()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetkill);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetkill);
	}
};

MODULE_INIT(NSSetKill)
