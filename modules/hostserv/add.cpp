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

class CommandHSAdd : public Command
{
 public:
	CommandHSAdd(Module *creator) : Command(creator, "hostserv/add", 2, 2)
	{
		this->SetDesc(_("Adds a vhost to an account"));
		this->SetSyntax(_("\037account\037 \037hostmask\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &nick = params[0];

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		Anope::string rawhostmask = params[1];

		Anope::string user, host;
		size_t a = rawhostmask.find('@');

		if (a == Anope::string::npos)
		{
			host = rawhostmask;
		}
		else
		{
			user = rawhostmask.substr(0, a);
			host = rawhostmask.substr(a + 1);
		}

		if (host.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (!user.empty())
		{
			if (!IRCD->CanSetVIdent)
			{
				source.Reply(_("Vhosts may not contain a username."));
				return;
			}

			if (!IRCD->IsIdentValid(user))
			{
				source.Reply(_("The requested username is not valid."));
				return;
			}
		}

		if (host.length() > Config->GetBlock("networkinfo")->Get<unsigned>("hostlen"))
		{
			source.Reply(_("The requested vhost is too long, please use a hostname no longer than {0} characters."), Config->GetBlock("networkinfo")->Get<unsigned>("hostlen"));
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(_("The requested hostname is not valid."));
			return;
		}

		unsigned int max_vhosts = Config->GetModule("hostserv/main")->Get<unsigned int>("max_vhosts");
		if (max_vhosts && max_vhosts >= na->GetAccount()->GetRefs<HostServ::VHost *>().size())
		{
			source.Reply(_("\002{0}\002 already has the maximum number of vhosts allowed (\002{1}\002)."), na->GetAccount()->GetDisplay(), max_vhosts);
			return;
		}

		Anope::string mask = (!user.empty() ? user + "@" : "") + host;
		Log(LOG_ADMIN, source, this) << "to add the vhost " << mask << " to " << na->GetAccount()->GetDisplay();

		HostServ::VHost *vhost = Serialize::New<HostServ::VHost *>();
		if (vhost == nullptr)
		{
			source.Reply(_("Unable to create vhost, is hostserv enabled?"));
			return;
		}

		vhost->SetOwner(na->GetAccount());
		vhost->SetIdent(user);
		vhost->SetHost(host);
		vhost->SetCreator(source.GetNick());
		vhost->SetCreated(Anope::CurTime);

#warning "change event to broadcast account"
		EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, na);
		source.Reply(_("Vhost \002{0}\002 added to \002{1}\002."), mask, na->GetAccount()->GetDisplay());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Adds the vhost \037hostmask\037 to \037account\037."));
		return true;
	}
};

class HSAdd : public Module
{
	CommandHSAdd commandhsadd;

 public:
	HSAdd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsadd(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSAdd)
