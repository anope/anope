/* BotServ core functions
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

class CommandBSSet : public Command
{
 public:
	CommandBSSet() : Command("SET", 3, 3)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string option = params[1];
		Anope::string value = params[2];
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(Config->s_BotServ, u, BOT_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->Account()->HasCommand("botserv/set/private") && option.equals_ci("PRIVATE"))
		{
			BotInfo *bi;

			if (!(bi = findbot(chan)))
			{
				notice_lang(Config->s_BotServ, u, BOT_DOES_NOT_EXIST, chan.c_str());
				return MOD_CONT;
			}

			if (value.equals_ci("ON"))
			{
				bi->SetFlag(BI_PRIVATE);
				notice_lang(Config->s_BotServ, u, BOT_SET_PRIVATE_ON, bi->nick.c_str());
			}
			else if (value.equals_ci("OFF"))
			{
				bi->UnsetFlag(BI_PRIVATE);
				notice_lang(Config->s_BotServ, u, BOT_SET_PRIVATE_OFF, bi->nick.c_str());
			}
			else
				syntax_error(Config->s_BotServ, u, "SET PRIVATE", BOT_SET_PRIVATE_SYNTAX);
			return MOD_CONT;
		}
		else if (!(ci = cs_findchan(chan)))
			notice_lang(Config->s_BotServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
		else if (!u->Account()->HasPriv("botserv/administration") && !check_access(u, ci, CA_SET))
			notice_lang(Config->s_BotServ, u, ACCESS_DENIED);
		else
		{
			bool override = !check_access(u, ci, CA_SET);
			Log(override ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << option << value;

			if (option.equals_ci("DONTKICKOPS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKOPS);
					notice_lang(Config->s_BotServ, u, BOT_SET_DONTKICKOPS_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKOPS);
					notice_lang(Config->s_BotServ, u, BOT_SET_DONTKICKOPS_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET DONTKICKOPS", BOT_SET_DONTKICKOPS_SYNTAX);
			}
			else if (option.equals_ci("DONTKICKVOICES"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKVOICES);
					notice_lang(Config->s_BotServ, u, BOT_SET_DONTKICKVOICES_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKVOICES);
					notice_lang(Config->s_BotServ, u, BOT_SET_DONTKICKVOICES_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET DONTKICKVOICES", BOT_SET_DONTKICKVOICES_SYNTAX);
			}
			else if (option.equals_ci("FANTASY"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_FANTASY);
					notice_lang(Config->s_BotServ, u, BOT_SET_FANTASY_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_FANTASY);
					notice_lang(Config->s_BotServ, u, BOT_SET_FANTASY_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET FANTASY", BOT_SET_FANTASY_SYNTAX);
			}
			else if (option.equals_ci("GREET"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_GREET);
					notice_lang(Config->s_BotServ, u, BOT_SET_GREET_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_GREET);
					notice_lang(Config->s_BotServ, u, BOT_SET_GREET_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET GREET", BOT_SET_GREET_SYNTAX);
			}
			else if (u->Account()->HasCommand("botserv/set/nobot") && option.equals_ci("NOBOT"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_NOBOT);
					if (ci->bi)
						ci->bi->UnAssign(u, ci);
					notice_lang(Config->s_BotServ, u, BOT_SET_NOBOT_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_NOBOT);
					notice_lang(Config->s_BotServ, u, BOT_SET_NOBOT_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET NOBOT", BOT_SET_NOBOT_SYNTAX);
			}
			else if (option.equals_ci("SYMBIOSIS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_SYMBIOSIS);
					notice_lang(Config->s_BotServ, u, BOT_SET_SYMBIOSIS_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_SYMBIOSIS);
					notice_lang(Config->s_BotServ, u, BOT_SET_SYMBIOSIS_OFF, ci->name.c_str());
				}
				else
					syntax_error(Config->s_BotServ, u, "SET SYMBIOSIS", BOT_SET_SYMBIOSIS_SYNTAX);
			}
			else
				notice_help(Config->s_BotServ, u, BOT_SET_UNKNOWN, option.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config->s_BotServ, u, BOT_HELP_SET);
			if (u->Account() && u->Account()->IsServicesOper())
				notice_help(Config->s_BotServ, u, BOT_SERVADMIN_HELP_SET);
		}
		else if (subcommand.equals_ci("DONTKICKOPS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_SET_DONTKICKOPS);
		else if (subcommand.equals_ci("DONTKICKVOICES"))
			notice_help(Config->s_BotServ, u, BOT_HELP_SET_DONTKICKVOICES);
		else if (subcommand.equals_ci("FANTASY"))
			notice_help(Config->s_BotServ, u, BOT_HELP_SET_FANTASY);
		else if (subcommand.equals_ci("GREET"))
			notice_help(Config->s_BotServ, u, BOT_HELP_SET_GREET);
		else if (subcommand.equals_ci("SYMBIOSIS"))
			notice_lang(Config->s_BotServ, u, BOT_HELP_SET_SYMBIOSIS, Config->s_ChanServ.c_str());
		else if (subcommand.equals_ci("NOBOT"))
			notice_lang(Config->s_BotServ, u, BOT_SERVADMIN_HELP_SET_NOBOT);
		else if (subcommand.equals_ci("PRIVATE"))
			notice_lang(Config->s_BotServ, u, BOT_SERVADMIN_HELP_SET_PRIVATE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_BotServ, u, "SET", BOT_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_BotServ, u, BOT_HELP_CMD_SET);
	}
};

class BSSet : public Module
{
	CommandBSSet commandbsset;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsset);
	}
};

MODULE_INIT(BSSet)
