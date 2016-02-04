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

class CommandHSOff : public Command
{
 public:
	CommandHSOff(Module *creator) : Command(creator, "hostserv/off", 0, 0)
	{
		this->SetDesc(_("Deactivates your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(u->nick);

		if (!na || !na->HasVhost() || na->GetAccount() != source.GetAccount())
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		u->vhost.clear();
		IRCD->SendVhostDel(u);
		Log(LOG_COMMAND, source, this) << "to disable their vhost";
		source.Reply(_("Your vhost was removed and the normal cloaking restored."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Deactivates your vhost."));
		return true;
	}
};

class HSOff : public Module
{
	CommandHSOff commandhsoff;

 public:
	HSOff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsoff(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOff)
