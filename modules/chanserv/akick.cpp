/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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
#include "modules/chanserv/akick.h"

class AutoKickImpl : public AutoKick
{
	friend class AutoKickType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> mask, reason, creator;
	Serialize::Storage<time_t> addtime, last_time;

 public:
	using AutoKick::AutoKick;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *ci) override;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &mask) override;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *nc) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &r) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &c) override;

	time_t GetAddTime() override;
	void SetAddTime(const time_t &t) override;

	time_t GetLastUsed() override;
	void SetLastUsed(const time_t &t) override;
};

class AutoKickType : public Serialize::Type<AutoKickImpl>
{
 public:
	Serialize::ObjectField<AutoKickImpl, ChanServ::Channel *> channel;

	Serialize::Field<AutoKickImpl, Anope::string> mask;
	Serialize::ObjectField<AutoKickImpl, NickServ::Account *> account;

	Serialize::Field<AutoKickImpl, Anope::string> reason;
	Serialize::Field<AutoKickImpl, Anope::string> creator;
	Serialize::Field<AutoKickImpl, time_t> addtime;
	Serialize::Field<AutoKickImpl, time_t> last_time;

 	AutoKickType(Module *me)
		: Serialize::Type<AutoKickImpl>(me)
		, channel(this, "channel", &AutoKickImpl::channel, true)
		, mask(this, "mask", &AutoKickImpl::mask)
		, account(this, "account", &AutoKickImpl::account, true)
		, reason(this, "reason", &AutoKickImpl::reason)
		, creator(this, "creator", &AutoKickImpl::creator)
		, addtime(this, "addtime", &AutoKickImpl::addtime)
		, last_time(this, "last_time", &AutoKickImpl::last_time)
	{
	}
};

ChanServ::Channel *AutoKickImpl::GetChannel()
{
	return Get(&AutoKickType::channel);
}

void AutoKickImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&AutoKickType::channel, ci);
}

Anope::string AutoKickImpl::GetMask()
{
	return Get(&AutoKickType::mask);
}

void AutoKickImpl::SetMask(const Anope::string &m)
{
	Set(&AutoKickType::mask, m);
}

NickServ::Account *AutoKickImpl::GetAccount()
{
	return Get(&AutoKickType::account);
}

void AutoKickImpl::SetAccount(NickServ::Account *nc)
{
	Set(&AutoKickType::account, nc);
}

Anope::string AutoKickImpl::GetReason()
{
	return Get(&AutoKickType::reason);
}

void AutoKickImpl::SetReason(const Anope::string &r)
{
	Set(&AutoKickType::reason, r);
}

Anope::string AutoKickImpl::GetCreator()
{
	return Get(&AutoKickType::creator);
}

void AutoKickImpl::SetCreator(const Anope::string &c)
{
	Set(&AutoKickType::creator, c);
}

time_t AutoKickImpl::GetAddTime()
{
	return Get(&AutoKickType::addtime);
}

void AutoKickImpl::SetAddTime(const time_t &t)
{
	Set(&AutoKickType::addtime, t);
}

time_t AutoKickImpl::GetLastUsed()
{
	return Get(&AutoKickType::last_time);
}

void AutoKickImpl::SetLastUsed(const time_t &t)
{
	Set(&AutoKickType::last_time, t);
}

class CommandCSAKick : public Command
{
	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		Anope::string reason = params.size() > 3 ? params[3] : "";
		NickServ::Nick *na = NickServ::FindNick(mask);
		NickServ::Account *nc = NULL;
		unsigned reasonmax = Config->GetModule("chanserv/main")->Get<unsigned>("reasonmax", "200");

		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		if (IRCD->IsExtbanValid(mask))
			; /* If this is an extban don't try to complete the mask */
		else if (IRCD->IsChannelValid(mask))
		{
			/* Also don't try to complete the mask if this is a channel */

			if (mask.equals_ci(ci->GetName()) && ci->IsPeace())
			{
				source.Reply(_("Access denied."));
				return;
			}
		}
		else if (!na)
		{
			/* If the mask contains a realname the reason must be prepended with a : */
			if (mask.find('#') != Anope::string::npos)
			{
				size_t r = reason.find(':');
				if (r != Anope::string::npos)
				{
					mask += " " + reason.substr(0, r);
					mask.trim();
					reason = reason.substr(r + 1);
					reason.trim();
				}
				else
				{
					mask = mask + " " + reason;
					reason.clear();
				}
			}

			Entry e("", mask);

			mask = (e.nick.empty() ? "*" : e.nick) + "!"
				+ (e.user.empty() ? "*" : e.user) + "@"
				+ (e.host.empty() ? "*" : e.host);
			if (!e.real.empty())
				mask += "#" + e.real;
		}
		else
		{
			nc = na->GetAccount();
		}

		/* Check excepts BEFORE we get this far */
		if (Channel *c = ci->GetChannel())
		{
			std::vector<Anope::string> modes = c->GetModeList("EXCEPT");
			for (unsigned int i = 0; i < modes.size(); ++i)
			{
				if (Anope::Match(modes[i], mask))
				{
					source.Reply(_("\002{0}\002 matches an except on \002{1}\002 and cannot be banned until the except has been removed."), mask, ci->GetName());
					return;
				}
			}
		}

		/* Opers overriding get to bypass PEACE */
		if (source.IsOverride())
			;
		/* These peace checks are only for masks */
		else if (IRCD->IsChannelValid(mask))
			;
		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		else if (ci->IsPeace() && nc)
		{
			ChanServ::AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
			if (nc == ci->GetFounder() || nc_access >= u_access)
			{
				source.Reply(_("Access denied. You can not auto kick \002{0}\002 because they have more privileges than you on \002{1}\002 and \002{2}\002 is enabled."), nc->GetDisplay(), ci->GetName(), "PEACE");
				return;
			}
		}
		else if (ci->IsPeace())
		{
#warning "peace"
#if 0
			/* Match against all currently online users with equal or
			 * higher access. - Viper */
			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u2 = it->second;

				ChanServ::AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
				Entry entry_mask("", mask);

				if ((ci->AccessFor(u2).HasPriv("FOUNDER") || nc_access >= u_access) && entry_mask.Matches(u2))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			/* Match against the lastusermask of all nickalias's with equal
			 * or higher access. - Viper */
			for (nickalias_map::const_iterator it = NickServ::NickList->begin(), it_end = NickServ::NickList->end(); it != it_end; ++it)
			{
				na = it->second;

				ChanServ::AccessGroup nc_access = ci->AccessFor(na->GetAccount()), u_access = source.AccessFor(ci);
				if (na->GetAccount() && (na->GetAccount() == ci->GetFounder() || nc_access >= u_access))
				{
					Anope::string buf = na->GetNick() + "!" + na->GetLastUsermask();
					if (Anope::Match(buf, mask))
					{
						source.Reply(ACCESS_DENIED);
						return;
					}
				}
			 }
#endif
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);
			if (ak->GetAccount() ? ak->GetAccount() == nc : mask.equals_ci(ak->GetMask()))
			{
				source.Reply(_("\002{0}\002 already exists on \002{1}\002 autokick list."), ak->GetAccount() ? ak->GetAccount()->GetDisplay() : ak->GetMask(), ci->GetName());
				return;
			}
		}

		if (ci->GetAkickCount() >= Config->GetModule(this->GetOwner())->Get<unsigned>("autokickmax"))
		{
			source.Reply(_("Sorry, you can only have \002{0}\002 autokick masks on a channel."), Config->GetModule(this->GetOwner())->Get<unsigned>("autokickmax"));
			return;
		}

		AutoKick *ak;
		if (nc)
			ak = ci->AddAkick(source.GetNick(), nc, reason);
		else
			ak = ci->AddAkick(source.GetNick(), mask, reason);

		if (reason.empty())
			logger.Command(source, ci, _("{source} used {command} on {channel} to add {0}"), mask);
		else
			logger.Command(source, ci, _("{source} used {command} on {channel} to add {0} ({1})"), mask, reason);

		EventManager::Get()->Dispatch(&Event::Akick::OnAkickAdd, source, ci, ak);

		source.Reply(_("\002{0}\002 added to \002{1}\002 autokick list."), mask, ci->GetName());

		this->DoEnforce(source, ci);
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params[2];

		if (!ci->GetAkickCount())
		{
			source.Reply(_("The autokick list of \002{0}\002 is empty."), ci->GetName());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			unsigned int deleted = 0;

			NumberList(mask, true,
				[&](unsigned int number)
				{
					if (!number || number > ci->GetAkickCount())
						return;

					AutoKick *ak = ci->GetAkick(number - 1);

					EventManager::Get()->Dispatch(&Event::Akick::OnAkickDel, source, ci, ak);

					logger.Command(source, ci, _("{source} used {command} on {channel} to delete {0}"),
							ak->GetAccount() ? ak->GetAccount()->GetDisplay() : ak->GetMask());

					++deleted;
					ak->Delete();
				},
				[&]()
				{
					if (!deleted)
						source.Reply(_("There are no entries matching \002{0}\002 on the auto kick list of \002{1}\002."),
							mask, ci->GetName());
					else if (deleted == 1)
						source.Reply(_("Deleted \0021\002 entry from the autokick list of \002{0}\002."), ci->GetName());
					else
						source.Reply(_("Deleted \002{0}\002 entries from \002{1}\002 autokick list."), deleted, ci->GetName());
				});
		}
		else
		{
			NickServ::Nick *na = NickServ::FindNick(mask);
			NickServ::Account *nc = na ? na->GetAccount() : nullptr;
			AutoKick *match = nullptr;

			for (unsigned int i = 0; i < ci->GetAkickCount(); ++i)
			{
				AutoKick *ak = ci->GetAkick(i);

				if (ak->GetAccount() ? ak->GetAccount() == nc : mask.equals_ci(ak->GetMask()))
				{
					match = ak;
					break;
				}
			}

			if (match == nullptr)
			{
				source.Reply(_("\002{0}\002 was not found on the auto kick list of \002{1}\002."), mask, ci->GetName());
				return;
			}

			logger.Command(source, ci, _("{source} used {command} on {channel} to delete {0}"),
					match->GetAccount() ? match->GetAccount()->GetDisplay() : match->GetMask());

			EventManager::Get()->Dispatch(&Event::Akick::OnAkickDel, source, ci, match);

			source.Reply(_("\002{0}\002 deleted from the auto kick list of \002{1}\002."), match->GetAccount() ? match->GetAccount()->GetDisplay() : match->GetMask(), ci->GetName());

			match->Delete();
		}
	}

	void ProcessList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(mask, false,
				[&](unsigned int number)
				{
					if (!number || number > ci->GetAkickCount())
						return;

					AutoKick *ak = ci->GetAkick(number - 1);

					Anope::string timebuf, lastused;
					if (ak->GetAddTime())
						timebuf = Anope::strftime(ak->GetAddTime(), NULL, true);
					else
						timebuf = _("<unknown>");
					if (ak->GetLastUsed())
						lastused = Anope::strftime(ak->GetLastUsed(), NULL, true);
					else
						lastused = _("<unknown>");

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					if (ak->GetAccount())
						entry["Mask"] = ak->GetAccount()->GetDisplay();
					else
						entry["Mask"] = ak->GetMask();
					entry["Creator"] = ak->GetCreator();
					entry["Created"] = timebuf;
					entry["Last used"] = ak->GetLastUsed();
					entry["Reason"] = ak->GetReason();
					list.AddEntry(entry);
				},
				[]{});
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				AutoKick *ak = ci->GetAkick(i);

				if (!mask.empty())
				{
					if (!ak->GetAccount() && !Anope::Match(ak->GetMask(), mask))
						continue;
					if (ak->GetAccount() && !Anope::Match(ak->GetAccount()->GetDisplay(), mask))
						continue;
				}

				Anope::string timebuf, lastused;
				if (ak->GetAddTime())
					timebuf = Anope::strftime(ak->GetAddTime(), NULL, true);
				else
					timebuf = _("<unknown>");
				if (ak->GetLastUsed())
					lastused = Anope::strftime(ak->GetLastUsed(), NULL, true);
				else
					lastused = _("<unknown>");

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				if (ak->GetAccount())
					entry["Mask"] = ak->GetAccount()->GetDisplay();
				else
					entry["Mask"] = ak->GetMask();
				entry["Creator"] = ak->GetCreator();
				entry["Created"] = timebuf;
				entry["Last used"] = lastused;
				entry["Reason"] = ak->GetReason();
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("There are no entries matching \002{0}\002 on the autokick list of \002{1}\002."), mask, ci->GetName());
			return;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Autokick list for \002{0}\002:"), ci->GetName());

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of autokick list."));
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAkickCount())
		{
			source.Reply(_("The autokick list of \002{0}\002 is empty."), ci->GetName());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Reason"));
		this->ProcessList(source, ci, params, list);
	}

	void DoView(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAkickCount())
		{
			source.Reply(_("The autokick list of \002{0}\002 is empty."), ci->GetName());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Last used")).AddColumn(_("Reason"));
		this->ProcessList(source, ci, params, list);
	}

	void DoEnforce(CommandSource &source, ChanServ::Channel *ci)
	{
		Channel *c = ci->GetChannel();
		int count = 0;

		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
			return;
		}

		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			ChanUserContainer *uc = it->second;
			++it;

			if (c->CheckKick(uc->user))
				++count;
		}

		logger.Command(source, ci, _("{source} used {command} on {channel} to enforce the akick list, affects {0} users"), count);

		source.Reply(_("Autokick enforce for \002{0}\002 complete; \002{1}\002 users were affected."), ci->GetName(), count);
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		logger.Command(source, ci, _("{source} used {command} on {channel} to clear the akick list"));

		ci->ClearAkick();
		source.Reply(_("The autokick list of \002{0}\002 has been cleared."), ci->GetName());
	}

 public:
	CommandCSAKick(Module *creator) : Command(creator, "chanserv/akick", 2, 4)
	{
		this->SetDesc(_("Maintain the AutoKick list"));
		this->SetSyntax(_("\037channel\037 ADD {\037nick\037 | \037mask\037} [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037nick\037 | \037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 ENFORCE"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];
		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");
		
		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("AKICK") && !source.HasOverridePriv("chanserv/access/modify") && (!is_list || source.HasOverridePriv("chanserv/access/list")))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "AKICK", ci->GetName());
			return;
		}

		if (Anope::ReadOnly && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL") || cmd.equals_ci("CLEAR")))
		{
			source.Reply(_("Sorry, channel autokick list modification is temporarily disabled."));
			return;
		}

		if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, ci, params);
		else if (cmd.equals_ci("ENFORCE"))
			this->DoEnforce(source, ci);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
			source.Reply(_("The \002{0} ADD\002 command adds \037mask\037 to the auto kick list list of \037channel\037."
			               " If \037reason\037 is given, it is used when the user is kicked."
			               " If \037mask\037 is an account, then all users using the account will be affected.\n"
			               "\n"
			               "Examples:\n"
			               "         {command} #anope ADD Cronus noob!\n"
			               "         Adds the account \"Cronus\" to the auto kick list of \"#anope\" with the reason \"noob!\".\n"
			               "\n"
			               "         {command} #anope ADD Guest*!*@*\n"
			               "         Adds the mask \"Guest*!*@*\" to the auto kick list of \"#anope\"."),
			               source.GetCommand());
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes \037mask\037 from the auto kick list."
			               " It does not, however, remove any bans placed by an auto kick; those must be removed manually.\n"
			               "\n"
			               "Example:\n"
			               "         {command} #anope DEL Cronus!\n"
			               "         Removes the auto kick for \"Cronus\" from the auto kick list of \"#anope\".\n"),
			               source.GetCommand());
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 command displays the auto kick list of \037channel\037."
			               " If a wildcard mask is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the auto kick entry, when it was created, and when it was last used."
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope LIST 2-5,7-9\002\n"
			               "         Lists auto kick entries numbered 2 through 5 and 7 through 9 on #anope."),
			               source.GetCommand());
		else if (subcommand.equals_ci("ENFORCE"))
			source.Reply(_("The \002{0} ENFORCE\002 command enforces the auto kick list by forcibly removing users who match an entry on the auto kick list."
			               "This can be useful if someone does not authenticate or change their host mask until after joining.\n"
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope ENFORCE\002\n"
			               "         Enforces the auto kick list of #anope."),
			               source.GetCommand());
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command clears the auto kick list of \037channel\37. As with the \002DEL\002 command, existing bans placed by auto kick will not be removed.\n"
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope CLEAR\002\n"
			               "         Clears the auto kick list of #anope."),
			               source.GetCommand());
		else
		{
			source.Reply(_("Maintains the auto kick list for \037channel\037."
			               " If a user matching an entry on the auto kick list joins the channel, {0} will place a ban on the user and kick them.\n"
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
			               source.service->nick, "AKICK");
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (help)
				source.Reply(_("\n"
			 	               "The \002ADD\002 command adds \037nick\037 or \037mask\037 to the auto kick list of \037channel\037.\n"
				               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
				               "\n"
				               "The \002DEL\002 command removes \037mask\037 from the auto kick list of \037channel\037.\n"
				               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
				               "\n"
				               "The \002LIST\002 and \002VIEW\002 commands both display the auto kick list of \037channel\037, but \002VIEW\002 also displays who created the auto kick entry, when it was created, and when it was last used.\n"
				               "\002{msg}{service} {help} {command} [LIST | VIEW]\002 for more information.\n"
				               "\n"
				               "The \002ENFORCE\002 command enforces the auto kick list by forcibly removing users who match an entry on the auto kick list.\n"
				               "\002{msg}{service} {help} {command} ENFORCE\002 for more information.\n"
				               ""
				               "The \002CLEAR\002 clears the auto kick list of \037channel\037."
				               "\002{msg}{service} {help} {command} CLEAR\002 for more information.\n"),
				               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "help"_kw = help->cname, "command"_kw = source.GetCommand());
		}

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand = "") override
	{
		if (subcommand.equals_ci("ADD"))
		{
			SubcommandSyntaxError(source, subcommand, _("{\037nick\037 | \037mask\037} [\037reason\037]"));
		}
		else if (subcommand.equals_ci("DEL"))
		{
			SubcommandSyntaxError(source, subcommand, _("{\037nick\037 | \037mask\037 | \037entry-num\037 | \037list\037}"));
		}
		else
		{
			Command::OnSyntaxError(source, subcommand);
		}
	}
};

class CSAKick : public Module
	, public EventHook<Event::CheckKick>
{
	CommandCSAKick commandcsakick;
	AutoKickType akick_type;

 public:
	CSAKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CheckKick>(this)
		, commandcsakick(this)
		, akick_type(this)
	{
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		if (!ci || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);
			bool kick = false;

			if (ak->GetAccount())
				kick = ak->GetAccount() == u->Account();
			else if (IRCD->IsChannelValid(ak->GetMask()))
			{
				Channel *chan = Channel::Find(ak->GetMask());
				kick = chan != NULL && chan->FindUser(u);
			}
			else
				kick = Entry("BAN", ak->GetMask()).Matches(u);

			if (kick)
			{
				logger.Debug2("{0} matched akick {1}", u->nick, ak->GetAccount() ? ak->GetAccount()->GetDisplay() : ak->GetMask());

				ak->SetLastUsed(Anope::CurTime);
				if (!ak->GetAccount() && ak->GetMask().find('#') == Anope::string::npos)
					mask = ak->GetMask();
				reason = ak->GetReason();
				if (reason.empty())
				{
					reason = Language::Translate(u, Config->GetModule(this)->Get<Anope::string>("autokickreason"));
					reason = reason.replace_all_cs("%n", u->nick);
					reason = reason.replace_all_cs("%c", c->name);
				}
				if (reason.empty())
					reason = Language::Translate(u, _("User has been banned from the channel"));
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSAKick)
