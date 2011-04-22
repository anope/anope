/* HostServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "hostserv.h"

class CommandHSOn : public Command
{
 public:
	CommandHSOn() : Command("ON", 0, 0)
	{
		this->SetDesc(_("Activates your assigned vhost"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(u->nick);
		if (na && u->Account() == na->nc && na->hostinfo.HasVhost())
		{
			if (!na->hostinfo.GetIdent().empty())
				source.Reply(_("Your vhost of \002%s\002@\002%s\002 is now activated."), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
			else
				source.Reply(_("Your vhost of \002%s\002 is now activated."), na->hostinfo.GetHost().c_str());
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
			source.Reply(_(HOST_NOT_ASSIGNED));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002ON\002\n"
				"Activates the vhost currently assigned to the nick in use.\n"
				"When you use this command any user who performs a /whois\n"
				"on you will see the vhost instead of your real IP address."));
		return true;
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

		if (!hostserv)
			throw ModuleException("HostServ is not loaded!");

		this->AddCommand(hostserv->Bot(), &commandhson);
	}
};

MODULE_INIT(HSOn)
