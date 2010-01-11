/* BotServ core functions
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

class CommandBSAct : public Command
{
 public:
	CommandBSAct() : Command("ACT", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0].c_str());
		ci::string message = params[1];

		if (!check_access(u, ci, CA_SAY))
		{
			notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			notice_help(Config.s_BotServ, u, BOT_NOT_ASSIGNED);
			return MOD_CONT;
		}

		if (!ci->c || ci->c->usercount < Config.BSMinUsers)
		{
			notice_lang(Config.s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return MOD_CONT;
		}

		size_t i = 0;
		while ((i = message.find_first_of("\001"), i) && i != std::string::npos)
			message.erase(i, 1);

		ircdproto->SendAction(ci->bi, ci->name.c_str(), "%s", message.c_str());
		ci->bi->lastmsg = time(NULL);
		if (Config.LogBot && Config.LogChannel && LogChan && !debug && findchan(Config.LogChannel))
			ircdproto->SendPrivmsg(ci->bi, Config.LogChannel, "ACT %s %s %s", u->nick.c_str(), ci->name.c_str(), message.c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "ACT", BOT_ACT_SYNTAX);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_ACT);
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

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_ACT);
	}
};

MODULE_INIT(BSAct)
