/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/nickserv.h"
#include "modules/nickserv/access.h"

class NickAccessImpl : public NickAccess
{
	friend class NickAccessType;

	NickServ::Account *account = nullptr;
	Anope::string mask;

 public:
	NickAccessImpl(Serialize::TypeBase *type) : NickAccess(type) { }
	NickAccessImpl(Serialize::TypeBase *type, Serialize::ID id) : NickAccess(type, id) { }

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &) override;
};

class NickAccessType : public Serialize::Type<NickAccessImpl>
{
 public:
	Serialize::ObjectField<NickAccessImpl, NickServ::Account *> account;
	Serialize::Field<NickAccessImpl, Anope::string> mask;

	NickAccessType(Module *creator) : Serialize::Type<NickAccessImpl>(creator)
		, account(this, "account", &NickAccessImpl::account, true)
		, mask(this, "mask", &NickAccessImpl::mask)
	{
	}
};

NickServ::Account *NickAccessImpl::GetAccount()
{
	return Get(&NickAccessType::account);
}

void NickAccessImpl::SetAccount(NickServ::Account *acc)
{
	Set(&NickAccessType::account, acc);
}

Anope::string NickAccessImpl::GetMask()
{
	return Get(&NickAccessType::mask);
}

void NickAccessImpl::SetMask(const Anope::string &m)
{
	Set(&NickAccessType::mask, m);
}

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

		std::vector<NickAccess *> access = nc->GetRefs<NickAccess *>();

		if (access.size() >= Config->GetModule(this->GetOwner())->Get<unsigned>("accessmax", "32"))
		{
			source.Reply(_("Sorry, the maximum of \002{0}\002 access entries has been reached."), Config->GetModule(this->GetOwner())->Get<unsigned>("accessmax"));
			return;
		}

		for (NickAccess *a : access)
			if (a->GetMask().equals_ci(mask))
			{
				source.Reply(_("Mask \002{0}\002 already present on the access list of \002{1}\002."), mask, nc->GetDisplay());
				return;
			}

		NickAccess *a = Serialize::New<NickAccess *>();
		a->SetAccount(nc);
		a->SetMask(mask);

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD mask " << mask << " to " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 added to the access list of \002{1}\002."), mask, nc->GetDisplay());
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

		for (NickAccess *a : nc->GetRefs<NickAccess *>())
			if (a->GetMask().equals_ci(mask))
			{
				a->Delete();
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE mask " << mask << " from " << nc->GetDisplay();
				source.Reply(_("\002{0}\002 deleted from the access list of \002{1}\002."), mask, nc->GetDisplay());
				return;
			}


		source.Reply(_("\002{0}\002 not found on the access list of \002{1}\002."), mask, nc->GetDisplay());
	}

	void DoList(CommandSource &source, NickServ::Account *nc, const Anope::string &mask)
	{
		std::vector<NickAccess *> access = nc->GetRefs<NickAccess *>();
		if (access.empty())
		{
			source.Reply(_("The access list of \002{0}\002 is empty."), nc->GetDisplay());
			return;
		}

		source.Reply(_("Access list for \002{0}\002:"), nc->GetDisplay());
		for (NickAccess *a : access)
		{
			if (!mask.empty() && !Anope::Match(a->GetMask(), mask))
				continue;

			source.Reply("    {0}", a->GetMask());
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
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}

			if (Config->GetModule("nickserv/main")->Get<bool>("secureadmins", "yes") && source.GetAccount() != na->GetAccount() && na->GetAccount()->IsServicesOper() && !cmd.equals_ci("LIST"))
			{
				source.Reply(_("You may view but not modify the access list of other Services Operators."));
				return;
			}

			nc = na->GetAccount();
		}
		else
		{
			nc = source.nc;
		}

		if (!mask.empty() && (mask.find('@') == Anope::string::npos || mask.find('!') != Anope::string::npos))
		{
			source.Reply(_("Mask must be in the form \037user\037@\037host\037."));
			source.Reply(_("\002%s%s HELP %s\002 for more information."), Config->StrictPrivmsg, source.service->nick, source.command); // XXX
			return;
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc, mask);
		else if (nc->HasFieldS("NS_SUSPENDED"))
			source.Reply(_("\002{0}\002 is suspended."), nc->GetDisplay());
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
	NickAccessType nick_type;

 public:
	NSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<NickServ::Event::NickRegister>(this)
		, commandnsaccess(this)
		, nick_type(this)
	{
	}

	void OnNickRegister(User *u, NickServ::Nick *na, const Anope::string &) override
	{
		if (u && Config->GetModule(this)->Get<bool>("addaccessonreg"))
		{
			NickAccess *a = Serialize::New<NickAccess *>();
			a->SetAccount(na->GetAccount());
			a->SetMask(u->Mask());

			u->SendMessage(Config->GetClient("NickServ"),
					_("\002{0}\002 has been registered under your hostmask: \002{1}\002"), na->GetNick(), a->GetMask());
		}
	}
};

MODULE_INIT(NSAccess)
