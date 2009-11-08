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

class CommandBSSet : public Command
{
 public:
	CommandBSSet() : Command("SET", 3, 3)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string option = params[1];
		ci::string value = params[2];
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(s_BotServ, u, BOT_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->nc->HasCommand("botserv/set/private") && option == "PRIVATE")
		{
			BotInfo *bi;

			if (!(bi = findbot(chan)))
			{
				notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, chan);
				return MOD_CONT;
			}

			if (value == "ON")
			{
				bi->SetFlag(BI_PRIVATE);
				notice_lang(s_BotServ, u, BOT_SET_PRIVATE_ON, bi->nick);
			}
			else if (value == "OFF")
			{
				bi->UnsetFlag(BI_PRIVATE);
				notice_lang(s_BotServ, u, BOT_SET_PRIVATE_OFF, bi->nick);
			}
			else
			{
				syntax_error(s_BotServ, u, "SET PRIVATE", BOT_SET_PRIVATE_SYNTAX);
			}
			return MOD_CONT;
		} else if (!(ci = cs_findchan(chan)))
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (!u->nc->HasPriv("botserv/administration") && !check_access(u, ci, CA_SET))
			notice_lang(s_BotServ, u, ACCESS_DENIED);
		else {
			if (option == "DONTKICKOPS") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_DONTKICKOPS);
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_ON,
								ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_DONTKICKOPS);
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_OFF,
								ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET DONTKICKOPS",
								 BOT_SET_DONTKICKOPS_SYNTAX);
				}
			} else if (option == "DONTKICKVOICES") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_DONTKICKVOICES);
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_ON,
								ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_DONTKICKVOICES);
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_OFF,
								ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET DONTKICKVOICES",
								 BOT_SET_DONTKICKVOICES_SYNTAX);
				}
			} else if (option == "FANTASY") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_FANTASY);
					notice_lang(s_BotServ, u, BOT_SET_FANTASY_ON, ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_FANTASY);
					notice_lang(s_BotServ, u, BOT_SET_FANTASY_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET FANTASY",
								 BOT_SET_FANTASY_SYNTAX);
				}
			} else if (option == "GREET") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_GREET);
					notice_lang(s_BotServ, u, BOT_SET_GREET_ON, ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_GREET);
					notice_lang(s_BotServ, u, BOT_SET_GREET_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET GREET",
								 BOT_SET_GREET_SYNTAX);
				}
			} else if (u->nc->HasCommand("botserv/set/nobot") && option == "NOBOT") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_NOBOT);
					if (ci->bi)
						ci->bi->UnAssign(u, ci);
					notice_lang(s_BotServ, u, BOT_SET_NOBOT_ON, ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_NOBOT);
					notice_lang(s_BotServ, u, BOT_SET_NOBOT_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET NOBOT",
								 BOT_SET_NOBOT_SYNTAX);
				}
			} else if (option == "SYMBIOSIS") {
				if (value == "ON") {
					ci->botflags.SetFlag(BS_SYMBIOSIS);
					notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_ON, ci->name);
				} else if (value == "OFF") {
					ci->botflags.UnsetFlag(BS_SYMBIOSIS);
					notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET SYMBIOSIS",
								 BOT_SET_SYMBIOSIS_SYNTAX);
				}
			} else {
				notice_help(s_BotServ, u, BOT_SET_UNKNOWN, option.c_str());
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(s_BotServ, u, BOT_HELP_SET);
			if (u->nc && u->nc->IsServicesOper())
				notice_help(s_BotServ, u, BOT_SERVADMIN_HELP_SET);
		}
		else if (subcommand == "DONTKICKOPS")
			notice_help(s_BotServ, u, BOT_HELP_SET_DONTKICKOPS);
		else if (subcommand == "DONTKICKVOICES")
			notice_help(s_BotServ, u, BOT_HELP_SET_DONTKICKVOICES);
		else if (subcommand == "FANTASY")
			notice_help(s_BotServ, u, BOT_HELP_SET_FANTASY);
		else if (subcommand == "GREET")
			notice_help(s_BotServ, u, BOT_HELP_SET_GREET);
		else if (subcommand == "SYMBIOSIS")
		notice_lang(s_BotServ, u, BOT_HELP_SET_SYMBIOSIS, s_ChanServ);
		else if (subcommand == "NOBOT")
			notice_lang(s_BotServ, u, BOT_SERVADMIN_HELP_SET_NOBOT);
		else if (subcommand == "PRIVATE")
			notice_lang(s_BotServ, u, BOT_SERVADMIN_HELP_SET_PRIVATE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "SET", BOT_SET_SYNTAX);
	}
};
class BSSet : public Module
{
 public:
	BSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSSet());

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(s_BotServ, u, BOT_HELP_CMD_SET);
	}
};

MODULE_INIT(BSSet)
