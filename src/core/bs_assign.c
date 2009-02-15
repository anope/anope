/* BotServ core functions
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

void myBotServHelp(User * u);

class CommandBSAssign : public Command
{
 public:
	CommandBSAssign() : Command("ASSIGN", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		BotInfo *bi;
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
			return MOD_CONT;
		}

		if (!(bi = findbot(nick)))
		{
			notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
			return MOD_CONT;
		}

		if (bi->flags & BI_PRIVATE && !u->na->nc->HasCommand("botserv/assign/private"))
		{
			notice_lang(s_BotServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (ci->flags & CI_FORBIDDEN)
		{
			notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if ((ci->bi) && (stricmp(ci->bi->nick, nick) == 0))
		{
			notice_lang(s_BotServ, u, BOT_ASSIGN_ALREADY, ci->bi->nick, chan);
			return MOD_CONT;
		}
<<<<<<< HEAD:src/core/bs_assign.c

		if ((ci->botflags & BS_NOBOT) || (!check_access(u, ci, CA_ASSIGN) && !is_services_admin(u)))
=======
		
		if ((ci->botflags & BS_NOBOT) || (!check_access(u, ci, CA_ASSIGN) && !u->na->nc->HasCommand("botserv/administration")))
>>>>>>> Set required command string for botserv modules.:src/core/bs_assign.c
		{
			notice_lang(s_BotServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		bi->Assign(u, ci);
		notice_lang(s_BotServ, u, BOT_ASSIGN_ASSIGNED, bi->nick, ci->name);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_BotServ, u, BOT_HELP_ASSIGN);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "ASSIGN", BOT_ASSIGN_SYNTAX);
	}
};

class BSAssign : public Module
{
 public:
	BSAssign(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSAssign, MOD_UNIQUE);

		this->SetBotHelp(myBotServHelp);
	}
};

/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
	notice_lang(s_BotServ, u, BOT_HELP_CMD_ASSIGN);
}

MODULE_INIT("bs_assign", BSAssign)
