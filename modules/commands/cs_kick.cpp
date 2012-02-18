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

class CommandCSKick : public Command
{
 public:
	CommandCSKick(Module *creator) : Command(creator, "chanserv/kick", 2, 3)
	{
		this->SetDesc(_("Kicks a selected nick from a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037 [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 \037mask\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		Channel *c = findchan(params[0]);
		User *u2 = finduser(target);

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}
		else if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		AccessGroup u_access = ci->AccessFor(u);

		if (!u_access.HasPriv("KICK"))
			source.Reply(ACCESS_DENIED);
		else if (u2)
		{
			AccessGroup u2_access = ci->AccessFor(u2);
			if (u != u2 && ci->HasFlag(CI_PEACE) && u2_access >= u_access)
				source.Reply(ACCESS_DENIED);
			else if (u2->IsProtected())
				source.Reply(ACCESS_DENIED);
			else if (!c->FindUser(u2))
				source.Reply(NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
			else
			{
				 // XXX
				Log(LOG_COMMAND, u, this, ci) << "for " << u2->nick;

				if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !u_access.HasPriv("SIGNKICK")))
					c->Kick(ci->WhoSends(), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
				else
					c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
			}
		}
		else if (u_access.HasPriv("FOUNDER"))
		{
			Log(LOG_COMMAND, u, this, ci) << "for " << target;

			int matched = 0, kicked = 0;
			for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end;)
			{
				UserContainer *uc = *it++;

				if (Anope::Match(uc->user->nick, target) || Anope::Match(uc->user->GetDisplayedMask(), target))
				{
					++matched;

					AccessGroup u2_access = ci->AccessFor(uc->user);
					if (u != uc->user && ci->HasFlag(CI_PEACE) && u2_access >= u_access)
						continue;
					else if (uc->user->IsProtected())
						continue;

					++kicked;
					if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !u_access.HasPriv("SIGNKICK")))
						c->Kick(ci->WhoSends(), uc->user, "%s (Matches %s) (%s)", reason.c_str(), target.c_str(), u->nick.c_str());
					else
						c->Kick(ci->WhoSends(), uc->user, "%s (Matches %s)", reason.c_str(), target.c_str());
				}
			}

			if (matched)
				source.Reply(_("Kicked %d/%d users matching %s from %s."), kicked, matched, target.c_str(), c->name.c_str());
			else
				source.Reply(_("No users on %s match %s."), c->name.c_str(), target.c_str());
		}
		else
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Kicks a selected nick on a channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
				"and above on the channel. Channel founders may use masks too."));
		return true;
	}
};

class CSKick : public Module
{
	CommandCSKick commandcskick;

 public:
	CSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcskick(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSKick)
