/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "modules/help.h"
#include "modules/nickserv/update.h"
#include "modules/nickserv/info.h"
#include "modules/hostserv/del.h"
#include "vhosttype.h"

class HostServCore : public Module
	, public EventHook<Event::UserLogin>
	, public EventHook<Event::NickUpdate>
	, public EventHook<Event::Help>
	, public EventHook<Event::SetVhost>
	, public EventHook<Event::DeleteVhost>
	, public EventHook<Event::NickInfo>
{
	Reference<ServiceBot> HostServ;

	VHostType vhost_type;

 public:
	HostServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, EventHook<Event::UserLogin>(this)
		, EventHook<Event::NickUpdate>(this)
		, EventHook<Event::Help>(this)
		, EventHook<Event::SetVhost>(this)
		, EventHook<Event::DeleteVhost>(this)
		, EventHook<Event::NickInfo>(this)
		, vhost_type(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &hsnick = conf->GetModule(this)->Get<Anope::string>("client");

		if (hsnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(hsnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + hsnick);

		HostServ = bi;
	}

	void OnUserLogin(User *u) override
	{
		if (!IRCD->CanSetVHost)
			return;

		HostServ::VHost *vhost = HostServ::FindVHost(u->Account());

		if (vhost == nullptr)
			return;

		if (u->vhost.empty() || !u->vhost.equals_cs(vhost->GetHost()) || (!vhost->GetIdent().empty() && !u->GetVIdent().equals_cs(vhost->GetIdent())))
		{
			IRCD->Send<messages::VhostSet>(u, vhost->GetIdent(), vhost->GetHost());

			u->vhost = vhost->GetHost();
			u->UpdateHost();

			if (IRCD->CanSetVIdent && !vhost->GetIdent().empty())
				u->SetVIdent(vhost->GetIdent());

			if (HostServ)
			{
				if (!vhost->GetIdent().empty())
					u->SendMessage(*HostServ, _("Your vhost of \002{0}\002@\002{1}\002 is now activated."), vhost->GetIdent(), vhost->GetHost());
				else
					u->SendMessage(*HostServ, _("Your vhost of \002{0}\002 is now activated."), vhost->GetHost());
			}
		}
	}

	void OnNickUpdate(User *u) override
	{
		this->OnUserLogin(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *HostServ)
			return EVENT_CONTINUE;
		source.Reply(_("{0} commands:"), HostServ->nick);
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
	}

	void OnSetVhost(NickServ::Nick *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->GetNick());

			if (u && u->Account() == na->GetAccount())
			{
				HostServ::VHost *vhost = HostServ::FindVHost(u->Account());

				if (vhost == nullptr)
					return;

				IRCD->Send<messages::VhostSet>(u, vhost->GetIdent(), vhost->GetHost());

				u->vhost = vhost->GetHost();
				u->UpdateHost();

				if (IRCD->CanSetVIdent && !vhost->GetIdent().empty())
					u->SetVIdent(vhost->GetIdent());

				if (HostServ)
				{
					if (!vhost->GetIdent().empty())
						u->SendMessage(*HostServ, _("Your vhost of \002{0}\002@\002{1}\002 is now activated."), vhost->GetIdent(), vhost->GetHost());
					else
						u->SendMessage(*HostServ, _("Your vhost of \002{0}\002 is now activated."), vhost->GetHost());
				}
			}
		}
	}

	void OnDeleteVhost(NickServ::Nick *na) override
	{
		if (Config->GetModule(this)->Get<bool>("activate_on_set"))
		{
			User *u = User::Find(na->GetNick());

			if (u && u->Account() == na->GetAccount())
				IRCD->Send<messages::VhostDel>(u);
		}
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		if (show_hidden || source.HasPriv("hostserv/auspex"))
		{
			for (HostServ::VHost *vhost : na->GetAccount()->GetRefs<HostServ::VHost *>())
			{
				info[_("VHost")] = vhost->Mask() + (vhost->IsDefault() ? " (default)" : "");
			}
		}
	}
};

MODULE_INIT(HostServCore)

