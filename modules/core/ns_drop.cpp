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

class CommandNSDrop : public Command
{
 public:
	CommandNSDrop() : Command("DROP", 0, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string nick = !params.empty() ? params[0] : "";

		if (readonly)
		{
			source.Reply(NICK_DROP_DISABLED);
			return MOD_CONT;
		}

		NickAlias *na = findnick((u->Account() && !nick.empty() ? nick : u->nick));
		if (!na)
		{
			NickRequest *nr = findrequestnick(u->Account() && !nick.empty() ? nick : u->nick);
			if (nr && u->Account() && u->Account()->IsServicesOper())
			{
				if (Config->WallDrop)
					ircdproto->SendGlobops(NickServ, "\2%s\2 used DROP on \2%s\2", u->nick.c_str(), nick.c_str());

				Log(LOG_ADMIN, u, this) << "to drop nickname " << nr->nick << " (email: " << nr->email << ")";
				delete nr;
				source.Reply(NICK_X_DROPPED, nick.c_str());
			}
			else if (nr && !nick.empty())
			{
				int res = enc_check_password(nick, nr->password);
				if (res)
				{
					Log(LOG_COMMAND, u, this) << "to drop nick request " << nr->nick;
					source.Reply(NICK_X_DROPPED, nr->nick.c_str());
					delete nr;
				}
				else if (bad_password(u))
					return MOD_STOP;
				else
					source.Reply(PASSWORD_INCORRECT);
			}
			else
				source.Reply(NICK_NOT_REGISTERED);

			return MOD_CONT;
		}

		if (!u->Account())
		{
			source.Reply(NICK_IDENTIFY_REQUIRED, Config->s_NickServ.c_str());
			return MOD_CONT;
		}

		bool is_mine = u->Account() == na->nc;
		Anope::string my_nick;
		if (is_mine && nick.empty())
			my_nick = na->nick;

		if (!is_mine && !u->Account()->HasPriv("nickserv/drop"))
			source.Reply(ACCESS_DENIED);
		else if (Config->NSSecureAdmins && !is_mine && na->nc->IsServicesOper())
			source.Reply(ACCESS_DENIED);
		else
		{
			if (readonly)
				source.Reply(READ_ONLY_MODE);

			if (ircd->sqline && na->HasFlag(NS_FORBIDDEN))
			{
				XLine x(na->nick);
				ircdproto->SendSQLineDel(&x);
			}

			Log(!is_mine ? LOG_OVERRIDE : LOG_COMMAND, u, this) << "to drop nickname " << na->nick << " (group: " << na->nc->display << ") (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";
			delete na;

			FOREACH_MOD(I_OnNickDrop, OnNickDrop(!my_nick.empty() ? my_nick : nick));

			if (!is_mine)
			{
				if (Config->WallDrop)
					ircdproto->SendGlobops(NickServ, "\2%s\2 used DROP on \2%s\2", u->nick.c_str(), nick.c_str());
				source.Reply(NICK_X_DROPPED, nick.c_str());
			}
			else
			{
				if (!nick.empty())
					source.Reply(NICK_X_DROPPED, nick.c_str());
				else
					source.Reply(NICK_DROPPED);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->HasPriv("nickserv/drop"))
			source.Reply(NICK_SERVADMIN_HELP_DROP);
		else
			source.Reply(NICK_HELP_DROP);

		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_DROP);
	}
};

class NSDrop : public Module
{
	CommandNSDrop commandnsdrop;

 public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsdrop);
	}
};

MODULE_INIT(NSDrop)
