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

class CommandNSGhost : public Command
{
 public:
	CommandNSGhost() : Command("GHOST", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";
		User *user = finduser(nick);
		NickAlias *na = findnick(nick);

		if (!user)
			u->SendMessage(NickServ, NICK_X_NOT_IN_USE, nick.c_str());
		else if (!na)
			u->SendMessage(NickServ, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			u->SendMessage(NickServ, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			u->SendMessage(NickServ, NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			u->SendMessage(NickServ, NICK_NO_GHOST_SELF);
		else if ((u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc))) ||
				(!pass.empty() && enc_check_password(pass, na->nc->pass) == 1))
		{
			if (!user->IsIdentified() && FindCommand(NickServ, "RECOVER"))
				u->SendMessage(NickServ, NICK_GHOST_UNIDENTIFIED);
			else
			{
				Log(LOG_COMMAND, u, this) << "for " << nick;
				Anope::string buf = "GHOST command used by " + u->nick;
				kill_user(Config->s_NickServ, nick, buf);
				u->SendMessage(NickServ, NICK_GHOST_KILLED, nick.c_str());
			}
		}
		else
		{
			u->SendMessage(NickServ, ACCESS_DENIED);
			if (!pass.empty())
			{
				Log(LOG_COMMAND, u, this) << "with an invalid password for " << nick;
				if (bad_password(u))
					return MOD_STOP;
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(NickServ, NICK_HELP_GHOST);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_GHOST);
	}
};

class NSGhost : public Module
{
	CommandNSGhost commandnsghost;

 public:
	NSGhost(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsghost);
	}
};

MODULE_INIT(NSGhost)
