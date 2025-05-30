/* NickServ core functions
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
#include "modules/nickserv/cert.h"

static ServiceReference<NickServService> nickserv("NickServService", "NickServ");

class NSGroupRequest final
	: public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
	Anope::string nick;
	Reference<NickAlias> target;

public:
	NSGroupRequest(Module *o, CommandSource &src, Command *c, const Anope::string &n, NickAlias *targ, const Anope::string &pass)
		: IdentifyRequest(o, targ->nc->display, pass, src.ip)
		, source(src)
		, cmd(c)
		, nick(n)
		, target(targ)
	{
	}

	void OnSuccess() override
	{
		User *u = source.GetUser();

		/* user changed nick? */
		if (u != NULL && u->nick != nick)
			return;

		if (!target || !target->nc)
			return;

		NickAlias *na = NickAlias::Find(nick);
		/* If the nick is already registered, drop it. */
		if (na)
		{
			delete na;
		}

		na = new NickAlias(nick, target->nc);
		na->registered = na->last_seen = Anope::CurTime;

		if (u != NULL)
		{
			na->last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_realname = u->realname;
		}
		else
		{
			na->last_realname = source.GetNick();
		}

		if (u != NULL)
		{
			IRCD->SendLogin(u, na); // protocol modules prevent this on unconfirmed accounts
			u->Login(target->nc);
			FOREACH_MOD(OnNickGroup, (u, target));
		}

		Log(LOG_COMMAND, source, cmd) << "to make " << nick << " join group of " << target->nick << " (" << target->nc->display << ") (email: " << (!target->nc->email.empty() ? target->nc->email : "none") << ")";
		source.Reply(_("You are now in the group of \002%s\002."), target->nick.c_str());

		if (u)
			u->lastnickreg = Anope::CurTime;
	}

	void OnFail() override
	{
		User *u = source.GetUser();

		Log(LOG_COMMAND, source, cmd) << "and failed to group to " << target->nick;
		if (NickAlias::Find(GetAccount()) != NULL)
		{
			source.Reply(PASSWORD_INCORRECT);
			if (u)
				u->BadPassword();
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
	}
};

class CommandNSGroup final
	: public Command
{
public:
	CommandNSGroup(Module *creator) : Command(creator, "nickserv/group", 0, 2)
	{
		this->SetDesc(_("Join a group"));
		this->SetSyntax(_("\037[target]\037 \037[password]\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *user = source.GetUser();

		Anope::string nick;
		if (params.empty())
		{
			NickCore *core = source.GetAccount();
			if (core)
				nick = core->display;
		}
		else
			nick = params[0];

		if (nick.empty())
		{
			this->SendSyntax(source);
			return;
		}

		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (!IRCD->IsNickValid(source.GetNick()))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, source.GetNick().c_str());
			return;
		}

		if (Config->GetModule("nickserv").Get<bool>("restrictopernicks"))
		{
			for (auto *o : Oper::opers)
			{
				if (user != NULL && !user->HasMode("OPER") && user->nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, user->nick.c_str());
					return;
				}
			}
		}

		NickAlias *target, *na = NickAlias::Find(source.GetNick());
		time_t reg_delay = Config->GetModule("nickserv").Get<time_t>("regdelay");
		unsigned maxaliases = Config->GetModule(this->owner).Get<unsigned>("maxaliases");
		if (!(target = NickAlias::Find(nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (user && Anope::CurTime < user->lastnickreg + reg_delay)
		{
			auto waitperiod = (unsigned long)(reg_delay + user->lastnickreg) - Anope::CurTime;
			source.Reply(_("Please wait %s before using the GROUP command again."), Anope::Duration(waitperiod, source.GetAccount()).c_str());
		}
		else if (target->nc->HasExt("NS_SUSPENDED"))
		{
			Log(LOG_COMMAND, source, this) << "and tried to group to SUSPENDED nick " << target->nick;
			source.Reply(NICK_X_SUSPENDED, target->nick.c_str());
		}
		else if (na && Config->GetModule(this->owner).Get<bool>("nogroupchange"))
			source.Reply(_("Your nick is already registered."));
		else if (na && *target->nc == *na->nc)
			source.Reply(_("You are already a member of the group of \002%s\002."), target->nick.c_str());
		else if (na && na->nc != source.GetAccount())
			source.Reply(NICK_IDENTIFY_REQUIRED);
		else if (maxaliases && target->nc->aliases->size() >= maxaliases && !target->nc->IsServicesOper())
			source.Reply(_("There are too many nicks in your group."));
		else if (nickserv && nickserv->IsGuestNick(source.GetNick()))
			source.Reply(NICK_CANNOT_BE_REGISTERED, source.GetNick().c_str());
		else
		{
			bool ok = false;
			if (!na && source.GetAccount() == target->nc)
				ok = true;

			NSCertList *cl = target->nc->GetExt<NSCertList>("certificates");
			if (user != NULL && !user->fingerprint.empty() && cl && cl->FindCert(user->fingerprint))
				ok = true;

			if (!ok && !pass.empty())
			{
				auto *req = new NSGroupRequest(owner, source, this, source.GetNick(), target, pass);
				FOREACH_MOD(OnCheckAuthentication, (source.GetUser(), req));
				req->Dispatch();
			}
			else
			{
				NSGroupRequest req(owner, source, this, source.GetNick(), target, pass);

				if (ok)
					req.OnSuccess();
				else
					req.OnFail();
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command makes your nickname join the \037target\037 nickname's "
			"group. \037password\037 is the password of the target nickname. "
			"\n\n"
			"Joining a group will allow you to share your configuration, "
			"memos, and channel privileges with all the nicknames in the "
			"group, and much more!"
			"\n\n"
			"A group exists as long as it is useful. This means that even "
			"if a nick of the group is dropped, you won't lose the "
			"shared things described above, as long as there is at "
			"least one nick remaining in the group."
			"\n\n"
			"You may be able to use this command even if you have not registered "
			"your nick yet. If your nick is already registered, you'll "
			"need to identify yourself before using this command. "
			"\n\n"
			"It is recommended to use this command with a non-registered "
			"nick because it will be registered automatically when "
			"using this command. You may use it with a registered nick (to "
			"change your group) only if your network administrators allowed "
			"it."
			"\n\n"
			"You can only be in one group at a time. Group merging is "
			"not possible. "
			"\n\n"
			"\037Note\037: all the nicknames of a group have the same password."
		));
		return true;
	}
};

class CommandNSUngroup final
	: public Command
{
public:
	CommandNSUngroup(Module *creator) : Command(creator, "nickserv/ungroup", 0, 1)
	{
		this->SetDesc(_("Remove a nick from a group"));
		this->SetSyntax(_("[\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string nick = !params.empty() ? params[0] : "";
		NickAlias *na = NickAlias::Find(!nick.empty() ? nick : source.GetNick());

		if (source.GetAccount()->aliases->size() == 1)
			source.Reply(_("Your nick is not grouped to anything, you can't ungroup it."));
		else if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, !nick.empty() ? nick.c_str() : source.GetNick().c_str());
		else if (na->nc != source.GetAccount())
			source.Reply(_("Nick %s is not in your group."), na->nick.c_str());
		else
		{
			NickCore *oldcore = na->nc;

			std::vector<NickAlias *>::iterator it = std::find(oldcore->aliases->begin(), oldcore->aliases->end(), na);
			if (it != oldcore->aliases->end())
				oldcore->aliases->erase(it);

			if (na->nick.equals_ci(oldcore->display))
				oldcore->SetDisplay(oldcore->aliases->front());

			auto *nc = new NickCore(na->nick);
			na->nc = nc;
			nc->aliases->push_back(na);

			nc->pass = oldcore->pass;
			if (!oldcore->email.empty())
				nc->email = oldcore->email;
			nc->language = oldcore->language;

			Log(LOG_COMMAND, source, this) << "to make " << na->nick << " leave group of " << oldcore->display << " (email: " << (!oldcore->email.empty() ? oldcore->email : "none") << ")";
			source.Reply(_("Nick %s has been ungrouped from %s."), na->nick.c_str(), oldcore->display.c_str());

			User *user = User::Find(na->nick, true);
			if (user)
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(source.service, "REGISTERED");
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command ungroups your nick, or if given, the specified nick, "
			"from the group it is in. The ungrouped nick keeps its registration "
			"time, password, email, greet, language, and url. Everything else "
			"is reset. You may not ungroup yourself if there is only one nick in "
			"your group."
		));
		return true;
	}
};

class CommandNSGList final
	: public Command
{
public:
	CommandNSGList(Module *creator) : Command(creator, "nickserv/glist", 0, 1)
	{
		this->SetDesc(_("Lists all nicknames in your group"));
		this->SetSyntax(_("[\037nickname\037]"), [](auto &source) { return source.IsServicesOper(); });
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = !params.empty() ? params[0] : "";
		const NickCore *nc;

		if (!nick.empty())
		{
			const NickAlias *na = NickAlias::Find(nick);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return;
			}
			else if (na->nc != source.GetAccount() && !source.IsServicesOper())
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			nc = na->nc;
		}
		else
			nc = source.GetAccount();

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Nick")).AddColumn(_("Expires"));
		time_t nickserv_expire = Config->GetModule("nickserv").Get<time_t>("expire", "90d"),
		       unconfirmed_expire = Config->GetModule("ns_register").Get<time_t>("unconfirmedexpire", "1d");
		for (auto *na2 : *nc->aliases)
		{
			Anope::string expires;
			if (na2->HasExt("NS_NO_EXPIRE"))
				expires = NO_EXPIRE;
			else if (!nickserv_expire || Anope::NoExpire)
				;
			else if (na2->nc->HasExt("UNCONFIRMED") && unconfirmed_expire)
				expires = Anope::strftime(na2->registered + unconfirmed_expire, source.GetAccount());
			else
				expires = Anope::strftime(na2->last_seen + nickserv_expire, source.GetAccount());

			ListFormatter::ListEntry entry;
			entry["Nick"] = na2->nick;
			entry["Expires"] = expires;
			list.AddEntry(entry);
		}

		source.Reply(!nick.empty() ? _("List of nicknames in the group of \002%s\002:") : _("List of nicknames in your group:"), nc->display.c_str());
		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const auto &reply : replies)
			source.Reply(reply);

		source.Reply(nc->aliases->size(), N_("%zu nickname in the group.", "%zu nicknames in the group."), nc->aliases->size());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		if (source.IsServicesOper())
		{
			source.Reply(_(
				"Without a parameter, lists all nicknames that are in "
				"your group."
				"\n\n"
				"With a parameter, lists all nicknames that are in the "
				"group of the given nick."
				"\n\n"
				"Specifying a nick is limited to \002Services Operators\002."
			));
		}
		else
		{
			source.Reply(_("Lists all nicks in your group."));
		}

		return true;
	}
};

class NSGroup final
	: public Module
{
	CommandNSGroup commandnsgroup;
	CommandNSUngroup commandnsungroup;
	CommandNSGList commandnsglist;

public:
	NSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsgroup(this), commandnsungroup(this), commandnsglist(this)
	{
		if (Config->GetModule("nickserv").Get<bool>("nonicknameownership"))
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSGroup)
