// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

static Module *me;

class TempBan;
static std::vector<TempBan *> tempbans;

class TempBan final
	: public Timer
{
private:
	Anope::string channel;
	Anope::string mask;
	Anope::string mode;

public:
	TempBan(time_t seconds, Channel *c, const Anope::string &banmask, const Anope::string &mod)
		: Timer(me, seconds)
		, channel(c->name)
		, mask(banmask)
		, mode(mod)
	{
		tempbans.push_back(this);
	}

	~TempBan()
	{
		auto it = std::find(tempbans.begin(), tempbans.end(), this);
		if (it != tempbans.end())
			tempbans.erase(it);
	}

	bool Matches(Channel *chan, ChannelMode *cmode, const Anope::string &bmask) const
	{
		return chan->name.equals_ci(this->channel)
			&& cmode->name == this->mode
			&& bmask == this->mask;
	}

	void Tick() override
	{
		Channel *c = Channel::Find(this->channel);
		if (c)
			c->RemoveMode(NULL, mode, this->mask);
	}
};

class CommandCSBan final
	: public Command
{
public:
	CommandCSBan(Module *creator) : Command(creator, "chanserv/ban", 2, 4)
	{
		this->SetDesc(_("Bans a given nick or mask on a channel"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] {\037nick\037 | \037mask\037} [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const auto &block = Config->GetCommand(source);
		const Anope::string &mode = block.Get<Anope::string>("mode", "BAN");
		ChannelMode *cm = ModeManager::FindChannelModeByName(mode);
		if (cm == NULL)
			return;

		const Anope::string &chan = params[0];
		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		Channel *c = ci->c;
		if (c == NULL)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}
		else if (IRCD->GetMaxListFor(c, cm) && c->HasMode(mode) >= IRCD->GetMaxListFor(c, cm))
		{
			source.Reply(_("The %s list for %s is full."), mode.lower().c_str(), c->name.c_str());
			return;
		}

		Anope::string expiry, target, reason;
		time_t ban_time;
		if (params[1][0] == '+')
		{
			ban_time = Anope::DoTime(params[1]);
			if (ban_time < 0)
			{
				source.Reply(BAD_EXPIRY_TIME);
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

		auto reasonmax = Config->GetModule("chanserv").Get<unsigned>("reasonmax", "200");
		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		auto signkickformat = Config->GetModule("chanserv").Get<Anope::string>("signkickformat", "{message} ({nick})");

		User *u = source.GetUser();
		User *u2 = User::Find(target, true);

		AccessGroup u_access = source.AccessFor(ci);

		if (!u_access.HasPriv("BAN") && !source.HasPriv("chanserv/kick"))
			source.Reply(ACCESS_DENIED);
		else if (u2)
		{
			AccessGroup u2_access = ci->AccessFor(u2);

			if (u != u2 && ci->HasExt("PEACE") && u2_access >= u_access && !source.HasPriv("chanserv/kick"))
				source.Reply(ACCESS_DENIED);
			/*
			 * Don't ban/kick the user on channels where they are excepted
			 * to prevent services <-> server wars.
			 */
			else if (c->MatchesList(u2, "EXCEPT"))
				source.Reply(CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
			else if (u2->IsProtected())
				source.Reply(ACCESS_DENIED);
			else
			{
				Anope::string mask = ci->GetIdealBan(u2);

				bool override = !u_access.HasPriv("BAN") || (u != u2 && ci->HasExt("PEACE") && u2_access >= u_access);
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << mask;

				if (!c->HasMode(mode, mask))
				{
					c->SetMode(NULL, mode, mask);
					if (ban_time)
					{
						new TempBan(ban_time, c, mask, mode);
						source.Reply(_("Ban on \002%s\002 expires in %s."), mask.c_str(), Anope::Duration(ban_time, source.GetAccount()).c_str());
					}
				}

				/* We still allow host banning while not allowing to kick */
				if (!c->FindUser(u2))
					return;

				if (block.Get<bool>("kick", "yes"))
				{
					if (ci->HasExt("SIGNKICK") || (ci->HasExt("SIGNKICK_LEVEL") && !source.AccessFor(ci).HasPriv("SIGNKICK")))
					{
						signkickformat = Anope::Template(signkickformat, {
							{ "message", reason           },
							{ "nick",    source.GetNick() },
						});
						c->Kick(ci->WhoSends(), u2, signkickformat);
					}
					else
						c->Kick(ci->WhoSends(), u2, reason);
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
					source.Reply(_("Ban on \002%s\002 expires in %s."), mask.c_str(), Anope::Duration(ban_time, source.GetAccount()).c_str());
				}
			}

			int matched = 0, kicked = 0;
			for (auto it = c->users.begin(), it_end = c->users.end(); it != it_end;)
			{
				auto *memb = it->second;
				++it;

				Entry e(mask, mode);
				if (e.Matches(memb->user))
				{
					++matched;

					AccessGroup u2_access = ci->AccessFor(memb->user);

					if (matched > 1 && !founder)
						continue;
					if (u != memb->user && ci->HasExt("PEACE") && u2_access >= u_access)
						continue;
					else if (ci->c->MatchesList(memb->user, "EXCEPT"))
						continue;
					else if (memb->user->IsProtected())
						continue;

					if (block.Get<bool>("kick", "yes"))
					{
						++kicked;
						if (ci->HasExt("SIGNKICK") || (ci->HasExt("SIGNKICK_LEVEL") && !u_access.HasPriv("SIGNKICK")))
						{
							reason += " (Matches " + mask + ")";
							signkickformat = Anope::Template(signkickformat, {
								{ "message", reason           },
								{ "nick",    source.GetNick() },
							});
							c->Kick(ci->WhoSends(), memb->user, signkickformat);
						}
						else
							c->Kick(ci->WhoSends(), memb->user, "%s (Matches %s)", reason.c_str(), mask.c_str());
					}
				}
			}

			if (matched)
				source.Reply(_("Kicked %d/%d users matching %s from %s."), kicked, matched, mask.c_str(), c->name.c_str());
			else
				source.Reply(_("No users on %s match %s."), c->name.c_str(), mask.c_str());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Bans a given nick or mask on a channel. An optional expiry may "
			"be given to cause services to remove the ban after a set amount "
			"of time."
			"\n\n"
			"By default, limited to AOPs or those with level 5 access "
			"and above on the channel. Channel founders may ban masks."
		));
		return true;
	}
};

class CSBan final
	: public Module
{
	CommandCSBan commandcsban;

public:
	CSBan(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandcsban(this)
	{
		me = this;
	}

	EventReturn OnChannelModeUnset(Channel *c, MessageSource &setter, ChannelMode *cmode, const Anope::string &param) override
	{
		for (const auto *tempban : tempbans)
		{
			if (tempban->Matches(c, cmode, param))
			{
				delete tempban;
				break;
			}
		}
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSBan)
