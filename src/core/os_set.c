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
		notice_lang(Config.s_OperServ, u, index, "IGNORE");
		index = readonly ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config.s_OperServ, u, index, "READONLY");
		index = LogChan ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config.s_OperServ, u, index, "LOGCHAN");
		index = debug ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config.s_OperServ, u, index, "DEBUG");
		index = noexpire ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config.s_OperServ, u, index, "NOEXPIRE");

		return MOD_CONT;
	}

	CommandReturn DoSetIgnore(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "IGNORE");
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			allow_ignore = 1;
			notice_lang(Config.s_OperServ, u, OPER_SET_IGNORE_ON);
		}
		else if (setting == "OFF")
		{
			allow_ignore = 0;
			notice_lang(Config.s_OperServ, u, OPER_SET_IGNORE_OFF);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SET_IGNORE_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetReadOnly(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "READONLY");
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			readonly = 1;
			Alog() << "Read-only mode activated";
			close_log();
			notice_lang(Config.s_OperServ, u, OPER_SET_READONLY_ON);
		}
		else if (setting == "OFF")
		{
			readonly = 0;
			open_log();
			Alog() << "Read-only mode deactivated";
			notice_lang(Config.s_OperServ, u, OPER_SET_READONLY_OFF);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SET_READONLY_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetLogChan(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";
		Channel *c;

		if (setting.empty())
		{
			this->OnSyntaxError(u, "LOGCHAN");
			return MOD_CONT;
		}

		/* Unlike the other SET commands where only stricmp is necessary,
		 * we also have to ensure that Config.LogChannel is defined or we can't
		 * send to it.
		 *
		 * -jester
		 */
		if (Config.LogChannel && setting == "ON")
		{
			if (ircd->join2msg)
			{
				c = findchan(Config.LogChannel);
				ircdproto->SendJoin(findbot(Config.s_GlobalNoticer), Config.LogChannel, c ? c->creation_time : time(NULL));
			}
			LogChan = true;
			Alog() << "Now sending log messages to " << Config.LogChannel;
			notice_lang(Config.s_OperServ, u, OPER_SET_LOGCHAN_ON, Config.LogChannel);
		}
		else if (Config.LogChannel && setting == "OFF")
		{
			Alog() << "No longer sending log messages to a channel";
			if (ircd->join2msg)
				ircdproto->SendPart(findbot(Config.s_GlobalNoticer), findchan(Config.LogChannel), NULL);
			LogChan = false;
			notice_lang(Config.s_OperServ, u, OPER_SET_LOGCHAN_OFF);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SET_LOGCHAN_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetSuperAdmin(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "SUPERADMIN");
			return MOD_CONT;
		}

		/**
		 * Allow the user to turn super admin on/off
		 *
		 * Rob
		 **/
		if (!Config.SuperAdmin)
			notice_lang(Config.s_OperServ, u, OPER_SUPER_ADMIN_NOT_ENABLED);
		else if (setting == "ON")
		{
			u->isSuperAdmin = 1;
			notice_lang(Config.s_OperServ, u, OPER_SUPER_ADMIN_ON);
			Alog() << Config.s_OperServ << ": " << u->nick << " is a SuperAdmin";
			ircdproto->SendGlobops(findbot(Config.s_OperServ), getstring(OPER_SUPER_ADMIN_WALL_ON), u->nick.c_str());
		}
		else if (setting == "OFF")
		{
			u->isSuperAdmin = 0;
			notice_lang(Config.s_OperServ, u, OPER_SUPER_ADMIN_OFF);
			Alog() << Config.s_OperServ << ": " << u->nick << " is no longer a SuperAdmin";
			ircdproto->SendGlobops(findbot(Config.s_OperServ), getstring(OPER_SUPER_ADMIN_WALL_OFF), u->nick.c_str());
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetDebug(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "DEBUG");
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			debug = 1;
			Alog() << "Debug mode activated";
			notice_lang(Config.s_OperServ, u, OPER_SET_DEBUG_ON);
		}
		else if (setting == "OFF" || (setting[0] == '0' && !atoi(setting.c_str())))
		{
			Alog() << "Debug mode deactivated";
			debug = 0;
			notice_lang(Config.s_OperServ, u, OPER_SET_DEBUG_OFF);
		}
		else if (isdigit(setting[0]) && atoi(setting.c_str()) > 0)
		{
			debug = atoi(setting.c_str());
			Alog() << "Debug mode activated (level " << debug << ")";
			notice_lang(Config.s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SET_DEBUG_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User *u, const std::vector<ci::string> &params)
	{
		ci::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "NOEXPIRE");
			return MOD_CONT;
		}

		if (setting == "ON")
		{
			noexpire = 1;
			Alog() << "No expire mode activated";
			notice_lang(Config.s_OperServ, u, OPER_SET_NOEXPIRE_ON);
		}
		else if (setting == "OFF")
		{
			noexpire = 0;
			Alog() << "No expire mode deactivated";
			notice_lang(Config.s_OperServ, u, OPER_SET_NOEXPIRE_OFF);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_SET_NOEXPIRE_ERROR);

		return MOD_CONT;
	}
 public:
	CommandOSSet() : Command("SET", 1, 2, "operserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
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
			notice_lang(Config.s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config.s_OperServ, u, OPER_HELP_SET);
		else if (subcommand == "LIST")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_LIST);
		else if (subcommand == "READONLY")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_READONLY);
		else if (subcommand == "LOGCHAN")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_LOGCHAN);
		else if (subcommand == "DEBUG")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_DEBUG);
		else if (subcommand == "NOEXPIRE")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_NOEXPIRE);
		else if (subcommand == "IGNORE")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_IGNORE);
		else if (subcommand == "SUPERADMIN")
			notice_help(Config.s_OperServ, u, OPER_HELP_SET_SUPERADMIN);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SET", OPER_SET_SYNTAX);
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

		this->AddCommand(OPERSERV, new CommandOSSet());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SET);
	}
};

MODULE_INIT(OSSet)
