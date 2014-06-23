/* OperServ core functions
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
		User *u2;

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

		/* Check for a nick in use or a forbidden/suspended nick */
		if (!(u2 = User::Find(nick, true)))
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
		else if (!nick.equals_ci(newnick) && User::Find(newnick))
			source.Reply(_("\002{0}\002 is currently in use."), newnick);
		else
		{
			source.Reply(_("\002{0}\002 is now being changed to \002{1}\002."), nick, newnick);
			Log(LOG_ADMIN, source, this) << "to change " << nick << " to " << newnick;
			IRCD->SendForceNickChange(u2, newnick, Anope::CurTime);
		}
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
			source.Reply(_("\002{0}\002 isn't currently online."), params[0]);
		else if (source.GetUser() != target && (target->IsProtected() || target->server == Me))
			source.Reply(_("Access denied."));
		else if (!c && !IRCD->IsChannelValid(params[1]))
			source.Reply(_("\002{0}\002 isn't a valid channel."), params[1]);
		else if (c && c->FindUser(target))
			source.Reply(_("\002{0}\002 is already in \002{1}\002."), target->nick, c->name);
		else
		{
			IRCD->SendSVSJoin(*source.service, target, params[1], "");
			Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to join " << params[1];
			source.Reply(_("\002{0}\002 has been joined to \002{1}\002."), target->nick, params[1]);
		}
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
		Channel *c = Channel::Find(params[1]);
		const Anope::string &reason = params.size() > 2 ? params[2] : "";
		if (target == NULL)
			source.Reply(_("\002{0}\002 isn't currently online."), params[0]);
		else if (source.GetUser() != target && (target->IsProtected() || target->server == Me))
			source.Reply(_("Access denied."));
		else if (!c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), params[1]);
		else if (!c->FindUser(target))
			source.Reply(_("\002{0}\002 is not in \002{1}\002."), target->nick, c->name);
		else
		{
			IRCD->SendSVSPart(*source.service, target, params[1], reason);
			if (!reason.empty())
				Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to part " << c->name << " with reason " << reason;
			else
				Log(LOG_ADMIN, source, this) << "to force " << target->nick << " to part " << c->name;
			source.Reply(_("\002{0}\002 has been parted from \002{1}\002."), target->nick, c->name);
		}
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
