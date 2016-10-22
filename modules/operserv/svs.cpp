/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick(Module *creator) : Command(creator, "operserv/svsnick", 2, 2)
	{
		this->SetDesc(_("Forcefully change a user's nickname"));
		this->SetSyntax(_("\037nick\037 \037newnick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		Anope::string newnick = params[1];

		if (!IRCD->CanSVSNick)
		{
			source.Reply(_("Your IRCd does not support SVSNICK."));
			return;
		}

		/* Truncate long nicknames to nicklen characters */
		unsigned nicklen = Config->GetBlock("networkinfo")->Get<unsigned>("nicklen");
		if (newnick.length() > nicklen)
		{
			source.Reply(_("Nick \002{0}\002 was truncated to \002{1}\002 characters."), newnick, nicklen);
			newnick = params[1].substr(0, nicklen);
		}

		/* Check for valid characters */
		if (!IRCD->IsNickValid(newnick))
		{
			source.Reply(_("Nick \002{0}\002 is an illegal nickname and cannot be used."), newnick);
			return;
		}

		User *u2 = User::Find(nick, true);
		if (u2 == nullptr)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
			return;
		}

		if (!nick.equals_ci(newnick) && User::Find(newnick, true))
		{
			source.Reply(_("\002{0}\002 is currently in use."), newnick);
			return;
		}

		source.Reply(_("\002{0}\002 is now being changed to \002{1}\002."), nick, newnick);
		Log(LOG_ADMIN, source, this) << "to change " << nick << " to " << newnick;
		IRCD->SendForceNickChange(u2, newnick, Anope::CurTime);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Forcefully changes a user's nickname from \037nick\037 to \037newnick\037."));
		return true;
	}
};

class CommandOSSVSJoin : public Command
{
 public:
	CommandOSSVSJoin(Module *creator) : Command(creator, "operserv/svsjoin", 2, 2)
	{
		this->SetDesc(_("Forcefully join a user to a channel"));
		this->SetSyntax(_("\037user\037 \037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSVSJoin)
		{
			source.Reply(_("Your IRCd does not support SVSJOIN."));
			return;
		}

		User *target = User::Find(params[0], true);
		Channel *c = Channel::Find(params[1]);
		if (target == NULL)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), params[0]);
			return;
		}

		if (source.GetUser() != target && (target->IsProtected() || target->server == Me))
		{
			source.Reply(_("Access denied."));
			return;
		}

		if (!c && !IRCD->IsChannelValid(params[1]))
		{
			source.Reply(_("\002{0}\002 isn't a valid channel."), params[1]);
			return;
		}

		if (c && c->FindUser(target))
		{
			source.Reply(_("\002{0}\002 is already in \002{1}\002."), target->nick, c->name);
			return;
		}

		IRCD->SendSVSJoin(*source.service, target, params[1], "");
		Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to join " << params[1];
		source.Reply(_("\002{0}\002 has been joined to \002{1}\002."), target->nick, params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Forcefully join \037user\037 to \037channel\037."));
		return true;
	}
};

class CommandOSSVSPart : public Command
{
 public:
	CommandOSSVSPart(Module *creator) : Command(creator, "operserv/svspart", 2, 3)
	{
		this->SetDesc(_("Forcefully part a user from a channel"));
		this->SetSyntax(_("\037nick\037 \037channel\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSVSJoin)
		{
			source.Reply(_("Your IRCd does not support SVSPART."));
			return;
		}

		User *target = User::Find(params[0], true);
		if (target == nullptr)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), params[0]);
			return;
		}

		if (source.GetUser() != target && (target->IsProtected() || target->server == Me))
		{
			source.Reply(_("Access denied."));
			return;
		}

		Channel *c = Channel::Find(params[1]);
		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), params[1]);
			return;
		}

		if (!c->FindUser(target))
		{
			source.Reply(_("\002{0}\002 is not in \002{1}\002."), target->nick, c->name);
			return;
		}

		const Anope::string &reason = params.size() > 2 ? params[2] : "";
		IRCD->SendSVSPart(*source.service, target, params[1], reason);
		if (!reason.empty())
			Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to part " << c->name << " with reason " << reason;
		else
			Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to part " << c->name;
		source.Reply(_("\002{0}\002 has been parted from \002{1}\002."), target->nick, c->name);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Forcefully part \037user\037 from \037channel\037."));
		return true;
	}
};

class OSSVS : public Module
{
	CommandOSSVSNick commandossvsnick;
	CommandOSSVSJoin commandossvsjoin;
	CommandOSSVSPart commandossvspart;

 public:
	OSSVS(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandossvsnick(this), commandossvsjoin(this), commandossvspart(this)
	{
	}
};

MODULE_INIT(OSSVS)
