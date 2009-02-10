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
	CommandReturn DoServAdminList(User *u, std::vector<std::string> &params, NickAlias *na)
	{
		const char *mask = params.size() > 2 ? params[2].c_str() : NULL;
		char **access;
		int i;

		if (!na->nc->accesscount)
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X_EMPTY, na->nick);
			return MOD_CONT;
		}

		if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		if (na->nc->flags & NI_SUSPENDED)
		{
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X, params[1].c_str());
		for (access = na->nc->access, i = 0; i < na->nc->accesscount; ++access, ++i)
		{
			if (mask && !match_wild(mask, *access))
				continue;
			notice_user(s_NickServ, u, "    %s", *access);
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, std::vector<std::string> &params, NickAlias *na, const char *mask)
	{
		char **access;
		int i;

		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (na->nc->accesscount >= NSAccessMax)
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_REACHED_LIMIT, NSAccessMax);
			return MOD_CONT;
		}

		for (access = na->nc->access, i = 0; i < na->nc->accesscount; ++access, ++i)
		{
			if (!strcmp(*access, mask))
			{
				notice_lang(s_NickServ, u, NICK_ACCESS_ALREADY_PRESENT, *access);
				return MOD_CONT;
			}
		}

		++na->nc->accesscount;
		na->nc->access = static_cast<char **>(srealloc(na->nc->access, sizeof(char *) * na->nc->accesscount));
		na->nc->access[na->nc->accesscount - 1] = sstrdup(mask);
		notice_lang(s_NickServ, u, NICK_ACCESS_ADDED, mask);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params, NickAlias *na, const char *mask)
	{
		char **access;
		int i;

		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		for (access = na->nc->access, i = 0; i < na->nc->accesscount; ++access, ++i)
		{
			if (!stricmp(*access, mask))
				break;
		}
		if (i == na->nc->accesscount)
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_NOT_FOUND, mask);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_DELETED, *access);
		delete [] *access;
		--na->nc->accesscount;
		if (i < na->nc->accesscount) /* if it wasn't the last entry... */
			memmove(access, access + 1, (na->nc->accesscount - i) * sizeof(char *));
		if (na->nc->accesscount) /* if there are any entries left... */
			na->nc->access = static_cast<char **>(srealloc(na->nc->access, na->nc->accesscount * sizeof(char *)));
		else
		{
			free(na->nc->access);
			na->nc->access = NULL;
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params, NickAlias *na, const char *mask)
	{
		char **access;
		int i;

		if (!na->nc->accesscount)
		{
			notice_lang(s_NickServ, u, NICK_ACCESS_LIST_EMPTY, u->nick);
			return MOD_CONT;
		}

		notice_lang(s_NickServ, u, NICK_ACCESS_LIST);
		for (access = na->nc->access, i = 0; i < na->nc->accesscount; ++access, ++i)
		{
			if (mask && !match_wild(mask, *access))
				continue;
			notice_user(s_NickServ, u, "    %s", *access);
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

		if (!stricmp(cmd, "LIST") && is_services_admin(u) && mask && (na = findnick(params[1].c_str())))
			return this->DoServAdminList(u, params, na);

		if (mask && !strchr(mask, '@'))
		{
			notice_lang(s_NickServ, u, BAD_USERHOST_MASK);
			notice_lang(s_NickServ, u, MORE_INFO, s_NickServ, "ACCESS");

		}
		else if (!(na = u->na))
			notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->flags & NI_SUSPENDED)
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!nick_identified(u))
			notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params, na, mask);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params, na, mask);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params, na, mask);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
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
