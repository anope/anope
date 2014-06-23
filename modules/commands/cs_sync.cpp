/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSSync : public Command
{
 public:
	CommandCSSync(Module *creator) : Command(creator, "chanserv/sync", 1, 1)
	{
		this->SetDesc(_("Sync users channel modes"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (ci->c == NULL)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->name);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "ACCESS_CHANGE", ci->name);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && source.HasPriv("chanserv/administration");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci);

		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			ci->c->SetCorrectModes(it->second->user, true);

		source.Reply(_("All user modes on \002{0}\002 have been synced."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &params) override
	{
		source.Reply(_("Syncs all channel status modes on all users on \037channel\037 with the modes they should have based on the channel access list.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "ACCESS_CHNAGE");
		return true;
	}
};

class CSSync : public Module
{
	CommandCSSync commandcssync;

 public:
	CSSync(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcssync(this)
	{

	}
};

MODULE_INIT(CSSync)
