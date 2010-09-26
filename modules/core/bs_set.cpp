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
			u->SendMessage(BotServ, BOT_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->Account()->HasCommand("botserv/set/private") && option.equals_ci("PRIVATE"))
		{
			BotInfo *bi;

			if (!(bi = findbot(chan)))
			{
				u->SendMessage(BotServ, BOT_DOES_NOT_EXIST, chan.c_str());
				return MOD_CONT;
			}

			if (value.equals_ci("ON"))
			{
				bi->SetFlag(BI_PRIVATE);
				u->SendMessage(BotServ, BOT_SET_PRIVATE_ON, bi->nick.c_str());
			}
			else if (value.equals_ci("OFF"))
			{
				bi->UnsetFlag(BI_PRIVATE);
				u->SendMessage(BotServ, BOT_SET_PRIVATE_OFF, bi->nick.c_str());
			}
			else
				SyntaxError(BotServ, u, "SET PRIVATE", BOT_SET_PRIVATE_SYNTAX);
			return MOD_CONT;
		}
		else if (!(ci = cs_findchan(chan)))
			u->SendMessage(BotServ, CHAN_X_NOT_REGISTERED, chan.c_str());
		else if (!u->Account()->HasPriv("botserv/administration") && !check_access(u, ci, CA_SET))
			u->SendMessage(BotServ, ACCESS_DENIED);
		else
		{
			bool override = !check_access(u, ci, CA_SET);
			Log(override ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << option << " " << value;

			if (option.equals_ci("DONTKICKOPS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKOPS);
					u->SendMessage(BotServ, BOT_SET_DONTKICKOPS_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKOPS);
					u->SendMessage(BotServ, BOT_SET_DONTKICKOPS_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET DONTKICKOPS", BOT_SET_DONTKICKOPS_SYNTAX);
			}
			else if (option.equals_ci("DONTKICKVOICES"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKVOICES);
					u->SendMessage(BotServ, BOT_SET_DONTKICKVOICES_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKVOICES);
					u->SendMessage(BotServ, BOT_SET_DONTKICKVOICES_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET DONTKICKVOICES", BOT_SET_DONTKICKVOICES_SYNTAX);
			}
			else if (option.equals_ci("FANTASY"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_FANTASY);
					u->SendMessage(BotServ, BOT_SET_FANTASY_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_FANTASY);
					u->SendMessage(BotServ, BOT_SET_FANTASY_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET FANTASY", BOT_SET_FANTASY_SYNTAX);
			}
			else if (option.equals_ci("GREET"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_GREET);
					u->SendMessage(BotServ, BOT_SET_GREET_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_GREET);
					u->SendMessage(BotServ, BOT_SET_GREET_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET GREET", BOT_SET_GREET_SYNTAX);
			}
			else if (u->Account()->HasCommand("botserv/set/nobot") && option.equals_ci("NOBOT"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_NOBOT);
					if (ci->bi)
						ci->bi->UnAssign(u, ci);
					u->SendMessage(BotServ, BOT_SET_NOBOT_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_NOBOT);
					u->SendMessage(BotServ, BOT_SET_NOBOT_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET NOBOT", BOT_SET_NOBOT_SYNTAX);
			}
			else if (option.equals_ci("SYMBIOSIS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_SYMBIOSIS);
					u->SendMessage(BotServ, BOT_SET_SYMBIOSIS_ON, ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_SYMBIOSIS);
					u->SendMessage(BotServ, BOT_SET_SYMBIOSIS_OFF, ci->name.c_str());
				}
				else
					SyntaxError(BotServ, u, "SET SYMBIOSIS", BOT_SET_SYMBIOSIS_SYNTAX);
			}
			else
				u->SendMessage(BotServ, BOT_SET_UNKNOWN, option.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			u->SendMessage(BotServ, BOT_HELP_SET);
			if (u->Account() && u->Account()->IsServicesOper())
				u->SendMessage(BotServ, BOT_SERVADMIN_HELP_SET);
		}
		else if (subcommand.equals_ci("DONTKICKOPS"))
			u->SendMessage(BotServ, BOT_HELP_SET_DONTKICKOPS);
		else if (subcommand.equals_ci("DONTKICKVOICES"))
			u->SendMessage(BotServ, BOT_HELP_SET_DONTKICKVOICES);
		else if (subcommand.equals_ci("FANTASY"))
			u->SendMessage(BotServ, BOT_HELP_SET_FANTASY);
		else if (subcommand.equals_ci("GREET"))
			u->SendMessage(BotServ, BOT_HELP_SET_GREET);
		else if (subcommand.equals_ci("SYMBIOSIS"))
			u->SendMessage(BotServ, BOT_HELP_SET_SYMBIOSIS, Config->s_ChanServ.c_str());
		else if (subcommand.equals_ci("NOBOT"))
			u->SendMessage(BotServ, BOT_SERVADMIN_HELP_SET_NOBOT);
		else if (subcommand.equals_ci("PRIVATE"))
			u->SendMessage(BotServ, BOT_SERVADMIN_HELP_SET_PRIVATE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(BotServ, u, "SET", BOT_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(BotServ, BOT_HELP_CMD_SET);
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
