/* hs_request.c - Add request and activate functionality to HostServ
 *
 * (C) 2003-2025 Anope Team
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
#include "modules/dns.h"
#include "modules/hostserv/request.h"

static ServiceReference<DNS::Manager> dnsmanager("DNS::Manager", "dns/manager");
static ServiceReference<MemoServService> memoserv("MemoServService", "MemoServ");

static void req_send_memos(Module *me, CommandSource &source, const Anope::string &vident, const Anope::string &vhost);

namespace
{
	// The name of the DNS record used for validation.
	Anope::string validation_record;
}

struct HostRequestImpl final
	: HostRequest
	, Serializable
{
	HostRequestImpl(Extensible *)
		: Serializable("HostRequest")
	{
	}

	static HostRequestImpl *Get(NickAlias *na)
	{
		return na ? na->GetExt<HostRequestImpl>("hostrequest") : nullptr;
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
		Anope::string snick;
		data["nick"] >> snick;

		NickAlias *na = NickAlias::Find(snick);
		if (na == NULL)
			return NULL;

		HostRequestImpl *req;
		if (obj)
			req = anope_dynamic_static_cast<HostRequestImpl *>(obj);
		else
			req = na->Extend<HostRequestImpl>("hostrequest");
		if (req)
		{
			req->nick = na->nick;
			data["ident"] >> req->ident;
			data["host"] >> req->host;
			data["time"] >> req->time;
			data["validation_token"] >> req->validation_token;
			data["last_validation"] >> req->last_validation;
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
	DNSHostResolver(Command *cmd, HostRequest *hr, NickAlias *na, const CommandSource &src)
		: Request(dnsmanager, cmd->module, hr->host, DNS::QUERY_TXT, false)
		, command(cmd)
		, nickalias(na)
		, source(src)
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

			if (Config->GetModule(command->module).Get<bool>("memouser") && memoserv)
				memoserv->Send(source.service->nick, na->nick, _("Your requested vhost has been validated via DNS."), true);

			source.Reply(_("VHost for %s has been validated using DNS."), na->nick.c_str());
			Log(LOG_COMMAND, source, command) << "for " << na->nick << " for vhost " << hr->Mask();
			na->Shrink<HostRequestImpl>("hostrequest");

			return; // We're done.
		}

		HandleError(hr);
	}
};

class CommandHSRequest final
	: public Command
{
public:
	CommandHSRequest(Module *creator) : Command(creator, "hostserv/request", 1, 1)
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
				source.Reply(HOST_SET_IDENTTOOLONG, IRCD->MaxUser);
				return;
			}
			else if (!IRCD->CanSetVIdent)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}

			if (!IRCD->IsIdentValid(user))
			{
				source.Reply(HOST_SET_IDENT_ERROR);
				return;
			}
		}

		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_TOOLONG, IRCD->MaxHost);
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		time_t send_delay = Config->GetModule("memoserv").Get<time_t>("senddelay");
		if (Config->GetModule(this->owner).Get<bool>("memooper") && send_delay > 0 && u && u->lastmemosend + send_delay > Anope::CurTime)
		{
			auto waitperiod = (u->lastmemosend + send_delay) -  Anope::CurTime;
			source.Reply(_("Please wait %s before requesting a new vhost."), Anope::Duration(waitperiod, source.GetAccount()).c_str());
			u->lastmemosend = Anope::CurTime;
			return;
		}

		HostRequestImpl req(na);
		req.nick = source.GetNick();
		req.ident = user;
		req.host = host;
		req.time = Anope::CurTime;
		req.validation_token = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", "15"));
		na->Extend<HostRequestImpl>("hostrequest", req);

		BotInfo *bi;
		Anope::string cmd;
		if (dnsmanager && Command::FindCommandFromService("hostserv/validate", bi, cmd))
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
public:
	CommandHSActivate(Module *creator) : Command(creator, "hostserv/activate", 1, 1)
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
		HostRequestImpl *req = na ? na->GetExt<HostRequestImpl>("hostrequest") : NULL;
		if (req)
		{
			na->SetVHost(req->ident, req->host, source.GetNick(), req->time);
			FOREACH_MOD(OnSetVHost, (na));

			if (Config->GetModule(this->owner).Get<bool>("memouser") && memoserv)
				memoserv->Send(source.service->nick, na->nick, _("Your requested vhost has been approved."), true);

			source.Reply(_("VHost for %s has been activated."), na->nick.c_str());
			Log(LOG_COMMAND, source, this) << "for " << na->nick << " for vhost " << (!req->ident.empty() ? req->ident + "@" : "") << req->host;
			na->Shrink<HostRequestImpl>("hostrequest");
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
public:
	CommandHSReject(Module *creator) : Command(creator, "hostserv/reject", 1, 2)
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
		HostRequestImpl *req = na ? na->GetExt<HostRequestImpl>("hostrequest") : NULL;
		if (req)
		{
			na->Shrink<HostRequestImpl>("hostrequest");

			if (Config->GetModule(this->owner).Get<bool>("memouser") && memoserv)
			{
				Anope::string message;
				if (!reason.empty())
					message = Anope::Format(_("Your requested vhost has been rejected. Reason: %s"), reason.c_str());
				else
					message = _("Your requested vhost has been rejected.");

				memoserv->Send(source.service->nick, nick, Language::Translate(source.GetAccount(), message.c_str()), true);
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

		for (const auto &[nick, na] : *NickAliasList)
		{
			HostRequestImpl *hr = na->GetExt<HostRequestImpl>("hostrequest");
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
public:
	time_t cooldown;

	CommandHSValidate(Module *creator)
		: Command(creator, "hostserv/validate", 0)
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

		auto next_validation = req->last_validation + cooldown;
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

			res = new DNSHostResolver(this, req, na, source);
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
		// TODO
		return true;
	}
};

class HSRequest final
	: public Module
{
	CommandHSRequest commandhsrequest;
	CommandHSActivate commandhsactive;
	CommandHSReject commandhsreject;
	CommandHSWaiting commandhswaiting;
	CommandHSValidate commandhsvalidate;
	ExtensibleItem<HostRequestImpl> hostrequest;
	HostRequestTypeImpl request_type;

public:
	HSRequest(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandhsrequest(this)
		, commandhsactive(this)
		, commandhsreject(this)
		, commandhswaiting(this)
		, commandhsvalidate(this)
		, hostrequest(this, "hostrequest")
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		commandhsvalidate.cooldown = block.Get<time_t>("validationcooldown", "5m");
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

	if (Config->GetModule(me).Get<bool>("memooper") && memoserv)
	{
		for (auto *o : Oper::opers)
		{
			const NickAlias *na = NickAlias::Find(o->name);
			if (!na)
				continue;

			Anope::string message = Anope::Format(_("VHost \002%s\002 has been requested by %s."), host.c_str(), source.GetNick().c_str());

			memoserv->Send(source.service->nick, na->nick, message, true);
		}
	}
}

MODULE_INIT(HSRequest)
