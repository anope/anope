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

int do_unassign(User * u);
void myBotServHelp(User * u);

class CommandBSUnassign : public Command
{
 public:
	CommandBSUnassign() : Command("UNASSIGN", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		ChannelInfo *ci;

		if (readonly)
			notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
		else if (!(ci = cs_findchan(chan)))
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (ci->flags & CI_VERBOTEN)
			notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
		else if (!is_services_admin(u) && !check_access(u, ci, CA_ASSIGN))
			notice_lang(s_BotServ, u, ACCESS_DENIED);
		else if (!ci->bi)
			notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
		else {
			ci->bi->UnAssign(u, ci);
			notice_lang(s_BotServ, u, BOT_UNASSIGN_UNASSIGNED, ci->name);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_BotServ, u, BOT_HELP_UNASSIGN);
		return true;
	}

	void OnBadSyntax(User *u)
	{
		syntax_error(s_BotServ, u, "UNASSIGN", BOT_UNASSIGN_SYNTAX);
	}
};

class BSUnassign : public Module
{
 public:
	BSUnassign(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSUnassign, MOD_UNIQUE);

		this->SetBotHelp(myBotServHelp);
	}
};



/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
	notice_lang(s_BotServ, u, BOT_HELP_CMD_UNASSIGN);
}

MODULE_INIT("bs_unassign", BSUnassign)
