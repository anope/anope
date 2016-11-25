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

static Module *me;

class TempBan : public Timer
{
 private:
 	Anope::string channel;
	Anope::string mask;
	Anope::string mode;

 public:
	TempBan(time_t seconds, Channel *c, const Anope::string &banmask, const Anope::string &mod) : Timer(me, seconds), channel(c->name), mask(banmask), mode(mod) { }

	void Tick(time_t ctime) override
	{
		Channel *c = Channel::Find(this->channel);
		if (c)
			c->RemoveMode(NULL, mode, this->mask);
	}
};

class CommandCSBan : public Command
{
 public:
	CommandCSBan(Module *creator) : Command(creator, "chanserv/ban", 2, 4)
	{
		this->SetDesc(_("Bans a given nick or mask on a channel"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] {\037nick\037 | \037mask\037} [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		Configuration::Block *block = Config->GetCommand(source);
		const Anope::string &mode = block->Get<Anope::string>("mode", "BAN");

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		Channel *c = ci->c;
		if (c == NULL)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
			return;
		}

		if (IRCD->GetMaxListFor(c) && c->HasMode(mode) >= IRCD->GetMaxListFor(c))
		{
			source.Reply(_("The %s list for %s is full."), mode.lower().c_str(), c->name.c_str());
			return;
		}

		Anope::string expiry, target, reason;
		time_t ban_time;
		if (params[1][0] == '+')
		{
			ban_time = Anope::DoTime(params[1]);
			if (ban_time == -1)
			{
				source.Reply(_("Invalid expiry time \002{0}\002."), params[1]);
				return;
			}
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

		unsigned reasonmax = Config->GetModule("chanserv/main")->Get<unsigned>("reasonmax", "200");
		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		Anope::string signkickformat = Config->GetModule("chanserv/main")->Get<Anope::string>("signkickformat", "%m (%n)");
		signkickformat = signkickformat.replace_all_cs("%n", source.GetNick());

		User *u = source.GetUser();
		User *u2 = User::Find(target, true);

		ChanServ::AccessGroup u_access = source.AccessFor(ci);

		if (!u_access.HasPriv("BAN") && !source.HasPriv("chanserv/kick"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "BAN", ci->GetName());
			return;
		}

		if (u2)
		{
			ChanServ::AccessGroup u2_access = ci->AccessFor(u2);

			if (u != u2 && ci->IsPeace() && u2_access >= u_access && !source.HasPriv("chanserv/kick"))
			{
				source.Reply(_("Access denied. \002{0}\002 has the same or more privileges than you on \002{1}\002."), u2->nick, ci->GetName());
				return;
			}

			/*
			 * Don't ban/kick the user on channels where he is excepted
			 * to prevent services <-> server wars.
			 */
			if (c->MatchesList(u2, "EXCEPT"))
			{
				source.Reply(_("\002{0}\002 matches an except on \002{1}\002 and can not be banned until the except has been removed."), u2->nick, ci->GetName());
				return;
			}

			if (u2->IsProtected())
			{
				source.Reply(_("Access denied. \002{0}\002 is protected and can not be kicked."), u2->nick);
				return;
			}

			Anope::string mask = ci->GetIdealBan(u2);

			bool override = !u_access.HasPriv("BAN") || (u != u2 && ci->IsPeace() && u2_access >= u_access);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << mask;

			if (!c->HasMode(mode, mask))
			{
				c->SetMode(NULL, mode, mask);
				if (ban_time)
				{
					new TempBan(ban_time, c, mask, mode);
					source.Reply(_("Ban on \002{0}\002 expires in \002{1}\002."), mask, Anope::Duration(ban_time, source.GetAccount()));
				}
			}

			/* We still allow host banning while not allowing to kick */
			if (!c->FindUser(u2))
				return;

			if (block->Get<bool>("kick", "yes"))
			{
				if (ci->IsSignKick() || (ci->IsSignKickLevel() && !source.AccessFor(ci).HasPriv("SIGNKICK")))
				{
					signkickformat = signkickformat.replace_all_cs("%m", reason);
					c->Kick(ci->WhoSends(), u2, "%s", signkickformat.c_str());
				}
				else
				{
					c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
				}
			}
		}
		else
		{
			bool founder = u_access.HasPriv("FOUNDER");
			bool override = !founder && !u_access.HasPriv("BAN");

			Anope::string mask = IRCD->NormalizeMask(target);

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << mask;

			if (!c->HasMode(mode, mask))
			{
				c->SetMode(NULL, mode, mask);
				if (ban_time)
				{
					new TempBan(ban_time, c, mask, mode);
					source.Reply(_("Ban on \002{0}\002 expires in \002{1}\002."), mask, Anope::Duration(ban_time, source.GetAccount()));
				}
			}

			int matched = 0, kicked = 0;
			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end;)
			{
				ChanUserContainer *uc = it->second;
				++it;

				Entry e(mode, mask);
				if (e.Matches(uc->user))
				{
					++matched;

					ChanServ::AccessGroup u2_access = ci->AccessFor(uc->user);

					if (matched > 1 && !founder)
						continue;
					if (u != uc->user && ci->IsPeace() && u2_access >= u_access)
						continue;
					else if (ci->c->MatchesList(uc->user, "EXCEPT"))
						continue;
					else if (uc->user->IsProtected())
						continue;

					if (block->Get<bool>("kick", "yes"))
					{
						++kicked;

						if (ci->IsSignKick() || (ci->IsSignKickLevel() && !u_access.HasPriv("SIGNKICK")))
						{
							reason += " (Matches " + mask + ")";
							signkickformat = signkickformat.replace_all_cs("%m", reason);
							c->Kick(ci->WhoSends(), uc->user, "%s", signkickformat.c_str());
						}
						else
						{
							c->Kick(ci->WhoSends(), uc->user, "%s (Matches %s)", reason.c_str(), mask.c_str());
						}
					}
				}
			}

			if (matched)
				source.Reply(_("Kicked \002{0}/{1}\002 users matching \002{2}\002 from \002{3}\002."), kicked, matched, mask, c->name);
			else
				source.Reply(_("No users on \002{0}\002 match \002{1}\002."), c->name, mask);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Bans a given \037nick\037 or \037mask\037 on \002channel\002. An optional expiry may be given to cause services to remove the ban after a set amount of time.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037.\n"
		               "\n"
		               "Examples:\n"
			       "         {command} #anope Jobe\n"
			       "         Bans the user \"Jobe\" from \"#anope\".\n"
			       "\n"
			       "         {command} #anope Guest!*@*\n"
			       "         Bans the mask \"Guest!*@*\" on \"#anope\"."),
		               "BAN");
		return true;
	}
};

class CSBan : public Module
{
	CommandCSBan commandcsban;

 public:
	CSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsban(this)
	{
		me = this;
	}
};

MODULE_INIT(CSBan)
