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
#include "modules/nickserv/group.h"

class CommandHSGroup : public Command
{
	bool setting;

 public:
	void Sync(NickServ::Nick *na)
	{
		if (setting)
			return;

		HostServ::VHost *v = na->GetVHost();

		if (v == nullptr)
			return;

		setting = true;
		for (NickServ::Nick *nick : na->GetAccount()->GetRefs<NickServ::Nick *>())
		{
			if (nick == na)
				continue;

			HostServ::VHost *vhost = Serialize::New<HostServ::VHost *>();
			if (vhost == nullptr)
				continue;

			vhost->SetOwner(nick);
			vhost->SetIdent(v->GetIdent());
			vhost->SetHost(v->GetHost());
			vhost->SetCreator(v->GetCreator());
			vhost->SetCreated(Anope::CurTime);

			nick->SetVHost(vhost);

			EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, nick);
		}
		setting = false;
	}

	CommandHSGroup(Module *creator) : Command(creator, "hostserv/group", 0, 0), setting(false)
	{
		this->SetDesc(_("Syncs the vhost for all nicks in a group"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(source.GetNick());
		if (!na || na->GetAccount() != source.GetAccount())
		{
			source.Reply(_("Access denied."));
			return;
		}

		HostServ::VHost *vhost = na->GetVHost();
		if (vhost == nullptr)
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		this->Sync(na);
		if (!vhost->GetIdent().empty())
			source.Reply(_("All vhosts in the group \002{0}\002 have been set to \002{1}\002@\002{2}\002."), source.nc->GetDisplay(), vhost->GetIdent(), vhost->GetHost());
		else
			source.Reply(_("All vhosts in the group \002{0}\002 have been set to \002{1}\002."), source.nc->GetDisplay(), vhost->GetHost());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the vhost of all nicks in your group to the vhost of your current nick."));
		return true;
	}
};

class HSGroup : public Module
	, public EventHook<Event::SetVhost>
	, public EventHook<Event::NickGroup>
{
	CommandHSGroup commandhsgroup;
	bool syncongroup;
	bool synconset;

 public:
	HSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::SetVhost>(this)
		, EventHook<Event::NickGroup>(this)
		, commandhsgroup(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnSetVhost(NickServ::Nick *na) override
	{
		if (!synconset)
			return;

		commandhsgroup.Sync(na);
	}

	void OnNickGroup(User *u, NickServ::Nick *na) override
	{
		if (!syncongroup)
			return;

		commandhsgroup.Sync(na);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		syncongroup = block->Get<bool>("syncongroup");
		synconset = block->Get<bool>("synconset");
	}
};

MODULE_INIT(HSGroup)
