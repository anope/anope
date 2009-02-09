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

class CommandOSSet : public Command
{
 private:
	CommandResult DoList(User *u, std::vector<std::string> &params)
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

	CommandResult DoSetIgnore(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;

		if (!setting)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(setting, "ON"))
		{
			allow_ignore = 1;
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_ON);
		}
		else if (!stricmp(setting, "OFF"))
		{
			allow_ignore = 0;
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_OFF);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_IGNORE_ERROR);

		return MOD_CONT;
	}

	CommandResult DoSetReadOnly(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;

		if (!setting)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(setting, "ON"))
		{
			readonly = 1;
			alog("Read-only mode activated");
			close_log();
			notice_lang(s_OperServ, u, OPER_SET_READONLY_ON);
		}
		else if (!stricmp(setting, "OFF"))
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

	CommandResult DoSetLogChan(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;
		Channel *c;

		if (!setting)
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
		if (LogChannel && !stricmp(setting, "ON"))
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
		else if (LogChannel && !stricmp(setting, "OFF"))
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

	CommandResult DoSetSuperAdmin(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;

		if (!setting)
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
		else if (!stricmp(setting, "ON"))
		{
			u->isSuperAdmin = 1;
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ON);
			alog("%s: %s is a SuperAdmin ", s_OperServ, u->nick);
			ircdproto->SendGlobops(s_OperServ, getstring2(NULL, OPER_SUPER_ADMIN_WALL_ON), u->nick);
		}
		else if (!stricmp(setting, "OFF"))
		{
			u->isSuperAdmin = 0;
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_OFF);
			alog("%s: %s is no longer a SuperAdmin", s_OperServ, u->nick);
			ircdproto->SendGlobops(s_OperServ, getstring2(NULL, OPER_SUPER_ADMIN_WALL_OFF), u->nick);
		}
		else
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);

		return MOD_CONT;
	}

	CommandResult DoSetDebug(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;

		if (!setting)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(setting, "ON"))
		{
			debug = 1;
			alog("Debug mode activated");
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_ON);
		}
		else if (!stricmp(setting, "OFF") || (*setting == '0' && !atoi(setting)))
		{
			alog("Debug mode deactivated");
			debug = 0;
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_OFF);
		}
		else if (isdigit(*setting) && atoi(setting) > 0)
		{
			debug = atoi(setting);
			alog("Debug mode activated (level %d)", debug);
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
		}
		else
			notice_lang(s_OperServ, u, OPER_SET_DEBUG_ERROR);

		return MOD_CONT;
	}

	CommandResult DoSetNoExpire(User *u, std::vector<std::string> &params)
	{
		const char *setting = params.size() > 1 ? params[1].c_str() : NULL;

		if (!setting)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(setting, "ON"))
		{
			noexpire = 1;
			alog("No expire mode activated");
			notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ON);
		}
		else if (!stricmp(setting, "OFF"))
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
	CommandOSSet() : Command("SET", 1, 2)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		char *option = params[0].c_str();
		char *setting = strtok(NULL, " ");
		int index;
		Channel *c;

		if (!stricmp(option, "LIST"))
			return this->DoList(u, params);
		else if (!stricmp(option, "IGNORE"))
			return this->DoSetIgnore(u, params);
		else if (!stricmp(option, "READONLY"))
			return this->DoSetReadOnly(u, params);
		else if (!stricmp(option, "LOGCHAN"))
			return this->DoSetLogChan(u, params);
		else if (!stricmp(option, "SUPERADMIN"))
			return this->DoSetSuperAdmin(u, params);
		else if (!stricmp(option, "DEBUG"))
			return this->DoSetDebug(u, params);
		else if (!stricmp(option, "NOEXPIRE"))
			return this->DoSetNoExpire(u, params);
		else
			notice_lang(s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		// This needs to change XXX
		notice_lang(s_OperServ, u, OPER_HELP_SET);
		notice_lang(s_OperServ, u, OPER_HELP_SET_LIST);
		notice_lang(s_OperServ, u, OPER_HELP_SET_READONLY);
		notice_lang(s_OperServ, u, OPER_HELP_SET_LOGCHAN);
		notice_lang(s_OperServ, u, OPER_HELP_SET_DEBUG);
		notice_lang(s_OperServ, u, OPER_HELP_SET_NOEXPIRE);
		notice_lang(s_OperServ, u, OPER_HELP_SET_IGNORE);
		notice_lang(s_OperServ, u, OPER_HELP_SET_SUPERADMIN);

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

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_root(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SET);
}

MODULE_INIT("os_set", OSSet)
