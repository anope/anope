/* ChanServ core functions
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

class CommandCSSetRestricted : public Command
{
 public:
	CommandCSSetRestricted(Module *creator, const Anope::string &cname = "chanserv/set/restricted") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Restrict access to the channel"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_RESTRICTED);
			source.Reply(_("Restricted access option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_RESTRICTED);
			source.Reply(_("Restricted access option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "RESTRICTED");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002restricted access\002 option for a\n"
				"channel. When \002restricted access\002 is set, users not on the access list will\n"
				"instead be kicked and banned from the channel."));
		return true;
	}
};

class CommandCSSASetRestricted : public CommandCSSetRestricted
{
 public:
	CommandCSSASetRestricted(Module *creator) : CommandCSSetRestricted(creator, "chanserv/saset/restricted")
	{
	}
};

class CSSetRestricted : public Module
{
	CommandCSSetRestricted commandcssetrestricted;
	CommandCSSASetRestricted commandcssasetrestricted;

 public:
	CSSetRestricted(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetrestricted(this), commandcssasetrestricted(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetRestricted)
