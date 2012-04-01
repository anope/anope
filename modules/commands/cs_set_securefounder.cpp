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

class CommandCSSetSecureFounder : public Command
{
 public:
	CommandCSSetSecureFounder(Module *creator, const Anope::string &cname = "chanserv/set/securefounder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of channel founder status"));
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


		if (source.permission.empty() && ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !ci->AccessFor(u).HasPriv("FOUNDER"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREFOUNDER");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002secure founder\002 option for a channel.\n"
			"When \002secure founder\002 is set, only the real founder will be\n"
			"able to drop the channel, change its founder and its successor,\n"
			"and not those who have founder level access through\n"
			"the access/qop command."));
		return true;
	}
};

class CommandCSSASetSecureFounder : public CommandCSSetSecureFounder
{
 public:
	CommandCSSASetSecureFounder(Module *creator) : CommandCSSetSecureFounder(creator, "chanserv/saset/securefounder")
	{
	}
};

class CSSetSecureFounder : public Module
{
	CommandCSSetSecureFounder commandcssetsecurefounder;
	CommandCSSASetSecureFounder commandcssasetsecurefounder;

 public:
	CSSetSecureFounder(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetsecurefounder(this), commandcssasetsecurefounder(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetSecureFounder)
