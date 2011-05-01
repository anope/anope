/* NickServ core functions
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

class CommandNSGroup : public Command
{
 public:
	CommandNSGroup() : Command("GROUP", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Join a group"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		if (readonly)
		{
			source.Reply(_("Sorry, nickname grouping is temporarily disabled."));
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			source.Reply(_(NICK_X_FORBIDDEN), u->nick.c_str());
			return MOD_CONT;
		}

		if (Config->RestrictOperNicks)
			for (unsigned i = 0; i < Config->Opers.size(); ++i)
			{
				Oper *o = Config->Opers[i];

				if (!u->HasMode(UMODE_OPER) && u->nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(_(NICK_CANNOT_BE_REGISTERED), u->nick.c_str());
					return MOD_CONT;
				}
			}

		NickAlias *target, *na = findnick(u->nick);
		if (!(target = findnick(nick)))
			source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());
		else if (Anope::CurTime < u->lastnickreg + Config->NSRegDelay)
			source.Reply(_("Please wait %d seconds before using the GROUP command again."), (Config->NSRegDelay + u->lastnickreg) - Anope::CurTime);
		else if (u->Account() && u->Account()->HasFlag(NI_SUSPENDED))
		{
			Log(NickServ) << NickServ << u->GetMask() << " tried to use GROUP from SUSPENDED nick " << target->nick;
			source.Reply(_(NICK_X_SUSPENDED), u->nick.c_str());
		}
		else if (target && target->nc->HasFlag(NI_SUSPENDED))
		{
			Log(LOG_COMMAND, u, this) << "tried to use GROUP for SUSPENDED nick " << target->nick;
			source.Reply(_(NICK_X_SUSPENDED), target->nick.c_str());
		}
		else if (target->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), nick.c_str());
		else if (na && target->nc == na->nc)
			source.Reply(_("You are already a member of the group of \002%s\002."), target->nick.c_str());
		else if (na && na->nc != u->Account())
			source.Reply(_(NICK_IDENTIFY_REQUIRED), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
		else if (na && Config->NSNoGroupChange)
			source.Reply(_("Your nick is already registered; type \002%s%s DROP\002 first."), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
		else if (Config->NSMaxAliases && (target->nc->aliases.size() >= Config->NSMaxAliases) && !target->nc->IsServicesOper())
			source.Reply(_("There are too many nicks in %s's group; list them and drop some.\n"
					"Type \002%s%s HELP GLIST\002 and \002%s%s HELP DROP\002\n"
					"for more information."), target->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
		else
		{
			bool ok = false;
			if (!na && u->Account())
				ok = true;
			else if (!u->fingerprint.empty() && target->nc->FindCert(u->fingerprint))
				ok = true;
			else if (!pass.empty())
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(u, this, params, target->nc->display, pass));
				if (MOD_RESULT == EVENT_STOP)
					return MOD_CONT;
				else if (MOD_RESULT == EVENT_ALLOW)
					ok = true;
			}
			if (ok)
			{
				/* If the nick is already registered, drop it.
				 * If not, check that it is valid.
				 */
				if (na)
					delete na;
				else
				{
					size_t prefixlen = Config->NSGuestNickPrefix.length();
					size_t nicklen = u->nick.length();

					if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config->NSGuestNickPrefix) && !u->nick.substr(prefixlen).find_first_not_of("1234567890"))
					{
						source.Reply(_(NICK_CANNOT_BE_REGISTERED), u->nick.c_str());
						return MOD_CONT;
					}
				}

				na = new NickAlias(u->nick, target->nc);

				Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
				na->last_usermask = last_usermask;
				na->last_realname = u->realname;
				na->time_registered = na->last_seen = Anope::CurTime;

				u->Login(na->nc);
				FOREACH_MOD(I_OnNickGroup, OnNickGroup(u, target));
				ircdproto->SetAutoIdentificationToken(u);
				if (target->nc->HasFlag(NI_UNCONFIRMED) == false)
					u->SetMode(NickServ, UMODE_REGISTERED);

				Log(LOG_COMMAND, u, this) << "makes " << u->nick << " join group of " << target->nick << " (" << target->nc->display << ") (email: " << (!target->nc->email.empty() ? target->nc->email : "none") << ")";
				source.Reply(_("You are now in the group of \002%s\002."), target->nick.c_str());

				u->lastnickreg = Anope::CurTime;

				check_memos(u);
			}
			else
			{
				Log(LOG_COMMAND, u, this) << "failed group for " << target->nick;
				source.Reply(_(PASSWORD_INCORRECT));
				bad_password(u);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002GROUP \037target\037 \037password\037\002\n"
				" \n"
				"This command makes your nickname join the \037target\037 nickname's \n"
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
				"You can use this command even if you have not registered\n"
				"your nick yet. If your nick is already registered, you'll\n"
				"need to identify yourself before using this command. Type\n"
				"\037%s%s HELP IDENTIFY\037 for more information. This\n"
				"last may be not possible on your IRC network.\n"
				" \n"
				"It is recommended to use this command with a non-registered\n"
				"nick because it will be registered automatically when \n"
				"using this command. You may use it with a registered nick (to \n"
				"change your group) only if your network administrators allowed \n"
				"it.\n"
				" \n"
				"You can only be in one group at a time. Group merging is\n"
				"not possible.\n"
				" \n"
				"\037Note\037: all the nicknames of a group have the same password."),
				Config->UseStrictPrivMsgString.c_str(), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GROUP", _("\037target\037 \037password\037"));
	}
};

class CommandNSUngroup : public Command
{
 public:
	CommandNSUngroup() : Command("UNGROUP", 0, 1)
	{
		this->SetDesc(_("Remove a nick from a group"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string nick = !params.empty() ? params[0] : "";
		NickAlias *na = findnick(!nick.empty() ? nick : u->nick);

		if (u->Account()->aliases.size() == 1)
			source.Reply(_("Your nick is not grouped to anything, you can't ungroup it."));
		else if (!na)
			source.Reply(_(NICK_X_NOT_REGISTERED), !nick.empty() ? nick.c_str() : u->nick.c_str());
		else if (na->nc != u->Account())
			source.Reply(_("The nick %s is not in your group."), na->nick.c_str());
		else
		{
			NickCore *oldcore = na->nc;

			std::list<NickAlias *>::iterator it = std::find(oldcore->aliases.begin(), oldcore->aliases.end(), na);
			if (it != oldcore->aliases.end())
				oldcore->aliases.erase(it);

			if (na->nick.equals_ci(oldcore->display))
				change_core_display(oldcore);

			na->nc = new NickCore(na->nick);
			na->nc->aliases.push_back(na);

			na->nc->pass = oldcore->pass;
			if (!oldcore->email.empty())
				na->nc->email = oldcore->email;
			if (!oldcore->greet.empty())
				na->nc->greet = oldcore->greet;
			na->nc->language = oldcore->language;

			source.Reply(_("Nick %s has been ungrouped from %s."), na->nick.c_str(), oldcore->display.c_str());

			User *user = finduser(na->nick);
			if (user)
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(NickServ, UMODE_REGISTERED);
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002UNGROUP \037[nick]\037\002\n"
				" \n"
				"This command ungroups your nick, or if given, the specificed nick,\n"
				"from the group it is in. The ungrouped nick keeps its registration\n"
				"time, password, email, greet, language, url, and icq. Everything\n"
				"else is reset. You may not ungroup yourself if there is only one\n"
				"nick in your group."));
		return true;
	}
};

class CommandNSGList : public Command
{
 public:
	CommandNSGList() : Command("GLIST", 0, 1)
	{
		this->SetDesc(_("Lists all nicknames in your group"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string nick = !params.empty() ? params[0] : "";

		const NickCore *nc = u->Account();

		if (!nick.empty() && (!nick.equals_ci(u->nick) && !u->IsServicesOper()))
			source.Reply(_(ACCESS_DENIED), Config->s_NickServ.c_str());
		else if (!nick.empty() && (!findnick(nick) || !(nc = findnick(nick)->nc)))
			source.Reply(nick.empty() ? _(NICK_NOT_REGISTERED) : _(NICK_X_NOT_REGISTERED), nick.c_str());
		else
		{
			source.Reply(!nick.empty() ? _("List of nicknames in the group of \002%s\002:") : _("List of nicknames in your group:"), nc->display.c_str());
			for (std::list<NickAlias *>::const_iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
			{
				NickAlias *na2 = *it;

				source.Reply(na2->HasFlag(NS_NO_EXPIRE) ? _("   %s (does not expire)") : _("   %s (expires in %s)"), na2->nick.c_str(), do_strftime(na2->last_seen + Config->NSExpire).c_str());
			}
			source.Reply(_("%d nicknames in the group."), nc->aliases.size());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->IsServicesOper())
			source.Reply(_("Syntax: \002GLIST [\037nickname\037]\002\n"
					" \n"
					"Without a parameter, lists all nicknames that are in\n"
					"your group.\n"
					" \n"
					"With a parameter, lists all nicknames that are in the\n"
					"group of the given nick.\n"
					"This use limited to \002Services Operators\002."));
		else
			source.Reply(_("Syntax: \002GLIST\002\n"
					" \n"
					"Lists all nicks in your group."));

		return true;
	}
};

class NSGroup : public Module
{
	CommandNSGroup commandnsgroup;
	CommandNSUngroup commandnsungroup;
	CommandNSGList commandnsglist;

 public:
	NSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsgroup);
		this->AddCommand(NickServ, &commandnsungroup);
		this->AddCommand(NickServ, &commandnsglist);
	}
};

MODULE_INIT(NSGroup)
