/* NickServ core functions
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
#include "modules/ns_cert.h"
#include "modules/ns_group.h"

class NSGroupRequestListener : public NickServ::IdentifyRequestListener
{
	EventHandlers<Event::NickGroup> &onnickgroup;
	CommandSource source;
	Command *cmd;
	Anope::string nick;
	Reference<NickServ::Nick> target;

 public:
	NSGroupRequestListener(EventHandlers<Event::NickGroup> &event, CommandSource &src, Command *c, const Anope::string &n, NickServ::Nick *targ) : onnickgroup(event), source(src), cmd(c), nick(n), target(targ) { }

	void OnSuccess(NickServ::IdentifyRequest *) override
	{
		if (!source.GetUser() || source.GetUser()->nick != nick || !target || !target->nc)
			return;

		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(nick);
		/* If the nick is already registered, drop it. */
		if (na)
		{
			Event::OnChangeCoreDisplay(&Event::ChangeCoreDisplay::OnChangeCoreDisplay, na->nc, u->nick);
			delete na;
		}

		na = NickServ::service->CreateNick(nick, target->nc);

		Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
		na->last_usermask = last_usermask;
		na->last_realname = u->realname;
		na->time_registered = na->last_seen = Anope::CurTime;

		u->Login(target->nc);
		this->onnickgroup(&Event::NickGroup::OnNickGroup, u, target);

		Log(LOG_COMMAND, source, cmd) << "to make " << nick << " join group of " << target->nick << " (" << target->nc->display << ") (email: " << (!target->nc->email.empty() ? target->nc->email : "none") << ")";
		source.Reply(_("You are now in the group of \002{0}\002."), target->nick);

		u->lastnickreg = Anope::CurTime;

	}

	void OnFail(NickServ::IdentifyRequest *) override
	{
		if (!source.GetUser())
			return;

		Log(LOG_COMMAND, source, cmd) << "and failed to group to " << target->nick;
		source.Reply(_("Password incorrect."));
		source.GetUser()->BadPassword();
	}
};

class CommandNSGroup : public Command
{
	EventHandlers<Event::NickGroup> &onnickgroup;

 public:
	CommandNSGroup(Module *creator, EventHandlers<Event::NickGroup> &event) : Command(creator, "nickserv/group", 0, 2), onnickgroup(event)
	{
		this->SetDesc(_("Join a group"));
		this->SetSyntax(_("\037[target]\037 \037[password]\037"));
		this->AllowUnregistered(true);
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		Anope::string nick;
		if (params.empty())
		{
			NickServ::Account* core = u->Account();
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
			source.Reply(_("Sorry, nickname grouping is temporarily disabled."));
			return;
		}

		if (!IRCD->IsNickValid(u->nick))
		{
			source.Reply(_("\002{0}\002 may not be registered."), u->nick);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("restrictopernicks"))
			for (unsigned i = 0; i < Oper::opers.size(); ++i)
			{
				Oper *o = Oper::opers[i];

				if (!u->HasMode("OPER") && u->nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(_("\002{0}\002 may not be registered because it is too similar to an operator nick."), u->nick);
					return;
				}
			}

		NickServ::Nick *target, *na = NickServ::FindNick(u->nick);
		const Anope::string &guestnick = Config->GetModule("nickserv")->Get<const Anope::string>("guestnickprefix", "Guest");
		time_t reg_delay = Config->GetModule("nickserv")->Get<time_t>("regdelay");
		unsigned maxaliases = Config->GetModule(this->owner)->Get<unsigned>("maxaliases");
		if (!(target = NickServ::FindNick(nick)))
			source.Reply(_("\002{0}\002 isn't registered."), nick);
		else if (Anope::CurTime < u->lastnickreg + reg_delay)
			source.Reply(_("Please wait \002{0}\002 seconds before using the \002{1}\002 command again."), (reg_delay + u->lastnickreg) - Anope::CurTime, source.command);
		else if (target->nc->HasExt("NS_SUSPENDED"))
		{
			Log(LOG_COMMAND, source, this) << "and tried to group to suspended nick " << target->nick;
			source.Reply(_("\002{0}\002 is suspended."), target->nick);
		}
		else if (na && *target->nc == *na->nc)
			source.Reply(_("You are already a member of the group of \002{0}\002."), target->nick);
		else if (na && na->nc != u->Account())
			source.Reply(_("\002{0}\002 is already registered."), na->nick);
		else if (na && Config->GetModule(this->owner)->Get<bool>("nogroupchange"))
			source.Reply(_("Your nick is already registered."));
		else if (maxaliases && target->nc->aliases->size() >= maxaliases && !target->nc->IsServicesOper())
			source.Reply(_("There are too many nicknames in your group."));
		else if (u->nick.length() <= guestnick.length() + 7 &&
			u->nick.length() >= guestnick.length() + 1 &&
			!u->nick.find_ci(guestnick) && !u->nick.substr(guestnick.length()).find_first_not_of("1234567890"))
		{
			source.Reply(_("\002{0}\002 may not be registered."), u->nick);
		}
		else
		{
			bool ok = false;
			if (!na && u->Account())
				ok = true;

			NSCertList *cl = target->nc->GetExt<NSCertList>("certificates");
			if (!u->fingerprint.empty() && cl && cl->FindCert(u->fingerprint))
				ok = true;

			if (ok == false && !pass.empty())
			{
				NickServ::IdentifyRequest *req = NickServ::service->CreateIdentifyRequest(new NSGroupRequestListener(onnickgroup, source, this, u->nick, target), owner, target->nc->display, pass);
				Event::OnCheckAuthentication(&Event::CheckAuthentication::OnCheckAuthentication, source.GetUser(), req);
				req->Dispatch();
			}
			else
			{
				NSGroupRequestListener req(onnickgroup, source, this, u->nick, target);

				if (ok)
					req.OnSuccess(nullptr);
				else
					req.OnFail(nullptr);
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command makes your nickname join the \037target\037 nickname's group."
		               " \037password\037 is the password of the target nickname.\n"
		               "\n"
		               "Nicknames in the same group share channel privileges, memos, and most settings, including password."
		               "\n"
		               "You may be able to use this command even if you have not registered your nick yet."
		               " If your nick is already registered, you'll need to identify yourself before using this command."));
		return true;
	}
};

class CommandNSUngroup : public Command
{
 public:
	CommandNSUngroup(Module *creator) : Command(creator, "nickserv/ungroup", 0, 1)
	{
		this->SetDesc(_("Remove a nick from a group"));
		this->SetSyntax(_("[\037nickname\037]"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Anope::string nick = !params.empty() ? params[0] : "";
		NickServ::Nick *na = NickServ::FindNick(!nick.empty() ? nick : u->nick);

		if (u->Account()->aliases->size() == 1)
			source.Reply(_("Your nickname is not grouped to anything, so you can't ungroup it."));
		else if (!na)
			source.Reply(_("\002{0}\002 isn't registered."), !nick.empty() ? nick : u->nick);
		else if (na->nc != u->Account())
			source.Reply(_("\002{0}\002 is not in your group."), na->nick);
		else
		{
			NickServ::Account *oldcore = na->nc;

			std::vector<NickServ::Nick *>::iterator it = std::find(oldcore->aliases->begin(), oldcore->aliases->end(), na);
			if (it != oldcore->aliases->end())
				oldcore->aliases->erase(it);

			if (na->nick.equals_ci(oldcore->display))
				oldcore->SetDisplay(oldcore->aliases->front());

			NickServ::Account *nc = NickServ::service->CreateAccount(na->nick);
			na->nc = nc;
			nc->aliases->push_back(na);

			nc->pass = oldcore->pass;
			if (!oldcore->email.empty())
				nc->email = oldcore->email;
			nc->language = oldcore->language;

			source.Reply(_("\002{0}\002 has been ungrouped from \002{1}\002."), na->nick, oldcore->display);

			User *user = User::Find(na->nick);
			if (user)
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(source.service, "REGISTERED");
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command ungroups your nickname, or if given, the specificed \037nickname\037, from the group it is in."
		              " The ungrouped nick keeps its registration time, password, email, and language. Everything else is reset."
		              " You may not ungroup yourself if there is only one nickname in your group."));
		return true;
	}
};

class CommandNSGList : public Command
{
 public:
	CommandNSGList(Module *creator) : Command(creator, "nickserv/glist", 0, 1)
	{
		this->SetDesc(_("Lists all nicknames in your group"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = !params.empty() ? params[0] : "";
		const NickServ::Account *nc;

		if (!nick.empty() && source.IsServicesOper())
		{
			const NickServ::Nick *na = NickServ::FindNick(nick);
			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}

			nc = na->nc;
		}
		else
			nc = source.GetAccount();

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Nick")).AddColumn(_("Expires"));
		time_t nickserv_expire = Config->GetModule("nickserv")->Get<time_t>("expire", "21d"),
		       unconfirmed_expire = Config->GetModule("nickserv")->Get<time_t>("unconfirmedexpire", "1d");
		for (unsigned i = 0; i < nc->aliases->size(); ++i)
		{
			const NickServ::Nick *na2 = nc->aliases->at(i);

			Anope::string expires;
			if (na2->HasExt("NS_NO_EXPIRE"))
				expires = "Does not expire";
			else if (!nickserv_expire || Anope::NoExpire)
				;
			else if (na2->nc->HasExt("UNCONFIRMED") && unconfirmed_expire)
				expires = Anope::strftime(na2->time_registered + unconfirmed_expire, source.GetAccount());
			else
				expires = Anope::strftime(na2->last_seen + nickserv_expire, source.GetAccount());

			ListFormatter::ListEntry entry;
			entry["Nick"] = na2->nick;
			entry["Expires"] = expires;
			list.AddEntry(entry);
		}

		source.Reply(nc != source.GetAccount() ? _("List of nicknames in the group of \002%s\002:") : _("List of nicknames in your group:"), nc->display.c_str());
		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("%d nickname(s) in the group."), nc->aliases->size());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (source.IsServicesOper())
			source.Reply(_("Without a parameter, lists all nicknames that are in your group.\n"
					"\n"
					"With a parameter, lists all nicknames that are in the group of the given nick.\n"
					"Specifying a nick is limited to \002Services Operators\002."),
					source.command);
		else
			source.Reply(_("Lists all nicknames in your group."));

		return true;
	}
};

class NSGroup : public Module
{
	CommandNSGroup commandnsgroup;
	CommandNSUngroup commandnsungroup;
	CommandNSGList commandnsglist;

	EventHandlers<Event::NickGroup> onnickgroup;

 public:
	NSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsgroup(this, onnickgroup)
		, commandnsungroup(this)
		, commandnsglist(this)
		, onnickgroup(this, "OnNickGroup")
	{
		if (Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSGroup)
