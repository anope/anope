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

class CommandNSDrop : public Command
{
 public:
	CommandNSDrop() : Command("DROP", 0, 1)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params.size() ? params[0].c_str() : NULL;
		NickAlias *na;
		NickRequest *nr = NULL;
		int is_servadmin = is_services_admin(u);
		int is_mine; /* Does the nick being dropped belong to the user that is dropping? */
		char *my_nick = NULL;

		if (readonly && !is_servadmin)
		{
			notice_lang(s_NickServ, u, NICK_DROP_DISABLED);
			return MOD_CONT;
		}

		if (!(na = (nick ? findnick(nick) : u->na)))
		{
			if (nick)
			{
				if ((nr = findrequestnick(nick)) && is_servadmin)
				{
					if (readonly)
						notice_lang(s_NickServ, u, READ_ONLY_MODE);
					if (WallDrop)
						ircdproto->SendGlobops(s_NickServ, "\2%s\2 used DROP on \2%s\2", u->nick, nick);
					alog("%s: %s!%s@%s dropped nickname %s (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nr->nick, nr->email);
					delnickrequest(nr);
					notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
				}
				else
					notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
			}
			else
				notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
			return MOD_CONT;
		}

		is_mine = u->na && u->na->nc == na->nc;
		if (is_mine && !nick)
			my_nick = sstrdup(na->nick);

		if (is_mine && !nick_identified(u))
			notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (!is_mine && !is_servadmin)
			notice_lang(s_NickServ, u, ACCESS_DENIED);
		else if (NSSecureAdmins && !is_mine && nick_is_services_admin(na->nc) && !is_services_root(u))
			notice_lang(s_NickServ, u, PERMISSION_DENIED);
		else
		{
			if (readonly)
				notice_lang(s_NickServ, u, READ_ONLY_MODE);

			if (ircd->sqline && (na->status & NS_FORBIDDEN))
				ircdproto->SendSQLineDel(na->nick);

			alog("%s: %s!%s@%s dropped nickname %s (group %s) (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, na->nick, na->nc->display, na->nc->email ? na->nc->email : "none");
			delnick(na);
			send_event(EVENT_NICK_DROPPED, 1, my_nick ? my_nick : nick);

			if (!is_mine)
			{
				if (WallDrop)
					ircdproto->SendGlobops(s_NickServ, "\2%s\2 used DROP on \2%s\2", u->nick, nick);
				notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
			}
			else
			{
				if (nick)
					notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
				else
					notice_lang(s_NickServ, u, NICK_DROPPED);
				if (my_nick)
					delete [] my_nick;
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (is_services_admin(u))
			notice_lang(s_NickServ, u, NICK_SERVADMIN_HELP_DROP);
		else
			notice_lang(s_NickServ, u, NICK_HELP_DROP);

		return true;
	}
};

class NSDrop : public Module
{
 public:
	NSDrop(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSDrop(), MOD_UNIQUE);

		this->SetNickHelp(myNickServHelp);
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User *u)
{
	notice_lang(s_NickServ, u, NICK_HELP_CMD_DROP);
}

MODULE_INIT("ns_drop", NSDrop)
