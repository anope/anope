/* BotServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandBSUnassign : public Command
{
 public:
	CommandBSUnassign() : Command("UNASSIGN", 1, 1)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PERM);

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(BOT_ASSIGN_READONLY);
		else if (!u->Account()->HasPriv("botserv/administration") && !check_access(u, ci, CA_ASSIGN))
			source.Reply(ACCESS_DENIED);
		else if (!ci->bi)
			source.Reply(BOT_NOT_ASSIGNED);
		else if (ci->HasFlag(CI_PERSIST) && !cm)
			source.Reply(BOT_UNASSIGN_PERSISTANT_CHAN);
		else
		{
			bool override = !check_access(u, ci, CA_ASSIGN);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << ci->bi->nick;

			ci->bi->UnAssign(u, ci);
			source.Reply(BOT_UNASSIGN_UNASSIGNED, ci->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(BOT_HELP_UNASSIGN);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNASSIGN", BOT_UNASSIGN_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(BOT_HELP_CMD_UNASSIGN);
	}
};

class BSUnassign : public Module
{
	CommandBSUnassign commandbsunassign;

 public:
	BSUnassign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsunassign);
	}
};

MODULE_INIT(BSUnassign)
