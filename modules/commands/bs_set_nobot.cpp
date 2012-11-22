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

class CommandBSSetNoBot : public Command
{
 public:
	CommandBSSetNoBot(Module *creator, const Anope::string &sname = "botserv/set/nobot") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned to a channel"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.HasCommand("botserv/set/nobot"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_ADMIN : LOG_COMMAND, source, this, ci) << "to enable nobot"; 

			ci->botflags.SetFlag(BS_NOBOT);
			if (ci->bi)
				ci->bi->UnAssign(source.GetUser(), ci);
			source.Reply(_("No Bot mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_ADMIN : LOG_COMMAND, source, this, ci) << "to disable nobot"; 

			ci->botflags.UnsetFlag(BS_NOBOT);
			source.Reply(_("No Bot mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"This option makes a channel be unassignable. If a bot \n"
				"is already assigned to the channel, it is unassigned\n"
				"automatically when you enable the option."));
		return true;
	}
};

class BSSetNoBot : public Module
{
	CommandBSSetNoBot commandbssetnobot;

 public:
	BSSetNoBot(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetnobot(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetNoBot)
