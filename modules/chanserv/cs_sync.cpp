/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSSync final
	: public Command
{
public:
	CommandCSSync(Module *creator) : Command(creator, "chanserv/sync", 1, 1)
	{
		this->SetDesc(_("Sync users channel modes"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (ci->c == NULL)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && !source.HasPriv("chanserv/administration"))
			source.Reply(ACCESS_DENIED);
		else
		{
			bool override = !source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && source.HasPriv("chanserv/administration");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci);

			for (const auto &[_, uc] : ci->c->users)
				ci->c->SetCorrectModes(uc->user, true);

			source.Reply(_("All user modes on \002%s\002 have been synced."), ci->name.c_str());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &params) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Syncs all modes set on users on the channel with the modes "
			"they should have based on their access."
		));
		return true;
	}
};

class CSSync final
	: public Module
{
	CommandCSSync commandcssync;
public:
	CSSync(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcssync(this)
	{

	}
};

MODULE_INIT(CSSync)
