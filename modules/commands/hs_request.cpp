/* hs_request.c - Add request and activate functionality to HostServ,
 *
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.11
 * Anope Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 */

#include "module.h"
#include "modules/memoserv.h"

static void req_send_memos(Module *me, CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost);

struct HostRequest : Serializable
{
	Anope::string nick;
	Anope::string ident;
	Anope::string host;
	time_t time;

	HostRequest(Extensible *) : Serializable("HostRequest") { }

	void Serialize(Serialize::Data &data) const override
	{
		data["nick"] << this->nick;
		data["ident"] << this->ident;
		data["host"] << this->host;
		data.SetType("time", Serialize::Data::DT_INT); data["time"] << this->time;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string snick;
		data["nick"] >> snick;

		NickServ::Nick *na = NickServ::FindNick(snick);
		if (na == NULL)
			return NULL;

		HostRequest *req;
		if (obj)
			req = anope_dynamic_static_cast<HostRequest *>(obj);
		else
			req = na->Extend<HostRequest>("hostrequest");
		if (req)
		{
			req->nick = na->nick;
			data["ident"] >> req->ident;
			data["host"] >> req->host;
			data["time"] >> req->time;
		}

		return req;
	}
};

class CommandHSRequest : public Command
{
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
		NickServ::Nick *na = NickServ::FindNick(source.GetNick());
		if (!na || na->nc != source.GetAccount())
		{
			source.Reply(_("Access denied.")); //XXX with nonickownership this should be allowed.
			return;
		}

		Anope::string rawhostmask = params[0];

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

		time_t send_delay = Config->GetModule("memoserv")->Get<time_t>("senddelay");
		if (Config->GetModule(this->owner)->Get<bool>("memooper") && send_delay > 0 && u && u->lastmemosend + send_delay > Anope::CurTime)
		{
			source.Reply(_("Please wait %d seconds before requesting a new vHost."), send_delay);
			u->lastmemosend = Anope::CurTime;
			return;
		}

		HostRequest req(na);
		req.nick = source.GetNick();
		req.ident = user;
		req.host = host;
		req.time = Anope::CurTime;
		na->Extend<HostRequest>("hostrequest", req);

		source.Reply(_("Your vhost has been requested."));
		req_send_memos(owner, source, user, host);
		Log(LOG_COMMAND, source, this) << "to request new vhost " << (!user.empty() ? user + "@" : "") << host;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Request the given \036vhost\037 to be actived for your nick by the network administrators. Please be patient while your request is being considered."));
		return true;
	}
};

class CommandHSActivate : public Command
{
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
		HostRequest *req = na ? na->GetExt<HostRequest>("hostrequest") : NULL;
		if (req)
		{
			na->SetVhost(req->ident, req->host, source.GetNick(), req->time);
			Event::OnSetVhost(&Event::SetVhost::OnSetVhost, na);

			if (Config->GetModule(this->owner)->Get<bool>("memouser") && MemoServ::service)
				MemoServ::service->Send(source.service->nick, na->nick, _("[auto memo] Your requested vHost has been approved."), true);

			source.Reply(_("Vhost for \002{0}\002 has been activated."), na->nick);
			Log(LOG_COMMAND, source, this) << "for " << na->nick << " for vhost " << (!req->ident.empty() ? req->ident + "@" : "") << req->host;
			na->Shrink<HostRequest>("hostrequest");
		}
		else
			source.Reply(_("\002{0}\002 does not have a pending vhost request."), nick);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Activate the requested vhost for the given user."));
		if (Config->GetModule(this->owner)->Get<bool>("memouser"))
			source.Reply(_("A memo informing the user will also be sent."));

		return true;
	}
};

class CommandHSReject : public Command
{
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
		HostRequest *req = na ? na->GetExt<HostRequest>("hostrequest") : NULL;
		if (req)
		{
			na->Shrink<HostRequest>("hostrequest");

			if (Config->GetModule(this->owner)->Get<bool>("memouser") && MemoServ::service)
			{
				Anope::string message;
				if (!reason.empty())
					message = Anope::printf(_("[auto memo] Your requested vHost has been rejected. Reason: %s"), reason.c_str());
				else
					message = _("[auto memo] Your requested vHost has been rejected.");

				MemoServ::service->Send(source.service->nick, nick, Language::Translate(source.GetAccount(), message.c_str()), true);
			}

			source.Reply(_("Vhost for \002{0}\002 has been rejected."), na->nick);
			Log(LOG_COMMAND, source, this) << "to reject vhost for " << nick << " (" << (!reason.empty() ? reason : "no reason") << ")";
		}
		else
			source.Reply(_("\002{0}\002 does not have a pending vhost request."), nick);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Reject the requested vhost for the given user."));
		if (Config->GetModule(this->owner)->Get<bool>("memouser"))
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
		unsigned display_counter = 0, listmax = Config->GetModule(this->owner)->Get<unsigned>("listmax");
		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Number")).AddColumn(_("Nick")).AddColumn(_("Vhost")).AddColumn(_("Created"));

		for (auto& it : NickServ::service->GetNickList())
		{
			const NickServ::Nick *na = it.second;
			HostRequest *hr = na->GetExt<HostRequest>("hostrequest");
			if (!hr)
				continue;

			if (!listmax || display_counter < listmax)
			{
				++display_counter;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(display_counter);
				entry["Nick"] = na->nick;
				if (!hr->ident.empty())
					entry["Vhost"] = hr->ident + "@" + hr->host;
				else
					entry["Vhost"] = hr->host;
				entry["Created"] = Anope::strftime(hr->time, NULL, true);
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
	ExtensibleItem<HostRequest> hostrequest;
	Serialize::Type request_type;

 public:
	HSRequest(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsrequest(this)
		, commandhsactive(this)
		, commandhsreject(this)
		, commandhswaiting(this)
		, hostrequest(this, "hostrequest")
		, request_type("HostRequest", HostRequest::Unserialize)
	{

		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

static void req_send_memos(Module *me, CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost)
{
	Anope::string host;
	std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

	if (!vIdent.empty())
		host = vIdent + "@" + vHost;
	else
		host = vHost;

	if (Config->GetModule(me)->Get<bool>("memooper") && MemoServ::service)
		for (unsigned i = 0; i < Oper::opers.size(); ++i)
		{
			Oper *o = Oper::opers[i];

			const NickServ::Nick *na = NickServ::FindNick(o->name);
			if (!na)
				continue;

			Anope::string message = Anope::printf(_("[auto memo] vHost \002%s\002 has been requested by %s."), host.c_str(), source.GetNick().c_str());

			MemoServ::service->Send(source.service->nick, na->nick, message, true);
		}
}

MODULE_INIT(HSRequest)
