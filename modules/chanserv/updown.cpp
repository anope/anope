/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class CommandCSUp : public Command
{
	void SetModes(User *u, Channel *c)
	{
		if (!c->ci)
			return;

		/* whether or not we are giving modes */
		bool giving = true;
		/* whether or not we have given a mode */
		bool given = false;
		ChanServ::AccessGroup u_access = c->ci->AccessFor(u);

		for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
		{
			ChannelModeStatus *cm = ModeManager::GetStatusChannelModesByRank()[i];
			bool has_priv = u_access.HasPriv("AUTO" + cm->name) || u_access.HasPriv(cm->name);

			if (has_priv)
			{
				/* Always give op. If we have already given one mode, don't give more until it has a symbol */
				if (cm->name == "OP" || !given || (giving && cm->symbol))
				{
					c->SetMode(NULL, cm, u->GetUID(), false);
					/* Now if this contains a symbol don't give any more modes, to prevent setting +qaohv etc on users */
					giving = !cm->symbol;
					given = true;
				}
			}
		}
	}
 public:
	CommandCSUp(Module *creator) : Command(creator, "chanserv/up", 0, 2)
	{
		this->SetDesc(_("Updates a selected nicks status on a channel"));
		this->SetSyntax(_("[\037channel\037 [\037user\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.empty())
		{
			if (!source.GetUser())
				return;
			for (User::ChanUserList::iterator it = source.GetUser()->chans.begin(); it != source.GetUser()->chans.end(); ++it)
			{
				Channel *c = it->second->chan;
				SetModes(source.GetUser(), c);
			}
			Log(LOG_COMMAND, source, this, NULL) << "on all channels to update their status modes";
		}
		else
		{
			const Anope::string &chan = params[0];
			const Anope::string &nick = params.size() > 1 ? params[1] : source.GetNick();

			ChanServ::Channel *ci = ChanServ::Find(chan);
			if (ci == NULL)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
				return;
			}

			if (ci->c == NULL)
			{
				source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
				return;
			}

			User *u = User::Find(nick, true);
			User *srcu = source.GetUser();
			Channel *c = ci->c;
			bool override = false;

			if (u == NULL)
			{
				source.Reply(_("User \002{0}\002 isn't currently online."), nick);
				return;
			}

			if (srcu && !srcu->FindChannel(c))
			{
				source.Reply(_("You must be in \002%s\002 to use this command."), c->name.c_str());
				return;
			}

			if (!u->FindChannel(c))
			{
				source.Reply(_("You must be on channel \002{0}\002 to use this command."), c->name);
				return;
			}

			if (!u->FindChannel(c))
			{
				source.Reply(_("\002{0}\002 is not on channel \002{1}\002."), u->nick, c->name);
				return;
			}

			if (source.GetUser() && u != source.GetUser() && c->ci->HasFieldS("PEACE"))
			{
				if (c->ci->AccessFor(u) >= c->ci->AccessFor(source.GetUser()))
				{
					if (source.HasPriv("chanserv/administration"))
						override = true;
					else
					{
						source.Reply(_("Access denied. \002{0}\002 has more privileges than you on \002{1}\002."), u->nick, ci->GetName());
						return;
					}
				}
			}

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, c->ci) << "to update the status modes of " << u->nick;
			SetModes(u, c);
		}

	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Updates the status modes of \037user\037 on \037channel\037."
		               " If \037user\037 is omitted  then your status is updated."
		               " If \037channel\037 is omitted then your status is updated on every channel you are in."));
		return true;
	}
};

class CommandCSDown : public Command
{
	void RemoveAll(User *u, Channel *c)
	{
		ChanUserContainer *cu = c->FindUser(u);
		if (cu != NULL)
			for (size_t i = cu->status.Modes().length(); i > 0;)
				c->RemoveMode(NULL, ModeManager::FindChannelModeByChar(cu->status.Modes()[--i]), u->GetUID());
	}

 public:
	CommandCSDown(Module *creator) : Command(creator, "chanserv/down", 0, 2)
	{
		this->SetDesc(_("Removes a selected nicks status from a channel"));
		this->SetSyntax(_("[\037channel\037 [\037nick\037]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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
			Log(LOG_COMMAND, source, this, NULL) << "on all channels to remove their status modes";
		}
		else
		{
			const Anope::string &channel = params[0];
			const Anope::string &nick = params.size() > 1 ? params[1] : source.GetNick();

			ChanServ::Channel *ci = ChanServ::Find(channel);
			if (ci == NULL)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
				return;
			}

			if (ci->c == NULL)
			{
				source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
				return;
			}

			User *u = User::Find(nick, true);
			Channel *c = ci->c;

			User *srcu = source.GetUser();
			bool override = false;

			if (u == NULL)
			{
				source.Reply(_("User \002{0}\002 isn't currently online."), nick);
				return;
			}

			if (srcu && !srcu->FindChannel(c))
			{
				source.Reply(_("You must be on channel \002{0}\002 to use this command."), c->name);
				return;
			}

			if (srcu && !srcu->FindChannel(c))
			{
				source.Reply(_("You must be in \002%s\002 to use this command."), c->name.c_str());
				return;
			}

			if (!u->FindChannel(c))
			{
				source.Reply(_("\002%s\002 is not on channel %s."), u->nick, c->name);
				return;
			}

			if (source.GetUser() && u != source.GetUser() && c->ci->HasFieldS("PEACE"))
			{
				if (c->ci->AccessFor(u) >= c->ci->AccessFor(source.GetUser()))
				{
					if (source.HasPriv("chanserv/administration"))
						override = true;
					else
					{
						source.Reply(_("Access denied. \002{0}\002 has more privileges than you on \002{1}\002."), u->nick, ci->GetName());
						return;
					}
				}
			}

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, c->ci) << "to remove the status modes from " << u->nick;
			RemoveAll(u, c);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Removes a selected nicks status modes on a channel."
		               " If \037nick\037 is ommited then your status is removed."
		               " If \037channel\037 is ommited then channel status is removed on every channel you are in."));
		return true;
	}
};

class CSUpDown : public Module
{
	CommandCSUp commandcsup;
	CommandCSDown commandcsdown;

 public:
	CSUpDown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsup(this)
		, commandcsdown(this)
	{

	}
};

MODULE_INIT(CSUpDown)
