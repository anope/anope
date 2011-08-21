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

class CommandBSAssign : public Command
{
 public:
	CommandBSAssign(Module *creator) : Command(creator, "botserv/assign", 2, 2)
	{
		this->SetDesc(_("Assigns a bot to a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];

		User *u = source.u;

		if (readonly)
		{
			source.Reply(BOT_ASSIGN_READONLY);
			return;
		}

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		BotInfo *bi = findbot(nick);
		if (!bi)
		{
			source.Reply(BOT_DOES_NOT_EXIST, nick.c_str());
			return;
		}

 		if (ci->botflags.HasFlag(BS_NOBOT) || (!ci->AccessFor(u).HasPriv(CA_ASSIGN) && !u->HasPriv("botserv/administration")))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (bi->HasFlag(BI_PRIVATE) && !u->HasCommand("botserv/assign/private"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (ci->bi == bi)
		{
			source.Reply(_("Bot \002%s\002 is already assigned to channel \002%s\002."), ci->bi->nick.c_str(), chan.c_str());
			return;
		}

		bool override = !ci->AccessFor(u).HasPriv(CA_ASSIGN);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << bi->nick;

		bi->Assign(u, ci);
		source.Reply(_("Bot \002%s\002 has been assigned to %s."), bi->nick.c_str(), ci->name.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Assigns a bot pointed out by nick to the channel chan. You\n"
				"can then configure the bot for the channel so it fits\n"
				"your needs."));
		return true;
	}
};

class CommandBSUnassign : public Command
{
 public:
	CommandBSUnassign(Module *creator) : Command(creator, "botserv/unassign", 1, 1)
	{
		this->SetDesc(_("Unassigns a bot from a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (readonly)
		{
			source.Reply(BOT_ASSIGN_READONLY);
			return;
		}

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!u->HasPriv("botserv/administration") && !ci->AccessFor(u).HasPriv(CA_ASSIGN))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->bi)
		{
			source.Reply(BOT_NOT_ASSIGNED);
			return;
		}

		if (ci->HasFlag(CI_PERSIST) && !ModeManager::FindChannelModeByName(CMODE_PERM))
		{
			source.Reply(_("You can not unassign bots while persist is set on the channel."));
			return;
		}

		bool override = !ci->AccessFor(u).HasPriv(CA_ASSIGN);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << ci->bi->nick;

		ci->bi->UnAssign(u, ci);
		source.Reply(_("There is no bot assigned to %s anymore."), ci->name.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Unassigns a bot from a channel. When you use this command,\n"
				"the bot won't join the channel anymore. However, bot\n"
				"configuration for the channel is kept, so you will always\n"
				"be able to reassign a bot later without have to reconfigure\n"
				"it entirely."));
		return true;
	}
};

class BSAssign : public Module
{
	CommandBSAssign commandbsassign;
	CommandBSUnassign commandbsunassign;

 public:
	BSAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbsassign(this), commandbsunassign(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandbsassign);
		ModuleManager::RegisterService(&commandbsunassign);
	}
};

MODULE_INIT(BSAssign)
