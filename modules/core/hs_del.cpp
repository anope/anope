/* HostServ core functions
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

class CommandHSDel : public Command
{
 public:
	CommandHSDel() : Command("DEL", 1, 1, "hostserv/del")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na;
		Anope::string nick = params[0];
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				u->SendMessage(HostServ, NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}
			Log(LOG_ADMIN, u, this) << "for user " << na->nick;
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			na->hostinfo.RemoveVhost();
			u->SendMessage(HostServ, HOST_DEL, nick.c_str());
		}
		else
			u->SendMessage(HostServ, HOST_NOREG, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(HostServ, HOST_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(HostServ, u, "DEL", HOST_DEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(HostServ, HOST_HELP_CMD_DEL);
	}
};

class HSDel : public Module
{
	CommandHSDel commandhsdel;

 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsdel);
	}
};

MODULE_INIT(HSDel)
