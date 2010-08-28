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

class CommandOSSet : public Command
{
 private:
	CommandReturn DoList(User *u)
	{
		Log(LOG_ADMIN, u, this);

		int index;

		index = allow_ignore ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config->s_OperServ, u, index, "IGNORE");
		index = readonly ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config->s_OperServ, u, index, "READONLY");
		index = debug ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config->s_OperServ, u, index, "DEBUG");
		index = noexpire ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		notice_lang(Config->s_OperServ, u, index, "NOEXPIRE");

		return MOD_CONT;
	}

	CommandReturn DoSetIgnore(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "IGNORE");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			Log(LOG_ADMIN, u, this) << "IGNORE ON";
			allow_ignore = 1;
			notice_lang(Config->s_OperServ, u, OPER_SET_IGNORE_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, u, this) << "IGNORE OFF";
			allow_ignore = 0;
			notice_lang(Config->s_OperServ, u, OPER_SET_IGNORE_OFF);
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_SET_IGNORE_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetReadOnly(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "READONLY");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			readonly = true;
			Log(LOG_ADMIN, u, this) << "READONLY ON";
			notice_lang(Config->s_OperServ, u, OPER_SET_READONLY_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			readonly = false;
			Log(LOG_ADMIN, u, this) << "READONLY OFF";
			notice_lang(Config->s_OperServ, u, OPER_SET_READONLY_OFF);
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_SET_READONLY_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetSuperAdmin(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string setting = params.size() > 1 ? params[1] : "";

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
		if (!Config->SuperAdmin)
			notice_lang(Config->s_OperServ, u, OPER_SUPER_ADMIN_NOT_ENABLED);
		else if (setting.equals_ci("ON"))
		{
			u->isSuperAdmin = 1;
			notice_lang(Config->s_OperServ, u, OPER_SUPER_ADMIN_ON);
			Log(LOG_ADMIN, u, this) << "SUPERADMIN ON";
			ircdproto->SendGlobops(OperServ, getstring(OPER_SUPER_ADMIN_WALL_ON), u->nick.c_str());
		}
		else if (setting.equals_ci("OFF"))
		{
			u->isSuperAdmin = 0;
			notice_lang(Config->s_OperServ, u, OPER_SUPER_ADMIN_OFF);
			Log(LOG_ADMIN, u, this) << "SUPERADMIN OFF";
			ircdproto->SendGlobops(OperServ, getstring(OPER_SUPER_ADMIN_WALL_OFF), u->nick.c_str());
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetDebug(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "DEBUG");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			debug = 1;
			Log(LOG_ADMIN, u, this) << "DEBUG ON";
			notice_lang(Config->s_OperServ, u, OPER_SET_DEBUG_ON);
		}
		else if (setting.equals_ci("OFF") || (setting[0] == '0' && setting.is_number_only() && !convertTo<int>(setting)))
		{
			Log(LOG_ADMIN, u, this) << "DEBUG OFF";
			debug = 0;
			notice_lang(Config->s_OperServ, u, OPER_SET_DEBUG_OFF);
		}
		else if (setting.is_number_only() && convertTo<int>(setting) > 0)
		{
			debug = convertTo<int>(setting);
			Log(LOG_ADMIN, u, this) << "DEBUG " << debug;
			notice_lang(Config->s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_SET_DEBUG_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(u, "NOEXPIRE");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			noexpire = true;
			Log(LOG_ADMIN, u, this) << "NOEXPIRE ON";
			notice_lang(Config->s_OperServ, u, OPER_SET_NOEXPIRE_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			noexpire = false;
			Log(LOG_ADMIN, u, this) << "NOEXPIRE OFF";
			notice_lang(Config->s_OperServ, u, OPER_SET_NOEXPIRE_OFF);
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_SET_NOEXPIRE_ERROR);

		return MOD_CONT;
	}
 public:
	CommandOSSet() : Command("SET", 1, 2, "operserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string option = params[0];

		if (option.equals_ci("LIST"))
			return this->DoList(u);
		else if (option.equals_ci("IGNORE"))
			return this->DoSetIgnore(u, params);
		else if (option.equals_ci("READONLY"))
			return this->DoSetReadOnly(u, params);
		else if (option.equals_ci("SUPERADMIN"))
			return this->DoSetSuperAdmin(u, params);
		else if (option.equals_ci("DEBUG"))
			return this->DoSetDebug(u, params);
		else if (option.equals_ci("NOEXPIRE"))
			return this->DoSetNoExpire(u, params);
		else
			notice_lang(Config->s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config->s_OperServ, u, OPER_HELP_SET);
		else if (subcommand.equals_ci("LIST"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_LIST);
		else if (subcommand.equals_ci("READONLY"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_READONLY);
		else if (subcommand.equals_ci("DEBUG"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_DEBUG);
		else if (subcommand.equals_ci("NOEXPIRE"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_NOEXPIRE);
		else if (subcommand.equals_ci("IGNORE"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_IGNORE);
		else if (subcommand.equals_ci("SUPERADMIN"))
			notice_help(Config->s_OperServ, u, OPER_HELP_SET_SUPERADMIN);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "SET", OPER_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_SET);
	}
};

class OSSet : public Module
{
	CommandOSSet commandosset;

 public:
	OSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosset);
	}
};

MODULE_INIT(OSSet)
