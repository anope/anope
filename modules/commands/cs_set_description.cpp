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

class CommandCSSetDescription : public Command
{
 public:
	CommandCSSetDescription(Module *creator, const Anope::string &cname = "chanserv/set/description") : Command(creator, cname, 1, 2)
	{
		this->SetDesc(_("Set the channel description"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
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

		if (params.size() > 1)
		{
			ci->desc = params[1];
			source.Reply(_("Description of %s changed to \002%s\002."), ci->name.c_str(), ci->desc.c_str());
		}
		else
		{
			ci->desc.clear();
			source.Reply(_("Description of %s unset."), ci->name.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the description for the channel, which shows up with\n"
				"the \002LIST\002 and \002INFO\002 commands."), this->name.c_str());
		return true;
	}
};

class CSSetDescription : public Module
{
	CommandCSSetDescription commandcssetdescription;

 public:
	CSSetDescription(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetdescription(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetDescription)
