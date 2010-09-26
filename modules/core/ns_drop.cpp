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
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = !params.empty() ? params[0] : "";
		NickAlias *na;
		NickRequest *nr = NULL;
		int is_mine; /* Does the nick being dropped belong to the user that is dropping? */
		Anope::string my_nick;

		if (readonly)
		{
			u->SendMessage(NickServ, NICK_DROP_DISABLED);
			return MOD_CONT;
		}

		if (!(na = findnick(!nick.empty() ? nick : u->nick)))
		{
			if (!nick.empty())
			{
				if ((nr = findrequestnick(nick)) && u->Account()->IsServicesOper())
				{
					if (Config->WallDrop)
						ircdproto->SendGlobops(NickServ, "\2%s\2 used DROP on \2%s\2", u->nick.c_str(), nick.c_str());
					Log(LOG_ADMIN, u, this) << "to drop nickname " << nr->nick << " (email: " << nr->email << ")";
					delete nr;
					u->SendMessage(NickServ, NICK_X_DROPPED, nick.c_str());
				}
				else
					u->SendMessage(NickServ, NICK_X_NOT_REGISTERED, nick.c_str());
			}
			else
				u->SendMessage(NickServ, NICK_NOT_REGISTERED);
			return MOD_CONT;
		}

		is_mine = u->Account() && u->Account() == na->nc;
		if (is_mine && nick.empty())
			my_nick = na->nick;

		if (!is_mine && !u->Account()->HasPriv("nickserv/drop"))
			u->SendMessage(NickServ, ACCESS_DENIED);
		else if (Config->NSSecureAdmins && !is_mine && na->nc->IsServicesOper())
			u->SendMessage(NickServ, ACCESS_DENIED);
		else
		{
			if (readonly)
				u->SendMessage(NickServ, READ_ONLY_MODE);

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
				u->SendMessage(NickServ, NICK_X_DROPPED, nick.c_str());
			}
			else
			{
				if (!nick.empty())
					u->SendMessage(NickServ, NICK_X_DROPPED, nick.c_str());
				else
					u->SendMessage(NickServ, NICK_DROPPED);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->HasPriv("nickserv/drop"))
			u->SendMessage(NickServ, NICK_SERVADMIN_HELP_DROP);
		else
			u->SendMessage(NickServ, NICK_HELP_DROP);

		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_DROP);
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
