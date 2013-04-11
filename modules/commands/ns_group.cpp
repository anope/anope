/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class NSGroupRequest : public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
	Anope::string nick;
	Reference<NickAlias> target;
 
 public:
	NSGroupRequest(Module *o, CommandSource &src, Command *c, const Anope::string &n, NickAlias *targ, const Anope::string &pass) : IdentifyRequest(o, targ->nc->display, pass), source(src), cmd(c), nick(n), target(targ) { }

	void OnSuccess() anope_override
	{
		if (!source.GetUser() || source.GetUser()->nick != nick || !target || !target->nc)
			return;

		User *u = source.GetUser();
		NickAlias *na = NickAlias::Find(nick);
		/* If the nick is already registered, drop it. */
		if (na)
		{
			FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(na->nc, u->nick));
			delete na;
		}

		na = new NickAlias(nick, target->nc);

		Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
		na->last_usermask = last_usermask;
		na->last_realname = u->realname;
		na->time_registered = na->last_seen = Anope::CurTime;

		u->Login(target->nc);
		IRCD->SendLogin(u);
		if (!Config->NoNicknameOwnership && na->nc == u->Account() && na->nc->HasExt("UNCONFIRMED") == false)
			u->SetMode(NickServ, "REGISTERED");
		FOREACH_MOD(I_OnNickGroup, OnNickGroup(u, target));

		Log(LOG_COMMAND, source, cmd) << "makes " << nick << " join group of " << target->nick << " (" << target->nc->display << ") (email: " << (!target->nc->email.empty() ? target->nc->email : "none") << ")";
		source.Reply(_("You are now in the group of \002%s\002."), target->nick.c_str());

		u->lastnickreg = Anope::CurTime;

	}

	void OnFail() anope_override
	{
		if (!source.GetUser())
			return;

		Log(LOG_COMMAND, source, cmd) << "and failed to group to " << target->nick;
		if (NickAlias::Find(GetAccount()) != NULL)
		{
			source.Reply(PASSWORD_INCORRECT);
			source.GetUser()->BadPassword();
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
	}
};

class CommandNSGroup : public Command
{
 public:
	CommandNSGroup(Module *creator) : Command(creator, "nickserv/group", 1, 2)
	{
		this->SetDesc(_("Join a group"));
		this->SetSyntax(_("\037target\037 \037password\037"));
		this->AllowUnregistered(true);
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, nickname grouping is temporarily disabled."));
			return;
		}

		if (!IRCD->IsNickValid(u->nick))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return;
		}

		if (Config->RestrictOperNicks)
			for (unsigned i = 0; i < Config->Opers.size(); ++i)
			{
				Oper *o = Config->Opers[i];

				if (!u->HasMode("OPER") && u->nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return;
				}
			}

		NickAlias *target, *na = NickAlias::Find(u->nick);
		if (!(target = NickAlias::Find(nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (Anope::CurTime < u->lastnickreg + Config->NSRegDelay)
			source.Reply(_("Please wait %d seconds before using the GROUP command again."), (Config->NSRegDelay + u->lastnickreg) - Anope::CurTime);
		else if (target->nc->HasExt("SUSPENDED"))
		{
			Log(LOG_COMMAND, source, this) << "tried to use GROUP for SUSPENDED nick " << target->nick;
			source.Reply(NICK_X_SUSPENDED, target->nick.c_str());
		}
		else if (na && *target->nc == *na->nc)
			source.Reply(_("You are already a member of the group of \002%s\002."), target->nick.c_str());
		else if (na && na->nc != u->Account())
			source.Reply(NICK_IDENTIFY_REQUIRED, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		else if (na && Config->NSNoGroupChange)
			source.Reply(_("Your nick is already registered."));
		else if (Config->NSMaxAliases && (target->nc->aliases->size() >= Config->NSMaxAliases) && !target->nc->IsServicesOper())
			source.Reply(_("There are too many nicks in %s's group."));
		else if (u->nick.length() <= Config->NSGuestNickPrefix.length() + 7 &&
			u->nick.length() >= Config->NSGuestNickPrefix.length() + 1 &&
			!u->nick.find_ci(Config->NSGuestNickPrefix) && !u->nick.substr(Config->NSGuestNickPrefix.length()).find_first_not_of("1234567890"))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
		}
		else
		{
			bool ok = false;
			if (!na && u->Account())
				ok = true;
			else if (!u->fingerprint.empty() && target->nc->FindCert(u->fingerprint))
				ok = true;

			if (ok == false && !pass.empty())
			{
				NSGroupRequest *req = new NSGroupRequest(owner, source, this, u->nick, target, pass);
				FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(source.GetUser(), req));
				req->Dispatch();
			}
			else
			{
				NSGroupRequest req(owner, source, this, u->nick, target, pass);

				if (ok)
					req.OnSuccess();
				else
					req.OnFail();
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command makes your nickname join the \037target\037 nickname's\n"
				"group. \037password\037 is the password of the target nickname.\n"
				" \n"
				"Joining a group will allow you to share your configuration,\n"
				"memos, and channel privileges with all the nicknames in the\n"
				"group, and much more!\n"
				" \n"
				"A group exists as long as it is useful. This means that even\n"
				"if a nick of the group is dropped, you won't lose the\n"
				"shared things described above, as long as there is at\n"
				"least one nick remaining in the group.\n"
				" \n"
				"You may be able to use this command even if you have not registered\n"
				"your nick yet. If your nick is already registered, you'll\n"
				"need to identify yourself before using this command.\n"
				" \n"
				"It is recommended to use this command with a non-registered\n"
				"nick because it will be registered automatically when\n"
				"using this command. You may use it with a registered nick (to\n"
				"change your group) only if your network administrators allowed\n"
				"it.\n"
				" \n"
				"You can only be in one group at a time. Group merging is\n"
				"not possible.\n"
				" \n"
				"\037Note\037: all the nicknames of a group have the same password."));
		return true;
	}
};

class CommandNSUngroup : public Command
{
 public:
	CommandNSUngroup(Module *creator) : Command(creator, "nickserv/ungroup", 0, 1)
	{
		this->SetDesc(_("Remove a nick from a group"));
		this->SetSyntax(_("[\037nick\037]"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		Anope::string nick = !params.empty() ? params[0] : "";
		NickAlias *na = NickAlias::Find(!nick.empty() ? nick : u->nick);

		if (u->Account()->aliases->size() == 1)
			source.Reply(_("Your nick is not grouped to anything, you can't ungroup it."));
		else if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, !nick.empty() ? nick.c_str() : u->nick.c_str());
		else if (na->nc != u->Account())
			source.Reply(_("Nick %s is not in your group."), na->nick.c_str());
		else
		{
			NickCore *oldcore = na->nc;

			std::vector<NickAlias *>::iterator it = std::find(oldcore->aliases->begin(), oldcore->aliases->end(), na);
			if (it != oldcore->aliases->end())
				oldcore->aliases->erase(it);

			if (na->nick.equals_ci(oldcore->display))
				oldcore->SetDisplay(oldcore->aliases->front());

			NickCore *nc = new NickCore(na->nick);
			na->nc = nc;
			nc->aliases->push_back(na);

			nc->pass = oldcore->pass;
			if (!oldcore->email.empty())
				nc->email = oldcore->email;
			if (!oldcore->greet.empty())
				nc->greet = oldcore->greet;
			nc->language = oldcore->language;

			source.Reply(_("Nick %s has been ungrouped from %s."), na->nick.c_str(), oldcore->display.c_str());

			User *user = User::Find(na->nick);
			if (user)
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(NickServ, "REGISTERED");
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command ungroups your nick, or if given, the specificed nick,\n"
				"from the group it is in. The ungrouped nick keeps its registration\n"
				"time, password, email, greet, language, and url. Everything else\n"
				"is reset. You may not ungroup yourself if there is only one nick in\n"
				"your group."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
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
				source.Reply(ACCESS_DENIED, Config->NickServ.c_str());
				return;
			}

			nc = na->nc;
		}
		else
			nc = source.GetAccount();

		ListFormatter list;
		list.AddColumn("Nick").AddColumn("Expires");
		for (unsigned i = 0; i < nc->aliases->size(); ++i)
		{
			const NickAlias *na2 = nc->aliases->at(i);

			ListFormatter::ListEntry entry;
			entry["Nick"] = na2->nick;
			entry["Expires"] = (na2->HasExt("NO_EXPIRE") || !Config->NSExpire) ? "Does not expire" : ("expires in " + Anope::strftime(na2->last_seen + Config->NSExpire));
			list.AddEntry(entry);
		}

		source.Reply(!nick.empty() ? _("List of nicknames in the group of \002%s\002:") : _("List of nicknames in your group:"), nc->display.c_str());
		std::vector<Anope::string> replies;
		list.Process(replies);
	
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("%d nicknames in the group."), nc->aliases->size());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		if (source.IsServicesOper())
			source.Reply(_("Syntax: \002%s [\037nickname\037]\002\n"
					" \n"
					"Without a parameter, lists all nicknames that are in\n"
					"your group.\n"
					" \n"
					"With a parameter, lists all nicknames that are in the\n"
					"group of the given nick.\n"
					"Specifying a nick is limited to \002Services Operators\002."),
					source.command.c_str());
		else
			source.Reply(_("Syntax: \002%s\002\n"
					" \n"
					"Lists all nicks in your group."), source.command.c_str());

		return true;
	}
};

class NSGroup : public Module
{
	CommandNSGroup commandnsgroup;
	CommandNSUngroup commandnsungroup;
	CommandNSGList commandnsglist;

 public:
	NSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsgroup(this), commandnsungroup(this), commandnsglist(this)
	{

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSGroup)
