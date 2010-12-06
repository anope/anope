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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		User *user = finduser(nick);
		NickAlias *na = findnick(nick);

		if (!user)
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			source.Reply(NICK_NO_GHOST_SELF);
		else if ((u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc))) ||
				(!pass.empty() && enc_check_password(pass, na->nc->pass) == 1))
		{
			if (!user->IsIdentified() && FindCommand(NickServ, "RECOVER"))
				source.Reply(NICK_GHOST_UNIDENTIFIED);
			else
			{
				Log(LOG_COMMAND, u, this) << "for " << nick;
				Anope::string buf = "GHOST command used by " + u->nick;
				kill_user(Config->s_NickServ, user, buf);
				source.Reply(NICK_GHOST_KILLED, nick.c_str());
			}
		}
		else
		{
			source.Reply(ACCESS_DENIED);
			if (!pass.empty())
			{
				Log(LOG_COMMAND, u, this) << "with an invalid password for " << nick;
				if (bad_password(u))
					return MOD_STOP;
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(NICK_HELP_GHOST);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GHOST", NICK_GHOST_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_GHOST);
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
