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

class CommandCSSetAutoOp : public Command
{
 public:
	CommandCSSetAutoOp(Module *creator, const Anope::string &cname = "chanserv/set/autoop") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Should services automatically give status to users"));
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
			ci->UnsetFlag(CI_NOAUTOOP);
			source.Reply(_("Services will now automatically give modes to users in \2%s\2"), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->SetFlag(CI_NOAUTOOP);
			source.Reply(_("Services will no longer automatically give modes to users in \2%s\2"), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables %s's autoop feature for a\n"
			"channel. When disabled, users who join the channel will\n"
			"not automatically gain any status from %s"), Config->ChanServ.c_str(),
			Config->ChanServ.c_str(), this->name.c_str());
		return true;
	}
};


class CSSetAutoOp : public Module
{
	CommandCSSetAutoOp commandcssetautoop;

 public:
	CSSetAutoOp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetautoop(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetAutoOp)
