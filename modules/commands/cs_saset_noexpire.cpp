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

class CommandCSSASetNoexpire : public Command
{
 public:
	CommandCSSASetNoexpire(Module *creator) : Command(creator, "chanserv/saset/noexpire", 2, 2)
	{
		this->SetDesc(_("Prevent the channel from expiring"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
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
			ci->SetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will not\002 expire."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will\002 expire."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given channel will expire.  Setting this\n"
				"to ON prevents the channel from expiring."));
		return true;
	}
};

class CSSetNoexpire : public Module
{
	CommandCSSASetNoexpire commandcssasetnoexpire;

 public:
	CSSetNoexpire(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssasetnoexpire(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetNoexpire)
