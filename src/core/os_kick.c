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

class CommandOSKick : public Command
{
 public:
	CommandOSKick() : Command("KICK", 3, 3, "operserv/kick")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *argv[3];
		const char *chan = params[0].c_str(), *nick = params[1].c_str(), *s = params[2].c_str();
		Channel *c;

		if (!(c = findchan(chan)))
		{
			notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		ircdproto->SendKick(findbot(s_OperServ), chan, nick, "%s (%s)", u->nick, s);
		if (WallOSKick)
			ircdproto->SendGlobops(s_OperServ, "%s used KICK on %s/%s", u->nick, nick, chan);
		argv[0] = sstrdup(chan);
		argv[1] = sstrdup(nick);
		argv[2] = sstrdup(s);
		do_kick(s_OperServ, 3, argv);
		delete [] argv[2];
		delete [] argv[1];
		delete [] argv[0];
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
	}
};

class OSKick : public Module
{
 public:
	OSKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSKick());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_KICK);
	}
};

MODULE_INIT(OSKick)
