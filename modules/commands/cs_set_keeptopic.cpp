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

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(Module *creator, const Anope::string &cname = "chanserv/set/keeptopic") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Retain topic when channel is not in use"));
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
			ci->SetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "KEEPTOPIC");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002topic retention\002 option for a\n"
				"channel. When \002%s\002 is set, the topic for the\n"
				"channel will be remembered by %s even after the\n"
				"last user leaves the channel, and will be restored the\n"
				"next time the channel is created."), this->name.c_str(), source.owner->nick.c_str());
		return true;
	}
};

class CommandCSSASetKeepTopic : public CommandCSSetKeepTopic
{
 public:
	CommandCSSASetKeepTopic(Module *creator) : CommandCSSetKeepTopic(creator, "chanserv/saset/keeptopic")
	{
	}
};

class CSSetKeepTopic : public Module
{
	CommandCSSetKeepTopic commandcssetkeeptopic;
	CommandCSSASetKeepTopic commandcssasetkeeptopic;

 public:
	CSSetKeepTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetkeeptopic(this), commandcssasetkeeptopic(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetKeepTopic)
