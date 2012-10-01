/* hs_request.c - Add request and activate functionality to HostServ,
 *
 *
 * (C) 2003-2012 Anope Team
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
#include "memoserv.h"

static bool HSRequestMemoUser = false;
static bool HSRequestMemoOper = false;

void req_send_memos(CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost);

struct HostRequest : ExtensibleItem, Serializable
{
	Anope::string nick;
	Anope::string ident;
	Anope::string host;
	time_t time;

	HostRequest() : Serializable("HostRequest") { }

	Serialize::Data serialize() const anope_override
	{
		Serialize::Data data;

		data["nick"] << this->nick;
		data["ident"] << this->ident;
		data["host"] << this->host;
		data["time"].setType(Serialize::DT_INT) << this->time;

		return data;
	}

	static Serializable* unserialize(Serializable *obj, Serialize::Data &data)
	{
		NickAlias *na = findnick(data["nick"].astr());
		if (na == NULL)
			return NULL;

		HostRequest *req;
		if (obj)
			req = anope_dynamic_static_cast<HostRequest *>(obj);
		else
			req = new HostRequest;
		req->nick = na->nick;
		data["ident"] >> req->ident;
		data["host"] >> req->host;
		data["time"] >> req->time;

		if (!obj)
			na->Extend("hs_request", req);
		return req;
	}
};

class CommandHSRequest : public Command
{
	bool isvalidchar(char c)
	{
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
			return true;
		return false;
	}

 public:
	CommandHSRequest(Module *creator) : Command(creator, "hostserv/request", 1, 1)
	{
		this->SetDesc(_("Request a vHost for your nick"));
		this->SetSyntax(_("vhost"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		NickAlias *na = findnick(source.GetNick());
		if (!na || na->nc != source.GetAccount())
		{
			source.Reply(ACCESS_DENIED);
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
			if (user.length() > Config->UserLen)
			{
				source.Reply(HOST_SET_IDENTTOOLONG, Config->UserLen);
				return;
			}
			else if (!ircdproto->CanSetVIdent)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}
			for (Anope::string::iterator s = user.begin(), s_end = user.end(); s != s_end; ++s)
				if (!isvalidchar(*s))
				{
					source.Reply(HOST_SET_IDENT_ERROR);
					return;
				}
		}

		if (host.length() > Config->HostLen)
		{
			source.Reply(HOST_SET_TOOLONG, Config->HostLen);
			return;
		}

		if (!IsValidHost(host))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		if (HSRequestMemoOper && Config->MSSendDelay > 0 && u && u->lastmemosend + Config->MSSendDelay > Anope::CurTime)
		{
			source.Reply(_("Please wait %d seconds before requesting a new vHost"), Config->MSSendDelay);
			u->lastmemosend = Anope::CurTime;
			return;
		}


		HostRequest *req = new HostRequest;
		req->nick = u->nick;
		req->ident = user;
		req->host = host;
		req->time = Anope::CurTime;
		na->Extend("hs_request", req);

		source.Reply(_("Your vHost has been requested"));
		req_send_memos(source, user, host);
		Log(LOG_COMMAND, source, this) << "to request new vhost " << (!user.empty() ? user + "@" : "") << host;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Request the given vHost to be actived for your nick by the\n"
			"network administrators. Please be patient while your request\n"
			"is being considered."));
		return true;
	}
};

class CommandHSActivate : public Command
{
 public:
	CommandHSActivate(Module *creator) : Command(creator, "hostserv/activate", 1, 1)
	{
		this->SetDesc(_("Approve the requested vHost of a user"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{

		const Anope::string &nick = params[0];

		NickAlias *na = findnick(nick);
		HostRequest *req = na ? na->GetExt<HostRequest *>("hs_request") : NULL;
		if (req)
		{
			na->SetVhost(req->ident, req->host, source.GetNick(), req->time);
			FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));

			if (HSRequestMemoUser && memoserv)
				memoserv->Send(Config->HostServ, na->nick, _("[auto memo] Your requested vHost has been approved."), true);

			source.Reply(_("vHost for %s has been activated"), na->nick.c_str());
			Log(LOG_COMMAND, source, this) << "for " << na->nick << " for vhost " << (!req->ident.empty() ? req->ident + "@" : "") << req->host;
			na->Shrink("hs_request");
		}
		else
			source.Reply(_("No request for nick %s found."), nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Activate the requested vHost for the given nick."));
		if (HSRequestMemoUser)
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
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{

		const Anope::string &nick = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		NickAlias *na = findnick(nick);
		HostRequest *req = na ? na->GetExt<HostRequest *>("hs_request") : NULL;
		if (req)
		{
			na->Shrink("hs_request");

			if (HSRequestMemoUser && memoserv)
			{
				Anope::string message;
				if (!reason.empty())
					message = Anope::printf(_("[auto memo] Your requested vHost has been rejected. Reason: %s"), reason.c_str());
				else
					message = _("[auto memo] Your requested vHost has been rejected.");

				memoserv->Send(Config->HostServ, nick, message, true);
			}

			source.Reply(_("vHost for %s has been rejected"), nick.c_str());
			Log(LOG_COMMAND, source, this, NULL) << "to reject vhost for " << nick << " (" << (!reason.empty() ? reason : "") << ")";
		}
		else
			source.Reply(_("No request for nick %s found."), nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Reject the requested vHost for the given nick."));
		if (HSRequestMemoUser)
			source.Reply(_("A memo informing the user will also be sent."));

		return true;
	}
};

class CommandHSWaiting : public Command
{
	void DoList(CommandSource &source)
	{
		int counter = 1;
		int from = 0, to = 0;
		unsigned display_counter = 0;
		ListFormatter list;

		list.addColumn("Number").addColumn("Nick").addColumn("Vhost").addColumn("Created");

		for (nickalias_map::const_iterator it = NickAliasList->begin(), it_end = NickAliasList->end(); it != it_end; ++it)
		{
			const NickAlias *na = it->second;
			HostRequest *hr = na->GetExt<HostRequest *>("hs_request");
			if (!hr)
				continue;

			if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config->NSListMax)
			{
				++display_counter;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(counter);
				entry["Nick"] = it->first;
				if (!hr->ident.empty())
					entry["Vhost"] = hr->ident + "@" + hr->host;
				else
					entry["Vhost"] = hr->host;
				entry["Created"] = do_strftime(hr->time);
				list.addEntry(entry);
			}
			++counter;
		}
		source.Reply(_("Displayed all records (Count: \002%d\002)"), display_counter);

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

 public:
	CommandHSWaiting(Module *creator) : Command(creator, "hostserv/waiting", 0, 0)
	{
		this->SetDesc(_("Retrieves the vhost requests"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoList(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command retrieves the vhost requests"));

		return true;
	}
};

class HSRequest : public Module
{
	SerializeType request_type;
	CommandHSRequest commandhsrequest;
	CommandHSActivate commandhsactive;
	CommandHSReject commandhsreject;
	CommandHSWaiting commandhswaiting;

 public:
	HSRequest(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		request_type("HostRequest", HostRequest::unserialize), commandhsrequest(this), commandhsactive(this), commandhsreject(this), commandhswaiting(this)
	{
		this->SetAuthor("Anope");

		if (!ircdproto || !ircdproto->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	~HSRequest()
	{
		for (nickalias_map::const_iterator it = NickAliasList->begin(), it_end = NickAliasList->end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;
			na->Shrink("hs_request");
		}
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		HSRequestMemoUser = config.ReadFlag("hs_request", "memouser", "no", 0);
		HSRequestMemoOper = config.ReadFlag("hs_request", "memooper", "no", 0);

		Log(LOG_DEBUG) << "[hs_request] Set config vars: MemoUser=" << HSRequestMemoUser << " MemoOper=" <<  HSRequestMemoOper;
	}
};

void req_send_memos(CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost)
{
	Anope::string host;
	std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

	if (!vIdent.empty())
		host = vIdent + "@" + vHost;
	else
		host = vHost;

	if (HSRequestMemoOper == 1 && memoserv)
		for (unsigned i = 0; i < Config->Opers.size(); ++i)
		{
			Oper *o = Config->Opers[i];
			
			const NickAlias *na = findnick(o->name);
			if (!na)
				continue;

			Anope::string message = Anope::printf(_("[auto memo] vHost \002%s\002 has been requested by %s."), host.c_str(), source.GetNick().c_str());

			memoserv->Send(Config->HostServ, na->nick, message, true);
		}
}

MODULE_INIT(HSRequest)
