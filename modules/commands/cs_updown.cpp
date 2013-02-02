/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSUp : public Command
{
 public:
	CommandCSUp(Module *creator) : Command(creator, "chanserv/up", 0, 1)
	{
		this->SetDesc(_("Updates your status on a channel"));
		this->SetSyntax(_("[\037channel\037]"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();

		if (params.empty())
			for (User::ChanUserList::iterator it = u->chans.begin(); it != u->chans.end(); ++it)
			{
				Channel *c = (*it)->chan;
				c->SetCorrectModes(u, true, false);
			}
		else
		{
			const Anope::string &channel = params[0];
			Channel *c = Channel::Find(params[0]);
			
			if (c == NULL)
				source.Reply(CHAN_X_NOT_IN_USE, channel.c_str());
			else if (!c->ci)
				source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
			else
				c->SetCorrectModes(u, true, false);
		}

	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Updates your status modes on a channel. If \037channel\037 is ommited\n"
				"your channel status is updated on every channel you are in."));
		return true;
	}
};

class CommandCSDown : public Command
{
	void RemoveAll(User *u, Channel *c)
	{
		for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
		{
			ChannelMode *cm = ModeManager::ChannelModes[i];

			if (cm != NULL && cm->type == MODE_STATUS)
				c->RemoveMode(NULL, cm, u->nick);
		}
	}

 public:
	CommandCSDown(Module *creator) : Command(creator, "chanserv/down", 0, 1)
	{
		this->SetDesc(_("Removes your status from a channel"));
		this->SetSyntax(_("[\037channel\037]"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();

		if (params.empty())
			for (User::ChanUserList::iterator it = u->chans.begin(); it != u->chans.end(); ++it)
			{
				Channel *c = (*it)->chan;
				RemoveAll(u, c);
			}
		else
		{
			const Anope::string &channel = params[0];
			Channel *c = Channel::Find(params[0]);
			
			if (c == NULL)
				source.Reply(CHAN_X_NOT_IN_USE, channel.c_str());
			else if (!c->ci)
				source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
			else
				RemoveAll(u, c);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Removes your status modes on a channel. If \037channel\037 is ommited\n"
				"your channel status is removed on every channel you are in."));
		return true;
	}
};

class CSUpDown : public Module
{
	CommandCSUp commandcsup;
	CommandCSDown commandcsdown;

 public:
	CSUpDown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsup(this), commandcsdown(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSUpDown)
