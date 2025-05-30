/* OperServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/operserv/ignore.h"

struct IgnoreDataImpl final
	: IgnoreData
	, Serializable
{
	IgnoreDataImpl() : Serializable("IgnoreData") { }
	~IgnoreDataImpl() override;
};

IgnoreDataImpl::~IgnoreDataImpl()
{
	if (ignore_service)
		ignore_service->DelIgnore(this);
}

struct IgnoreDataTypeImpl final
	: public Serialize::Type
{
	IgnoreDataTypeImpl()
		: Serialize::Type("IgnoreData")
	{
	}
	void Serialize(Serializable *obj, Serialize::Data &data) const override;
	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
};

void IgnoreDataTypeImpl::Serialize(Serializable *obj, Serialize::Data &data) const
{
	const auto *ign = static_cast<const IgnoreDataImpl *>(obj);
	data.Store("mask", ign->mask);
	data.Store("creator", ign->creator);
	data.Store("reason", ign->reason);
	data.Store("time", ign->time);
}

Serializable *IgnoreDataTypeImpl::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	if (!ignore_service)
		return NULL;

	IgnoreDataImpl *ign;
	if (obj)
		ign = anope_dynamic_static_cast<IgnoreDataImpl *>(obj);
	else
	{
		ign = new IgnoreDataImpl();
		ignore_service->AddIgnore(ign);
	}

	data["mask"] >> ign->mask;
	data["creator"] >> ign->creator;
	data["reason"] >> ign->reason;
	data["time"] >> ign->time;

	return ign;
}


class OSIgnoreService final
	: public IgnoreService
{
	Serialize::Checker<std::vector<IgnoreData *> > ignores;

public:
	OSIgnoreService(Module *o) : IgnoreService(o), ignores("IgnoreData") { }

	void AddIgnore(IgnoreData *ign) override
	{
		ignores->push_back(ign);
	}

	void DelIgnore(IgnoreData *ign) override
	{
		std::vector<IgnoreData *>::iterator it = std::find(ignores->begin(), ignores->end(), ign);
		if (it != ignores->end())
			ignores->erase(it);
	}

	void ClearIgnores() override
	{
		for (unsigned i = ignores->size(); i > 0; --i)
		{
			IgnoreData *ign = ignores->at(i - 1);
			delete ign;
		}
	}

	IgnoreData *Create() override
	{
		return new IgnoreDataImpl();
	}

	IgnoreData *Find(const Anope::string &mask) override
	{
		User *u = User::Find(mask, true);
		std::vector<IgnoreData *>::iterator ign = this->ignores->begin(), ign_end = this->ignores->end();

		if (u)
		{
			for (; ign != ign_end; ++ign)
			{
				Entry ignore_mask("", (*ign)->mask);
				if (ignore_mask.Matches(u, true))
					break;
			}
		}
		else
		{
			size_t user, host;
			Anope::string tmp;
			/* We didn't get a user.. generate a valid mask. */
			if ((host = mask.find('@')) != Anope::string::npos)
			{
				if ((user = mask.find('!')) != Anope::string::npos)
				{
					/* this should never happen */
					if (user > host)
						return NULL;
					tmp = mask;
				}
				else
					/* We have user@host. Add nick wildcard. */
				tmp = "*!" + mask;
			}
			/* We only got a nick.. */
			else
				tmp = mask + "!*@*";

			for (; ign != ign_end; ++ign)
				if (Anope::Match(tmp, (*ign)->mask, false, true))
					break;
		}

		/* Check whether the entry has timed out */
		if (ign != ign_end)
		{
			IgnoreData *id = *ign;

			if (id->time && !Anope::NoExpire && id->time <= Anope::CurTime)
			{
				Log(LOG_NORMAL, "expire/ignore", Config->GetClient("OperServ")) << "Expiring ignore entry " << id->mask;
				delete id;
			}
			else
				return id;
		}

		return NULL;
	}

	std::vector<IgnoreData *> &GetIgnores() override
	{
		return *ignores;
	}
};

class CommandOSIgnore final
	: public Command
{
private:
	static Anope::string RealMask(const Anope::string &mask)
	{
		/* If it s an existing user, we ignore the hostmask. */
		User *u = User::Find(mask, true);
		if (u)
			return "*!*@" + u->host;

		size_t host = mask.find('@');
		/* Determine whether we get a nick or a mask. */
		if (host != Anope::string::npos)
		{
			size_t user = mask.find('!');
			/* Check whether we have a nick too.. */
			if (user != Anope::string::npos)
			{
				if (user > host)
					/* this should never happen */
					return "";
				else
					return mask;
			}
			else
				/* We have user@host. Add nick wildcard. */
				return "*!" + mask;
		}

		/* We only got a nick.. */
		return mask + "!*@*";
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!ignore_service)
			return;

		const Anope::string &time = params.size() > 1 ? params[1] : "";
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &reason = params.size() > 3 ? params[3] : "";

		if (time.empty() || nick.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}
		else
		{
			time_t t = Anope::DoTime(time);

			if (t <= -1)
			{
				source.Reply(BAD_EXPIRY_TIME);
				return;
			}

			Anope::string mask = RealMask(nick);
			if (mask.empty())
			{
				source.Reply(BAD_USERHOST_MASK);
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			auto *ign = new IgnoreDataImpl();
			ign->mask = mask;
			ign->creator = source.GetNick();
			ign->reason = reason;
			ign->time = t ? Anope::CurTime + t : 0;

			ignore_service->AddIgnore(ign);
			if (!t)
			{
				source.Reply(_("\002%s\002 will now permanently be ignored."), mask.c_str());
				Log(LOG_ADMIN, source, this) << "to add a permanent ignore for " << mask;
			}
			else
			{
				source.Reply(_("\002%s\002 will now be ignored for \002%s\002."), mask.c_str(), Anope::Duration(t, source.GetAccount()).c_str());
				Log(LOG_ADMIN, source, this) << "to add an ignore on " << mask << " for " << Anope::Duration(t);
			}
		}
	}

	static void DoList(CommandSource &source)
	{
		if (!ignore_service)
			return;

		std::vector<IgnoreData *> &ignores = ignore_service->GetIgnores();
		for (unsigned i = ignores.size(); i > 0; --i)
		{
			IgnoreData *id = ignores[i - 1];

			if (id->time && !Anope::NoExpire && id->time <= Anope::CurTime)
			{
				Log(LOG_NORMAL, "expire/ignore", Config->GetClient("OperServ")) << "Expiring ignore entry " << id->mask;
				delete id;
			}
		}

		if (ignores.empty())
			source.Reply(_("Ignore list is empty."));
		else
		{
			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Mask")).AddColumn(_("Creator")).AddColumn(_("Reason")).AddColumn(_("Expires"));

			for (unsigned i = ignores.size(); i > 0; --i)
			{
				const IgnoreData *ignore = ignores[i - 1];

				ListFormatter::ListEntry entry;
				entry["Mask"] = ignore->mask;
				entry["Creator"] = ignore->creator;
				entry["Reason"] = ignore->reason;
				entry["Expires"] = Anope::Expires(ignore->time, source.GetAccount());
				list.AddEntry(entry);
			}

			source.Reply(_("Services ignore list:"));

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (const auto &reply : replies)
				source.Reply(reply);
		}
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!ignore_service)
			return;

		const Anope::string nick = params.size() > 1 ? params[1] : "";
		if (nick.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		Anope::string mask = RealMask(nick);
		if (mask.empty())
		{
			source.Reply(BAD_USERHOST_MASK);
			return;
		}

		IgnoreData *ign = ignore_service->Find(mask);
		if (ign)
		{
			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			Log(LOG_ADMIN, source, this) << "to remove an ignore on " << mask;
			source.Reply(_("\002%s\002 will no longer be ignored."), mask.c_str());
			delete ign;
		}
		else
			source.Reply(_("\002%s\002 not found on ignore list."), mask.c_str());
	}

	void DoClear(CommandSource &source)
	{
		if (!ignore_service)
			return;

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		ignore_service->ClearIgnores();
		Log(LOG_ADMIN, source, this) << "to CLEAR the list";
		source.Reply(_("Ignore list has been cleared."));

		return;
	}

public:
	CommandOSIgnore(Module *creator) : Command(creator, "operserv/ignore", 1, 4)
	{
		this->SetDesc(_("Modify the services ignore list"));
		this->SetSyntax(_("ADD \037expiry\037 {\037nick\037|\037mask\037} [\037reason\037]"));
		this->SetSyntax(_("DEL {\037nick\037|\037mask\037}"));
		this->SetSyntax("LIST");
		this->SetSyntax("CLEAR");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Allows Services Operators to make services ignore a nick or mask "
			"for a certain time or until the next restart. The default "
			"time format is seconds. You can specify it by using units. "
			"Valid units are: \037s\037 for seconds, \037m\037 for minutes, "
			"\037h\037 for hours and \037d\037 for days. "
			"Combinations of these units are not permitted. "
			"To make services permanently ignore the user, type 0 as time. "
			"When adding a \037mask\037, it should be in the format nick!user@host, "
			"everything else will be considered a nick. Wildcards are permitted."
			"\n\n"
			"Ignores will not be enforced on IRC Operators."
		));

		const Anope::string &regexengine = Config->GetBlock("options").Get<const Anope::string>("regexengine");
		if (!regexengine.empty())
		{
			source.Reply(" ");
			source.Reply(_(
					"Regex matches are also supported using the %s engine. "
					"Enclose your pattern in // if this is desired."
				),
				regexengine.c_str());
		}

		return true;
	}
};

class OSIgnore final
	: public Module
{
	IgnoreDataTypeImpl ignoredata_type;
	OSIgnoreService osignoreservice;
	CommandOSIgnore commandosignore;

public:
	OSIgnore(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, osignoreservice(this)
		, commandosignore(this)
	{
	}

	void Prioritize() override
	{
		ModuleManager::SetPriority(this, I_OnBotPrivmsg, PRIORITY_FIRST);
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message, const Anope::map<Anope::string> &tags) override
	{
		if (!u->HasMode("OPER") && this->osignoreservice.Find(u->nick))
			return EVENT_STOP;

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSIgnore)
