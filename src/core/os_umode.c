/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode() : Command("UMODE", 2, 2, "operserv/umode")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *modes = params[1].c_str();

		User *u2;

		/**
		 * Only accept a +/- mode string
		 *-rob
		 **/
		if (modes[0] != '+' && modes[0] != '-')
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}
		if (!(u2 = finduser(nick)))
			notice_lang(Config.s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else
		{
			u2->SetModes(findbot(Config.s_OperServ), modes);

			notice_lang(Config.s_OperServ, u, OPER_UMODE_SUCCESS, nick);
			notice_lang(Config.s_OperServ, u2, OPER_UMODE_CHANGED, u->nick.c_str());

			if (Config.WallOSMode)
				ircdproto->SendGlobops(findbot(Config.s_OperServ), "\2%s\2 used UMODE on %s", u->nick.c_str(), nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_UMODE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
	}
};

class OSUMode : public Module
{
 public:
	OSUMode(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSUMode());

		if (!ircd->umode)
			throw ModuleException("Your IRCd does not support setting umodes");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_UMODE);
	}
};

MODULE_INIT(OSUMode)
