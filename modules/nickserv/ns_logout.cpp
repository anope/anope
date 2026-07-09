// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/nickserv/service.h"

class CommandNSLogout final
	: public Command
{
public:
	CommandNSLogout(Module *creator) : Command(creator, "nickserv/logout", 0, 2)
	{
		this->SetDesc(_("Reverse the effect of the IDENTIFY command"));
		this->SetSyntax(_("[\037nickname\037 [REVALIDATE]]"), [](auto &source) { return source.IsServicesOper(); });
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = !params.empty() ? params[0] : "";
		const Anope::string &param = params.size() > 1 ? params[1] : "";

		User *u2;
		if (!source.IsServicesOper() && !nick.empty())
			this->OnSyntaxError(source, "");
		else if (!(u2 = (!nick.empty() ? User::Find(nick, true) : source.GetUser())))
			source.Reply(NICK_X_NOT_IN_USE, !nick.empty() ? nick.c_str() : source.GetNick().c_str());
		else if (!nick.empty() && u2->IsServicesOper())
			source.Reply(_("You can't logout %s, they are a Services Operator."), nick.c_str());
		else
		{
			if (!nick.empty() && !param.empty() && param.equals_ci("REVALIDATE") && NickServ::service)
				NickServ::service->Validate(u2);

			u2->super_admin = false; /* Don't let people logout and remain a SuperAdmin */
			Log(LOG_COMMAND, source, this) << "to logout " << u2->nick;

			/* Remove founder status from this user in all channels */
			if (!nick.empty())
				source.Reply(_("Nick %s has been logged out."), nick.c_str());
			else
				source.Reply(_("Your nick has been logged out."));

			IRCD->SendLogout(u2);
			u2->RemoveMode(source.service, "REGISTERED");
			u2->Logout();

			/* Send out an event */
			FOREACH_MOD(OnNickLogout, (u2));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");

		if (source.IsServicesOper())
		{
			source.Reply(_(
				"Without a parameter, reverses the effect of the \002IDENTIFY\002 "
				"command, i.e. make you not recognized as the real owner of the nick "
				"anymore. Note, however, that you won't be asked to reidentify "
				"yourself."
				"\n\n"
				"With a parameter, does the same for the given nick. If you "
				"specify \002REVALIDATE\002 as well, services will ask the given nick "
				"to re-identify. This is limited to \002Services Operators\002."
			));
		}
		else
		{
			source.Reply(_(
				"Reverses the effect of the \002IDENTIFY\002 command, i.e. make you not "
				"recognized as the real owner of the nick anymore. Note, however, that "
				"you won't be asked to reidentify yourself."
			));
		}
		return true;
	}
};

class NSLogout final
	: public Module
{
	CommandNSLogout commandnslogout;

public:
	NSLogout(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnslogout(this)
	{

	}
};

MODULE_INIT(NSLogout)
