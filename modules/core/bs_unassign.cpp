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
		this->SetDesc(_("Unassigns a bot from a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PERM);

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(_(BOT_ASSIGN_READONLY));
		else if (!u->Account()->HasPriv("botserv/administration") && !check_access(u, ci, CA_ASSIGN))
			source.Reply(_(ACCESS_DENIED));
		else if (!ci->bi)
			source.Reply(_(BOT_NOT_ASSIGNED));
		else if (ci->HasFlag(CI_PERSIST) && !cm)
			source.Reply(_("You can not unassign bots while persist is set on the channel."));
		else
		{
			bool override = !check_access(u, ci, CA_ASSIGN);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << ci->bi->nick;

			ci->bi->UnAssign(u, ci);
			source.Reply(_("There is no bot assigned to %s anymore."), ci->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002UNASSIGN \037chan\037\002\n"
				" \n"
				"Unassigns a bot from a channel. When you use this command,\n"
				"the bot won't join the channel anymore. However, bot\n"
				"configuration for the channel is kept, so you will always\n"
				"be able to reassign a bot later without have to reconfigure\n"
				"it entirely."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNASSIGN", _("UNASSIGN \037chan\037"));
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
