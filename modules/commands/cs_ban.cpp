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

class CommandCSBan : public Command
{
 public:
	CommandCSBan(Module *creator) : Command(creator, "chanserv/ban", 2, 3)
	{
		this->SetDesc(_("Bans a selected nick on a channel"));
		this->SetSyntax(_("\037#channel\037 \037nick\037 [\037reason\037]"));
		this->SetSyntax(_("\037#channel\037 \037mask\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		User *u = source.u;
		Channel *c = ci->c;
		User *u2 = finduser(target);

		AccessGroup u_access = ci->AccessFor(u);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!u_access.HasPriv("BAN"))
			source.Reply(ACCESS_DENIED);
		/*
		 * Dont ban/kick the user on channels where he is excepted
		 * to prevent services <-> server wars.
		 */
		else if (u2)
		{
			AccessGroup u2_access = ci->AccessFor(u2);

			if (u != u2 && ci->HasFlag(CI_PEACE) && u2_access >= u_access)
				source.Reply(ACCESS_DENIED);
			else if (matches_list(ci->c, u2, CMODE_EXCEPT))
				source.Reply(CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
			else if (u2->IsProtected())
				source.Reply(ACCESS_DENIED);
			else
			{
				Anope::string mask;
				get_idealban(ci, u2, mask);

				// XXX need a way to detect if someone is overriding
				Log(LOG_COMMAND, u, this, ci) << "for " << mask;

				c->SetMode(NULL, CMODE_BAN, mask);

				/* We still allow host banning while not allowing to kick */
				if (!c->FindUser(u2))
					return;

				if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !ci->AccessFor(u).HasPriv("SIGNKICK")))
					c->Kick(ci->WhoSends(), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
				else
					c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
			}
		}
		else if (u_access.HasPriv("FOUNDER"))
		{
			Log(LOG_COMMAND, u, this, ci) << "for " << target;

			c->SetMode(NULL, CMODE_BAN, target);

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
					else if (matches_list(ci->c, uc->user, CMODE_EXCEPT))
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Bans a selected nick on a channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel. Channel founders may ban masks."));
		return true;
	}
};

class CSBan : public Module
{
	CommandCSBan commandcsban;

 public:
	CSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcsban(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSBan)
