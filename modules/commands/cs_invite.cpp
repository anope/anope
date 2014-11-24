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

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite(Module *creator) : Command(creator, "chanserv/invite", 1, 3)
	{
		this->SetDesc(_("Invites you or an optionally specified nick into a channel"));
		this->SetSyntax(_("\037channel\037 [\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		User *u = source.GetUser();
		Channel *c = Channel::Find(chan);

		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), chan);
			return;
		}

		ChanServ::Channel *ci = c->ci;
		if (!ci)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), c->name);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("INVITE") && !source.HasCommand("chanserv/invite"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "INVITE", ci->GetName());
			return;
		}

		User *u2;
		if (params.size() == 1)
			u2 = u;
		else
			u2 = User::Find(params[1], true);

		if (!u2)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), params.size() > 1 ? params[1] : source.GetNick());
			return;
		}

		if (c->FindUser(u2))
		{
			if (u2 == u)
				source.Reply(_("You are already in \002{0}\002!"), c->name);
			else
				source.Reply(_("\002{0}\002 is already in \002{1}\002!"), u2->nick, c->name);

			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("INVITE");

		IRCD->SendInvite(ci->WhoSends(), c, u2);
		if (u2 != u)
		{
			source.Reply(_("\002{0}\002 has been invited to \002{1}\002."), u2->nick, c->name);
			u2->SendMessage(ci->WhoSends(), _("You have been invited to \002{0}\002 by \002{1}\002."), c->name, source.GetNick());
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << u2->nick;
		}
		else
		{
			u2->SendMessage(ci->WhoSends(), _("You have been invited to \002{0}\002."), c->name);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes \002{0}\002 to invite you or an optionally specified \037nick\037 to \037channel\037.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               source.service->nick, "INVITE");
		return true;
	}
};

class CSInvite : public Module
{
	CommandCSInvite commandcsinvite;

 public:
	CSInvite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsinvite(this)
	{

	}
};

MODULE_INIT(CSInvite)
