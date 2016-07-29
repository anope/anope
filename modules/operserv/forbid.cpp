/* OperServ core functions
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
#include "modules/operserv/forbid.h"
#include "modules/nickserv.h"
#include "modules/chanserv.h"

class ForbidDataImpl : public ForbidData
{
	friend class ForbidDataType;

	Anope::string mask, creator, reason;
	time_t created = 0, expires = 0;
	ForbidType type = static_cast<ForbidType>(0);

 public:
	ForbidDataImpl(Serialize::TypeBase *type) : ForbidData(type) { }
	ForbidDataImpl(Serialize::TypeBase *type, Serialize::ID id) : ForbidData(type, id) { }

	Anope::string GetMask() override;
	void SetMask(const Anope::string &) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &) override;

	time_t GetExpires() override;
	void SetExpires(const time_t &) override;

	ForbidType GetType() override;
	void SetType(const ForbidType &) override;
};

class ForbidDataType : public Serialize::Type<ForbidDataImpl>
{
 public:
	Serialize::Field<ForbidDataImpl, Anope::string> mask, creator, reason;
	Serialize::Field<ForbidDataImpl, time_t> created, expires;
	Serialize::Field<ForbidDataImpl, ForbidType> type;

	ForbidDataType(Module *me) : Serialize::Type<ForbidDataImpl>(me)
		, mask(this, "mask", &ForbidDataImpl::mask)
		, creator(this, "creator", &ForbidDataImpl::creator)
		, reason(this, "reason", &ForbidDataImpl::reason)
		, created(this, "created", &ForbidDataImpl::created)
		, expires(this, "expires", &ForbidDataImpl::expires)
		, type(this, "type", &ForbidDataImpl::type)
	{
	}
};

Anope::string ForbidDataImpl::GetMask()
{
	return Get(&ForbidDataType::mask);
}

void ForbidDataImpl::SetMask(const Anope::string &m)
{
	Set(&ForbidDataType::mask, m);
}

Anope::string ForbidDataImpl::GetCreator()
{
	return Get(&ForbidDataType::creator);
}

void ForbidDataImpl::SetCreator(const Anope::string &c)
{
	Set(&ForbidDataType::creator, c);
}

Anope::string ForbidDataImpl::GetReason()
{
	return Get(&ForbidDataType::reason);
}

void ForbidDataImpl::SetReason(const Anope::string &r)
{
	Set(&ForbidDataType::reason, r);
}

time_t ForbidDataImpl::GetCreated()
{
	return Get(&ForbidDataType::created);
}

void ForbidDataImpl::SetCreated(const time_t &t)
{
	Set(&ForbidDataType::created, t);
}

time_t ForbidDataImpl::GetExpires()
{
	return Get(&ForbidDataType::expires);
}

void ForbidDataImpl::SetExpires(const time_t &t)
{
	Set(&ForbidDataType::expires, t);
}

ForbidType ForbidDataImpl::GetType()
{
	return Get(&ForbidDataType::type);
}

void ForbidDataImpl::SetType(const ForbidType &t)
{
	Set(&ForbidDataType::type, t);
}

class MyForbidService : public ForbidService
{
 public:
	MyForbidService(Module *m) : ForbidService(m)
	{
	}

	ForbidData *FindForbid(const Anope::string &mask, ForbidType ftype) override
	{
		for (ForbidData *d : GetForbids())
			if (d->GetType() == ftype && Anope::Match(mask, d->GetMask(), false, true))
				return d;
		return NULL;
	}

	std::vector<ForbidData *> GetForbids() override
	{
		for (ForbidData *d : Serialize::GetObjects<ForbidData *>())
			if (d->GetExpires() && !Anope::NoExpire && Anope::CurTime >= d->GetExpires())
			{
				Anope::string ftype = "none";
				if (d->GetType() == FT_NICK)
					ftype = "nick";
				else if (d->GetType() == FT_CHAN)
					ftype = "chan";
				else if (d->GetType() == FT_EMAIL)
					ftype = "email";

				Log(LOG_NORMAL, "expire/forbid", Config->GetClient("OperServ")) << "Expiring forbid for " << d->GetMask() << " type " << ftype;
				d->Delete();
			}
		return Serialize::GetObjects<ForbidData *>();
	}
};

class CommandOSForbid : public Command
{
	ServiceReference<ForbidService> fs;
	
 public:
	CommandOSForbid(Module *creator) : Command(creator, "operserv/forbid", 1, 5)
	{
		this->SetDesc(_("Forbid usage of nicknames, channels, and emails"));
		this->SetSyntax(_("ADD {NICK|CHAN|EMAIL|REGISTER} [+\037expiry\037] \037entry\037 \037reason\037"));
		this->SetSyntax(_("DEL {NICK|CHAN|EMAIL|REGISTER} \037entry\037"));
		this->SetSyntax("LIST [NICK|CHAN|EMAIL|REGISTER]");
	}

	void OnUserNickChange(User *u)
	{
		if (u->HasMode("OPER"))
			return;

		ForbidData *d = fs->FindForbid(u->nick, FT_NICK);
		if (d != NULL)
		{
			ServiceBot *bi = Config->GetClient("NickServ");
			if (!bi)
				bi = Config->GetClient("OperServ");
			if (bi)
				u->SendMessage(bi, _("This nickname has been forbidden: \002{0}\002"), d->GetReason());
			if (NickServ::service)
				NickServ::service->Collide(u, NULL);
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!this->fs)
			return;

		const Anope::string &command = params[0];
		const Anope::string &subcommand = params.size() > 1 ? params[1] : "";

		ForbidType ftype = FT_SIZE;
		if (subcommand.equals_ci("NICK"))
			ftype = FT_NICK;
		else if (subcommand.equals_ci("CHAN"))
			ftype = FT_CHAN;
		else if (subcommand.equals_ci("EMAIL"))
			ftype = FT_EMAIL;
		else if (subcommand.equals_ci("REGISTER"))
			ftype = FT_REGISTER;

		if (command.equals_ci("ADD") && params.size() > 3 && ftype != FT_SIZE)
		{
			const Anope::string &expiry = params[2][0] == '+' ? params[2] : "";
			const Anope::string &entry = !expiry.empty() ? params[3] : params[2];
			Anope::string reason;
			if (expiry.empty())
				reason = params[3] + " ";
			if (params.size() > 4)
				reason += params[4];
			reason.trim();

			if (entry.replace_all_cs("?*", "").empty())
			{
				source.Reply(_("The mask must contain at least one non wildcard character."));
				return;
			}

			time_t expiryt = 0;

			if (!expiry.empty())
			{
				expiryt = Anope::DoTime(expiry);
				if (expiryt == -1)
				{
					source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
					return;
				}
				else if (expiryt)
					expiryt += Anope::CurTime;
			}

			NickServ::Nick *target = NickServ::FindNick(entry);
			if (target != NULL && Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && target->GetAccount()->IsServicesOper())
			{
				source.Reply(_("Access denied."));
				return;
			}

			ForbidData *d = this->fs->FindForbid(entry, ftype);
			bool created = false;
			if (d == NULL)
			{
				d = Serialize::New<ForbidData *>();
				created = true;
			}

			d->SetMask(entry);
			d->SetCreator(source.GetNick());
			d->SetReason(reason);
			d->SetCreated(Anope::CurTime);
			d->SetExpires(expiryt);
			d->SetType(ftype);

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			Log(LOG_ADMIN, source, this) << "to add a forbid on " << entry << " of type " << subcommand;
			source.Reply(_("Added a forbid on \002{0}\002 of type \002{1}\002 to expire on \002{2}\002."), entry, subcommand.lower(), expiryt ? Anope::strftime(expiryt, source.GetAccount()) : "never");

			/* apply forbid */
			switch (ftype)
			{
				case FT_NICK:
				{
					int na_matches = 0;

					for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
						this->OnUserNickChange(it->second);

					for (auto it = NickServ::service->GetNickList().begin(); it != NickServ::service->GetNickList().end();)
					{
						NickServ::Nick *na = *it;
						++it;

						d = this->fs->FindForbid(na->GetNick(), FT_NICK);
						if (d == NULL)
							continue;

						++na_matches;

						delete na;
					}

					source.Reply(_("\002{0}\002 nickname(s) dropped."), na_matches);
					break;
				}
				case FT_CHAN:
				{
					int chan_matches = 0, ci_matches = 0;

					for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end;)
					{
						Channel *c = it->second;
						++it;

						d = this->fs->FindForbid(c->name, FT_CHAN);
						if (d == NULL)
							continue;

						ServiceBot *OperServ = Config->GetClient("OperServ");
						if (IRCD->CanSQLineChannel && OperServ)
						{
							time_t inhabit = Config->GetModule("chanserv")->Get<time_t>("inhabit", "15s");
#warning "xline allocated on stack"
#if 0
							XLine x(c->name, OperServ->nick, Anope::CurTime + inhabit, d->GetReason());
							IRCD->SendSQLine(NULL, &x);
#endif
						}
						else if (ChanServ::service)
						{
							ChanServ::service->Hold(c);
						}

						++chan_matches;

						for (Channel::ChanUserList::const_iterator cit = c->users.begin(), cit_end = c->users.end(); cit != cit_end;)
						{
							User *u = cit->first;
							++cit;

							if (u->server == Me || u->HasMode("OPER"))
								continue;

							reason = Anope::printf(Language::Translate(u, _("This channel has been forbidden: \002%s\002")), d->GetReason().c_str());

							c->Kick(source.service, u, "%s", reason.c_str());
						}
					}

					for (auto it = ChanServ::service->GetChannels().begin(); it != ChanServ::service->GetChannels().end();)
					{
						ChanServ::Channel *ci = it->second;
						++it;

						d = this->fs->FindForbid(ci->GetName(), FT_CHAN);
						if (d == NULL)
							continue;

						++ci_matches;

						delete ci;
					}

					source.Reply(_("\002{0}\002 channel(s) cleared, and \002{1}\002 channel(s) dropped."), chan_matches, ci_matches);

					break;
				}
				default:
					break;
			}

		}
		else if (command.equals_ci("DEL") && params.size() > 2 && ftype != FT_SIZE)
		{
			const Anope::string &entry = params[2];

			ForbidData *d = this->fs->FindForbid(entry, ftype);
			if (d == nullptr)
			{
				source.Reply(_("Forbid on \002{0}\002 was not found."), entry);
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			Log(LOG_ADMIN, source, this) << "to remove forbid on " << d->GetMask() << " of type " << subcommand;
			source.Reply(_("\002{0}\002 deleted from the \002{1}\002 forbid list."), d->GetMask(), subcommand);
			d->Delete();
		}
		else if (command.equals_ci("LIST"))
		{
			const std::vector<ForbidData *> &forbids = this->fs->GetForbids();
			if (forbids.empty())
			{
				source.Reply(_("Forbid list is empty."));
				return;
			}

			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Mask")).AddColumn(_("Type")).AddColumn(_("Creator")).AddColumn(_("Expires")).AddColumn(_("Reason"));

			unsigned shown = 0;
			for (unsigned i = 0; i < forbids.size(); ++i)
			{
				ForbidData *d = forbids[i];

				if (ftype != FT_SIZE && ftype != d->GetType())
					continue;

				Anope::string stype;
				if (d->GetType() == FT_NICK)
					stype = "NICK";
				else if (d->GetType() == FT_CHAN)
					stype = "CHAN";
				else if (d->GetType() == FT_EMAIL)
					stype = "EMAIL";
				else if (d->GetType() == FT_REGISTER)
					stype = "REGISTER";
				else
					continue;

				ListFormatter::ListEntry entry;
				entry["Mask"] = d->GetMask();
				entry["Type"] = stype;
				entry["Creator"] = d->GetCreator();
				entry["Expires"] = d->GetExpires() ? Anope::strftime(d->GetExpires(), NULL, true).c_str() : Language::Translate(source.GetAccount(), _("Never"));
				entry["Reason"] = d->GetReason();
				list.AddEntry(entry);
				++shown;
			}

			if (!shown)
			{
				source.Reply(_("There are no forbids of type \002{0}\002."), subcommand.upper());
				return;
			}

			source.Reply(_("Forbid list:"));

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			if (shown >= forbids.size())
				source.Reply(_("End of forbid list."));
			else
				source.Reply(_("End of forbid list - \002{0}\002/\002{1}\002 entries shown."), shown, forbids.size());
		}
		else
			this->OnSyntaxError(source, command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Forbid allows you to forbid usage of certain nicknames, channels, and email addresses. Wildcards are accepted for all entries."));

		const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
		if (!regexengine.empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the %s engine. Enclose your pattern in // if this is desired."), regexengine);
		}

		return true;
	}
};

class OSForbid : public Module
	, public EventHook<Event::UserConnect>
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::CheckKick>
	, public EventHook<Event::PreCommand>
{
	MyForbidService forbid_service;
	ForbidDataType type;
	CommandOSForbid commandosforbid;

 public:
	OSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserConnect>(this)
		, EventHook<Event::UserNickChange>(this)
		, EventHook<Event::CheckKick>(this)
		, EventHook<Event::PreCommand>(this)
		, forbid_service(this)
		, type(this)
		, commandosforbid(this)
	{
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (u->Quitting() || exempt)
			return;

		commandosforbid.OnUserNickChange(u);
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		commandosforbid.OnUserNickChange(u);
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		ServiceBot *OperServ = Config->GetClient("OperServ");
		if (u->HasMode("OPER") || !OperServ)
			return EVENT_CONTINUE;

		ForbidData *d = this->forbid_service.FindForbid(c->name, FT_CHAN);
		if (d != NULL)
		{
			if (IRCD->CanSQLineChannel)
			{
				time_t inhabit = Config->GetModule("chanserv")->Get<time_t>("inhabit", "15s");
#warning "xline allocated on stack"
#if 0
				XLine x(c->name, OperServ->nick, Anope::CurTime + inhabit, d->GetReason());
				IRCD->SendSQLine(NULL, &x);
#endif
			}
			else if (ChanServ::service)
			{
				ChanServ::service->Hold(c);
			}

			reason = Anope::printf(Language::Translate(u, _("This channel has been forbidden: %s")), d->GetReason().c_str());

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		if (command->GetName() == "nickserv/info" && params.size() > 0)
		{
			ForbidData *d = this->forbid_service.FindForbid(params[0], FT_NICK);
			if (d != NULL)
			{
				if (source.IsOper())
					source.Reply(_("Nick \002%s\002 is forbidden by %s: %s"), params[0], d->GetCreator(), d->GetReason());
				else
					source.Reply(_("Nick \002%s\002 is forbidden."), params[0]);
				return EVENT_STOP;
			}
		}
		else if (command->GetName() == "chanserv/info" && params.size() > 0)
		{
			ForbidData *d = this->forbid_service.FindForbid(params[0], FT_CHAN);
			if (d != NULL)
			{
				if (source.IsOper())
					source.Reply(_("Channel \002%s\002 is forbidden by %s: %s"), params[0], d->GetCreator(), d->GetReason());
				else
					source.Reply(_("Channel \002%s\002 is forbidden."), params[0]);
				return EVENT_STOP;
			}
		}
		else if (source.IsOper())
			return EVENT_CONTINUE;
		else if (command->GetName() == "nickserv/register" && params.size() > 1)
		{
			ForbidData *d = this->forbid_service.FindForbid(source.GetNick(), FT_REGISTER);
			if (d != NULL)
			{
				source.Reply(_("\002{0}\002 may not be registered."), source.GetNick());
				return EVENT_STOP;
			}

			d = this->forbid_service.FindForbid(params[1], FT_EMAIL);
			if (d != NULL)
			{
				source.Reply(_("Your email address is not allowed, choose a different one."));
				return EVENT_STOP;
			}
		}
		else if (command->GetName() == "nickserv/set/email" && params.size() > 0)
		{
			ForbidData *d = this->forbid_service.FindForbid(params[0], FT_EMAIL);
			if (d != NULL)
			{
				source.Reply(_("Your email address is not allowed, choose a different one."));
				return EVENT_STOP;
			}
		}
		else if (command->GetName() == "chanserv/register" && !params.empty())
		{
			ForbidData *d = this->forbid_service.FindForbid(params[0], FT_REGISTER);
			if (d != NULL)
			{
				source.Reply(_("Channel \002{0}\002 is currently suspended."), params[0]);
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSForbid)
