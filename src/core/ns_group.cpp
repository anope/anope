/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na, *target;
		const char *nick = params[0].c_str();
		std::string pass = params[1].c_str();
		std::list<std::pair<ci::string, ci::string> >::iterator it;

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

		if (!ircdproto->IsNickValid(u->nick.c_str()))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config.RestrictOperNicks)
		{
			for (it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
			{
				if (!is_oper(u) && u->nick.find(it->first.c_str()) != std::string::npos)
				{
					notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}
			}
		}

		na = findnick(u->nick);
		if (!(target = findnick(nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
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
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, target->nick);
		}
		else if (target->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, nick);
		else if (na && target->nc == na->nc)
			notice_lang(Config.s_NickServ, u, NICK_GROUP_SAME, target->nick);
		else if (na && na->nc != u->Account())
			notice_lang(Config.s_NickServ, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ);
		else if (Config.NSMaxAliases && (target->nc->aliases.size() >= Config.NSMaxAliases) && !target->nc->IsServicesOper())
			notice_lang(Config.s_NickServ, u, NICK_GROUP_TOO_MANY, target->nick, Config.s_NickServ, Config.s_NickServ);
		else if (enc_check_password(pass, target->nc->pass) != 1)
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
				int prefixlen = strlen(Config.NSGuestNickPrefix);
				int nicklen = u->nick.length();

				if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && stristr(u->nick.c_str(), Config.NSGuestNickPrefix) == u->nick.c_str() && strspn(u->nick.c_str() + prefixlen, "1234567890") == nicklen - prefixlen)
				{
					notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}
			}

			na = new NickAlias(u->nick, target->nc);

			if (na)
			{
				std::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
				na->last_usermask = sstrdup(last_usermask.c_str());
				na->last_realname = sstrdup(u->realname);
				na->time_registered = na->last_seen = time(NULL);

				u->Login(na->nc);
				FOREACH_MOD(I_OnNickGroup, OnNickGroup(u, target));
				ircdproto->SetAutoIdentificationToken(u);

				Alog() << Config.s_NickServ << ": " << u->GetMask() << " makes " << u->nick
						<< " join group of " << target->nick << " (" << target->nc->display
						<< ") (e-mail: " <<  (target->nc->email ? target->nc->email : "none") << ")";
				notice_lang(Config.s_NickServ, u, NICK_GROUP_JOINED, target->nick);

				u->lastnickreg = time(NULL);

				check_memos(u);
			}
			else
			{
				Alog() << Config.s_NickServ << ": makealias(" << u->nick << ") failed";
				notice_lang(Config.s_NickServ, u, NICK_GROUP_FAILED);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_GROUP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params.size() ? params[0].c_str() : NULL;
		NickAlias *na = nick ? findnick(nick) : findnick(u->nick);

		if (u->Account()->aliases.size() == 1)
			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_ONE_NICK);
		else if (!na)
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick ? nick : u->nick.c_str());
		else if (na->nc != u->Account())
			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_NOT_IN_GROUP, na->nick);
		else
		{
			NickCore *oldcore = na->nc;

			std::list<NickAlias *>::iterator it = std::find(oldcore->aliases.begin(), oldcore->aliases.end(), na);
			if (it != oldcore->aliases.end())
			{
				oldcore->aliases.erase(it);
			}

			if (!stricmp(oldcore->display, na->nick))
			{
				change_core_display(oldcore);
			}

			na->nc = new NickCore(na->nick);
			na->nc->aliases.push_back(na);

			na->nc->pass = oldcore->pass;
			if (oldcore->email)
				na->nc->email = sstrdup(oldcore->email);
			if (oldcore->greet)
				na->nc->greet = sstrdup(oldcore->greet);
			na->nc->icq = oldcore->icq;
			if (oldcore->url)
				na->nc->url = sstrdup(oldcore->url);
			na->nc->language = oldcore->language;

			notice_lang(Config.s_NickServ, u, NICK_UNGROUP_SUCCESSFUL, na->nick, oldcore->display);

			User *user = finduser(na->nick);
			if (user)
			{
				/* The user on the nick who was ungrouped may be identified to the old group, set -r */
				user->RemoveMode(NickServ, UMODE_REGISTERED);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params.size() ? params[0].c_str() : NULL;

		NickCore *nc = u->Account();

		if (nick && (stricmp(nick, u->nick.c_str()) && !u->Account()->IsServicesOper()))
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED, Config.s_NickServ);
		else if (nick && (!findnick(nick) || !(nc = findnick(nick)->nc)))
			notice_lang(Config.s_NickServ, u, !nick ? NICK_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nick);
		else
		{
			time_t expt;
			struct tm *tm;
			char buf[BUFSIZE];
			int wont_expire;

			notice_lang(Config.s_NickServ, u, nick ? NICK_GLIST_HEADER_X : NICK_GLIST_HEADER, nc->display);
			for (std::list<NickAlias *>::iterator it = nc->aliases.begin(); it != nc->aliases.end(); ++it)
			{
				NickAlias *na2 = *it;

				if (!(wont_expire = na2->HasFlag(NS_NO_EXPIRE)))
				{
					expt = na2->last_seen + Config.NSExpire;
					tm = localtime(&expt);
					strftime_lang(buf, sizeof(buf), finduser(na2->nick), STRFTIME_DATE_TIME_FORMAT, tm);
				}
				notice_lang(Config.s_NickServ, u, wont_expire ? NICK_GLIST_REPLY_NOEXPIRE : NICK_GLIST_REPLY, na2->nick, buf);
			}
			notice_lang(Config.s_NickServ, u, NICK_GLIST_FOOTER, nc->aliases.size());
		}
		return MOD_CONT;
	}


	bool OnHelp(User *u, const ci::string &subcommand)
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
 public:
	NSGroup(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSGroup());
		this->AddCommand(NickServ, new CommandNSUngroup());
		this->AddCommand(NickServ, new CommandNSGList());
	}
};

MODULE_INIT(NSGroup)
