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

void myNickServHelp(User *u);
NickAlias *makenick(const char *nick);

class CommandNSForbid : public Command
{
 public:
	CommandNSForbid() : Command("FORBID", 1, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na;
		const char *nick = params[0].c_str();
		const char *reason = params.size() > 1 ? params[1].c_str() : NULL;

		/* Assumes that permission checking has already been done. */
		if (ForceForbidReason && !reason) {
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (readonly)
			notice_lang(s_NickServ, u, READ_ONLY_MODE);
		if (!ircdproto->IsNickValid(nick))
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
			return MOD_CONT;
		}
		if ((na = findnick(nick)))
		{
			if (NSSecureAdmins && na->nc->IsServicesOper())
			{
				notice_lang(s_NickServ, u, PERMISSION_DENIED);
				return MOD_CONT;
			}
			delnick(na);
		}
		na = makenick(nick);
		if (na)
		{
			na->status |= NS_FORBIDDEN;
			na->last_usermask = sstrdup(u->nick);
			if (reason)
				na->last_realname = sstrdup(reason);

			User *curr = finduser(na->nick);

			if (curr)
			{
				notice_lang(s_NickServ, curr, FORCENICKCHANGE_NOW);
				collide(na, 0);
			}


			if (ircd->sqline)
				ircdproto->SendSQLine(na->nick, reason ? reason : "Forbidden");

			if (WallForbid)
				ircdproto->SendGlobops(s_NickServ, "\2%s\2 used FORBID on \2%s\2", u->nick, nick);

			alog("%s: %s set FORBID for nick %s", s_NickServ, u->nick, nick);
			notice_lang(s_NickServ, u, NICK_FORBID_SUCCEEDED, nick);
			send_event(EVENT_NICK_FORBIDDEN, 1, nick);
		}
		else
		{
			alog("%s: Valid FORBID for %s by %s failed", s_NickServ, nick, u->nick);
			notice_lang(s_NickServ, u, NICK_FORBID_FAILED, nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_FORBID);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "FORBID", ForceForbidReason ? NICK_FORBID_SYNTAX_REASON : NICK_FORBID_SYNTAX);
	}
};

class NSForbid : public Module
{
 public:
	NSForbid(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSForbid(), MOD_UNIQUE);

		this->SetNickHelp(myNickServHelp);
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User *u)
{
	notice_lang(s_NickServ, u, NICK_HELP_CMD_FORBID);
}

NickAlias *makenick(const char *nick)
{
	NickAlias *na;
	NickCore *nc;

	/* First make the core */
	nc = new NickCore();
	nc->display = sstrdup(nick);
	slist_init(&nc->aliases);
	insert_core(nc);
	alog("%s: group %s has been created", s_NickServ, nc->display);

	/* Then make the alias */
	na = new NickAlias;
	na->nick = sstrdup(nick);
	na->nc = nc;
	slist_add(&nc->aliases, na);
	alpha_insert_alias(na);
	return na;
}

MODULE_INIT("ns_forbid", NSForbid)
