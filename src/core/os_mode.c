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

void myOperServHelp(User *u);

class CommandOSMode : public Command
{
 public:
	CommandOSMode() : Command("MODE", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		int ac;
		const char **av;
		const char *chan = params[0].c_str(), *modes = params[1].c_str();
		Channel *c;

		if (!(c = findchan(chan)))
			notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
		else if (c->bouncy_modes)
			notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
		else if (ircd->adminmode && !is_services_admin(u) && (c->mode & ircd->adminmode))
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
		else
		{
			ircdproto->SendMode(findbot(s_OperServ), chan, "%s", modes);

			ac = split_buf((char *)modes, &av, 1);
			chan_set_modes(s_OperServ, c, ac, av, -1);
			free(av);

			if (WallOSMode)
				ircdproto->SendGlobops(s_OperServ, "%s used MODE %s on %s", u->nick, modes, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_oper(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_MODE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
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

		this->AddCommand(OPERSERV, new CommandOSMode(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_oper(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODE);
}

MODULE_INIT("os_mode", OSMode)
