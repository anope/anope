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
#include "modules/hostserv/del.h"

class CommandHSDel : public Command
{
 public:
	CommandHSDel(Module *creator) : Command(creator, "hostserv/del", 1, 2)
	{
		this->SetDesc(_("Delete the vhost of a user"));
		this->SetSyntax(_("\037user\037 [\037vhost\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		const Anope::string &nick = params[0];
		const Anope::string &host = params.size() > 1 ? params[1] : "";

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		if (!host.empty())
		{
			HostServ::VHost *vhost = HostServ::FindVHost(na->GetAccount(), host);

			if (vhost == nullptr)
			{
				source.Reply(_("\002{0}\002 doesn't have vhost \002{1}\002."), na->GetAccount()->GetDisplay(), host);
				return;
			}

			Log(LOG_ADMIN, source, this) << "on " << na->GetAccount()->GetDisplay() << " to remove vhost " << vhost->Mask();
			source.Reply(_("Vhost \002{0}\002 for \002{1}\002 has been removed."), vhost->Mask(), na->GetAccount()->GetDisplay());
			vhost->Delete();
			return;
		}

		std::vector<HostServ::VHost *> vhosts = na->GetAccount()->GetRefs<HostServ::VHost *>();
		if (vhosts.empty())
		{
			source.Reply(_("\002{0}\002 doesn't have a vhost."), na->GetAccount()->GetDisplay());
			return;
		}

		Log(LOG_ADMIN, source, this) << "on " << na->GetAccount()->GetDisplay();
#warning "send account"
		EventManager::Get()->Dispatch(&Event::DeleteVhost::OnDeleteVhost, na);

		for (HostServ::VHost *v : vhosts)
			v->Delete();

		source.Reply(_("Vhost(s) for \002{0}\002 has been removed."), na->GetAccount()->GetDisplay());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Removes the vhost of \037user\037."));
		return true;
	}
};

class HSDel : public Module
{
	CommandHSDel commandhsdel;

 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsdel(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSDel)
