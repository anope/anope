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
#include "modules/nickserv.h"

class CommandNSAccess : public Command
{
 private:
	void DoAdd(CommandSource &source, NickServ::Account *nc, const Anope::string &mask)
	{
		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (nc->access.size() >= Config->GetModule(this->owner)->Get<unsigned>("accessmax", "32"))
		{
			source.Reply(_("Sorry, the maximum of \002{0}\002 access entries has been reached."), Config->GetModule(this->owner)->Get<unsigned>("accessmax"));
			return;
		}

		if (nc->FindAccess(mask))
		{
			source.Reply(_("Mask \002{0}\002 already present on the access list of \002{1}\002."), mask, nc->display);
			return;
		}

		nc->AddAccess(mask);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD mask " << mask << " to " << nc->display;
		source.Reply(_("\002{0}\002 added to the access list of \002{1}\002."), mask, nc->display);
	}

	void DoDel(CommandSource &source, NickServ::Account *nc, const Anope::string &mask)
	{
		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (!nc->FindAccess(mask))
		{
			source.Reply(_("\002{0}\002 not found on the access list of \002{1}\002."), mask, nc->display);
			return;
		}

		nc->EraseAccess(mask);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE mask " << mask << " from " << nc->display;
		source.Reply(_("\002{0}\002 deleted from the access list of \002{1}\002."), mask, nc->display);
	}

	void DoList(CommandSource &source, NickServ::Account *nc, const Anope::string &mask)
	{
		unsigned i, end;

		if (nc->access.empty())
		{
			source.Reply(_("The access list of \002{0}\002 is empty."), nc->display);
			return;
		}

		source.Reply(_("Access list for \002{0}\002:"), nc->display);
		for (i = 0, end = nc->access.size(); i < end; ++i)
		{
			Anope::string access = nc->GetAccess(i);
			if (!mask.empty() && !Anope::Match(access, mask))
				continue;
			source.Reply("    {0}", access);
		}
	}
 public:
	CommandNSAccess(Module *creator) : Command(creator, "nickserv/access", 1, 3)
	{
		this->SetDesc(_("Modify the list of authorized addresses"));
		this->SetSyntax(_("ADD [\037nickname\037] \037mask\037"));
		this->SetSyntax(_("DEL [\037nickname\037] \037mask\037"));
		this->SetSyntax(_("LIST [\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		Anope::string nick, mask;

		if (cmd.equals_ci("LIST"))
			nick = params.size() > 1 ? params[1] : "";
		else
		{
			nick = params.size() == 3 ? params[1] : "";
			mask = params.size() > 1 ? params[params.size() - 1] : "";
		}

		NickServ::Account *nc;
		if (!nick.empty() && source.HasPriv("nickserv/access"))
		{
			const NickServ::Nick *na = NickServ::FindNick(nick);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}

			if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.GetAccount() != na->nc && na->nc->IsServicesOper() && !cmd.equals_ci("LIST"))
			{
				source.Reply(_("You may view but not modify the access list of other Services Operators."));
				return;
			}

			nc = na->nc;
		}
		else
			nc = source.nc;

		if (!mask.empty() && (mask.find('@') == Anope::string::npos || mask.find('!') != Anope::string::npos))
		{
			source.Reply(_("Mask must be in the form \037user\037@\037host\037."));
			source.Reply(_("\002%s%s HELP %s\002 for more information."), Config->StrictPrivmsg, source.service->nick, source.command); // XXX
		}
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc, mask);
		else if (nc->HasExt("NS_SUSPENDED"))
			source.Reply(_("\002{0}\002 is suspended."), nc->display);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, mask);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Modifies or displays the access list for your account."
		               " The access list is a list of addresses that {1} uses to recognize you."
		               " If you match one of the hosts on the access list, services will not force you to change your nickname if the \002KILL\002 option is set."
		               " Furthermore, if the \002SECURE\002 option is disabled, services will recognize you just based on your hostmask, without having to supply a password."
		               " To gain access to channels when only recognized by your hostmask, the channel must too have the \002SECURE\002 option off."
		               " Services Operators may provide \037nickname\037 to modify other user's access lists.\n"
		               "\n"
		               "Examples:\n"
		               " \n"
		               "         {command} ADD anyone@*.bepeg.com\n"
		               "         Allows access to user \"anyone\" from any machine in the \"bepeg.com\" domain.\n"
		               "\n"
		               "         {command} DEL anyone@*.bepeg.com\n"
		               "         Reverses the previous command.\n"
		               "\n"
		               "         {command} LIST\n"
		               "         Displays the current access list."),
		               source.command, source.service->nick);
		return true;
	}
};

class NSAccess : public Module
	, public EventHook<NickServ::Event::NickRegister>
{
	CommandNSAccess commandnsaccess;

 public:
	NSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<NickServ::Event::NickRegister>("OnNickRegister")
		, commandnsaccess(this)
	{
	}

	void OnNickRegister(User *u, NickServ::Nick *na, const Anope::string &) override
	{
		if (u && Config->GetModule(this)->Get<bool>("addaccessonreg"))
			na->nc->AddAccess(u->Mask());
	}
};

MODULE_INIT(NSAccess)
