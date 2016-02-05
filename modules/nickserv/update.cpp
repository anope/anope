/* NickServ core functions
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
#include "modules/nickserv/update.h"

class CommandNSUpdate : public Command
{
	EventHandlers<Event::NickUpdate> &onnickupdate;

 public:
	CommandNSUpdate(Module *creator, EventHandlers<Event::NickUpdate> &event) : Command(creator, "nickserv/update", 0, 0), onnickupdate(event)
	{
		this->SetDesc(_("Updates your current status, i.e. it checks for new memos"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(u->nick);

		if (na && na->GetAccount() == source.GetAccount())
		{
			na->SetLastRealname(u->realname);
			na->SetLastSeen(Anope::CurTime);
		}

		this->onnickupdate(&Event::NickUpdate::OnNickUpdate, u);

		source.Reply(_("Status updated (memos, vhost, chmodes, flags)."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Updates your current status, i.e. it checks for new memos,\n"
				"sets needed channel modes and updates your vhost and\n"
				"your userflags (lastseentime, etc)."));
		return true;
	}
};

class NSUpdate : public Module
{
	CommandNSUpdate commandnsupdate;
	EventHandlers<Event::NickUpdate> onnickupdate;

 public:
	NSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsupdate(this, onnickupdate)
		, onnickupdate(this)
	{

	}
};

MODULE_INIT(NSUpdate)
