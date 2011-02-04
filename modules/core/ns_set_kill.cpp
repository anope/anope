/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetKill");
		NickCore *nc = na->nc;

		Anope::string param = params[1];
		Anope::string arg = params.size() > 2 ? params[2] : "";

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->SetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002\002 for \002%s\002, with a reduced delay."), nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->NSAllowKillImmed)
			{
				nc->SetFlag(NI_KILLPROTECT);
				nc->SetFlag(NI_KILL_IMMED);
				nc->UnsetFlag(NI_KILL_QUICK);
				source.Reply(_("Protection is now \002\002 for \002%s\002, with no delay."), nc->display.c_str());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "KILL");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET KILL {ON | QUICK | IMMED | OFF}\002\n"
				" \n"
				"Turns the automatic protection option for your nick\n"
				"on or off.  With protection on, if another user\n"
				"tries to take your nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60.  If you select\n"
				"\002IMMED\002, user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET KILL", Config->NSAllowKillImmed ? _("SET KILL {ON | QUICK | IMMED | OFF}") : _("SET KILL {ON | QUICK | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    KILL       Turn protection on or off"));
	}
};

class CommandNSSASetKill : public CommandNSSetKill
{
 public:
	CommandNSSASetKill() : CommandNSSetKill("nickserv/saset/kill")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 KILL {ON | QUICK | IMMED | OFF}\002\n"
				" \n"
				"Turns the automatic protection option for the nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take the nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET KILL", Config->NSAllowKillImmed ? _("SASET \037nickname\037 KILL {ON | QUICK | IMMED | OFF}") : _("SASET \037nickname\037 KILL {ON | QUICK | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    KILL       Turn protection on or off"));
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
			c->AddSubcommand(this, &commandnssetkill);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetkill);
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
