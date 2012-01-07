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

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite(Module *creator) : Command(creator, "chanserv/invite", 1, 3)
	{
		this->SetDesc(_("Invites you or an optionally specified nick into a channel"));
		this->SetSyntax(_("\037channel\037 [\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		Channel *c = findchan(chan);

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}

		ChannelInfo *ci = c->ci;
		if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		if (!ci->AccessFor(u).HasPriv("INVITE") && !u->HasCommand("chanserv/invite"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		User *u2;
		if (params.size() == 1)
			u2 = u;
		else
		{
			if (!(u2 = finduser(params[1])))
			{
				source.Reply(NICK_X_NOT_IN_USE, params[1].c_str());
				return;
			}
		}

		if (c->FindUser(u2))
		{
			if (u2 == u)
				source.Reply(_("You are already in \002%s\002!"), c->name.c_str());
			else
				source.Reply(_("\002%s\002 is already in \002%s\002!"), u2->nick.c_str(), c->name.c_str());
		}
		else
		{
			bool override = !ci->AccessFor(u).HasPriv("INVITE");

			ircdproto->SendInvite(ci->WhoSends(), chan, u2->nick);
			if (u2 != u)
			{
				source.Reply(_("\002%s\002 has been invited to \002%s\002."), u2->nick.c_str(), c->name.c_str());
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "for " << u2->nick;
			}
			else
			{
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);
			}
			u2->SendMessage(ci->WhoSends(), _("You have been invited to \002%s\002."), c->name.c_str());
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Tells %s to invite you or an optionally specified\n"
				"nick into the given channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 and above\n"
				"on the channel."), source.owner->nick.c_str());
		return true;
	}
};

class CSInvite : public Module
{
	CommandCSInvite commandcsinvite;

 public:
	CSInvite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcsinvite(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSInvite)
