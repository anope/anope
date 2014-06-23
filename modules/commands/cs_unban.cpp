/* ChanServ core functions
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

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban(Module *creator) : Command(creator, "chanserv/unban", 0, 2)
	{
		this->SetDesc(_("Remove all bans preventing a user from entering a channel"));
		this->SetSyntax(_("\037channel\037 [\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName("BAN");
		if (!cm)
			return;

		std::vector<ChannelMode *> modes = cm->listeners;
		modes.push_back(cm);

		if (params.empty())
		{
			if (!source.GetUser())
				return;

			std::deque<ChanServ::Channel *> queue;
			source.GetAccount()->GetChannelReferences(queue);

			unsigned count = 0;
			for (unsigned i = 0; i < queue.size(); ++i)
			{
				ChanServ::Channel *ci = queue[i];

				if (!ci->c || !source.AccessFor(ci).HasPriv("UNBAN"))
					continue;

				for (unsigned j = 0; j < modes.size(); ++j)
					if (ci->c->Unban(source.GetUser(), modes[j]->name, true))
						++count;
			}

			Log(LOG_COMMAND, source, this, NULL) << "on all channels";
			source.Reply(_("You have been unbanned from %d channels."), count);

			return;
		}

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

		if (!source.AccessFor(ci).HasPriv("UNBAN") && !source.HasPriv("chanserv/kick"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "UNBAN", ci->name);
			return;
		}

		User *u2 = source.GetUser();
		if (params.size() > 1)
			u2 = User::Find(params[1], true);

		if (!u2)
		{
			if (params.size() > 1)
				source.Reply(_("User \002{0}\002 isn't currently online."), params[1]);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("UNBAN") && source.HasPriv("chanserv/kick");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to unban " << u2->nick;

		for (unsigned i = 0; i < modes.size(); ++i)
			ci->c->Unban(u2, modes[i]->name, source.GetUser() == u2);
		if (u2 == source.GetUser())
			source.Reply(_("You have been unbanned from \002{0}\002."), ci->c->name);
		else
			source.Reply(_("\002{0}\002 has been unbanned from \002{1}\002."), u2->nick, ci->c->name);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Tells {0} to remove all bans preventing you or the given \037user\037 from entering \037channel\037."
		               " If no channel is given, all bans affecting you in channels you have access in are removed.\n"
				"\n"
				"Use of this command requires the \002{1}\002 privilege on \037channel\037."),
				source.service->nick, "UNBAN");
		return true;
	}
};

class CSUnban : public Module
{
	CommandCSUnban commandcsunban;

 public:
	CSUnban(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsunban(this)
	{

	}
};

MODULE_INIT(CSUnban)
