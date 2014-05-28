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
#include "modules/hs_del.h"

class CommandHSDel : public Command
{
	EventHandlers<Event::DeleteVhost> &OnDeleteVhost;

 public:
	CommandHSDel(Module *creator, EventHandlers<Event::DeleteVhost> &onDeleteVhost) : Command(creator, "hostserv/del", 1, 1), OnDeleteVhost(onDeleteVhost)
	{
		this->SetDesc(_("Delete the vhost of another user"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na)
		{
			Log(LOG_ADMIN, source, this) << "for user " << na->nick;
			this->OnDeleteVhost(&Event::DeleteVhost::OnDeleteVhost, na);
			na->RemoveVhost();
			source.Reply(_("Vhost for \002%s\002 removed."), nick.c_str());
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deletes the vhost assigned to the given nick from the\n"
				"database."));
		return true;
	}
};

class CommandHSDelAll : public Command
{
	EventHandlers<Event::DeleteVhost> &ondeletevhost;

 public:
	CommandHSDelAll(Module *creator, EventHandlers<Event::DeleteVhost> &event) : Command(creator, "hostserv/delall", 1, 1), ondeletevhost(event)
	{
		this->SetDesc(_("Delete the vhost for all nicks in a group"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na)
		{
			this->ondeletevhost(&Event::DeleteVhost::OnDeleteVhost, na);
			const NickServ::Account *nc = na->nc;
			for (unsigned i = 0; i < nc->aliases->size(); ++i)
			{
				na = nc->aliases->at(i);
				na->RemoveVhost();
			}
			Log(LOG_ADMIN, source, this) << "for all nicks in group " << nc->display;
			source.Reply(_("vhosts for group \002%s\002 have been removed."), nc->display.c_str());
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deletes the vhost for all nicks in the same group as\n"
				"that of the given nick."));
		return true;
	}
};

class HSDel : public Module
{
	CommandHSDel commandhsdel;
	CommandHSDelAll commandhsdelall;
	EventHandlers<Event::DeleteVhost> ondeletevhost;

 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsdel(this, ondeletevhost)
		, commandhsdelall(this, ondeletevhost)
		, ondeletevhost(this, "OnDeleteVhost")
	{

	}
};

MODULE_INIT(HSDel)
