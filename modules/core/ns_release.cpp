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

class CommandNSRelease : public Command
{
 public:
	CommandNSRelease() : Command("RELEASE", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";
		NickAlias *na;

		if (!(na = findnick(nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick.c_str());
		else if (!na->HasFlag(NS_HELD))
			notice_lang(Config.s_NickServ, u, NICK_RELEASE_NOT_HELD, nick.c_str());
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (res == 1)
			{
				na->Release();
				notice_lang(Config.s_NickServ, u, NICK_RELEASED);
			}
			else
			{
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					Alog() << Config.s_NickServ << ": RELEASE: invalid password for " << nick << " by " << u->GetMask();
					if (bad_password(u))
						return MOD_STOP;
				}
			}
		}
		else
		{
			if (u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc)))
			{
				na->Release();
				notice_lang(Config.s_NickServ, u, NICK_RELEASED);
			}
			else
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		/* Convert Config.NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(u->Account(), Config.NSReleaseTimeout);

		notice_help(Config.s_NickServ, u, NICK_HELP_RELEASE, relstr.c_str());
		//do_help_limited(Config.s_NickServ, u, this);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RELEASE);
	}
};

class NSRelease : public Module
{
	CommandNSRelease commandnsrelease;

 public:
	NSRelease(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsrelease);
	}
};

MODULE_INIT(NSRelease)
