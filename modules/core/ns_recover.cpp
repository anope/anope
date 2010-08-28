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

class CommandNSRecover : public Command
{
 public:
	CommandNSRecover() : Command("RECOVER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";
		NickAlias *na;
		User *u2;

		if (!(u2 = finduser(nick)))
			notice_lang(Config->s_NickServ, u, NICK_X_NOT_IN_USE, nick.c_str());
		else if (!(na = findnick(u2->nick)))
			notice_lang(Config->s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config->s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config->s_NickServ, u, NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			notice_lang(Config->s_NickServ, u, NICK_NO_RECOVER_SELF);
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);

			if (res == 1)
			{
				notice_lang(Config->s_NickServ, u2, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(na->nc, Config->NSReleaseTimeout);

				notice_lang(Config->s_NickServ, u, NICK_RECOVERED, Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
			{
				notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					Log(LOG_COMMAND, u, this) << "with invalid password for " << nick;
					if (bad_password(u))
						return MOD_STOP;
				}
			}
		}
		else
		{
			if (u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc)))
			{
				notice_lang(Config->s_NickServ, u2, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(na->nc, Config->NSReleaseTimeout);

				notice_lang(Config->s_NickServ, u, NICK_RECOVERED, Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
				notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(u->Account(), Config->NSReleaseTimeout);

		notice_help(Config->s_NickServ, u, NICK_HELP_RECOVER, relstr.c_str());

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_NickServ, u, "RECOVER", NICK_RECOVER_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_RECOVER);
	}
};

class NSRecover : public Module
{
	CommandNSRecover commandnsrecover;

 public:
	NSRecover(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsrecover);
	}
};

MODULE_INIT(NSRecover)
