/* HostServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSGroup : public Command
{
	void Sync(const NickAlias *na)
	{
		if (!na || !na->HasVhost())
			return;
	
		for (unsigned i = 0; i < na->nc->aliases->size(); ++i)
		{
			NickAlias *nick = na->nc->aliases->at(i);
			if (nick)
				nick->SetVhost(na->GetVhostIdent(), na->GetVhostHost(), na->GetVhostCreator());
		}
	}

 public:
	CommandHSGroup(Module *creator) : Command(creator, "hostserv/group", 0, 0)
	{
		this->SetDesc(_("Syncs the vhost for all nicks in a group"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickAlias *na = NickAlias::Find(source.GetNick());
		if (na && source.GetAccount() == na->nc && na->HasVhost())
		{
			this->Sync(na);
			if (!na->GetVhostIdent().empty())
				source.Reply(_("All vhost's in the group \002%s\002 have been set to \002%s\002@\002%s\002."), source.nc->display.c_str(), na->GetVhostIdent().c_str(), na->GetVhostHost().c_str());
			else
				source.Reply(_("All vhost's in the group \002%s\002 have been set to \002%s\002."), source.nc->display.c_str(), na->GetVhostHost().c_str());
		}
		else
			source.Reply(HOST_NOT_ASSIGNED);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command allows users to set the vhost of their\n"
				"CURRENT nick to be the vhost for all nicks in the same\n"
				"group."));
		return true;
	}
};

class HSGroup : public Module
{
	CommandHSGroup commandhsgroup;

 public:
	HSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhsgroup(this)
	{

	}
};

MODULE_INIT(HSGroup)
