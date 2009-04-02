/* NickServ core functions
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

void myNickServHelp(User *u);

class CommandNSAccess : public Command
{
 private:
	CommandReturn DoServAdminList(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *mask = params.size() > 2 ? params[2].c_str() : NULL;
		unsigned i;

		if (nc->access.empty())
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X_EMPTY, nc->display);
			return MOD_CONT;
		}

/* reinstate when forbidden is moved to a group flag
		if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}
*/

		if (nc->flags & NI_SUSPENDED)
		{
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, nc->display);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X, params[1].c_str());
		for (i = 0; i < nc->access.size(); ++i)
		{
			std::string access = nc->GetAccess(i);
			if (mask && !Anope::Match(access, mask, true))
				continue;
			u->SendMessage(s_NickServ, "    %s", access.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, std::vector<std::string> &params, NickCore *nc, const char *mask)
	{
		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (nc->access.size() >= NSAccessMax)
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_REACHED_LIMIT, NSAccessMax);
			return MOD_CONT;
		}

		if (nc->FindAccess(mask))
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_ALREADY_PRESENT, *access);
			return MOD_CONT;
		}

		nc->AddAccess(mask);
		notice_lang(s_NickServ, u, NICK_ACCESS_ADDED, mask);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params, NickCore *nc, const char *mask)
	{
		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!nc->FindAccess(mask))
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_NOT_FOUND, mask);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_DELETED, mask);
		nc->EraseAccess(mask);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params, NickCore *nc, const char *mask)
	{
		unsigned i;

		if (nc->access.empty())
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_LIST_EMPTY, u->nick);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_LIST);
		for (i = 0; i < nc->access.size(); ++i)
		{
			std::string access = nc->GetAccess(i);
			if (mask && !Anope::Match(access, mask, true))
				continue;
			u->SendMessage(s_NickServ, "    %s", access.c_str());
		}

		return MOD_CONT;
	}
 public:
	CommandNSAccess() : Command("ACCESS", 1, 3)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;
		NickAlias *na;

		if (!stricmp(cmd, "LIST") && u->nc->IsServicesOper() && mask && (na = findnick(params[1].c_str())))
			return this->DoServAdminList(u, params, na->nc);

		if (mask && !strchr(mask, '@'))
		{
			notice_lang(s_NickServ, u, BAD_USERHOST_MASK);
			notice_lang(s_NickServ, u, MORE_INFO, s_NickServ, "ACCESS");

		}
		/*
		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
			*/
		else if (u->nc->flags & NI_SUSPENDED)
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nc->display);
		else if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params, u->nc, mask);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params, u->nc, mask);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params, u->nc, mask);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_ACCESS);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);
	}
};

class NSAccess : public Module
{
 public:
	NSAccess(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSAccess(), MOD_UNIQUE);

		this->SetNickHelp(myNickServHelp);
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User *u)
{
	notice_lang(s_NickServ, u, NICK_HELP_CMD_ACCESS);
}

MODULE_INIT("ns_access", NSAccess)
