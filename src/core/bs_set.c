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

class CommandBSSet : public Command
{
 public:
	CommandBSSet() : Command("SET", 3, 3)
	{
		this->help_param1 = s_ChanServ;
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *option = params[1].c_str();
		const char *value = params[2].c_str();
		int is_servadmin = is_services_admin(u);
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(s_BotServ, u, BOT_SET_DISABLED);
			return MOD_CONT;
		}

		if (is_servadmin && !stricmp(option, "PRIVATE"))
		{
			BotInfo *bi;

			if (!(bi = findbot(chan)))
			{
				notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, chan);
				return MOD_CONT;
			}

			if (!stricmp(value, "ON"))
			{
				bi->flags |= BI_PRIVATE;
				notice_lang(s_BotServ, u, BOT_SET_PRIVATE_ON, bi->nick);
			}
			else if (!stricmp(value, "OFF"))
			{
				bi->flags &= ~BI_PRIVATE;
				notice_lang(s_BotServ, u, BOT_SET_PRIVATE_OFF, bi->nick);
			}
			else
			{
				syntax_error(s_BotServ, u, "SET PRIVATE", BOT_SET_PRIVATE_SYNTAX);
			}
			return MOD_CONT;
		} else if (!(ci = cs_findchan(chan)))
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (ci->flags & CI_FORBIDDEN)
			notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
		else if (!is_servadmin && !check_access(u, ci, CA_SET))
			notice_lang(s_BotServ, u, ACCESS_DENIED);
		else {
			if (!stricmp(option, "DONTKICKOPS")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_DONTKICKOPS;
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_ON,
								ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_DONTKICKOPS;
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_OFF,
								ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET DONTKICKOPS",
								 BOT_SET_DONTKICKOPS_SYNTAX);
				}
			} else if (!stricmp(option, "DONTKICKVOICES")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_DONTKICKVOICES;
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_ON,
								ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_DONTKICKVOICES;
					notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_OFF,
								ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET DONTKICKVOICES",
								 BOT_SET_DONTKICKVOICES_SYNTAX);
				}
			} else if (!stricmp(option, "FANTASY")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_FANTASY;
					notice_lang(s_BotServ, u, BOT_SET_FANTASY_ON, ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_FANTASY;
					notice_lang(s_BotServ, u, BOT_SET_FANTASY_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET FANTASY",
								 BOT_SET_FANTASY_SYNTAX);
				}
			} else if (!stricmp(option, "GREET")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_GREET;
					notice_lang(s_BotServ, u, BOT_SET_GREET_ON, ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_GREET;
					notice_lang(s_BotServ, u, BOT_SET_GREET_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET GREET",
								 BOT_SET_GREET_SYNTAX);
				}
			} else if (is_servadmin && !stricmp(option, "NOBOT")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_NOBOT;
					if (ci->bi)
						ci->bi->UnAssign(u, ci);
					notice_lang(s_BotServ, u, BOT_SET_NOBOT_ON, ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_NOBOT;
					notice_lang(s_BotServ, u, BOT_SET_NOBOT_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET NOBOT",
								 BOT_SET_NOBOT_SYNTAX);
				}
			} else if (!stricmp(option, "SYMBIOSIS")) {
				if (!stricmp(value, "ON")) {
					ci->botflags |= BS_SYMBIOSIS;
					notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_ON, ci->name);
				} else if (!stricmp(value, "OFF")) {
					ci->botflags &= ~BS_SYMBIOSIS;
					notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_OFF, ci->name);
				} else {
					syntax_error(s_BotServ, u, "SET SYMBIOSIS",
								 BOT_SET_SYMBIOSIS_SYNTAX);
				}
			} else {
				notice_help(s_BotServ, u, BOT_SET_UNKNOWN, option);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		// This needs to change XXX
		notice_lang(s_BotServ, u, BOT_HELP_SET);
		notice_lang(s_BotServ, u, BOT_HELP_SET_DONTKICKOPS);
		notice_lang(s_BotServ, u, BOT_HELP_SET_DONTKICKVOICES);
		notice_lang(s_BotServ, u, BOT_HELP_SET_FANTASY);
		notice_lang(s_BotServ, u, BOT_HELP_SET_GREET);
		notice_lang(s_BotServ, u, BOT_HELP_SET_SYMBIOSIS);

		if (is_services_admin(u) || is_services_root(u))
		{
			notice_lang(s_BotServ, u, BOT_SERVADMIN_HELP_SET);
			notice_lang(s_BotServ, u, BOT_SERVADMIN_HELP_SET_NOBOT);
			notice_lang(s_BotServ, u, BOT_SERVADMIN_HELP_SET_PRIVATE);
		}

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
		this->AddCommand(BOTSERV, new CommandBSSet(), MOD_UNIQUE);

		this->SetBotHelp(myBotServHelp);
	}
};


/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
	notice_lang(s_BotServ, u, BOT_HELP_CMD_SET);
}

MODULE_INIT("bs_set", BSSet)
