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
	CommandReturn DoList(CommandSource &source)
	{
		Log(LOG_ADMIN, source.u, this);

		LanguageString index;

		index = allow_ignore ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		source.Reply(index, "IGNORE");
		index = readonly ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		source.Reply(index, "READONLY");
		index = debug ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		source.Reply(index, "DEBUG");
		index = noexpire ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF;
		source.Reply(index, "NOEXPIRE");

		return MOD_CONT;
	}

	CommandReturn DoSetIgnore(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "IGNORE");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			Log(LOG_ADMIN, u, this) << "ON";
			allow_ignore = 1;
			source.Reply(OPER_SET_IGNORE_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, u, this) << "OFF";
			allow_ignore = 0;
			source.Reply(OPER_SET_IGNORE_OFF);
		}
		else
			source.Reply(OPER_SET_IGNORE_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetReadOnly(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "READONLY");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			readonly = true;
			Log(LOG_ADMIN, u, this) << "READONLY ON";
			source.Reply(OPER_SET_READONLY_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			readonly = false;
			Log(LOG_ADMIN, u, this) << "READONLY OFF";
			source.Reply(OPER_SET_READONLY_OFF);
		}
		else
			source.Reply(OPER_SET_READONLY_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetSuperAdmin(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "SUPERADMIN");
			return MOD_CONT;
		}

		/**
		 * Allow the user to turn super admin on/off
		 *
		 * Rob
		 **/
		if (!Config->SuperAdmin)
			source.Reply(OPER_SUPER_ADMIN_NOT_ENABLED);
		else if (setting.equals_ci("ON"))
		{
			u->isSuperAdmin = 1;
			source.Reply(OPER_SUPER_ADMIN_ON);
			Log(LOG_ADMIN, u, this) << "SUPERADMIN ON";
			ircdproto->SendGlobops(OperServ, GetString(OPER_SUPER_ADMIN_WALL_ON).c_str(), u->nick.c_str());
		}
		else if (setting.equals_ci("OFF"))
		{
			u->isSuperAdmin = 0;
			source.Reply(OPER_SUPER_ADMIN_OFF);
			Log(LOG_ADMIN, u, this) << "SUPERADMIN OFF";
			ircdproto->SendGlobops(OperServ, GetString(OPER_SUPER_ADMIN_WALL_OFF).c_str(), u->nick.c_str());
		}
		else
			source.Reply(OPER_SUPER_ADMIN_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetDebug(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "DEBUG");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			debug = 1;
			Log(LOG_ADMIN, u, this) << "DEBUG ON";
			source.Reply(OPER_SET_DEBUG_ON);
		}
		else if (setting.equals_ci("OFF") || (setting[0] == '0' && setting.is_number_only() && !convertTo<int>(setting)))
		{
			Log(LOG_ADMIN, u, this) << "DEBUG OFF";
			debug = 0;
			source.Reply(OPER_SET_DEBUG_OFF);
		}
		else if (setting.is_number_only() && convertTo<int>(setting) > 0)
		{
			debug = convertTo<int>(setting);
			Log(LOG_ADMIN, u, this) << "DEBUG " << debug;
			source.Reply(OPER_SET_DEBUG_LEVEL, debug);
		}
		else
			source.Reply(OPER_SET_DEBUG_ERROR);

		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "NOEXPIRE");
			return MOD_CONT;
		}

		if (setting.equals_ci("ON"))
		{
			noexpire = true;
			Log(LOG_ADMIN, u, this) << "NOEXPIRE ON";
			source.Reply(OPER_SET_NOEXPIRE_ON);
		}
		else if (setting.equals_ci("OFF"))
		{
			noexpire = false;
			Log(LOG_ADMIN, u, this) << "NOEXPIRE OFF";
			source.Reply(OPER_SET_NOEXPIRE_OFF);
		}
		else
			source.Reply(OPER_SET_NOEXPIRE_ERROR);

		return MOD_CONT;
	}
 public:
	CommandOSSet() : Command("SET", 1, 2, "operserv/set")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &option = params[0];

		if (option.equals_ci("LIST"))
			return this->DoList(source);
		else if (option.equals_ci("IGNORE"))
			return this->DoSetIgnore(source, params);
		else if (option.equals_ci("READONLY"))
			return this->DoSetReadOnly(source, params);
		else if (option.equals_ci("SUPERADMIN"))
			return this->DoSetSuperAdmin(source, params);
		else if (option.equals_ci("DEBUG"))
			return this->DoSetDebug(source, params);
		else if (option.equals_ci("NOEXPIRE"))
			return this->DoSetNoExpire(source, params);
		else
			source.Reply(OPER_SET_UNKNOWN_OPTION, option.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			source.Reply(OPER_HELP_SET);
		else if (subcommand.equals_ci("LIST"))
			source.Reply(OPER_HELP_SET_LIST);
		else if (subcommand.equals_ci("READONLY"))
			source.Reply(OPER_HELP_SET_READONLY);
		else if (subcommand.equals_ci("NOEXPIRE"))
			source.Reply(OPER_HELP_SET_NOEXPIRE);
		else if (subcommand.equals_ci("IGNORE"))
			source.Reply(OPER_HELP_SET_IGNORE);
		else if (subcommand.equals_ci("SUPERADMIN"))
			source.Reply(OPER_HELP_SET_SUPERADMIN);
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", OPER_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_SET);
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
