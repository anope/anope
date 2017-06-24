/*
 * Anope IRC Services
 *
 * Copyright (C) 2017 Anope Team <team@anope.org>
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

class BanImpl : public HostServ::Ban
{
	friend class BanType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> creator, reason;
	Serialize::Storage<time_t> created, expires;

 public:
	using HostServ::Ban::Ban;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &) override;

	time_t GetCreated() override;
	void SetCreated(time_t) override;

	time_t GetExpires() override;
	void SetExpires(time_t) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &) override;
};

class BanType : public Serialize::Type<BanImpl>
{
 public:
	Serialize::ObjectField<BanImpl, NickServ::Account *> account;
	Serialize::Field<BanImpl, Anope::string> creator;
	Serialize::Field<BanImpl, time_t> created, expires;
	Serialize::Field<BanImpl, Anope::string> reason;

	BanType(Module *me) : Serialize::Type<BanImpl>(me)
		, account(this, "account", &BanImpl::account, true)
		, creator(this, "creator", &BanImpl::creator)
		, created(this, "created", &BanImpl::created)
		, expires(this, "expires", &BanImpl::expires)
		, reason(this, "reason", &BanImpl::reason)
	{
	}
};

NickServ::Account *BanImpl::GetAccount()
{
	return Get(&BanType::account);
}

void BanImpl::SetAccount(NickServ::Account *acc)
{
	Set(&BanType::account, acc);
}

Anope::string BanImpl::GetCreator()
{
	return Get(&BanType::creator);
}

void BanImpl::SetCreator(const Anope::string &c)
{
	Set(&BanType::creator, c);
}

time_t BanImpl::GetCreated()
{
	return Get(&BanType::created);
}

void BanImpl::SetCreated(time_t cr)
{
	Set(&BanType::created, cr);
}

time_t BanImpl::GetExpires()
{
	return Get(&BanType::expires);
}

void BanImpl::SetExpires(time_t ex)
{
	Set(&BanType::expires, ex);
}

Anope::string BanImpl::GetReason()
{
	return Get(&BanType::reason);
}

void BanImpl::SetReason(const Anope::string &r)
{
	Set(&BanType::reason, r);
}

class CommandHSBan : public Command
{
 public:
	CommandHSBan(Module *creator) : Command(creator, "hostserv/ban", 2, 3)
	{
		this->SetDesc(_("Ban a user from requesting vhosts"));
		this->SetSyntax(_("\037user\037 [+\037expiry\037] \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &user = params[0];
		Anope::string expiry, reason;

		if (params[1][0] == '+')
		{
			expiry = params[1];
			reason = params.size() > 2 ? params[2] : "";
		}
		else
		{
			reason = params[2];
			if (params.size() > 3)
				reason += " " + params[3];
		}

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : Config->GetModule(this->GetOwner())->Get<time_t>("eexpiry", "30d");

		if (reason.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (expires && expires < 60)
		{
			source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
			return;
		}

		if (expires > 0)
		{
			expires += Anope::CurTime;
		}

		NickServ::Nick *nick = NickServ::FindNick(user);
		if (nick == nullptr)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}

		HostServ::Ban *ban = nick->GetAccount()->GetRef<HostServ::Ban *>();
		if (ban != nullptr)
		{
			source.Reply(_("\002{0}\002 is already banned from requesting vhosts."), nick->GetAccount()->GetDisplay());
			return;
		}

		ban = Serialize::New<HostServ::Ban *>();
		ban->SetAccount(nick->GetAccount());
		ban->SetCreator(source.GetNick());
		ban->SetCreated(Anope::CurTime);
		ban->SetExpires(expires);
		ban->SetReason(reason);

		logger.Command(LogType::ADMIN, source, _("{source} used {command} to ban {0} from requesting new vhosts"), nick->GetAccount()->GetDisplay());

		source.Reply(_("\002{0}\002 has been banned from requesting new vhosts."), nick->GetAccount()->GetDisplay());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Ban the given \037user\037 from requesting vhosts for the given duration."));
		return true;
	}
};

class CommandHSUnban : public Command
{
 public:
	CommandHSUnban(Module *creator) : Command(creator, "hostserv/unban", 1, 1)
	{
		this->SetDesc(_("Unban a user from requesting vhosts"));
		this->SetSyntax(_("\037user\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &user = params[0];

		if (user.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			std::vector<HostServ::Ban *> bans = Serialize::GetObjects<HostServ::Ban *>();
			unsigned int deleted = 0;
			Anope::string nicks;

			NumberList(user, true,
				[&](unsigned int num)
				{
					if (num == 0 || num > bans.size())
						return;

					HostServ::Ban *ban = bans[num - 1];

					if (!nicks.empty())
						nicks.append(", ");
					nicks.append(ban->GetAccount()->GetDisplay());

					ban->Delete();

					++deleted;
				},
				[&]()
				{
					if (deleted == 0)
					{
						source.Reply(_("No matching entries on ban list."));
						return;
					}

					logger.Command(LogType::ADMIN, source, _("{source} used {command} to unban {0} from requesting new vhosts"), nicks);

					source.Reply(_("Deleted \002{0}\002 entries from the ban list."), deleted);
				});
		}
		else
		{
			NickServ::Nick *nick = NickServ::FindNick(user);
			if (nick == nullptr)
			{
				source.Reply(_("\002{0}\002 isn't registered."), user);
				return;
			}

			HostServ::Ban *ban = nick->GetAccount()->GetRef<HostServ::Ban *>();
			if (ban == nullptr)
			{
				source.Reply(_("\002{0}\002 isn't banned."), nick->GetNick());
				return;
			}

			logger.Command(LogType::ADMIN, source, _("{source} used {command} to unban {0} from requesting new vhosts"), nick->GetAccount()->GetDisplay());

			source.Reply(_("\002{0}\002 has been unbanned from requesting new vhosts."), nick->GetAccount()->GetDisplay());

			ban->Delete();
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unban the given \037user\037 from requesting vhosts."));
		return true;
	}
};

class CommandHSBanlist : public Command
{
 public:
	CommandHSBanlist(Module *creator) : Command(creator, "hostserv/banlist", 0, 1)
	{
		this->SetDesc(_("View the list of banned users"));
		this->SetSyntax(_("[\037search\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &search = params.size() > 0 ? params[0] : "";
		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Number"))
			.AddColumn(_("Nick"))
			.AddColumn(_("Expires"))
			.AddColumn(_("Creator"))
			.AddColumn(_("Created"))
			.AddColumn(_("Reason"));

		unsigned int count = 0, displayed = 0;

		for (HostServ::Ban *ban : Serialize::GetObjects<HostServ::Ban *>())
		{
			++count;

			if (!search.empty() && !Anope::Match(ban->GetAccount()->GetDisplay(), search))
				continue;

			++displayed;

			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(count);
			entry["Nick"] = ban->GetAccount()->GetDisplay();
			entry["Expires"] = Anope::Expires(ban->GetExpires(), source.GetAccount());
			entry["Creator"] = ban->GetCreator();
			entry["Created"] = Anope::strftime(ban->GetCreated(), NULL, true);
			entry["Reason"] = ban->GetReason();
			list.AddEntry(entry);
		}

		if (count == 0)
		{
			source.Reply(_("Ban list is empty."));
			return;
		}

		if (displayed == 0)
		{
			source.Reply(_("No entries match \002{0}\002."), search);
			return;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const Anope::string &reply : replies)
			source.Reply(reply);

		source.Reply(_("Displayed \002{0}\002/\002{1}\002 records."), displayed, count);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists the users banned from requesting vhosts."));
		return true;
	}
};

class HSBan : public Module
	, public EventHook<Event::VhostRequest>
{
	CommandHSBan commandhsban;
	CommandHSUnban commandhsunban;
	CommandHSBanlist commandhsbanlist;

	BanType bantype;

 public:
	HSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::VhostRequest>(this)
		, commandhsban(this)
		, commandhsunban(this)
		, commandhsbanlist(this)
		, bantype(this)
	{
	}

	EventReturn OnVhostRequest(CommandSource *source, NickServ::Account *account, const Anope::string &user, const Anope::string &host) override
	{
		HostServ::Ban *ban = account->GetRef<HostServ::Ban *>();
		if (ban == nullptr)
			return EVENT_CONTINUE;

		if (source)
		{
			if (ban->GetExpires() > 0)
				source->Reply(_("You are banned from requesting vhosts until {0}. Reason: {1}"),
					Anope::strftime(ban->GetExpires(), account), ban->GetReason());
			else
				source->Reply(_("You are banned from requesting vhosts. Reason: {0}"),
					ban->GetReason());
		}

		return EVENT_STOP;
	}
};

MODULE_INIT(HSBan)
