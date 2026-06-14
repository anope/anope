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
#include "modules/dns.h"
#include "modules/hostserv/request.h"
#include "modules/memoserv/service.h"

static ServiceReference<DNS::Manager> dnsmanager("DNS::Manager", "dns/manager");

static void req_send_memos(Module *me, CommandSource &source, const Anope::string &vident, const Anope::string &vhost);

namespace
{
	// The name of the DNS record used for validation.
	Anope::string validation_record;
}

struct SharedData final
{
	// How long after a requested vhost is activated does a user have to wait before they can request a new vhost.
	time_t activationcooldown = 0;

	// How long after a requested vhost is rejected does a user have to wait before they can request a new vhost.
	time_t rejectioncooldown = 0;

	// How long should users have to wait between attempts at DNS validation.
	time_t validationcooldown = 0;

	// Extensible that stores the time a user had a vhost activated/rejected.
	SerializableExtensibleItem<time_t> requestcooldown;

	// The name of the DNS record used for validation.
	Anope::string validationrecord;

	SharedData(Module *mod)
		: requestcooldown(mod, "HS_REQUEST_COOLDOWN")
	{
	}
};

struct HostRequestImpl final
	: HostServ::HostRequest
	, Serializable
{
	HostRequestImpl(Extensible *)
		: Serializable("HostRequest")
	{
	}

	static HostRequestImpl *Get(NickAlias *na)
	{
		return na ? na->GetExt<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT) : nullptr;
	}

	Anope::string Mask() const
	{
		if (ident.empty())
			return host;
		return ident + "@" + host;
	}

	Anope::string GetValidationRecord() const
	{
		return Anope::Format("%s=%s", validation_record.c_str(), this->validation_token.c_str());
	}
};

struct HostRequestTypeImpl final
	: Serialize::Type
{
	HostRequestTypeImpl()
		: Serialize::Type("HostRequest")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *req = static_cast<const HostRequestImpl *>(obj);
		data.Store("nick", req->nick);
		data.Store("ident", req->ident);
		data.Store("host", req->host);
		data.Store("time", req->time);
		data.Store("validation_token", req->validation_token);
		data.Store("last_validation", req->last_validation);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		auto *na = NickAlias::Find(data.Load("nick"));
		if (na == NULL)
			return NULL;

		HostRequestImpl *req;
		if (obj)
			req = anope_dynamic_static_cast<HostRequestImpl *>(obj);
		else
			req = na->Extend<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT);
		if (req)
		{
			req->nick = na->nick;
			req->ident = data.Load("ident");
			req->host = data.Load("host");
			req->time = data.Load<time_t>("time");
			req->validation_token = data.Load("validation_token");
			req->last_validation = data.Load<time_t>("last_validation");
		}

		return req;
	}
};

class DNSHostResolver final
	: public DNS::Request
{
private:
	Command *command;
	Reference<NickAlias> nickalias;
	CommandSource source;
	SharedData &data;

	void HandleError(HostRequestImpl *hr)
	{
		source.Reply(_(
				"Unable to find the DNS record required to validate \002%s\002. If you have not already "
				"done this add a TXT record for %s with the value %s and re-execute this command."
			),
			hr->Mask().c_str(),
			hr->host.c_str(),
			hr->GetValidationRecord().c_str()
		);
	}

public:
	DNSHostResolver(Command *cmd, HostServ::HostRequest *hr, NickAlias *na, const CommandSource &src, SharedData &sd)
		: Request(dnsmanager, cmd->module, hr->host, DNS::QUERY_TXT, false)
		, command(cmd)
		, nickalias(na)
		, source(src)
		, data(sd)
	{
		hr->last_validation = Anope::CurTime;
		Log(LOG_DEBUG) << "Checking " << hr->host << " for " << hr->validation_token;
	}

	void OnError(const DNS::Query *record) override
	{
		NickAlias *na = nickalias;
		if (!na)
			return; // Nick has been dropped.

		auto *hr = HostRequestImpl::Get(na);
		if (!hr)
		{
			source.Reply(_("No request for nick %s found."), source.GetNick().c_str());
			return;
		}

		HandleError(hr);
	}

	void OnLookupComplete(const DNS::Query *record) override
	{
		NickAlias *na = nickalias;
		if (!na)
			return; // Nick has been dropped.

		auto *hr = HostRequestImpl::Get(na);
		if (!hr)
		{
			source.Reply(_("No request for nick %s found."), source.GetNick().c_str());
			return;
		}

		for (const auto &answer : record->answers)
		{
			if (answer.rdata != hr->GetValidationRecord())
				continue; // Not for us.

			na->SetVHost(hr->ident, hr->host, source.GetNick(), hr->time);
			FOREACH_MOD(OnSetVHost, (na));

			if (Config->GetModule(command->module).Get<bool>("memouser") && MemoServ::service)
				MemoServ::service->Send(source.service->nick, na->nick, _("Your requested vhost has been validated via DNS."), true);

			source.Reply(_("VHost for %s has been validated using DNS."), na->nick.c_str());
			Log(LOG_COMMAND, source, command) << "for " << na->nick << " for vhost " << hr->Mask();

			data.requestcooldown.Set(na, Anope::CurTime + data.activationcooldown);
			na->Shrink<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT);

			return; // We're done.
		}

		HandleError(hr);
	}
};

class CommandHSRequest final
	: public Command
{
private:
	SharedData &data;

public:
	CommandHSRequest(Module *creator, SharedData &sd)
		: Command(creator, "hostserv/request", 1, 1)
		, data(sd)
	{
		this->SetDesc(_("Request a vhost for your nick"));
		this->SetSyntax(_("vhost"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		User *u = source.GetUser();
		NickAlias *na = NickAlias::Find(source.GetNick());
		if (!na || na->nc != source.GetAccount())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (source.GetAccount()->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You must confirm your account before you may request a vhost."));
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
			if (user.length() > IRCD->MaxUser)
			{
				source.Reply(HOST_SET_VIDENT_TOO_LONG, IRCD->MaxUser);
				return;
			}
			else if (!IRCD->CanSetVIdent)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}

			if (!IRCD->IsIdentValid(user))
			{
				source.Reply(HOST_SET_VIDENT_ERROR);
				return;
			}
		}

		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_VHOST_TOO_LONG, IRCD->MaxHost);
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_VHOST_ERROR);
			return;
		}

		time_t waituntil = 0;
		{
			// Check whether the user is on a request cooldown.
			const auto *last_req = data.requestcooldown.Get(na);
			if (last_req)
				waituntil = *last_req;
		}
		if (Config->GetModule(this->owner).Get<bool>("memooper"))
		{
			// Check whether the user can send a memo to opers yet.
			const auto send_delay = Config->GetModule("memoserv").Get<time_t>("senddelay");
			if (send_delay > 0 && u && u->lastmemosend)
				waituntil = std::max(waituntil, u->lastmemosend + send_delay);
		}

		if (waituntil && waituntil > Anope::CurTime)
		{
			const auto waitperiod = waituntil - Anope::CurTime;
			source.Reply(_("Please wait %s before requesting a new vhost."), Anope::Duration(waitperiod, source.GetAccount()).c_str());
			return;
		}

		HostRequestImpl req(na);
		req.nick = source.GetNick();
		req.ident = user;
		req.host = host;
		req.time = Anope::CurTime;
		req.validation_token = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", "15"));
		na->Extend<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT, req);

		BotInfo *bi;
		Anope::string cmd;
		if (dnsmanager && Command::FindFromService("hostserv/validate", bi, cmd))
		{
			source.Reply(_(
					"Your vhost \002%s\002 has been requested. If the requested vhost is for a valid "
					"DNS name you can add a TXT record for %s with the value %s and automatically "
					"approve your vhost using \002%s\002."
				),
				req.Mask().c_str(),
				req.host.c_str(),
				req.GetValidationRecord().c_str(),
				bi->GetQueryCommand("hostserv/validate").c_str()
			);
		}
		else
			source.Reply(_("Your vhost \002%s\002 has been requested."), req.Mask().c_str());


		req_send_memos(owner, source, user, host);
		Log(LOG_COMMAND, source, this) << "to request new vhost " << (!user.empty() ? user + "@" : "") << host;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Request the given vhost to be activated for your nick by the "
			"network administrators. Please be patient while your request "
			"is being considered."
		));
		return true;
	}
};

class CommandHSActivate final
	: public Command
{
private:
	SharedData &data;

public:
	CommandHSActivate(Module *creator, SharedData &sd)
		: Command(creator, "hostserv/activate", 1, 1)
		, data(sd)
	{
		this->SetDesc(_("Approve the requested vhost of a user"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];

		NickAlias *na = NickAlias::Find(nick);
		auto *req = HostRequestImpl::Get(na);
		if (req)
		{
			na->SetVHost(req->ident, req->host, source.GetNick(), req->time);
			FOREACH_MOD(OnSetVHost, (na));

			if (Config->GetModule(this->owner).Get<bool>("memouser") && MemoServ::service)
				MemoServ::service->Send(source.service->nick, na->nick, _("Your requested vhost has been approved."), true);

			source.Reply(_("VHost for %s has been activated."), na->nick.c_str());
			Log(LOG_COMMAND, source, this) << "for " << na->nick << " for vhost " << (!req->ident.empty() ? req->ident + "@" : "") << req->host;

			data.requestcooldown.Set(na, Anope::CurTime + data.activationcooldown);
			na->Shrink<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT);
		}
		else
			source.Reply(_("No request for nick %s found."), nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Activate the requested vhost for the given nick."));
		if (Config->GetModule(this->owner).Get<bool>("memouser"))
			source.Reply(_("A memo informing the user will also be sent."));

		return true;
	}
};

class CommandHSReject final
	: public Command
{
private:
	SharedData &data;

public:
	CommandHSReject(Module *creator, SharedData &sd)
		: Command(creator, "hostserv/reject", 1, 2)
		, data(sd)
	{
		this->SetDesc(_("Reject the requested vhost of a user"));
		this->SetSyntax(_("\037nick\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		NickAlias *na = NickAlias::Find(nick);
		auto *req = HostRequestImpl::Get(na);
		if (req)
		{
			data.requestcooldown.Set(na, Anope::CurTime + data.rejectioncooldown);
			na->Shrink<HostRequestImpl>(HOSTSERV_HOST_REQUEST_EXT);

			if (Config->GetModule(this->owner).Get<bool>("memouser") && MemoServ::service)
			{
				Anope::string message;
				if (!reason.empty())
					message = Anope::Format(_("Your requested vhost has been rejected. Reason: %s"), reason.c_str());
				else
					message = _("Your requested vhost has been rejected.");

				MemoServ::service->Send(source.service->nick, nick, source.Translate(message.c_str()), true);
			}

			source.Reply(_("VHost for %s has been rejected."), nick.c_str());
			Log(LOG_COMMAND, source, this) << "to reject vhost for " << nick << " (" << (!reason.empty() ? reason : "no reason") << ")";
		}
		else
			source.Reply(_("No request for nick %s found."), nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Reject the requested vhost for the given nick."));
		if (Config->GetModule(this->owner).Get<bool>("memouser"))
			source.Reply(_("A memo informing the user will also be sent, which includes the reason for the rejection if supplied."));

		return true;
	}
};

class CommandHSWaiting final
	: public Command
{
public:
	CommandHSWaiting(Module *creator) : Command(creator, "hostserv/waiting", 0, 0)
	{
		this->SetDesc(_("Retrieves the vhost requests"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned counter = 0;
		unsigned display_counter = 0, listmax = Config->GetModule(this->owner).Get<unsigned>("listmax");

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Nick")).AddColumn(_("VHost")).AddColumn(_("Created"));
		list.SetFlexible(_("{number}: \002{nick}\002 = {vhost} -- created by {creator} at {created}"));

		for (const auto &[nick, na] : *NickAliasList)
		{
			auto *hr = HostRequestImpl::Get(na);
			if (!hr)
				continue;

			if (!listmax || display_counter < listmax)
			{
				++display_counter;

				ListFormatter::ListEntry entry;
				entry["Number"] = Anope::ToString(display_counter);
				entry["Nick"] = nick;
				if (!hr->ident.empty())
					entry["VHost"] = hr->ident + "@" + hr->host;
				else
					entry["VHost"] = hr->host;
				entry["Created"] = Anope::strftime(hr->time, NULL, true);
				list.AddEntry(entry);
			}
			++counter;
		}

		list.SendTo(source);
		source.Reply(_("Displayed \002%d\002 records (\002%d\002 total)."), display_counter, counter);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command retrieves the vhost requests."));

		return true;
	}
};

class CommandHSValidate final
	: public Command
{
private:
	SharedData &data;

public:
	CommandHSValidate(Module *creator, SharedData &sd)
		: Command(creator, "hostserv/validate", 0)
		, data(sd)
	{
		this->SetDesc(_("Validates a previously requested vhost using DNS"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		auto *na = NickAlias::Find(source.GetNick());
		if (!na || na->nc != source.GetAccount())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		auto *req = HostRequestImpl::Get(na);
		if (!req)
		{
			source.Reply(_("No request for nick %s found."), source.GetNick().c_str());
			return;
		}

		auto next_validation = req->last_validation + data.validationcooldown;
		if (req->last_validation && next_validation > Anope::CurTime)
		{
			source.Reply(_("You must wait for %s before trying DNS validation again."),
				Anope::Duration(next_validation - Anope::CurTime).c_str());
			return;
		}

		DNSHostResolver *res = nullptr;
		try
		{
			if (!dnsmanager)
				throw SocketException("DNS is not available");

			res = new DNSHostResolver(this, req, na, source, data);
			dnsmanager->Process(res);
		}
		catch (const SocketException &ex)
		{
			Log(this->module) << ex.GetReason();
			source.Reply("Unable to validate vhosts right now. Please try again later.");
			delete res;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Validates a previously requested vhost using DNS. If you own the domain you "
			"have requested as a vhost you can validate your ownership of it using a DNS "
			"TXT record to approve your own vhost."
		));
		return true;
	}
};

class HSRequest final
	: public Module
{
private:
	SharedData data;
	CommandHSRequest commandhsrequest;
	CommandHSActivate commandhsactivate;
	CommandHSReject commandhsreject;
	CommandHSWaiting commandhswaiting;
	CommandHSValidate commandhsvalidate;
	ExtensibleItem<HostRequestImpl> hostrequest;
	HostRequestTypeImpl request_type;

public:
	HSRequest(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, data(this)
		, commandhsrequest(this, data)
		, commandhsactivate(this, data)
		, commandhsreject(this, data)
		, commandhswaiting(this)
		, commandhsvalidate(this, data)
		, hostrequest(this, HOSTSERV_HOST_REQUEST_EXT)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		data.activationcooldown = block.Get<time_t>("activationcooldown", "24h");
		data.rejectioncooldown = block.Get<time_t>("rejectioncooldown", "24h");
		data.validationcooldown = block.Get<time_t>("validationcooldown", "5m");
		validation_record = block.Get<const Anope::string>("validationrecord", "anope-dns-validation");
	}
};

static void req_send_memos(Module *me, CommandSource &source, const Anope::string &vident, const Anope::string &vhost)
{
	Anope::string host;
	if (!vident.empty())
		host = vident + "@" + vhost;
	else
		host = vhost;

	if (Config->GetModule(me).Get<bool>("memooper") && MemoServ::service)
	{
		for (auto *o : Oper::opers)
		{
			const NickAlias *na = NickAlias::Find(o->name);
			if (!na)
				continue;

			Anope::string message = Anope::Format(_("VHost \002%s\002 has been requested by %s."), host.c_str(), source.GetNick().c_str());

			MemoServ::service->Send(source.service->nick, na->nick, message, true);
		}
	}
}

MODULE_INIT(HSRequest)
