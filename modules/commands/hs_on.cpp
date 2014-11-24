/* HostServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandHSOn : public Command
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
		NickServ::Nick *na = NickServ::FindNick(u->nick);

		if (!na || !na->HasVhost() || na->GetAccount() != u->Account())
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		if (!na->GetVhostIdent().empty())
			source.Reply(_("Your vhost of \002{0}\002@\002{1}\002 is now activated."), na->GetVhostIdent(), na->GetVhostHost());
		else
			source.Reply(_("Your vhost of \002{0}\002 is now activated."), na->GetVhostHost());
		
		Log(LOG_COMMAND, source, this) << "to enable their vhost of " << (!na->GetVhostIdent().empty() ? na->GetVhostIdent() + "@" : "") << na->GetVhostHost();
		IRCD->SendVhost(u, na->GetVhostIdent(), na->GetVhostHost());
		u->vhost = na->GetVhostHost();
		if (IRCD->CanSetVIdent && !na->GetVhostIdent().empty())
			u->SetVIdent(na->GetVhostIdent());
		u->UpdateHost();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Activates your vhost."));
		return true;
	}
};

class HSOn : public Module
{
	CommandHSOn commandhson;

 public:
	HSOn(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhson(this)
	{

	}
};

MODULE_INIT(HSOn)
