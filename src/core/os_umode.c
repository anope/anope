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

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode() : Command("UMODE", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *modes = params[1].c_str();

		User *u2;

		/* Only allow this if SuperAdmin is enabled */
		if (!u->isSuperAdmin)
		{
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
			return MOD_CONT;
		}

		/**
		 * Only accept a +/- mode string
		 *-rob
		 **/
		if (modes[0] != '+' && modes[0] != '-')
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}
		if (!(u2 = finduser(nick)))
			notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else
		{
			ircdproto->SendMode(findbot(s_OperServ), nick, "%s", modes);

			common_svsmode(u2, modes, NULL);

			notice_lang(s_OperServ, u, OPER_UMODE_SUCCESS, nick);
			notice_lang(s_OperServ, u2, OPER_UMODE_CHANGED, u->nick);

			if (WallOSMode)
				ircdproto->SendGlobops(s_OperServ, "\2%s\2 used UMODE on %s", u->nick, nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_UMODE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
	}
};

class OSUMode : public Module
{
 public:
	OSUMode(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSUMode(), MOD_UNIQUE);

		if (!ircd->umode)
			throw ModuleException("Your IRCd does not support setting umodes");
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_UMODE);
	}
};

MODULE_INIT("os_umode", OSUMode)
