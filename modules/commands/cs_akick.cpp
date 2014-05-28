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
	~AutoKickImpl()
	{
		if (this->ci)
		{
			std::vector<AutoKick *>::iterator it = std::find(this->ci->akick->begin(), this->ci->akick->end(), this);
			if (it != this->ci->akick->end())
				this->ci->akick->erase(it);

			const NickServ::Nick *na = NickServ::FindNick(this->mask);
			if (na != NULL)
				na->nc->RemoveChannelReference(this->ci);
		}
	}

	void Serialize(Serialize::Data &data) const override
	{
		data["ci"] << this->ci->name;
		if (this->nc)
			data["nc"] << this->nc->display;
		else
			data["mask"] << this->mask;
		data["reason"] << this->reason;
		data["creator"] << this->creator;
		data.SetType("addtime", Serialize::Data::DT_INT); data["addtime"] << this->addtime;
		data.SetType("last_used", Serialize::Data::DT_INT); data["last_used"] << this->last_used;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string sci, snc;

		data["ci"] >> sci;
		data["nc"] >> snc;

		ChanServ::Channel *ci = ChanServ::Find(sci);
		if (!ci)
			return NULL;

		AutoKick *ak;
		NickServ::Account *nc = NickServ::FindAccount(snc);
		if (obj)
		{
			ak = anope_dynamic_static_cast<AutoKick *>(obj);
			data["creator"] >> ak->creator;
			data["reason"] >> ak->reason;
			ak->nc = NickServ::FindAccount(snc);
			data["mask"] >> ak->mask;
			data["addtime"] >> ak->addtime;
			data["last_used"] >> ak->last_used;
		}
		else
		{
			time_t addtime, lastused;
			data["addtime"] >> addtime;
			data["last_used"] >> lastused;

			Anope::string screator, sreason, smask;

			data["creator"] >> screator;
			data["reason"] >> sreason;
			data["mask"] >> smask;

			if (nc)
				ak = ci->AddAkick(screator, nc, sreason, addtime, lastused);
			else
				ak = ci->AddAkick(screator, smask, sreason, addtime, lastused);
		}

		return ak;
	}
};

class CommandCSAKick : public Command
{
	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		Anope::string reason = params.size() > 3 ? params[3] : "";
		const NickServ::Nick *na = NickServ::FindNick(mask);
		NickServ::Account *nc = NULL;
		unsigned reasonmax = Config->GetModule("chanserv")->Get<unsigned>("reasonmax", "200");

		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		if (IRCD->IsExtbanValid(mask))
			; /* If this is an extban don't try to complete the mask */
		else if (IRCD->IsChannelValid(mask))
		{
			/* Also don't try to complete the mask if this is a channel */

			if (mask.equals_ci(ci->name) && ci->HasExt("PEACE"))
			{
				source.Reply(ACCESS_DENIED);
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
			nc = na->nc;

		/* Check excepts BEFORE we get this far */
		if (ci->c)
		{
			std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> modes = ci->c->GetModeList("EXCEPT");
			for (; modes.first != modes.second; ++modes.first)
			{
				if (Anope::Match(modes.first->second, mask))
				{
					source.Reply(CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
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
		else if (ci->HasExt("PEACE") && nc)
		{
			ChanServ::AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
			if (nc == ci->GetFounder() || nc_access >= u_access)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}
		else if (ci->HasExt("PEACE"))
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

				ChanServ::AccessGroup nc_access = ci->AccessFor(na->nc), u_access = source.AccessFor(ci);
				if (na->nc && (na->nc == ci->GetFounder() || nc_access >= u_access))
				{
					Anope::string buf = na->nick + "!" + na->last_usermask;
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
			if (ak->nc ? ak->nc == nc : mask.equals_ci(ak->mask))
			{
				source.Reply(_("\002%s\002 already exists on %s autokick list."), ak->nc ? ak->nc->display.c_str() : ak->mask.c_str(), ci->name.c_str());
				return;
			}
		}

		if (ci->GetAkickCount() >= Config->GetModule(this->owner)->Get<unsigned>("autokickmax"))
		{
			source.Reply(_("Sorry, you can only have %d autokick masks on a channel."), Config->GetModule(this->owner)->Get<unsigned>("autokickmax"));
			return;
		}

		AutoKick *ak;
		if (nc)
			ak = ci->AddAkick(source.GetNick(), nc, reason);
		else
			ak = ci->AddAkick(source.GetNick(), mask, reason);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << (reason == "" ? "" : ": ") << reason;

		this->akickevents(&Event::Akick::OnAkickAdd, source, ci, ak);

		source.Reply(_("\002%s\002 added to %s autokick list."), mask.c_str(), ci->name.c_str());

		this->DoEnforce(source, ci);
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params[2];
		unsigned i, end;

		if (!ci->GetAkickCount())
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AkickDelCallback : public NumberList
			{
				CommandSource &source;
				ChanServ::Channel *ci;
				CommandCSAKick *c;
				unsigned deleted;
				ChanServ::AccessGroup ag;
			 public:
				AkickDelCallback(CommandSource &_source, ChanServ::Channel *_ci, CommandCSAKick *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c), deleted(0), ag(source.AccessFor(ci))
				{
				}

				~AkickDelCallback()
				{
					if (!deleted)
						source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
					else if (deleted == 1)
						source.Reply(_("Deleted 1 entry from %s autokick list."), ci->name.c_str());
					else
						source.Reply(_("Deleted %d entries from %s autokick list."), deleted, ci->name.c_str());
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ci->GetAkickCount())
						return;

					const AutoKick *ak = ci->GetAkick(number - 1);

					c->akickevents(&Event::Akick::OnAkickDel, source, ci, ak);

					bool override = !ag.HasPriv("AKICK");
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << (ak->nc ? ak->nc->display : ak->mask);

					++deleted;
					ci->EraseAkick(number - 1);
				}
			}
			delcallback(source, ci, this, mask);
			delcallback.Process();
		}
		else
		{
			const NickServ::Nick *na = NickServ::FindNick(mask);
			const NickServ::Account *nc = na ? *na->nc : NULL;

			for (i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				const AutoKick *ak = ci->GetAkick(i);

				if (ak->nc ? ak->nc == nc : mask.equals_ci(ak->mask))
					break;
			}

			if (i == ci->GetAkickCount())
			{
				source.Reply(_("\002%s\002 not found on %s autokick list."), mask.c_str(), ci->name.c_str());
				return;
			}

			bool override = !source.AccessFor(ci).HasPriv("AKICK");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;

			this->akickevents(&Event::Akick::OnAkickDel, source, ci, ci->GetAkick(i));

			ci->EraseAkick(i);

			source.Reply(_("\002%s\002 deleted from %s autokick list."), mask.c_str(), ci->name.c_str());
		}
	}

	void ProcessList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AkickListCallback : public NumberList
			{
				ListFormatter &list;
				ChanServ::Channel *ci;

			 public:
				AkickListCallback(ListFormatter &_list, ChanServ::Channel *_ci, const Anope::string &numlist) : NumberList(numlist, false), list(_list), ci(_ci)
				{
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ci->GetAkickCount())
						return;

					const AutoKick *ak = ci->GetAkick(number - 1);

					Anope::string timebuf, lastused;
					if (ak->addtime)
						timebuf = Anope::strftime(ak->addtime, NULL, true);
					else
						timebuf = UNKNOWN;
					if (ak->last_used)
						lastused = Anope::strftime(ak->last_used, NULL, true);
					else
						lastused = UNKNOWN;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					if (ak->nc)
						entry["Mask"] = ak->nc->display;
					else
						entry["Mask"] = ak->mask;
					entry["Creator"] = ak->creator;
					entry["Created"] = timebuf;
					entry["Last used"] = lastused;
					entry["Reason"] = ak->reason;
					this->list.AddEntry(entry);
				}
			}
			nl_list(list, ci, mask);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				const AutoKick *ak = ci->GetAkick(i);

				if (!mask.empty())
				{
					if (!ak->nc && !Anope::Match(ak->mask, mask))
						continue;
					if (ak->nc && !Anope::Match(ak->nc->display, mask))
						continue;
				}

				Anope::string timebuf, lastused;
				if (ak->addtime)
					timebuf = Anope::strftime(ak->addtime, NULL, true);
				else
					timebuf = UNKNOWN;
				if (ak->last_used)
					lastused = Anope::strftime(ak->last_used, NULL, true);
				else
					lastused = UNKNOWN;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				if (ak->nc)
					entry["Mask"] = ak->nc->display;
				else
					entry["Mask"] = ak->mask;
				entry["Creator"] = ak->creator;
				entry["Created"] = timebuf;
				entry["Last used"] = lastused;
				entry["Reason"] = ak->reason;
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Autokick list for %s:"), ci->name.c_str());

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("End of autokick list"));
		}
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAkickCount())
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
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
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
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
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
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

		source.Reply(_("AKICK ENFORCE for \002%s\002 complete; \002%d\002 users were affected."), ci->name.c_str(), count);
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the akick list";

		ci->ClearAkick();
		source.Reply(_("Channel %s akick list has been cleared."), ci->name.c_str());
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
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string mask = params.size() > 2 ? params[2] : "";

		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
			this->OnSyntaxError(source, cmd);
		else if (!source.AccessFor(ci).HasPriv("AKICK") && !source.HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && Anope::ReadOnly)
			source.Reply(_("Sorry, channel autokick list modification is temporarily disabled."));
		else if (cmd.equals_ci("ADD"))
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

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		BotInfo *bi = Config->GetClient("NickServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002AutoKick list\002 for a channel.  If a user\n"
				"on the AutoKick list attempts to join the channel,\n"
				"%s will ban that user from the channel, then kick\n"
				"the user.\n"
				" \n"
				"The \002AKICK ADD\002 command adds the given nick or usermask\n"
				"to the AutoKick list.  If a \037reason\037 is given with\n"
				"the command, that reason will be used when the user is\n"
				"kicked; if not, the default reason is \"User has been\n"
				"banned from the channel\".\n"
				"When akicking a \037registered nick\037 the %s account\n"
				"will be added to the akick list instead of the mask.\n"
				"All users within that nickgroup will then be akicked.\n"),
				source.service->nick.c_str(), bi ? bi->nick.c_str() : "NickServ");
		source.Reply(_(
				" \n"
				"The \002AKICK DEL\002 command removes the given nick or mask\n"
				"from the AutoKick list.  It does not, however, remove any\n"
				"bans placed by an AutoKick; those must be removed\n"
				"manually.\n"
				" \n"
				"The \002AKICK LIST\002 command displays the AutoKick list, or\n"
				"optionally only those AutoKick entries which match the\n"
				"given mask.\n"
				" \n"
				"The \002AKICK VIEW\002 command is a more verbose version of the\n"
				"\002AKICK LIST\002 command.\n"
				" \n"
				"The \002AKICK ENFORCE\002 command causes %s to enforce the\n"
				"current AKICK list by removing those users who match an\n"
				"AKICK mask.\n"
				" \n"
				"The \002AKICK CLEAR\002 command clears all entries of the\n"
				"akick list."), source.service->nick.c_str());
		return true;
	}
};

class CSAKick : public Module
	, public AutoKickService
	, public EventHook<Event::CheckKick>
{
	CommandCSAKick commandcsakick;
	EventHandlers<Event::Akick> akickevents;
	Serialize::Type akick_type;

 public:
	CSAKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, AutoKickService(this)
		, EventHook<Event::CheckKick>("OnCheckKick")
		, commandcsakick(this, akickevents)
		, akickevents(this, "Akick")
		, akick_type("AutoKick", AutoKickImpl::Unserialize)
	{
	}

	AutoKick* Create() override
	{
		return new AutoKickImpl();
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (!c->ci || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;

		for (unsigned j = 0, end = c->ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *autokick = c->ci->GetAkick(j);
			bool kick = false;

			if (autokick->nc)
				kick = autokick->nc == u->Account();
			else if (IRCD->IsChannelValid(autokick->mask))
			{
				Channel *chan = Channel::Find(autokick->mask);
				kick = chan != NULL && chan->FindUser(u);
			}
			else
				kick = Entry("BAN", autokick->mask).Matches(u);

			if (kick)
			{
				Log(LOG_DEBUG_2) << u->nick << " matched akick " << (autokick->nc ? autokick->nc->display : autokick->mask);
				autokick->last_used = Anope::CurTime;
				if (!autokick->nc && autokick->mask.find('#') == Anope::string::npos)
					mask = autokick->mask;
				reason = autokick->reason;
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
