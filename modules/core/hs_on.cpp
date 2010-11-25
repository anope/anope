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

class CommandHSOn : public Command
{
 public:
	CommandHSOn() : Command("ON", 0, 0)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(u->nick);
		if (na && u->Account() == na->nc && na->hostinfo.HasVhost())
		{
			if (!na->hostinfo.GetIdent().empty())
				source.Reply(HOST_IDENT_ACTIVATED, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				source.Reply(HOST_ACTIVATED, na->hostinfo.GetHost().c_str());
			Log(LOG_COMMAND, u, this) << "to enable their vhost of " << (!na->hostinfo.GetIdent().empty() ? na->hostinfo.GetIdent() + "@" : "") << na->hostinfo.GetHost();
			ircdproto->SendVhost(u, na->hostinfo.GetIdent(), na->hostinfo.GetHost());
			if (ircd->vhost)
				u->vhost = na->hostinfo.GetHost();
			if (ircd->vident)
			{
				if (!na->hostinfo.GetIdent().empty())
					u->SetVIdent(na->hostinfo.GetIdent());
			}
			u->UpdateHost();
		}
		else
			source.Reply(HOST_NOT_ASSIGNED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(HostServ, HOST_HELP_ON);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(HostServ, HOST_HELP_CMD_ON);
	}
};

class HSOn : public Module
{
	CommandHSOn commandhson;

 public:
	HSOn(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhson);
	}
};

MODULE_INIT(HSOn)
