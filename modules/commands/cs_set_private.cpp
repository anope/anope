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

class CommandCSSetPrivate : public Command
{
 public:
	CommandCSSetPrivate(Module *creator, const Anope::string &cname = "chanserv/set/private") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Hide channel from LIST command"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
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

		if (source.permission.empty() && !ci->AccessFor(u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002private\002 option for a channel.\n"
				"When \002private\002 is set, a \002%s%s LIST\002 will not\n"
				"include the channel in any lists."),
				Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		return true;
	}
};

class CommandCSSASetPrivate : public CommandCSSetPrivate
{
 public:
	CommandCSSASetPrivate(Module *creator) : CommandCSSetPrivate(creator, "chanserv/saset/private")
	{
	}
};

class CSSetPrivate : public Module
{
	CommandCSSetPrivate commandcssetprivate;
	CommandCSSASetPrivate commandcssasetprivate;

 public:
	CSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetprivate(this), commandcssasetprivate(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetPrivate)
