/* OperServ core functions
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
#include "os_forbid.h"

class MyForbidService : public ForbidService
{
	Serialize::Checker<std::vector<ForbidData *> > forbid_data;

 public:
	MyForbidService(Module *m) : ForbidService(m), forbid_data("ForbidData") { }

	void AddForbid(ForbidData *d) anope_override
	{
		this->forbid_data->push_back(d);
	}

	void RemoveForbid(ForbidData *d) anope_override
	{
		std::vector<ForbidData *>::iterator it = std::find(this->forbid_data->begin(), this->forbid_data->end(), d);
		if (it != this->forbid_data->end())
			this->forbid_data->erase(it);
		d->Destroy();
	}

	ForbidData *FindForbid(const Anope::string &mask, ForbidType ftype) anope_override
	{
		const std::vector<ForbidData *> &forbids = this->GetForbids();
		for (unsigned i = forbids.size(); i > 0; --i)
		{
			ForbidData *d = forbids[i - 1];

			if ((ftype == FT_NONE || ftype == d->type) && Anope::Match(mask, d->mask, false, true))
				return d;
		}
		return NULL;
	}

	const std::vector<ForbidData *> &GetForbids() anope_override
	{
		for (unsigned i = this->forbid_data->size(); i > 0; --i)
		{
			ForbidData *d = this->forbid_data->at(i - 1);

			if (d->expires && Anope::CurTime >= d->expires)
			{
				Anope::string ftype = "none";
				if (d->type == FT_NICK)
					ftype = "nick";
				else if (d->type == FT_CHAN)
					ftype = "chan";
				else if (d->type == FT_EMAIL)
					ftype = "email";

				Log(LOG_NORMAL, "expire/forbid") << "Expiring forbid for " << d->mask << " type " << ftype;
				this->forbid_data->erase(this->forbid_data->begin() + i - 1);
				d->Destroy();
			}
		}

		return this->forbid_data;
	}
};

class CommandOSForbid : public Command
{
	ServiceReference<ForbidService> fs;
 public:
	CommandOSForbid(Module *creator) : Command(creator, "operserv/forbid", 1, 5), fs("ForbidService", "forbid")
	{
		this->SetDesc(_("Forbid usage of nicknames, channels, and emails"));
		this->SetSyntax(_("ADD {NICK|CHAN|EMAIL|REGISTER} [+\037expiry\037] \037entry\037\002 [\037reason\037]"));
		this->SetSyntax(_("DEL {NICK|CHAN|EMAIL|REGISTER} \037entry\037"));
		this->SetSyntax(_("LIST (NICK|CHAN|EMAIL|REGISTER)"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!this->fs)
			return;

		const Anope::string &command = params[0];
		const Anope::string &subcommand = params.size() > 1 ? params[1] : "";

		ForbidType ftype = FT_NONE;
		if (subcommand.equals_ci("NICK"))
			ftype = FT_NICK;
		else if (subcommand.equals_ci("CHAN"))
			ftype = FT_CHAN;
		else if (subcommand.equals_ci("EMAIL"))
			ftype = FT_EMAIL;
		else if (subcommand.equals_ci("REGISTER"))
			ftype = FT_REGISTER;

		if (command.equals_ci("ADD") && params.size() > 3 && ftype != FT_NONE)
		{
			const Anope::string &expiry = params[2][0] == '+' ? params[2] : "";
			const Anope::string &entry = !expiry.empty() ? params[3] : params[2];
			Anope::string reason;
			if (expiry.empty())
				reason = params[3] + " ";
			if (params.size() > 4)
				reason += params[4];
			reason.trim();

			time_t expiryt = 0;

			if (Config->ForceForbidReason && reason.empty())
			{
				this->OnSyntaxError(source, "");
				return;
			}

			if (!expiry.empty())
			{
				expiryt = Anope::DoTime(expiry);
				if (expiryt)
					expiryt += Anope::CurTime;
			}

			NickAlias *target = NickAlias::Find(entry);
			if (target != NULL && Config->NSSecureAdmins && target->nc->IsServicesOper())
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			ForbidData *d = this->fs->FindForbid(entry, ftype);
			bool created = false;
			if (d == NULL)
			{
				d = new ForbidData();
				created = true;
			}

			d->mask = entry;
			d->creator = source.GetNick();
			d->reason = reason;
			d->created = Anope::CurTime;
			d->expires = expiryt;
			d->type = ftype;
			if (created)
				this->fs->AddForbid(d);

			Log(LOG_ADMIN, source, this) << "to add a forbid on " << entry << " of type " << subcommand;
			source.Reply(_("Added a forbid on %s to expire on %s"), entry.c_str(), d->expires ? Anope::strftime(d->expires).c_str() : "never");
		}
		else if (command.equals_ci("DEL") && params.size() > 2 && ftype != FT_NONE)
		{
			const Anope::string &entry = params[2];

			ForbidData *d = this->fs->FindForbid(entry, ftype);
			if (d != NULL)
			{
				Log(LOG_ADMIN, source, this) << "to remove forbid " << d->mask << " of type " << subcommand;
				source.Reply(_("%s deleted from the %s forbid list."), d->mask.c_str(), subcommand.c_str());
				this->fs->RemoveForbid(d);
			}
			else
				source.Reply(_("Forbid on %s was not found."), entry.c_str());
		}
		else if (command.equals_ci("LIST"))
		{
			const std::vector<ForbidData *> &forbids = this->fs->GetForbids();
			if (forbids.empty())
				source.Reply(_("Forbid list is empty."));
			else
			{
				ListFormatter list;
				list.AddColumn("Mask").AddColumn("Type").AddColumn("Reason");

				for (unsigned i = 0; i < forbids.size(); ++i)
				{
					ForbidData *d = forbids[i];

					Anope::string stype;
					if (d->type == FT_NICK)
						stype = "NICK";
					else if (d->type == FT_CHAN)
						stype = "CHAN";
					else if (d->type == FT_EMAIL)
						stype = "EMAIL";
					else if (d->type == FT_REGISTER)
						stype = "REGISTER";
					else
						continue;

					ListFormatter::ListEntry entry;
					entry["Mask"] = d->mask;
					entry["Type"] = stype;
					entry["Creator"] = d->creator;
					entry["Expires"] = d->expires ? Anope::strftime(d->expires).c_str() : "never";
					entry["Reason"] = d->reason;
					list.AddEntry(entry);
				}

				source.Reply(_("Forbid list:"));

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);

				source.Reply(_("End of forbid list."));
			}
		}
		else
			this->OnSyntaxError(source, command);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Forbid allows you to forbid usage of certain nicknames, channels,\n"
				"and email addresses. Wildcards are accepted for all entries."));
		if (!Config->RegexEngine.empty())
			source.Reply(" \n"
					"Regex matches are also supported using the %s engine.\n"
					"Enclose your pattern in // if this desired.", Config->RegexEngine.c_str());
		return true;
	}
};

class OSForbid : public Module
{
	Serialize::Type forbiddata_type;
	MyForbidService forbidService;
	CommandOSForbid commandosforbid;

 public:
	OSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		forbiddata_type("ForbidData", ForbidData::Unserialize), forbidService(this), commandosforbid(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnUserConnect, I_OnUserNickChange, I_OnJoinChannel, I_OnPreCommand };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnUserConnect(User *u, bool &exempt) anope_override
	{
		if (u->Quitting() || exempt)
			return;

		this->OnUserNickChange(u, "");
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		if (u->HasMode(UMODE_OPER))
			return;

		ForbidData *d = this->forbidService.FindForbid(u->nick, FT_NICK);
		if (d != NULL)
		{
			if (OperServ)
			{
				if (d->reason.empty())
					u->SendMessage(OperServ, _("This nickname has been forbidden."));
				else
					u->SendMessage(OperServ, _("This nickname has been forbidden: %s"), d->reason.c_str());
			}
			u->Collide(NULL);
		}
	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		if (u->HasMode(UMODE_OPER) || !OperServ)
			return;

		ForbidData *d = this->forbidService.FindForbid(c->name, FT_CHAN);
		if (d != NULL)
		{
			if (IRCD->CanSQLineChannel)
			{
				XLine x(c->name, OperServ->nick, Anope::CurTime + Config->CSInhabit, d->reason);
				IRCD->SendSQLine(NULL, &x);
			}
			else if (!c->HasFlag(CH_INHABIT))
			{
				/* Join ChanServ and set a timer for this channel to part ChanServ later */
				c->Hold();

				/* Set +si to prevent rejoin */
				c->SetMode(NULL, CMODE_NOEXTERNAL);
				c->SetMode(NULL, CMODE_TOPIC);
				c->SetMode(NULL, CMODE_SECRET);
				c->SetMode(NULL, CMODE_INVITE);
			}

			if (d->reason.empty())
				c->Kick(OperServ, u, _("This channel has been forbidden."));
			else
				c->Kick(OperServ, u, _("This channel has been forbidden: %s"), d->reason.c_str());
		}
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		if (command->name == "nickserv/info" && params.size() > 0)
		{
			ForbidData *d = this->forbidService.FindForbid(params[0], FT_NICK);
			if (d != NULL)
			{
				if (source.IsOper())
					source.Reply(_("Nick \2%s\2 is forbidden by %s: %s"), params[0].c_str(), d->creator.c_str(), d->reason.c_str());
				else
					source.Reply(_("Nick \2%s\2 is forbidden."), params[0].c_str());
				return EVENT_STOP;
			}
		}
		else if (command->name == "chanserv/info" && params.size() > 0)
		{
			ForbidData *d = this->forbidService.FindForbid(params[0], FT_CHAN);
			if (d != NULL)
			{
				if (source.IsOper())
					source.Reply(_("Channel \2%s\2 is forbidden by %s: %s"), params[0].c_str(), d->creator.c_str(), d->reason.c_str());
				else
					source.Reply(_("Channel \2%s\2 is forbidden."), params[0].c_str());
				return EVENT_STOP;
			}
		}
		else if (source.IsOper())
			return EVENT_CONTINUE;
		else if (command->name == "nickserv/register" && params.size() > 1)
		{
			ForbidData *d = this->forbidService.FindForbid(source.GetNick(), FT_REGISTER);
			if (d != NULL)
			{
				source.Reply(NICK_CANNOT_BE_REGISTERED, source.GetNick().c_str());
				return EVENT_STOP;
			}

			d = this->forbidService.FindForbid(params[1], FT_EMAIL);
			if (d != NULL)
			{
				source.Reply("Your email address is not allowed, choose a different one.");
				return EVENT_STOP;
			}
		}
		else if (command->name == "nickserv/set/email" && params.size() > 0)
		{
			ForbidData *d = this->forbidService.FindForbid(params[0], FT_EMAIL);
			if (d != NULL)
			{
				source.Reply("Your email address is not allowed, choose a different one.");
				return EVENT_STOP;
			}
		}
		else if (command->name == "chanserv/register" && !params.empty())
		{
			ForbidData *d = this->forbidService.FindForbid(params[0], FT_REGISTER);
			if (d != NULL)
			{
				source.Reply(CHAN_X_INVALID, params[0].c_str());
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSForbid)
