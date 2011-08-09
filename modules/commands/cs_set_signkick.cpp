/* ChanServ core functions
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

class CommandCSSetSignKick : public Command
{
 public:
	CommandCSSetSignKick(Module *creator, const Anope::string &cname = "chanserv/set/signkick") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Sign kicks that are done with KICK command"));
		this->SetSyntax(_("\037channel\037 SIGNKICK {ON | LEVEL | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.permission.empty() && !ci->AccessFor(u).HasPriv(CA_SET))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("LEVEL"))
		{
			ci->SetFlag(CI_SIGNKICK_LEVEL);
			ci->UnsetFlag(CI_SIGNKICK);
			source.Reply(_("Signed kick option for %s is now \002ON\002, but depends of the\n"
				"level of the user that is using the command."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SIGNKICK");
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables signed kicks for a\n"
				"channel.  When \002SIGNKICK\002 is set, kicks issued with\n"
				"KICK command will have the nick that used the\n"
				"command in their reason.\n"
				" \n"
				"If you use \002LEVEL\002, those who have a level that is superior \n"
				"or equal to the SIGNKICK level on the channel won't have their \n"
				"kicks signed."));
		return true;
	}
};

class CommandCSSASetSignKick : public CommandCSSetSignKick
{
 public:
	CommandCSSASetSignKick(Module *creator) : CommandCSSetSignKick(creator, "chanserv/saset/signkick")
	{
	}
};

class CSSetSignKick : public Module
{
	CommandCSSetSignKick commandcssetsignkick;
	CommandCSSASetSignKick commandcssasetsignkick;

 public:
	CSSetSignKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetsignkick(this), commandcssasetsignkick(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcssetsignkick);
		ModuleManager::RegisterService(&commandcssasetsignkick);
	}
};

MODULE_INIT(CSSetSignKick)
