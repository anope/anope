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
#include "modules/cs_akick.h"

class AutoKickImpl : public AutoKick
{
 public:
	AutoKickImpl(Serialize::TypeBase *type) : AutoKick(type) { }
        AutoKickImpl(Serialize::TypeBase *type, Serialize::ID id) : AutoKick(type, id) { }

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
	Serialize::ObjectField<AutoKickImpl, ChanServ::Channel *> ci;

	Serialize::Field<AutoKickImpl, Anope::string> mask;
	Serialize::ObjectField<AutoKickImpl, NickServ::Account *> nc;

	Serialize::Field<AutoKickImpl, Anope::string> reason;
	Serialize::Field<AutoKickImpl, Anope::string> creator;
	Serialize::Field<AutoKickImpl, time_t> addtime;
	Serialize::Field<AutoKickImpl, time_t> last_time;

 	AutoKickType(Module *me)
		: Serialize::Type<AutoKickImpl>(me, "AutoKick")
		, ci(this, "ci", true)
		, mask(this, "mask")
		, nc(this, "nc", true)
		, reason(this, "reason")
		, creator(this, "creator")
		, addtime(this, "addtime")
		, last_time(this, "last_time")
	{
	}
};

ChanServ::Channel *AutoKickImpl::GetChannel()
{
	return Get(&AutoKickType::ci);
}

void AutoKickImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&AutoKickType::ci, ci);
}

Anope::string AutoKickImpl::GetMask()
{
	return Get(&AutoKickType::mask);
}

void AutoKickImpl::SetMask(const Anope::string &mask)
{
	Set(&AutoKickType::mask, mask);
}

NickServ::Account *AutoKickImpl::GetAccount()
{
	return Get(&AutoKickType::nc);
}

void AutoKickImpl::SetAccount(NickServ::Account *nc)
{
	Set(&AutoKickType::nc, nc);
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
		unsigned reasonmax = Config->GetModule("chanserv")->Get<unsigned>("reasonmax", "200");

		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		if (IRCD->IsExtbanValid(mask))
			; /* If this is an extban don't try to complete the mask */
		else if (IRCD->IsChannelValid(mask))
		{
			/* Also don't try to complete the mask if this is a channel */

			if (mask.equals_ci(ci->GetName()) && ci->HasFieldS("PEACE"))
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
			nc = na->GetAccount();

		/* Check excepts BEFORE we get this far */
		if (ci->c)
		{
			std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> modes = ci->c->GetModeList("EXCEPT");
			for (; modes.first != modes.second; ++modes.first)
			{
				if (Anope::Match(modes.first->second, mask))
				{
					source.Reply(_("\002{0}\002 matches an except on \002{1}\002 and cannot be banned until the except has been removed."), mask, ci->GetName());
					return;
				}
			}
		}

		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		/* Opers overriding get to bypass PEACE */
		if (override)
			;
		/* These peace checks are only for masks */
		else if (IRCD->IsChannelValid(mask))
			;
		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		else if (ci->HasFieldS("PEACE") && nc)
		{
			ChanServ::AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
			if (nc == ci->GetFounder() || nc_access >= u_access)
			{
				source.Reply(_("Access denied. You can not auto kick \002{0}\002 because they have more privileges than you on \002{1}\002 and \002{2}\002 is enabled."), nc->GetDisplay(), ci->GetName(), "PEACE");
				return;
			}
		}
		else if (ci->HasFieldS("PEACE"))
		{
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

		if (ci->GetAkickCount() >= Config->GetModule(this->owner)->Get<unsigned>("autokickmax"))
		{
			source.Reply(_("Sorry, you can only have \002{0}\002 autokick masks on a channel."), Config->GetModule(this->owner)->Get<unsigned>("autokickmax"));
			return;
		}

		AutoKick *ak;
		if (nc)
			ak = ci->AddAkick(source.GetNick(), nc, reason);
		else
			ak = ci->AddAkick(source.GetNick(), mask, reason);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << (reason == "" ? "" : ": ") << reason;

		this->akickevents(&Event::Akick::OnAkickAdd, source, ci, ak);

		source.Reply(_("\002{0}\002 added to \002{1}\002 autokick list."), mask, ci->GetName());

		this->DoEnforce(source, ci);
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params[2];
		bool override = !source.AccessFor(ci).HasPriv("AKICK");

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

					this->akickevents(&Event::Akick::OnAkickDel, source, ci, ak);

					Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << (ak->GetAccount() ? ak->GetAccount()->GetDisplay() : ak->GetMask());

					++deleted;
					delete ak;
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

			unsigned int i, end;
			for (i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				AutoKick *ak = ci->GetAkick(i);

				if (ak->GetAccount() ? ak->GetAccount() == nc : mask.equals_ci(ak->GetMask()))
					break;
			}

			if (i == ci->GetAkickCount())
			{
				source.Reply(_("\002{0}\002 was not found on the auto kick list of \002{1}\002."), mask, ci->GetName());
				return;
			}

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;

			this->akickevents(&Event::Akick::OnAkickDel, source, ci, ci->GetAkick(i));

			delete ci->GetAkick(i);

			source.Reply(_("\002{0}\002 deleted from the auto kick list of \002{1}\002."), mask, ci->GetName());
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
		Channel *c = ci->c;
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

		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "ENFORCE, affects " << count << " users";

		source.Reply(_("Autokick enforce for \002{0}\002 complete; \002{1}\002 users were affected."), ci->GetName(), count);
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the akick list";

		ci->ClearAkick();
		source.Reply(_("The autokick list of \002{0}\002 has been cleared."), ci->GetName());
	}

	EventHandlers<Event::Akick> &akickevents;

 public:
	CommandCSAKick(Module *creator, EventHandlers<Event::Akick> &events) : Command(creator, "chanserv/akick", 2, 4), akickevents(events)
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

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("AKICK") && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "AKICK", ci->GetName());
			return;
		}

		if (Anope::ReadOnly && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
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
			this->OnSyntaxError(source, "");
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
			               source.command);
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes \037mask\037 from the auto kick list."
			               " It does not, however, remove any bans placed by an auto kick; those must be removed manually.\n"
			               "\n"
			               "Example:\n"
			               "         {command} #anope DEL Cronus!\n"
			               "         Removes the auto kick for \"Cronus\" from the auto kick list of \"#anope\".\n"),
			               source.command);
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 command displays the auto kick list of \037channel\037."
			               " If a wildcard mask is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the auto kick entry, when it was created, and when it was last used."
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope LIST 2-5,7-9\002\n"
			               "         Lists auto kick entries numbered 2 through 5 and 7 through 9 on #anope."),
			               source.command);
		else if (subcommand.equals_ci("ENFORCE"))
			source.Reply(_("The \002{0} ENFORCE\002 command enforces the auto kick list by forcibly removing users who match an entry on the auto kick list."
			               "This can be useful if someone does not authenticate or change their host mask until after joining.\n"
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope ENFORCE\002\n"
			               "         Enforces the auto kick list of #anope."),
			               source.command);
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command clears the auto kick list of \037channel\37. As with the \002DEL\002 command, existing bans placed by auto kick will not be removed.\n"
			               "\n"
			               "Example:\n"
			               "         \002{0} #anope CLEAR\002\n"
			               "         Clears the auto kick list of #anope."),
			               source.command);
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
				               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "help"_kw = help->cname, "command"_kw = source.command);
		}

		return true;
	}
};

class CSAKick : public Module
	, public EventHook<Event::CheckKick>
{
	CommandCSAKick commandcsakick;
	EventHandlers<Event::Akick> akickevents;
	AutoKickType akick_type;

 public:
	CSAKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CheckKick>()
		, commandcsakick(this, akickevents)
		, akickevents(this)
		, akick_type(this)
	{
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (!c->ci || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;

		for (unsigned j = 0, end = c->ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = c->ci->GetAkick(j);
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
				Log(LOG_DEBUG_2) << u->nick << " matched akick " << (ak->GetAccount() ? ak->GetAccount()->GetDisplay() : ak->GetMask());
				ak->SetLastUsed(Anope::CurTime);
				if (!ak->GetAccount() && ak->GetMask().find('#') == Anope::string::npos)
					mask = ak->GetMask();
				reason = ak->GetReason();
				if (reason.empty())
					reason = Language::Translate(u, Config->GetModule(this)->Get<const Anope::string>("autokickreason").c_str());
				if (reason.empty())
					reason = Language::Translate(u, _("User has been banned from the channel"));
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSAKick)
