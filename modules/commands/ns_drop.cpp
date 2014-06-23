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
#include "modules/ns_drop.h"

class CommandNSDrop : public Command
{
	EventHandlers<Event::NickDrop> &onnickdrop;

 public:
	CommandNSDrop(Module *creator, EventHandlers<Event::NickDrop> &event) : Command(creator, "nickserv/drop", 1, 1), onnickdrop(event)
	{
		this->SetSyntax(_("\037nickname\037"));
		this->SetDesc(_("Cancel the registration of a nickname"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly && !source.HasPriv("nickserv/drop"))
		{
			source.Reply(_("Sorry, nickname de-registration is temporarily disabled."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		bool is_mine = source.GetAccount() == na->nc;

		if (!is_mine && !source.HasPriv("nickserv/drop"))
			source.Reply(_("Access denied. You do not have the correct operator privileges to drop other user's nicknames."));
		else if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && !is_mine && na->nc->IsServicesOper())
			source.Reply(_("You may not drop other Services Operators' nicknames."));
		else
		{
			this->onnickdrop(&Event::NickDrop::OnNickDrop, source, na);

			Log(!is_mine ? LOG_ADMIN : LOG_COMMAND, source, this) << "to drop nickname " << na->nick << " (group: " << na->nc->display << ") (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";
			delete na;

			source.Reply(_("\002{0}\002 has been dropped."), nick);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unregisters \037nickname\037. Once your nickname is dropped you may lose all of your access and channels that you may own. Any other user will be free to register \037nickname\037."));
		if (!source.HasPriv("nickserv/drop"))
			source.Reply(_("You may drop any nickname within your group."));
		else
			 source.Reply(_("As a Services Operator, you may drop any nick."));

		return true;
	}
};

class NSDrop : public Module
{
	CommandNSDrop commandnsdrop;
	EventHandlers<Event::NickDrop> onnickdrop;

 public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsdrop(this, onnickdrop)
		, onnickdrop(this, "OnNickDrop")
	{

	}
};

MODULE_INIT(NSDrop)
