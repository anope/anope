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

void defcon_sendlvls(User *u);

void myOperServHelp(User *u);

class CommandOSDEFCON : public Command
{
 public:
	CommandOSDEFCON() : Command("DEFCON", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *lvl = params[0].c_str();
		int newLevel = 0;
		const char *langglobal = getstring(NULL, DEFCON_GLOBAL);

		if (!DefConLevel) /* If we dont have a .conf setting! */
		{
			notice_lang(s_OperServ, u, OPER_DEFCON_NO_CONF);
			return MOD_CONT;
		}

		if (!lvl)
		{
			notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
			defcon_sendlvls(u);
			return MOD_CONT;
		}
		newLevel = atoi(lvl);
		if (newLevel < 1 || newLevel > 5)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}
		DefConLevel = newLevel;
		send_event(EVENT_DEFCON_LEVEL, 1, lvl);
		DefContimer = time(NULL);
		notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
		defcon_sendlvls(u);
		alog("Defcon level changed to %d by Oper %s", newLevel, u->nick);
		ircdproto->SendGlobops(s_OperServ, getstring2(NULL, OPER_DEFCON_WALL), u->nick, newLevel);
		/* Global notice the user what is happening. Also any Message that
		   the Admin would like to add. Set in config file. */
		if (GlobalOnDefcon)
		{
			if (DefConLevel == 5 && DefConOffMessage)
				oper_global(NULL, "%s", DefConOffMessage);
			else
				oper_global(NULL, langglobal, DefConLevel);
		}
		if (GlobalOnDefconMore)
		{
			if (!DefConOffMessage || DefConLevel != 5)
				oper_global(NULL, "%s", DefconMessage);
		}
		/* Run any defcon functions, e.g. FORCE CHAN MODE */
		runDefCon();
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_admin(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_DEFCON);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "DEFCON", OPER_DEFCON_SYNTAX);
	}
};

class OSDEFCON : public Module
{
 public:
	OSDEFCON(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSDEFCON(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_admin(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_DEFCON);
}

/**
 * Send a message to the oper about which precautions are "active" for this level
 **/
void defcon_sendlvls(User *u)
{
	if (checkDefCon(DEFCON_NO_NEW_CHANNELS))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CHANNELS);
	if (checkDefCon(DEFCON_NO_NEW_NICKS))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_NICKS);
	if (checkDefCon(DEFCON_NO_MLOCK_CHANGE))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_MLOCK_CHANGE);
	if (checkDefCon(DEFCON_FORCE_CHAN_MODES) && DefConChanModes)
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_FORCE_CHAN_MODES, DefConChanModes);
	if (checkDefCon(DEFCON_REDUCE_SESSION))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_REDUCE_SESSION, DefConSessionLimit);
	if (checkDefCon(DEFCON_NO_NEW_CLIENTS))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CLIENTS);
	if (checkDefCon(DEFCON_OPER_ONLY))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_OPER_ONLY);
	if (checkDefCon(DEFCON_SILENT_OPER_ONLY))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_SILENT_OPER_ONLY);
	if (checkDefCon(DEFCON_AKILL_NEW_CLIENTS))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_AKILL_NEW_CLIENTS);
	if (checkDefCon(DEFCON_NO_NEW_MEMOS))
		notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_MEMOS);
}

MODULE_INIT("os_defcon", OSDEFCON)
