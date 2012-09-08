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

class CommandCSSetSecureOps : public Command
{
 public:
	CommandCSSetSecureOps(Module *creator, const Anope::string &cname = "chanserv/set/secureops") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of chanop status"));
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
			ci->SetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002secure ops\002 option for a channel.\n"
				"When \002secure ops\002 is set, users who are not on the userlist\n"
				"will not be allowed chanop status."));
		return true;
	}
};

class CSSetSecureOps : public Module
{
	CommandCSSetSecureOps commandcssetsecureops;

 public:
	CSSetSecureOps(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetsecureops(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetSecureOps)
