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

class CommandBSSetGreet : public Command
{
 public:
	CommandBSSetGreet(Module *creator, const Anope::string &sname = "botserv/set/greet") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable greet messages"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		const Anope::string &value = params[1];

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

		if (value.equals_ci("ON"))
		{
			bool override = !ci->AccessFor(u).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to enable greets"; 

			ci->botflags.SetFlag(BS_GREET);
			source.Reply(_("Greet mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !ci->AccessFor(u).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to disable greets"; 

			ci->botflags.UnsetFlag(BS_GREET);
			source.Reply(_("Greet mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
			"Enables or disables \002greet\002 mode on a channel.\n"
			"When it is enabled, the bot will display greet\n"
			"messages of users joining the channel, provided\n"
			"they have enough access to the channel."));
		return true;
	}
};

class BSSetGreet : public Module
{
	CommandBSSetGreet commandbssetgreet;

 public:
	BSSetGreet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetgreet(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetGreet)
