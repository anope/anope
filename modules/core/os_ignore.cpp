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

class CommandOSIgnore : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string time = params.size() > 1 ? params[1] : "";
		Anope::string nick = params.size() > 2 ? params[2] : "";
		time_t t;

		if (time.empty() || nick.empty())
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}
		else
		{
			t = dotime(time);

			if (t <= -1)
			{
				u->SendMessage(OperServ, OPER_IGNORE_VALID_TIME);
				return MOD_CONT;
			}
			else if (!t)
			{
				add_ignore(nick, t);
				u->SendMessage(OperServ, OPER_IGNORE_PERM_DONE, nick.c_str());
			}
			else
			{
				add_ignore(nick, t);
				u->SendMessage(OperServ, OPER_IGNORE_TIME_DONE, nick.c_str(), time.c_str());
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u)
	{
		if (ignore.empty())
		{
			u->SendMessage(OperServ, OPER_IGNORE_LIST_EMPTY);
			return MOD_CONT;
		}

		u->SendMessage(OperServ, OPER_IGNORE_LIST);
		for (std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end(); ign != ign_end; ++ign)
			u->SendMessage(Config->s_OperServ, "%s", (*ign)->mask.c_str());

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params.size() > 1 ? params[1] : "";
		if (nick.empty())
			this->OnSyntaxError(u, "DEL");
		else
		{
			if (delete_ignore(nick))
			{
				u->SendMessage(OperServ, OPER_IGNORE_DEL_DONE, nick.c_str());
				return MOD_CONT;
			}
			u->SendMessage(OperServ, OPER_IGNORE_LIST_NOMATCH, nick.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		clear_ignores();
		u->SendMessage(OperServ, OPER_IGNORE_LIST_CLEARED);

		return MOD_CONT;
	}
 public:
	CommandOSIgnore() : Command("IGNORE", 1, 3, "operserv/ignore")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(u, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(u);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_IGNORE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "IGNORE", OPER_IGNORE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_IGNORE);
	}
};

class OSIgnore : public Module
{
	CommandOSIgnore commandosignore;

 public:
	OSIgnore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosignore);

		Implementation i[] = { I_OnDatabaseRead, I_OnDatabaseWrite };
		ModuleManager::Attach(i, this, 2);
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("OS") && params.size() >= 4 && params[1].equals_ci("IGNORE"))
		{
			IgnoreData *ign = new IgnoreData();
			ign->mask = params[2];
			ign->time = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
			ignore.push_front(ign);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const Anope::string &))
	{
		for (std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end(); ign != ign_end; )
		{
			if ((*ign)->time && (*ign)->time <= Anope::CurTime)
			{
				Log(LOG_DEBUG) << "[os_ignore] Expiring ignore entry " << (*ign)->mask;
				delete *ign;
				ign = ignore.erase(ign);
			}
			else
			{
				std::stringstream buf;
				buf << "OS IGNORE " << (*ign)->mask << " " << (*ign)->time;
				Write(buf.str());
				++ign;
			}
		}
	}
};

MODULE_INIT(OSIgnore)
