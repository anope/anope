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

class CommandNSRelease : public Command
{
 public:
	CommandNSRelease() : Command("RELEASE", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *pass = params.size() > 1 ? params[1].c_str() : NULL;
		NickAlias *na;

		if (!(na = findnick(nick)))
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->flags & NI_SUSPENDED)
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!(na->status & NS_KILL_HELD))
			notice_lang(s_NickServ, u, NICK_RELEASE_NOT_HELD, nick);
		else if (pass)
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (res == 1)
			{
				release(na, 0);
				notice_lang(s_NickServ, u, NICK_RELEASED);
			}
			else
			{
				notice_lang(s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					alog("%s: RELEASE: invalid password for %s by %s!%s@%s", s_NickServ, nick, u->nick, u->GetIdent().c_str(), u->host);
					bad_password(u);
				}
			}
		}
		else
		{
			if (u->nc == na->nc || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc)))
			{
				release(na, 0);
				notice_lang(s_NickServ, u, NICK_RELEASED);
			}
			else
				notice_lang(s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		char relstr[192];

		/* Convert NSReleaseTimeout seconds to string format */
		duration(u->nc, relstr, sizeof(relstr), NSReleaseTimeout);

		notice_help(s_NickServ, u, NICK_HELP_RELEASE, relstr);
		//do_help_limited(s_NickServ, u, this);

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
	}
};

class NSRelease : public Module
{
 public:
	NSRelease(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSRelease());
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_RELEASE);
	}
};

MODULE_INIT("ns_release", NSRelease)
