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

class CommandBSAssign : public Command
{
 public:
	CommandBSAssign() : Command("ASSIGN", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string nick = params[1];
		BotInfo *bi;
		ChannelInfo *ci;

		if (readonly)
		{
			u->SendMessage(BotServ, BOT_ASSIGN_READONLY);
			return MOD_CONT;
		}

		if (!(bi = findbot(nick)))
		{
			u->SendMessage(BotServ, BOT_DOES_NOT_EXIST, nick.c_str());
			return MOD_CONT;
		}

		ci = cs_findchan(chan);

		if (ci->botflags.HasFlag(BS_NOBOT) || (!check_access(u, ci, CA_ASSIGN) && !u->Account()->HasPriv("botserv/administration")))
		{
			u->SendMessage(BotServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (bi->HasFlag(BI_PRIVATE) && !u->Account()->HasCommand("botserv/assign/private"))
		{
			u->SendMessage(BotServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (ci->bi && nick.equals_ci(ci->bi->nick))
		{
			u->SendMessage(BotServ, BOT_ASSIGN_ALREADY, ci->bi->nick.c_str(), chan.c_str());
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_ASSIGN);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << bi->nick;

		bi->Assign(u, ci);
		u->SendMessage(BotServ, BOT_ASSIGN_ASSIGNED, bi->nick.c_str(), ci->name.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(BotServ, BOT_HELP_ASSIGN);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(BotServ, u, "ASSIGN", BOT_ASSIGN_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(BotServ, BOT_HELP_CMD_ASSIGN);
	}
};

class BSAssign : public Module
{
	CommandBSAssign commandbsassign;

 public:
	BSAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsassign);
	}
};

MODULE_INIT(BSAssign)
