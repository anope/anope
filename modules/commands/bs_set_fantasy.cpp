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

class CommandBSSetFantasy : public Command
{
 public:
	CommandBSSetFantasy(Module *creator, const Anope::string &sname = "botserv/set/fantasy") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable fantaisist commands"));
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

		if (!source.HasPriv("botserv/administration") && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable fantasy"; 

			ci->botflags.SetFlag(BS_FANTASY);
			source.Reply(_("Fantasy mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable fantasy"; 

			ci->botflags.UnsetFlag(BS_FANTASY);
			source.Reply(_("Fantasy mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002fantasy\002 mode on a channel.\n"
				"When it is enabled, users will be able to use\n"
				"%s commands on a channel when prefixed\n"
				"with one of the following fantasy characters: \002%s\002\n"
				" \n"
				"Note that users wanting to use fantaisist\n"
				"commands MUST have enough level for both\n"
				"the FANTASIA and another level depending\n"
				"of the command if required (for example, to use \n"
				"!op, user must have enough access for the OPDEOP\n"
				"level)."), Config->ChanServ.c_str(), Config->BSFantasyCharacter.c_str());
		return true;
	}
};

class BSSetFantasy : public Module
{
	CommandBSSetFantasy commandbssetfantasy;

 public:
	BSSetFantasy(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetfantasy(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetFantasy)
