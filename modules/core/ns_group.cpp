/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	CommandNSGroup() : Command("GROUP", 2, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na, *target;
		Anope::string nick = params[0];
		Anope::string pass = params[1];
		std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

		if (Config.NSEmailReg && findrequestnick(u->nick))
		{
			notice_lang(Config.s_NickServ, u, NICK_REQUESTED);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(Config.s_NickServ, u, NICK_GROUP_DISABLED);
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config.RestrictOperNicks)
			for (it = Config.Opers.begin(), it_end = Config.Opers.end(); it != it_end; ++it)
				if (!is_oper(u) && u->nick.find_ci(it->first) != Anope::string::npos)
				{
					notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}

		na = findnick(u->nick);
		if (!(target = findnick(nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (time(NULL) < u->lastnickreg + Config.NSRegDelay)
			notice_lang(Config.s_NickServ, u, NICK_GROUP_PLEASE_WAIT, (Config.NSRegDelay + u->lastnickreg) - time(NULL));
		else if (u->Account() && u->Account()->HasFlag(NI_SUSPENDED))
		{
			Alog() << Config.s_NickServ << ": " << u->GetMask() << " tried to use GROUP from SUSPENDED nick " << target->nick;
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->nick.c_str());
		}
		else if (target && target->nc->HasFlag(NI_SUSPENDED))
		{
			Alog() << Config.s_NickServ << ": " << u->GetMask() << " tried to use GROUP for SUSPENDED nick " << target->nick;
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, target->nick.c_str());
		}
		else if (target->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, nick.c_str());
		else if (na && target->nc == na->nc)
			notice_lang(Config.s_NickServ, u, NICK_GROUP_SAME, target->nick.c_str());
		else if (na && na->nc != u->Account())
			notice_lang(Config.s_NickServ, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ.c_str());
		else if (Config.NSMaxAliases && (target->nc->aliases.size() >= Config.NSMaxAliases) && !target->nc->IsServicesOper())
			notice_lang(Config.s_NickServ, u, NICK_GROUP_TOO_MANY, target->nick.c_str(), Config.s_NickServ.c_str(), Config.s_NickServ.c_str());
		else
		{
			int res = enc_check_password(pass, target->nc->pass);
			if (res == -1)
			{
				Alog() << Config.s_NickServ << ": Failed GROUP for " << u->GetMask() << " (invalid password)";
				notice_lang(Config.s_NickServ, u, PASSWORD_INCORRECT);
				if (bad_password(u))
					return MOD_STOP;
			}
			else
			{
				/* If the nick is already registered, drop it.
				 * If not, check that it is valid.
				 */
				if (na)
					delete na;
				else
				{
					size_t prefixlen = Config.NSGuestNickPrefix.length();
					size_t nicklen = u->nick.length();

					if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config.NSGuestNickPrefix) && !u->nick.substr(prefixlen).find_first_not_of("1234567890"))
					{
						notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
						return MOD_CONT;
					}
				}

				na = new NickAlias(u->nick, target->nc);

				if (na)
				{
					Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
					na->last_usermask = last_usermask;
					na->last_realname = u->realname;
					na->time_registered = na->last_seen = time(NULL);

					u->Login(na->nc);
					FOREACH_MOD(I_OnNickGroup, OnNickGroup(u, target));
					ircdproto->SetAutoIdentificationToken(u);

					Alog() << Config.s_NickServ << ": " << u->GetMask() << " makes " << u->nick << " join group of " << target->nick << " (" << target->nc->display << ") (e-mail: " <<
						(!target->nc->email.empty() ? target->nc->email : "none") << ")";
					notice_lang(Config.s_NickServ, u, NICK_GROUP_JOINED, target->nick.c_str());

					u->lastnickreg = time(NULL);

					check_memos(u);
				}
				else
				{
					Alog() << Config.s_NickServ << ": makealias(" << u->nick << ") failed";
					notice_lang(Config.s_NickServ, u, NICK_GROUP_FAILED);
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_GROUP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "GROUP", NICK_GROUP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_GROUP);
	}
};

class CommandNSUngroup : public Command
{
 public:
	CommandNSUngroup() : Command("UNGROUP", 0, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = !params.empty() ? params[0] : "";
		NickAlias *na = findnick(!nick.empty() ? nick : u->nick);

		if (u->Account()->aliases.size() == 1)
			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_ONE_NICK);
		else if (!na)
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, !nick.empty() ? nick.c_str() : u->nick.c_str());
		else if (na->nc != u->Account())
			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_NOT_IN_GROUP, na->nick.c_str());
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

			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_SUCCESSFUL, na->nick.c_str(), oldcore->display.c_str());

			User *user = finduser(na->nick);
			if (user)
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(NickServ, UMODE_REGISTERED);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_UNGROUP);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_UNGROUP);
	}
};

class CommandNSGList : public Command
{
 public:
	CommandNSGList() : Command("GLIST", 0, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = !params.empty() ? params[0] : "";

		const NickCore *nc = u->Account();

		if (!nick.empty() && (!nick.equals_ci(u->nick) && !u->Account()->IsServicesOper()))
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED, Config.s_NickServ.c_str());
		else if (!nick.empty() && (!findnick(nick) || !(nc = findnick(nick)->nc)))
			notice_lang(Config.s_NickServ, u, nick.empty() ? NICK_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nick.c_str());
		else
		{
			time_t expt;
			struct tm *tm;
			char buf[BUFSIZE];
			int wont_expire;

			notice_lang(Config.s_NickServ, u, !nick.empty() ? NICK_GLIST_HEADER_X : NICK_GLIST_HEADER, nc->display.c_str());
			for (std::list<NickAlias *>::const_iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
			{
				NickAlias *na2 = *it;

				if (!(wont_expire = na2->HasFlag(NS_NO_EXPIRE)))
				{
					expt = na2->last_seen + Config.NSExpire;
					tm = localtime(&expt);
					strftime_lang(buf, sizeof(buf), finduser(na2->nick), STRFTIME_DATE_TIME_FORMAT, tm);
				}
				notice_lang(Config.s_NickServ, u, wont_expire ? NICK_GLIST_REPLY_NOEXPIRE : NICK_GLIST_REPLY, na2->nick.c_str(), buf);
			}
			notice_lang(Config.s_NickServ, u, NICK_GLIST_FOOTER, nc->aliases.size());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config.s_NickServ, u, NICK_SERVADMIN_HELP_GLIST);
		else
			notice_help(Config.s_NickServ, u, NICK_HELP_GLIST);

		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_GLIST);
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
