/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
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
	std::vector<ForbidData *> forbidData;

 public:
	MyForbidService(Module *m) : ForbidService(m) { }

	void AddForbid(ForbidData *d)
	{
		this->forbidData.push_back(d);
	}

	void RemoveForbid(ForbidData *d)
	{
		std::vector<ForbidData *>::iterator it = std::find(this->forbidData.begin(), this->forbidData.end(), d);
		if (it != this->forbidData.end())
			this->forbidData.erase(it);
		delete d;
	}

	ForbidData *FindForbid(const Anope::string &mask, ForbidType type)
	{
		const std::vector<ForbidData *> &forbids = this->GetForbids();
		for (unsigned i = forbids.size(); i > 0; --i)
		{
			ForbidData *d = this->forbidData[i - 1];

			if ((type == FT_NONE || type == d->type) && Anope::Match(mask, d->mask))
				return d;
		}
		return NULL;
	}

	const std::vector<ForbidData *> &GetForbids()
	{
		for (unsigned i = this->forbidData.size(); i > 0; --i)
		{
			ForbidData *d = this->forbidData[i - 1];

			if (d->expires && Anope::CurTime >= d->expires)
			{
				Anope::string type = "none";
				if (d->type == FT_NICK)
					type = "nick";
				else if (d->type == FT_CHAN)
					type = "chan";
				else if (d->type == FT_EMAIL)
					type = "email";

				Log(LOG_NORMAL, Config->OperServ + "/forbid") << "Expiring forbid for " << d->mask << " type " << type;
				this->forbidData.erase(this->forbidData.begin() + i - 1);
				delete d;
			}
		}

		return this->forbidData;
	}
};

class CommandOSForbid : public Command
{
	service_reference<ForbidService, Base> fs;
 public:
	CommandOSForbid(Module *creator) : Command(creator, "operserv/forbid", 1, 5), fs("forbid")
	{
		this->SetDesc(_("Forbid usage of nicknames, channels, and emails"));
		this->SetSyntax(_("ADD {NICK|CHAN|EMAIL} [+\037expiry\037] \037entry\037\002 [\037reason\037]"));
		this->SetSyntax(_("DEL {NICK|CHAN|EMAIL} \037entry\037"));
		this->SetSyntax(_("LIST (NICK|CHAN|EMAIL)"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!this->fs)
			return;

		const Anope::string &command = params[0];
		const Anope::string &subcommand = params.size() > 1 ? params[1] : "";

		ForbidType type = FT_NONE;
		if (subcommand.equals_ci("NICK"))
			type = FT_NICK;
		else if (subcommand.equals_ci("CHAN"))
			type = FT_CHAN;
		else if (subcommand.equals_ci("EMAIL"))
			type = FT_EMAIL;

		if (command.equals_ci("ADD") && params.size() > 2 && type != FT_NONE)
		{
			const Anope::string &expiry = params[2][0] == '+' ? params[2] : "";
			const Anope::string &entry = params.size() > 3 && !expiry.empty() ? params[3] : params[2];
			Anope::string reason = !expiry.empty() && params.size() > 4 ? params[4] : (params.size() > 3 ? params[3] : "");

			time_t expiryt = 0;

			if (Config->ForceForbidReason && reason.empty())
			{
				this->OnSyntaxError(source, "");
				return;
			}

			if (!expiry.empty())
			{
				expiryt = dotime(expiry);
				if (expiryt)
					expiryt += Anope::CurTime;
			}

			ForbidData *d = this->fs->FindForbid(entry, type);
			bool created = false;
			if (d == NULL)
			{
				d = new ForbidData();
				created = true;
			}

			d->mask = entry;
			d->creator = source.u->nick;
			d->reason = reason;
			d->created = Anope::CurTime;
			d->expires = expiryt;
			d->type = type;
			if (created)
				this->fs->AddForbid(d);

			Log(LOG_ADMIN, source.u, this) << "to add a forbid on " << entry << " of type " << subcommand;
			source.Reply(_("Added a%s forbid on %s to expire on %s"), type == FT_CHAN ? "n" : "", entry.c_str(), d->expires ? do_strftime(d->expires).c_str() : "never");
		}
		else if (command.equals_ci("DEL") && params.size() > 2 && type != FT_NONE)
		{
			const Anope::string &entry = params[2];

			ForbidData *d = this->fs->FindForbid(entry, type);
			if (d != NULL)
			{
				Log(LOG_ADMIN, source.u, this) << "to remove forbid " << d->mask << " of type " << subcommand;
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
				source.Reply(_("Forbid list:"));
				source.Reply(_("Mask       Type  Reason"));

				for (unsigned i = 0; i < forbids.size(); ++i)
				{
					ForbidData *d = forbids[i];

					Anope::string ftype;
					if (d->type == FT_NICK)
						ftype = "NICK";
					else if (d->type == FT_CHAN)
						ftype = "CHAN";
					else if (d->type == FT_EMAIL)
						ftype = "EMAIL";
					else
						continue;

					source.Reply("%-10s %-5s %s", d->mask.c_str(), ftype.c_str(), d->reason.c_str());
					source.Reply(_("By %s, expires on %s"), d->creator.c_str(), d->expires ? do_strftime(d->expires).c_str() : "never");
				}

				source.Reply(_("End of forbid list."));
			}
		}
		else
			this->OnSyntaxError(source, command);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Forbid allows you to forbid usage of certain nicknames, channels,\n"
				"and email addresses. Wildcards are accepted for all entries."));
		return true;
	}
};

class OSForbid : public Module
{
	MyForbidService forbidService;
	CommandOSForbid commandosforbid;

 public:
	OSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		forbidService(this), commandosforbid(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnUserConnect, I_OnUserNickChange, I_OnJoinChannel, I_OnPreCommand, I_OnDatabaseWrite, I_OnDatabaseRead };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		

	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt)
	{
		if (!u || exempt)
			return;

		this->OnUserNickChange(u, "");
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		if (u->HasMode(UMODE_OPER))
			return;

		ForbidData *d = this->forbidService.FindForbid(u->nick, FT_NICK);
		if (d != NULL)
		{
			BotInfo *bi = findbot(Config->OperServ);
			if (bi)
			{
				if (d->reason.empty())
					u->SendMessage(bi, _("This nickname has been forbidden."));
				else
					u->SendMessage(bi, _("This nickname has been forbidden: %s"), d->reason.c_str());
			}
			u->Collide(NULL);
		}
	}

	void OnJoinChannel(User *u, Channel *c)
	{
		if (u->HasMode(UMODE_OPER))
			return;

		BotInfo *bi = findbot(Config->OperServ);
		ForbidData *d = this->forbidService.FindForbid(c->name, FT_CHAN);
		if (bi != NULL && d != NULL)
		{			
			if (!c->HasFlag(CH_INHABIT))
			{
				/* Join ChanServ and set a timer for this channel to part ChanServ later */
				new ChanServTimer(c);

				/* Set +si to prevent rejoin */
				c->SetMode(NULL, CMODE_NOEXTERNAL);
				c->SetMode(NULL, CMODE_TOPIC);
				c->SetMode(NULL, CMODE_SECRET);
				c->SetMode(NULL, CMODE_INVITE);
			}

			if (d->reason.empty())
				c->Kick(bi, u, _("This channel has been forbidden."));
			else
				c->Kick(bi, u, _("This channel has been forbidden: %s"), d->reason.c_str());
		}
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params)
	{
		if (source.u->HasMode(UMODE_OPER))
			return EVENT_CONTINUE;
		else if (command->name == "nickserv/register" && params.size() > 1)
		{
			ForbidData *d = this->forbidService.FindForbid(params[1], FT_EMAIL);
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

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const Anope::string &))
	{
		std::vector<ForbidData *> forbids = this->forbidService.GetForbids();
		for (unsigned i = 0; i < forbids.size(); ++i)
		{
			ForbidData *f = forbids[i];
			Anope::string ftype;
			if (f->type == FT_NICK)
				ftype = "NICK";
			else if (f->type == FT_CHAN)
				ftype = "CHAN";
			else if (f->type == FT_EMAIL)
				ftype = "EMAIL";
			Write("FORBID " + f->mask + " " + f->creator + " " + stringify(f->created) + " " + stringify(f->expires) + " " + ftype + " " + f->reason);
		}
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		if (params.size() > 5 && params[0] == "FORBID")
		{
			ForbidData *f = new ForbidData();
			f->mask = params[1];
			f->creator = params[2];
			f->created = convertTo<time_t>(params[3]);
			f->expires = convertTo<time_t>(params[4]);
			if (params[5] == "NICK")	
				f->type = FT_NICK;
			else if (params[5] == "CHAN")
				f->type = FT_CHAN;
			else if (params[5] == "EMAIL")
				f->type = FT_EMAIL;
			else
				f->type = FT_NONE;
			f->reason = params.size() > 6 ? params[6] : "";
			
			this->forbidService.AddForbid(f);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSForbid)
