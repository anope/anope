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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		User *u2;
		if (!(u2 = finduser(nick)))
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (!(na = findnick(u2->nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			source.Reply(NICK_NO_RECOVER_SELF);
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);

			if (res == 1)
			{
				u2->SendMessage(NickServ, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(na->nc, Config->NSReleaseTimeout);

				source.Reply(NICK_RECOVERED, Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
			{
				source.Reply(ACCESS_DENIED);
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
				u2->SendMessage(NickServ, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(na->nc, Config->NSReleaseTimeout);

				source.Reply(NICK_RECOVERED, Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
				source.Reply(ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(source.u->Account(), Config->NSReleaseTimeout);

		source.Reply(NICK_HELP_RECOVER, relstr.c_str());

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "RECOVER", NICK_RECOVER_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_RECOVER);
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
