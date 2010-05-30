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

class CommandOSKick : public Command
{
 public:
	CommandOSKick() : Command("KICK", 3, 3, "operserv/kick")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str(), *nick = params[1].c_str(), *s = params[2].c_str();
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config.s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(Config.s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		else if (!(u2 = finduser(nick)))
		{
			notice_lang(Config.s_OperServ, u, NICK_X_NOT_IN_USE, nick);
			return MOD_CONT;
		}

		c->Kick(OperServ, u2, "%s (%s)", u->nick.c_str(), s);
		if (Config.WallOSKick)
			ircdproto->SendGlobops(OperServ, "%s used KICK on %s/%s", u->nick.c_str(), u2->nick.c_str(), chan);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_KICK);
	}
};

class OSKick : public Module
{
 public:
	OSKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSKick());
	}
};

MODULE_INIT(OSKick)
