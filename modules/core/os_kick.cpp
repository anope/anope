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

class CommandOSKick : public Command
{
 public:
	CommandOSKick() : Command("KICK", 3, 3, "operserv/kick")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0], nick = params[1], s = params[2];
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config->s_OperServ, u, CHAN_X_NOT_IN_USE, chan.c_str());
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(Config->s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		else if (!(u2 = finduser(nick)))
		{
			notice_lang(Config->s_OperServ, u, NICK_X_NOT_IN_USE, nick.c_str());
			return MOD_CONT;
		}

		c->Kick(OperServ, u2, "%s (%s)", u->nick.c_str(), s.c_str());
		if (Config->WallOSKick)
			ircdproto->SendGlobops(OperServ, "%s used KICK on %s/%s", u->nick.c_str(), u2->nick.c_str(), chan.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_OperServ, u, OPER_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_KICK);
	}
};

class OSKick : public Module
{
	CommandOSKick commandoskick;

 public:
	OSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandoskick);
	}
};

MODULE_INIT(OSKick)
