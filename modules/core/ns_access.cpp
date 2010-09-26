/* NickServ core functions
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

class CommandNSAccess : public Command
{
 private:
	CommandReturn DoServAdminList(User *u, const std::vector<Anope::string> &params, NickCore *nc)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";
		unsigned i, end;

		if (nc->access.empty())
		{
			u->SendMessage(NickServ, NICK_ACCESS_LIST_X_EMPTY, nc->display.c_str());
			return MOD_CONT;
		}

		if (nc->HasFlag(NI_SUSPENDED))
		{
			u->SendMessage(NickServ, NICK_X_SUSPENDED, nc->display.c_str());
			return MOD_CONT;
		}

		u->SendMessage(NickServ, NICK_ACCESS_LIST_X, params[1].c_str());
		for (i = 0, end = nc->access.size(); i < end; ++i)
		{
			Anope::string access = nc->GetAccess(i);
			if (!mask.empty() && !Anope::Match(access, mask))
				continue;
			u->SendMessage(Config->s_NickServ, "    %s", access.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, NickCore *nc, const Anope::string &mask)
	{
		if (mask.empty())
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		if (nc->access.size() >= Config->NSAccessMax)
		{
			u->SendMessage(NickServ, NICK_ACCESS_REACHED_LIMIT, Config->NSAccessMax);
			return MOD_CONT;
		}

		if (nc->FindAccess(mask))
		{
			u->SendMessage(NickServ, NICK_ACCESS_ALREADY_PRESENT, mask.c_str());
			return MOD_CONT;
		}

		nc->AddAccess(mask);
		u->SendMessage(NickServ, NICK_ACCESS_ADDED, mask.c_str());

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, NickCore *nc, const Anope::string &mask)
	{
		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!nc->FindAccess(mask))
		{
			u->SendMessage(NickServ, NICK_ACCESS_NOT_FOUND, mask.c_str());
			return MOD_CONT;
		}

		u->SendMessage(NickServ, NICK_ACCESS_DELETED, mask.c_str());
		nc->EraseAccess(mask);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, NickCore *nc, const Anope::string &mask)
	{
		unsigned i, end;

		if (nc->access.empty())
		{
			u->SendMessage(NickServ, NICK_ACCESS_LIST_EMPTY, u->nick.c_str());
			return MOD_CONT;
		}

		u->SendMessage(NickServ, NICK_ACCESS_LIST);
		for (i = 0, end = nc->access.size(); i < end; ++i)
		{
			Anope::string access = nc->GetAccess(i);
			if (!mask.empty() && !Anope::Match(access, mask))
				continue;
			u->SendMessage(Config->s_NickServ, "    %s", access.c_str());
		}

		return MOD_CONT;
	}
 public:
	CommandNSAccess() : Command("ACCESS", 1, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];
		Anope::string mask = params.size() > 1 ? params[1] : "";
		NickAlias *na;

		if (cmd.equals_ci("LIST") && u->Account()->IsServicesOper() && !mask.empty() && (na = findnick(params[1])))
			return this->DoServAdminList(u, params, na->nc);

		if (!mask.empty() && mask.find('@') == Anope::string::npos)
		{
			u->SendMessage(NickServ, BAD_USERHOST_MASK);
			u->SendMessage(NickServ, MORE_INFO, Config->s_NickServ.c_str(), "ACCESS");
		}
		/*
		else if (na->HasFlag(NS_FORBIDDEN))
			u->SendMessage(NickServ, NICK_X_FORBIDDEN, na->nick);
			*/
		else if (u->Account()->HasFlag(NI_SUSPENDED))
			u->SendMessage(NickServ, NICK_X_SUSPENDED, u->Account()->display.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, u->Account(), mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(u, u->Account(), mask);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u, u->Account(), mask);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(NickServ, NICK_HELP_ACCESS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_ACCESS);
	}
};

class NSAccess : public Module
{
	CommandNSAccess commandnsaccess;

 public:
	NSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsaccess);
	}
};

MODULE_INIT(NSAccess)
