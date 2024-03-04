/* HostServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandHSOn final
	: public Command
{
public:
	CommandHSOn(Module *creator) : Command(creator, "hostserv/on", 0, 0)
	{
		this->SetDesc(_("Activates your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSetVHost)
			return; // HostServ wouldn't even be loaded at this point

		User *u = source.GetUser();
		const NickAlias *na = NickAlias::Find(u->nick);
		if (!na || na->nc != u->Account() || !na->HasVhost())
			na = NickAlias::Find(u->Account()->display);
		if (na && u->Account() == na->nc && na->HasVhost())
		{
			source.Reply(_("Your vhost of \002%s\002 is now activated."), na->GetVhostMask().c_str());
			Log(LOG_COMMAND, source, this) << "to enable their vhost of " << na->GetVhostMask();
			IRCD->SendVhost(u, na->GetVhostIdent(), na->GetVhostHost());
			u->vhost = na->GetVhostHost();
			if (IRCD->CanSetVIdent && !na->GetVhostIdent().empty())
				u->SetVIdent(na->GetVhostIdent());
			u->UpdateHost();
		}
		else
			source.Reply(HOST_NOT_ASSIGNED);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Activates the vhost currently assigned to the nick in use.\n"
				"When you use this command any user who performs a /whois\n"
				"on you will see the vhost instead of your real host/IP address."));
		return true;
	}
};

class HSOn final
	: public Module
{
	CommandHSOn commandhson;

public:
	HSOn(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhson(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOn)
