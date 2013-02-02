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

static Module *me;

class TempBan : public CallBack
{
 private:
 	Anope::string channel;
	Anope::string mask;

 public:
	TempBan(time_t seconds, Channel *c, const Anope::string &banmask) : CallBack(me, seconds), channel(c->name), mask(banmask) { }

	void Tick(time_t ctime) anope_override
	{
		Channel *c = Channel::Find(this->channel);
		if (c)
			c->RemoveMode(NULL, "BAN", this->mask);
	}
};

class CommandCSBan : public Command
{
 public:
	CommandCSBan(Module *creator) : Command(creator, "chanserv/ban", 2, 4)
	{
		this->SetDesc(_("Bans a given nick or mask on a channel"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] \037nick\037 [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] \037mask\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		Anope::string expiry, target, reason;
		time_t ban_time;
		if (params[1][0] == '+')
		{
			ban_time = Anope::DoTime(params[1]);
			if (params.size() < 3)
			{
				this->SendSyntax(source);
				return;
			}
			target = params[2];
			reason = "Requested";
			if (params.size() > 3)
				reason = params[3];
		}
		else
		{
			ban_time = 0;
			target = params[1];
			reason = "Requested";
			if (params.size() > 2)
				reason = params[2];
			if (params.size() > 3)
				reason += " " + params[3];
		}

		if (reason.length() > Config->CSReasonMax)
			reason = reason.substr(0, Config->CSReasonMax);

		Channel *c = ci->c;
		User *u = source.GetUser();
		User *u2 = User::Find(target, true);

		AccessGroup u_access = source.AccessFor(ci);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!u_access.HasPriv("BAN"))
			source.Reply(ACCESS_DENIED);
		else if (u2)
		{
			AccessGroup u2_access = ci->AccessFor(u2);

			if (u != u2 && ci->HasExt("PEACE") && u2_access >= u_access)
				source.Reply(ACCESS_DENIED);
			/*
			 * Dont ban/kick the user on channels where he is excepted
			 * to prevent services <-> server wars.
			 */
			else if (ci->c->MatchesList(u2, "EXCEPT"))
				source.Reply(CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
			else if (u2->IsProtected())
				source.Reply(ACCESS_DENIED);
			else
			{
				Anope::string mask = ci->GetIdealBan(u2);

				// XXX need a way to detect if someone is overriding
				Log(LOG_COMMAND, source, this, ci) << "for " << mask;

				if (!c->HasMode("BAN", mask))
				{
					c->SetMode(NULL, "BAN", mask);
					if (ban_time)
					{
						new TempBan(ban_time, c, mask);
						source.Reply(_("Ban on \002%s\002 expires in %s."), mask.c_str(), Anope::Duration(ban_time, source.GetAccount()).c_str());
					}
				}

				/* We still allow host banning while not allowing to kick */
				if (!c->FindUser(u2))
					return;

				if (ci->HasExt("SIGNKICK") || (ci->HasExt("SIGNKICK_LEVEL") && !source.AccessFor(ci).HasPriv("SIGNKICK")))
					c->Kick(ci->WhoSends(), u2, "%s (%s)", reason.c_str(), source.GetNick().c_str());
				else
					c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
			}
		}
		else if (u_access.HasPriv("FOUNDER"))
		{
			Log(LOG_COMMAND, source, this, ci) << "for " << target;

			if (!c->HasMode("BAN", target))
			{
				c->SetMode(NULL, "BAN", target);
				if (ban_time)
				{
					new TempBan(ban_time, c, target);
					source.Reply(_("Ban on \002%s\002 expires in %s."), target.c_str(), Anope::Duration(ban_time, source.GetAccount()).c_str());
				}
			}

			int matched = 0, kicked = 0;
			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end;)
			{
				ChanUserContainer *uc = *it++;

				if (Anope::Match(uc->user->nick, target) || Anope::Match(uc->user->GetDisplayedMask(), target))
				{
					++matched;

					AccessGroup u2_access = ci->AccessFor(uc->user);

					if (u != uc->user && ci->HasExt("PEACE") && u2_access >= u_access)
						continue;
					else if (ci->c->MatchesList(uc->user, "EXCEPT"))
						continue;
					else if (uc->user->IsProtected())
						continue;

					++kicked;
					if (ci->HasExt("SIGNKICK") || (ci->HasExt("SIGNKICK_LEVEL") && !u_access.HasPriv("SIGNKICK")))
						c->Kick(ci->WhoSends(), uc->user, "%s (Matches %s) (%s)", reason.c_str(), target.c_str(), source.GetNick().c_str());
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
		source.Reply(_("Bans a given nick or mask on a channel. An optional expiry may\n"
				"be given to cause services to remove the ban after a set amount\n"
				"of time.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
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
		me = this;
	}
};

MODULE_INIT(CSBan)
