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

class CommandHSSet : public Command
{
 public:
	CommandHSSet(Module *creator) : Command(creator, "hostserv/set", 2, 2)
	{
		this->SetDesc(_("Set the vhost of another user"));
		this->SetSyntax(_("\037user\037 \037hostmask\037"));
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
			host = rawhostmask;
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

		Log(LOG_ADMIN, source, this) << "to set the vhost of " << na->GetNick() << " to " << (!user.empty() ? user + "@" : "") << host;

		na->SetVhost(user, host, source.GetNick());
		EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, na);
		if (!user.empty())
			source.Reply(_("Vhost for \002{0}\002 set to \002{0}\002@\002{1}\002."), nick, user, host);
		else
			source.Reply(_("Vhost for \002{0}\002 set to \002{0}\002."), nick, host);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the vhost of the given \037user\037 to the given \037hostmask\037."));
		return true;
	}
};

class CommandHSSetAll : public Command
{
	void Sync(NickServ::Nick *na)
	{
		if (!na || !na->HasVhost())
			return;

		for (NickServ::Nick *nick : na->GetAccount()->GetRefs<NickServ::Nick *>())
			nick->SetVhost(na->GetVhostIdent(), na->GetVhostHost(), na->GetVhostCreator());
	}

 public:
	CommandHSSetAll(Module *creator) : Command(creator, "hostserv/setall", 2, 2)
	{
		this->SetDesc(_("Set the vhost for all nicks in a group"));
		this->SetSyntax(_("\037user\037 \037hostmask\037"));
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
			host = rawhostmask;
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

		Log(LOG_ADMIN, source, this) << "to set the vhost of " << na->GetNick() << " to " << (!user.empty() ? user + "@" : "") << host;

		na->SetVhost(user, host, source.GetNick());
		this->Sync(na);
		EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, na);
		if (!user.empty())
			source.Reply(_("Vhost for group \002{0}\002 set to \002{1}\002@\002{2}\002."), nick.c_str(), user.c_str(), host.c_str());
		else
			source.Reply(_("host for group \002{0}\002 set to \002{1}\002."), nick.c_str(), host.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the vhost for all nicknames in the group of \037user\037."));
		return true;
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;
	CommandHSSetAll commandhssetall;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsset(this)
		, commandhssetall(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSSet)
