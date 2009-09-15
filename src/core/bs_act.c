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

class CommandBSAct : public Command
{
 public:
	CommandBSAct() : Command("ACT", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0].c_str());

		if (!check_access(u, ci, CA_SAY))
		{
			notice_lang(s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
			return MOD_CONT;
		}

		if (!ci->c || ci->c->usercount < BSMinUsers)
		{
			notice_lang(s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name);
			return MOD_CONT;
		}

		size_t i = 0;
		while ((i = params[1].find_first_of("\001"), i) && i != std::string::npos)
			params[1].erase(i, 1);

		ircdproto->SendAction(ci->bi, ci->name, "%s", params[1].c_str());
		ci->bi->lastmsg = time(NULL);
		if (LogBot && LogChannel && logchan && !debug && findchan(LogChannel))
			ircdproto->SendPrivmsg(ci->bi, LogChannel, "ACT %s %s %s", u->nick, ci->name, params[1].c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "ACT", BOT_ACT_SYNTAX);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_BotServ, u, BOT_HELP_ACT);
		return true;
	}
};

class BSAct : public Module
{
 public:
	BSAct(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSAct());
	}
	void BotServHelp(User *u)
	{
		notice_lang(s_BotServ, u, BOT_HELP_CMD_ACT);
	}
};

MODULE_INIT("bs_act", BSAct)
