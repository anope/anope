/*
 * Anope IRC Services
 *
 * Copyright (C) 2005-2016 Anope Team <team@anope.org>
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
#include "modules/memoserv.h"

class HostRequest : public Serialize::Object
{
	friend class HostRequestType;

	Serialize::Storage<NickServ::Account *> acc;
	Serialize::Storage<Anope::string> ident, host;
	Serialize::Storage<time_t> time;

 public:
	static constexpr const char *const NAME = "hostrequest";
	
	using Serialize::Object::Object;

	NickServ::Account *GetAccount();
	void SetAccount(NickServ::Account *);

	Anope::string GetIdent();
	void SetIdent(const Anope::string &i);

	Anope::string GetHost();
	void SetHost(const Anope::string &h);

	time_t GetTime();
	void SetTime(const time_t &t);
};

class HostRequestType : public Serialize::Type<HostRequest>
{
 public:
	Serialize::ObjectField<HostRequest, NickServ::Account *> acc;
	Serialize::Field<HostRequest, Anope::string> ident, host;
	Serialize::Field<HostRequest, time_t> time;

	HostRequestType(Module *me) : Serialize::Type<HostRequest>(me)
		, acc(this, "acc", &HostRequest::acc, true)
		, ident(this, "ident", &HostRequest::ident)
		, host(this, "host", &HostRequest::host)
		, time(this, "time", &HostRequest::time)
	{
	}
};

NickServ::Account *HostRequest::GetAccount()
{
	return Get(&HostRequestType::acc);
}

void HostRequest::SetAccount(NickServ::Account *acc)
{
	Set(&HostRequestType::acc, acc);
}

Anope::string HostRequest::GetIdent()
{
	return Get(&HostRequestType::ident);
}

void HostRequest::SetIdent(const Anope::string &i)
{
	Set(&HostRequestType::ident, i);
}

Anope::string HostRequest::GetHost()
{
	return Get(&HostRequestType::host);
}

void HostRequest::SetHost(const Anope::string &h)
{
	Set(&HostRequestType::host, h);
}

time_t HostRequest::GetTime()
{
	return Get(&HostRequestType::time);
}

void HostRequest::SetTime(const time_t &t)
{
	Set(&HostRequestType::time, t);
}

class CommandHSRequest : public Command
{
	ServiceReference<MemoServ::MemoServService> memoserv;
	
	void SendMemos(CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost)
	{
		Anope::string host;

		if (!vIdent.empty())
			host = vIdent + "@" + vHost;
		else
			host = vHost;

		if (Config->GetModule(GetOwner())->Get<bool>("memooper") && memoserv)
			for (Oper *o : Serialize::GetObjects<Oper *>()) 
			{
				NickServ::Nick *na = NickServ::FindNick(o->GetName());
				if (!na)
					continue;

				Anope::string message = Anope::printf(_("[auto memo] vHost \002%s\002 has been requested by %s."), host.c_str(), source.GetNick().c_str());

				memoserv->Send(source.service->nick, na->GetNick(), message, true);
			}
	}

 public:
	CommandHSRequest(Module *creator) : Command(creator, "hostserv/request", 1, 1)
	{
		this->SetDesc(_("Request a vHost for your nick"));
		this->SetSyntax(_("vhost"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		User *u = source.GetUser();

		if (source.GetAccount()->IsUnconfirmed())
		{
			source.Reply(_("You must confirm your account before you may request a vhost."));
			return;
		}

		Anope::string rawhostmask = params[0];

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
			if (user.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
			{
				source.Reply(_("The username \002{0}\002 is too long, please use a username shorter than %d characters."), Config->GetBlock("networkinfo")->Get<unsigned>("userlen"));
				return;
			}

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

		time_t send_delay = Config->GetModule("memoserv/main")->Get<time_t>("senddelay");
		if (Config->GetModule(this->GetOwner())->Get<bool>("memooper") && send_delay > 0 && u && u->lastmemosend + send_delay > Anope::CurTime)
		{
			source.Reply(_("Please wait %d seconds before requesting a new vHost."), send_delay);
			u->lastmemosend = Anope::CurTime;
			return;
		}

		unsigned int max_vhosts = Config->GetModule("hostserv/main")->Get<unsigned int>("max_vhosts");
		if (max_vhosts && max_vhosts >= u->Account()->GetRefs<HostServ::VHost *>().size())
		{
			source.Reply(_("You already have the maximum number of vhosts allowed (\002{0}\002)."), max_vhosts);
			return;
		}

		HostRequest *req = u->Account()->GetRef<HostRequest *>();
		if (req != nullptr)
			req->Delete(); // delete old request

		req = Serialize::New<HostRequest *>();
		req->SetAccount(u->Account());
		req->SetIdent(user);
		req->SetHost(host);
		req->SetTime(Anope::CurTime);

		source.Reply(_("Your vhost has been requested."));
		this->SendMemos(source, user, host);
		logger.Command(LogType::COMMAND, source, _("{source} used {command} to request new vhost {0}"), (!user.empty() ? user + "@" : "") + host);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Request the given \036vhost\037 to be actived for your nick by the network administrators. Please be patient while your request is being considered."));
		return true;
	}
};

class CommandHSActivate : public Command
{
	ServiceReference<MemoServ::MemoServService> memoserv;
	
 public:
	CommandHSActivate(Module *creator) : Command(creator, "hostserv/activate", 1, 1)
	{
		this->SetDesc(_("Approve the requested vHost of a user"));
		this->SetSyntax(_("\037user\037"));
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

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		HostRequest *req = na->GetAccount()->GetRef<HostRequest *>();
		if (!req)
		{
			source.Reply(_("\002{0}\002 does not have a pending vhost request."), na->GetAccount()->GetDisplay());
			return;
		}

		unsigned int max_vhosts = Config->GetModule("hostserv/main")->Get<unsigned int>("max_vhosts");
		if (max_vhosts && max_vhosts >= na->GetAccount()->GetRefs<HostServ::VHost *>().size())
		{
			source.Reply(_("\002{0}\002 already has the maximum number of vhosts allowed (\002{1}\002)."), na->GetAccount()->GetDisplay(), max_vhosts);
			return;
		}

		HostServ::VHost *vhost = Serialize::New<HostServ::VHost *>();
		if (vhost == nullptr)
		{
			source.Reply(_("Unable to create vhost, is hostserv enabled?"));
			return;
		}

		vhost->SetAccount(na->GetAccount());
		vhost->SetIdent(req->GetIdent());
		vhost->SetHost(req->GetHost());
		vhost->SetCreator(source.GetNick());
		vhost->SetCreated(req->GetTime());

		EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, na);

		if (Config->GetModule(this->GetOwner())->Get<bool>("memouser") && memoserv)
			memoserv->Send(source.service->nick, na->GetNick(), _("[auto memo] Your requested vHost has been approved."), true);

		source.Reply(_("Vhost for \002{0}\002 has been activated."), na->GetNick());
		logger.Command(LogType::COMMAND, source, _("{source} used {command} for {0} for vhost {1}"),
				na->GetNick(), (!req->GetIdent().empty() ? req->GetIdent() + "@" : "") + req->GetHost());
		req->Delete();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Activate the requested vhost for the given user."));
		if (Config->GetModule(this->GetOwner())->Get<bool>("memouser"))
			source.Reply(_("A memo informing the user will also be sent."));

		return true;
	}
};

class CommandHSReject : public Command
{
	ServiceReference<MemoServ::MemoServService> memoserv;
	
 public:
	CommandHSReject(Module *creator) : Command(creator, "hostserv/reject", 1, 2)
	{
		this->SetDesc(_("Reject the requested vHost of a user"));
		this->SetSyntax(_("\037user\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &nick = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		HostRequest *req = na->GetAccount()->GetRef<HostRequest *>();
		if (!req)
		{
			source.Reply(_("\002{0}\002 does not have a pending vhost request."), na->GetNick());
			return;
		}

		req->Delete();

		if (Config->GetModule(this->GetOwner())->Get<bool>("memouser") && memoserv)
		{
			Anope::string message;
			if (!reason.empty())
				message = Anope::printf(_("[auto memo] Your requested vHost has been rejected. Reason: %s"), reason.c_str());
			else
				message = _("[auto memo] Your requested vHost has been rejected.");

			memoserv->Send(source.service->nick, nick, Language::Translate(source.GetAccount(), message.c_str()), true);
		}

		source.Reply(_("Vhost for \002{0}\002 has been rejected."), na->GetNick());
		logger.Command(LogType::COMMAND, source, _("{source} used {command} to reject vhost for {0} ({1})"),
				na->GetNick(), !reason.empty() ? reason : "no reason");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Reject the requested vhost for the given user."));
		if (Config->GetModule(this->GetOwner())->Get<bool>("memouser"))
			source.Reply(_("A memo informing the user will also be sent, which includes the reason for the rejection if supplied."));

		return true;
	}
};

class CommandHSWaiting : public Command
{
 public:
	CommandHSWaiting(Module *creator) : Command(creator, "hostserv/waiting", 0, 0)
	{
		this->SetDesc(_("Retrieves the vhost requests"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned counter = 0;
		unsigned display_counter = 0, listmax = Config->GetModule(this->GetOwner())->Get<unsigned>("listmax");
		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Number")).AddColumn(_("Nick")).AddColumn(_("Vhost")).AddColumn(_("Created"));

		for (HostRequest *hr : Serialize::GetObjects<HostRequest *>())
		{
			if (!listmax || display_counter < listmax)
			{
				++display_counter;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(display_counter);
				entry["Nick"] = hr->GetAccount()->GetDisplay();
				if (!hr->GetIdent().empty())
					entry["Vhost"] = hr->GetIdent() + "@" + hr->GetHost();
				else
					entry["Vhost"] = hr->GetHost();
				entry["Created"] = Anope::strftime(hr->GetTime(), NULL, true);
				list.AddEntry(entry);
			}
			++counter;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("Displayed \002{0}\002 records (\002{1}\002 total)."), display_counter, counter);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists the vhost requests."));

		return true;
	}
};

class HSRequest : public Module
{
	CommandHSRequest commandhsrequest;
	CommandHSActivate commandhsactive;
	CommandHSReject commandhsreject;
	CommandHSWaiting commandhswaiting;

	HostRequestType hrtype;

 public:
	HSRequest(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsrequest(this)
		, commandhsactive(this)
		, commandhsreject(this)
		, commandhswaiting(this)
		, hrtype(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSRequest)
