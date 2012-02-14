/* HostServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSOff : public Command
{
 public:
	CommandHSOff(Module *creator) : Command(creator, "hostserv/off", 0, 0)
	{
		this->SetDesc(_("Deactivates your assigned vhost"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(u->nick);

		if (!na || !na->HasVhost())
			source.Reply(HOST_NOT_ASSIGNED);
		else
		{
			ircdproto->SendVhostDel(u);
			Log(LOG_COMMAND, u, this) << "to disable their vhost";
			source.Reply(_("Your vhost was removed and the normal cloaking restored."));
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deactivates the vhost currently assigned to the nick in use.\n"
				"When you use this command any user who performs a /whois\n"
				"on you will see your real IP address."));
		return true;
	}
};

class HSOff : public Module
{
	CommandHSOff commandhsoff;

 public:
	HSOff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandhsoff(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(HSOff)
