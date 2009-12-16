/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
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

class CommandOSMode : public Command
{
 public:
	CommandOSMode() : Command("MODE", 2, 2, "operserv/mode")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str(), *modes = params[1].c_str();
		Channel *c;

		if (!(c = findchan(chan)))
			notice_lang(Config.s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
		else if (c->bouncy_modes)
			notice_lang(Config.s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
		else
		{
			c->SetModes(findbot(Config.s_OperServ), false, modes);

			if (Config.WallOSMode)
				ircdproto->SendGlobops(findbot(Config.s_OperServ), "%s used MODE %s on %s", u->nick, modes, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_MODE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
	}
};

class OSMode : public Module
{
 public:
	OSMode(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSMode());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_MODE);
	}
};

MODULE_INIT(OSMode)
