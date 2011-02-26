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
	CommandBSAssign() : Command("ASSIGN", 2, 2)
	{
		this->SetDesc("Assigns a bot to a channel");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
		{
			source.Reply(_(BOT_ASSIGN_READONLY));
			return MOD_CONT;
		}

		BotInfo *bi = findbot(nick);
		if (!bi)
		{
			source.Reply(_(BOT_DOES_NOT_EXIST), nick.c_str());
			return MOD_CONT;
		}

		if (ci->botflags.HasFlag(BS_NOBOT) || (!check_access(u, ci, CA_ASSIGN) && !u->Account()->HasPriv("botserv/administration")))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (bi->HasFlag(BI_PRIVATE) && !u->Account()->HasCommand("botserv/assign/private"))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (ci->bi && nick.equals_ci(ci->bi->nick))
		{
			source.Reply(_("Bot \002%s\002 is already assigned to channel \002%s\002."), ci->bi->nick.c_str(), chan.c_str());
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_ASSIGN);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << bi->nick;

		bi->Assign(u, ci);
		source.Reply(_("Bot \002%s\002 has been assigned to %s."), bi->nick.c_str(), ci->name.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002ASSIGN \037chan\037 \037nick\037\002\n"
				" \n"
				"Assigns a bot pointed out by nick to the channel chan. You\n"
				"can then configure the bot for the channel so it fits\n"
				"your needs."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "ASSIGN", _("ASSIGN \037chan\037 \037nick\037"));
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
