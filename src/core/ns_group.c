/* NickServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

NickAlias *makealias(const char *nick, NickCore *nc);

class CommandNSGroup : public Command
{
 public:
	CommandNSGroup() : Command("GROUP", 2, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na, *target;
		NickCore *nc;
		const char *nick = params[0].c_str();
		const char *pass = params[1].c_str();
		int i;
		char tsbuf[16];
		char modes[512];
		int len;
		std::list<std::pair<std::string, std::string> >::iterator it;

		if (NSEmailReg && findrequestnick(u->nick))
		{
			notice_lang(s_NickServ, u, NICK_REQUESTED);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(s_NickServ, u, NICK_GROUP_DISABLED);
			return MOD_CONT;
		}
		if (checkDefCon(DEFCON_NO_NEW_NICKS))
		{
			notice_lang(s_NickServ, u, OPER_DEFCON_DENIED);
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);
			return MOD_CONT;
		}

		if (RestrictOperNicks)
		{
			for (it = svsopers_in_config.begin(); it != svsopers_in_config.end(); it++)
			{
				const std::string nick = it->first;

				if (stristr(u->nick, const_cast<char *>(it->first.c_str())) && !is_oper(u))
				{
					notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
					return MOD_CONT;
				}
			}
		}

		if (!(target = findnick(nick)))
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (time(NULL) < u->lastnickreg + NSRegDelay)
			notice_lang(s_NickServ, u, NICK_GROUP_PLEASE_WAIT, NSRegDelay);
		else if (u->nc && (u->nc->flags & NI_SUSPENDED))
		{
			alog("%s: %s!%s@%s tried to use GROUP from SUSPENDED nick %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, target->nick);
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);
		}
		else if (target && (target->nc->flags & NI_SUSPENDED))
		{
			alog("%s: %s!%s@%s tried to use GROUP from SUSPENDED nick %s", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, target->nick);
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, target->nick);
		}
		else if (target->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
		else if (u->nc && (target->nc == u->nc))
			notice_lang(s_NickServ, u, NICK_GROUP_SAME, target->nick);
		else if (NSMaxAliases && (target->nc->aliases.count >= NSMaxAliases) && !target->nc->IsServicesOper())
			notice_lang(s_NickServ, u, NICK_GROUP_TOO_MANY, target->nick, s_NickServ, s_NickServ);
		else if (enc_check_password(pass, target->nc->pass) != 1)
		{
			alog("%s: Failed GROUP for %s!%s@%s (invalid password)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host);
			notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
			bad_password(u);
		}
		else
		{
			/* If the nick is already registered, drop it.
			 * If not, check that it is valid.
			 */
			if (findnick(u->nick))
				delnick(findnick(u->nick));
			else
			{
				int prefixlen = strlen(NSGuestNickPrefix);
				int nicklen = strlen(u->nick);

				if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && stristr(u->nick, NSGuestNickPrefix) == u->nick && strspn(u->nick + prefixlen, "1234567890") == nicklen - prefixlen)
				{
					notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
					return MOD_CONT;
				}
			}
			na = makealias(u->nick, target->nc);

			if (na)
			{
				na->last_usermask = new char[u->GetIdent().length() + u->GetDisplayedHost().length() + 2];
				sprintf(na->last_usermask, "%s@%s", u->GetIdent().c_str(), u->GetDisplayedHost().c_str());
				na->last_realname = sstrdup(u->realname);
				na->time_registered = na->last_seen = time(NULL);
				na->status = static_cast<int16>(NS_IDENTIFIED | NS_RECOGNIZED);

				u->nc = na->nc;

				FOREACH_MOD(I_OnNickGroup, OnNickGroup(u, target));

				alog("%s: %s!%s@%s makes %s join group of %s (%s) (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, u->nick, target->nick, target->nc->display, (target->nc->email ? target->nc->email : "none"));
				notice_lang(s_NickServ, u, NICK_GROUP_JOINED, target->nick);

				u->lastnickreg = time(NULL);
				snprintf(tsbuf, sizeof(tsbuf), "%lu", static_cast<unsigned long>(u->timestamp));
				if (ircd->modeonreg)
				{
					len = strlen(ircd->modeonreg);
					strncpy(modes, ircd->modeonreg, 512);
					if (ircd->tsonmode)
						common_svsmode(u, modes, tsbuf);
					else
						common_svsmode(u, modes, NULL);
				}

				check_memos(u);
			}
			else
			{
				alog("%s: makealias(%s) failed", s_NickServ, u->nick);
				notice_lang(s_NickServ, u, NICK_GROUP_FAILED);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_GROUP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "GROUP", NICK_GROUP_SYNTAX);
	}
};

class CommandNSGList : public Command
{
 public:
	CommandNSGList() : Command("GLIST", 0, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params.size() ? params[0].c_str() : NULL;

		NickCore *nc = u->nc;
		int i;

		if (nick && (stricmp(nick, u->nick) && !u->nc->IsServicesOper()))
			notice_lang(s_NickServ, u, ACCESS_DENIED, s_NickServ);
		else if (nick && (!findnick(nick) || !(nc = findnick(nick)->nc)))
			notice_lang(s_NickServ, u, !nick ? NICK_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nick);
/*		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);*/
		else
		{
			time_t expt;
			struct tm *tm;
			char buf[BUFSIZE];
			int wont_expire;

			notice_lang(s_NickServ, u, nick ? NICK_GLIST_HEADER_X : NICK_GLIST_HEADER, nc->display);
			for (i = 0; i < nc->aliases.count; ++i)
			{
				NickAlias *na2 = static_cast<NickAlias *>(nc->aliases.list[i]);
				if (na2->nc == nc)
				{
					if (!(wont_expire = na2->status & NS_NO_EXPIRE))
					{
						expt = na2->last_seen + NSExpire;
						tm = localtime(&expt);
						strftime_lang(buf, sizeof(buf), finduser(na2->nick), STRFTIME_DATE_TIME_FORMAT, tm);
					}
					notice_lang(s_NickServ, u, u->nc->IsServicesOper() && !wont_expire ? NICK_GLIST_REPLY_ADMIN : NICK_GLIST_REPLY, wont_expire ? '!' : ' ', na2->nick, buf);
				}
			}
			notice_lang(s_NickServ, u, NICK_GLIST_FOOTER, nc->aliases.count);
		}
		return MOD_CONT;
	}


	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_GLIST);
		else
			notice_help(s_NickServ, u, NICK_HELP_GLIST);

		return true;
	}
};

class NSGroup : public Module
{
 public:
	NSGroup(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSGroup(), MOD_UNIQUE);
		this->AddCommand(NICKSERV, new CommandNSGList(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_GROUP);
		notice_lang(s_NickServ, u, NICK_HELP_CMD_GLIST);
	}
};

/* Creates a new alias in NickServ database. */
NickAlias *makealias(const char *nick, NickCore *nc)
{
	NickAlias *na;

	/* Just need to make the alias */
	na = new NickAlias;
	na->nick = sstrdup(nick);
	na->nc = nc;
	slist_add(&nc->aliases, na);
	alpha_insert_alias(na);
	return na;
}

MODULE_INIT("ns_group", NSGroup)
