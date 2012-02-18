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

class CommandCSSetSecure : public Command
{
 public:
	CommandCSSetSecure(Module *creator, const Anope::string &cname = "chanserv/set/secure") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Activate security features"));
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

		if (source.permission.empty() && !ci->AccessFor(u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables security features for a\n"
			"channel. When \002%s\002 is set, only users who have\n"
			"registered their nicknames and IDENTIFY'd\n"
			"with their password will be given access to the channel\n"
			"as controlled by the access list."), this->name.c_str());
		return true;
	}
};

class CommandCSSASetSecure : public CommandCSSetSecure
{
 public:
	CommandCSSASetSecure(Module *creator) : CommandCSSetSecure(creator, "chanserv/saset/secure")
	{
	}
};

class CSSetSecure : public Module
{
	CommandCSSetSecure commandcssetsecure;
	CommandCSSASetSecure commandcssasetsecure;

 public:
	CSSetSecure(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetsecure(this), commandcssasetsecure(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetSecure)
