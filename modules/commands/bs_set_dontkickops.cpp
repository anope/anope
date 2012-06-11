/* BotServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandBSSetDontKickOps : public Command
{
 public:
	CommandBSSetDontKickOps(Module *creator, const Anope::string &sname = "botserv/set/dontkickops") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect ops against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!u->HasPriv("botserv/administration") && !ci->AccessFor(u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (readonly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			bool override = !ci->AccessFor(u).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to enable dontkickops"; 

			ci->botflags.SetFlag(BS_DONTKICKOPS);
			source.Reply(_("Bot \002won't kick ops\002 on channel %s."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !ci->AccessFor(u).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to disable dontkickops"; 

			ci->botflags.UnsetFlag(BS_DONTKICKOPS);
			source.Reply(_("Bot \002will kick ops\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002ops protection\002 mode on a channel.\n"
				"When it is enabled, ops won't be kicked by the bot\n"
				"even if they don't match the NOKICK level."));
		return true;
	}
};

class BSSetDontKickOps : public Module
{
	CommandBSSetDontKickOps commandbssetdontkickops;

 public:
	BSSetDontKickOps(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetdontkickops(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetDontKickOps)
