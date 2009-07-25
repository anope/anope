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

class CommandOSSet : public Command
{
 private:
	CommandReturn DoList(User *u)
	{
		int index;

		index = allow_ignore ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(s_OperServ, u, index, "IGNORE");
		index = readonly ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(s_OperServ, u, index, "READONLY");
		index = logchan ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(s_OperServ, u, index, "LOGCHAN");
		index = debug ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(s_OperServ, u, index, "DEBUG");
		index = noexpire ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(s_OperServ, u, index, "NOEXPIRE");

		return MOD_CONT;
	}

	CommandReturn DoSetIgnore(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			allow_ignore = 1;
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_ON);
		}
		else if (setting == "OFF")
		{
			allow_ignore = 0;
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_OFF);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetReadOnly(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			readonly = 1;
			alog("Read-only mode activated");
			close_log();
			notice_lang(s_OperServ, u, OPER_SET_READONLY_ON);
		}
		else if (setting == "OFF")
		{
			readonly = 0;
			open_log();
			alog("Read-only mode deactivated");
			notice_lang(s_OperServ, u, OPER_SET_READONLY_OFF);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_READONLY_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetLogChan(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";
		Channel *c;

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		/* Unlike the other SET commands where only stricmp is necessary,
		 * we also have to ensure that LogChannel is defined or we can't
		 * send to it.
		 *
		 * -jester
		 */
		if (LogChannel && setting == "ON")
		{
			if (ircd->join2msg)
			{
				c = findchan(LogChannel);
				ircdproto->SendJoin(findbot(s_GlobalNoticer), LogChannel, c ? c->creation_time : time(NULL));
			}
			logchan = 1;
			alog("Now sending log messages to %s", LogChannel);
			notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ON, LogChannel);
		}
		else if (LogChannel && setting == "OFF")
		{
			alog("No longer sending log messages to a channel");
			if (ircd->join2msg)
				ircdproto->SendPart(findbot(s_GlobalNoticer), LogChannel, NULL);
			logchan = 0;
			notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_OFF);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetSuperAdmin(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		/**
		 * Allow the user to turn super admin on/off
		 *
		 * Rob
		 **/
		if (!SuperAdmin)
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_NOT_ENABLED);
		else if (setting == "ON")
		{
			u->isSuperAdmin = 1;
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ON);
			alog("%s: %s is a SuperAdmin ", s_OperServ, u->nick);
			ircdproto->SendGlobops(s_OperServ, getstring(OPER_SUPER_ADMIN_WALL_ON), u->nick);
		}
		else if (setting == "OFF")
		{
			u->isSuperAdmin = 0;
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_OFF);
			alog("%s: %s is no longer a SuperAdmin", s_OperServ, u->nick);
			ircdproto->SendGlobops(s_OperServ, getstring(OPER_SUPER_ADMIN_WALL_OFF), u->nick);
		}
		else
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetDebug(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			debug = 1;
			alog("Debug mode activated");
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_ON);
		}
		else if (setting == "OFF" || (setting[0] == '0' && !atoi(setting.c_str())))
		{
			alog("Debug mode deactivated");
			debug = 0;
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_OFF);
		}
		else if (isdigit(setting[0]) && atoi(setting.c_str()) > 0)
		{
			debug = atoi(setting.c_str());
			alog("Debug mode activated (level %d)", debug);
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User *u, std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			noexpire = 1;
			alog("No expire mode activated");
			notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ON);
		}
		else if (setting == "OFF")
		{
			noexpire = 0;
			alog("No expire mode deactivated");
			notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_OFF);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ERROR);

		return MOD_CONT;
	}
 public:
	CommandOSSet() : Command("SET", 1, 2, "operserv/set")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		ci::string option = params[0];

		if (option == "LIST")
			return this->DoList(u);
		else if (option == "IGNORE")
			return this->DoSetIgnore(u, params);
		else if (option == "READONLY")
			return this->DoSetReadOnly(u, params);
		else if (option == "LOGCHAN")
			return this->DoSetLogChan(u, params);
		else if (option == "SUPERADMIN")
			return this->DoSetSuperAdmin(u, params);
		else if (option == "DEBUG")
			return this->DoSetDebug(u, params);
		else if (option == "NOEXPIRE")
			return this->DoSetNoExpire(u, params);
		else
			notice_lang(s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(s_OperServ, u, OPER_HELP_SET);
		else if (subcommand == "LIST")
			notice_help(s_OperServ, u, OPER_HELP_SET_LIST);
		else if (subcommand == "READONLY")
			notice_help(s_OperServ, u, OPER_HELP_SET_READONLY);
		else if (subcommand == "LOGCHAN")
			notice_help(s_OperServ, u, OPER_HELP_SET_LOGCHAN);
		else if (subcommand == "DEBUG")
			notice_help(s_OperServ, u, OPER_HELP_SET_DEBUG);
		else if (subcommand == "NOEXPIRE")
			notice_help(s_OperServ, u, OPER_HELP_SET_NOEXPIRE);
		else if (subcommand == "IGNORE")
			notice_help(s_OperServ, u, OPER_HELP_SET_IGNORE);
		else if (subcommand == "SUPERADMIN")
			notice_help(s_OperServ, u, OPER_HELP_SET_SUPERADMIN);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "SET", OPER_SET_SYNTAX);
	}
};

class OSSet : public Module
{
 public:
	OSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSet(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SET);
	}
};

MODULE_INIT("os_set", OSSet)
