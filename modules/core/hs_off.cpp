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

class CommandHSOff : public Command
{
 public:
	CommandHSOff() : Command("OFF", 0, 0)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(u->nick);

		if (!na || !na->hostinfo.HasVhost())
			source.Reply(HOST_NOT_ASSIGNED);
		else
		{
			ircdproto->SendVhostDel(u);
			Log(LOG_COMMAND, u, this) << "to disable their vhost";
			source.Reply(HOST_OFF);
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(HOST_HELP_OFF);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(HOST_HELP_CMD_OFF);
	}
};

class HSOff : public Module
{
	CommandHSOff commandhsoff;

 public:
	HSOff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsoff);
	}
};

MODULE_INIT(HSOff)
