/* OperServ core functions
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

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode() : Command("UMODE", 2, 2, "operserv/umode")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string modes = params[1];

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
			notice_lang(Config.s_OperServ, u, NICK_X_NOT_IN_USE, nick.c_str());
		else
		{
			u2->SetModes(OperServ, "%s", modes.c_str());

			notice_lang(Config.s_OperServ, u, OPER_UMODE_SUCCESS, nick.c_str());
			notice_lang(Config.s_OperServ, u2, OPER_UMODE_CHANGED, u->nick.c_str());

			if (Config.WallOSMode)
				ircdproto->SendGlobops(OperServ, "\2%s\2 used UMODE on %s", u->nick.c_str(), nick.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_UMODE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_UMODE);
	}
};

class OSUMode : public Module
{
 public:
	OSUMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->umode)
			throw ModuleException("Your IRCd does not support setting umodes");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSUMode());
	}
};

MODULE_INIT(OSUMode)
