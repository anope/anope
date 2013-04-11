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
	CommandCSUp(Module *creator) : Command(creator, "chanserv/up", 0, 2)
	{
		this->SetDesc(_("Updates a selected nicks status on a channel"));
		this->SetSyntax(_("[\037channel\037 [\037nick\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.empty())
		{
			if (!source.GetUser())
				return;
			for (User::ChanUserList::iterator it = source.GetUser()->chans.begin(); it != source.GetUser()->chans.end(); ++it)
			{
				Channel *c = it->second->chan;
				c->SetCorrectModes(source.GetUser(), true, false);
			}
		}
		else
		{
			const Anope::string &channel = params[0];
			const Anope::string &nick = params.size() > 1 ? params[1] : source.GetNick();

			Channel *c = Channel::Find(channel);

			if (c == NULL)
			{
				source.Reply(CHAN_X_NOT_IN_USE, channel.c_str());
				return;
			}
			else if (!c->ci)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
				return;
			}

			User *u = User::Find(nick, true);
			if (u == NULL)
			{
				source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
				return;
			}
			else if (source.GetUser() && u != source.GetUser() && c->ci->HasExt("PEACE"))
			{
				if (c->ci->AccessFor(u) > c->ci->AccessFor(source.GetUser()))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			c->SetCorrectModes(u, true, false);
		}

	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Updates a selected nicks status modes on a channel. If \037nick\037 is\n"
				"ommited then your status is updated. If \037channel\037 is ommited then\n"
				"your channel status is updated on every channel you are in."));
		return true;
	}
};

class CommandCSDown : public Command
{
	void RemoveAll(User *u, Channel *c)
	{
		ChanUserContainer *cu = c->FindUser(u);
		if (cu != NULL)
			for (size_t i = 0; i < cu->status.Modes().length(); ++i)
				c->RemoveMode(NULL, ModeManager::FindChannelModeByChar(cu->status.Modes()[i]), u->GetUID());
	}

 public:
	CommandCSDown(Module *creator) : Command(creator, "chanserv/down", 0, 2)
	{
		this->SetDesc(_("Removes a selected nicks status from a channel"));
		this->SetSyntax(_("[\037channel\037 [\037nick\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.empty())
		{
			if (!source.GetUser())
				return;
			for (User::ChanUserList::iterator it = source.GetUser()->chans.begin(); it != source.GetUser()->chans.end(); ++it)
			{
				Channel *c = it->second->chan;
				RemoveAll(source.GetUser(), c);
			}
		}
		else
		{
			const Anope::string &channel = params[0];
			const Anope::string &nick = params.size() > 1 ? params[1] : source.GetNick();

			Channel *c = Channel::Find(channel);
			
			if (c == NULL)
			{
				source.Reply(CHAN_X_NOT_IN_USE, channel.c_str());
				return;
			}
			else if (!c->ci)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
				return;
			}

			User *u = User::Find(nick, true);
			if (u == NULL)
			{
				source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
				return;
			}
			else if (source.GetUser() && u != source.GetUser() && c->ci->HasExt("PEACE"))
			{
				if (c->ci->AccessFor(u) > c->ci->AccessFor(source.GetUser()))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			RemoveAll(u, c);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Removes a selected nicks status modes on a channel. If \037nick\037 is\n"
				"ommited then your status is removed. If \037channel\037 is ommited then\n"
				"your channel status is removed on every channel you are in."));
		return true;
	}
};

class CSUpDown : public Module
{
	CommandCSUp commandcsup;
	CommandCSDown commandcsdown;

 public:
	CSUpDown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsup(this), commandcsdown(this)
	{

	}
};

MODULE_INIT(CSUpDown)
