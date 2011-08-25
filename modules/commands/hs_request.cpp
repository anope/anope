/* hs_request.c - Add request and activate functionality to HostServ,
 *
 *
 * (C) 2003-2011 Anope Team
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

void my_add_host_request(const Anope::string &nick, const Anope::string &vIdent, const Anope::string &vhost, const Anope::string &creator, time_t tmp_time);
void req_send_memos(CommandSource &source, const Anope::string &vIdent, const Anope::string &vHost);

struct HostRequest
{
	Anope::string ident;
	Anope::string host;
	time_t time;
};

typedef std::map<Anope::string, HostRequest *, std::less<ci::string> > RequestMap;
RequestMap Requests;

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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

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
			else if (!ircd->vident)
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
		my_add_host_request(u->nick, user, host, u->nick, Anope::CurTime);

		source.Reply(_("Your vHost has been requested"));
		req_send_memos(source, user, host);
		Log(LOG_COMMAND, u, this, NULL) << "to request new vhost " << (!user.empty() ? user + "@" : "") << host;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];

		NickAlias *na = findnick(nick);
		if (na)
		{
			RequestMap::iterator it = Requests.find(na->nick);
			if (it != Requests.end())
			{
				na->hostinfo.SetVhost(it->second->ident, it->second->host, u->nick, it->second->time);
				FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));

				if (HSRequestMemoUser && memoserv)
					memoserv->Send(Config->HostServ, na->nick, _("[auto memo] Your requested vHost has been approved."), true);

				source.Reply(_("vHost for %s has been activated"), na->nick.c_str());
				Log(LOG_COMMAND, u, this, NULL) << "for " << na->nick << " for vhost " << (!it->second->ident.empty() ? it->second->ident + "@" : "") << it->second->host;
				delete it->second;
				Requests.erase(it);
			}
			else
				source.Reply(_("No request for nick %s found."), nick.c_str());
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		RequestMap::iterator it = Requests.find(nick);
		if (it != Requests.end())
		{
			delete it->second;
			Requests.erase(it);

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
			Log(LOG_COMMAND, u, this, NULL) << "to reject vhost for " << nick << " (" << (!reason.empty() ? reason : "") << ")";
		}
		else
			source.Reply(_("No request for nick %s found."), nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Reject the requested vHost for the given nick."));
		if (HSRequestMemoUser)
			source.Reply(_("A memo informing the user will also be sent."));

		return true;
	}
};

class HSListBase : public Command
{
 protected:
	void DoList(CommandSource &source)
	{
		int counter = 1;
		int from = 0, to = 0;
		unsigned display_counter = 0;

		for (RequestMap::iterator it = Requests.begin(), it_end = Requests.end(); it != it_end; ++it)
		{
			HostRequest *hr = it->second;
			if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config->NSListMax)
			{
				++display_counter;
				if (!hr->ident.empty())
					source.Reply(_("#%d Nick:\002%s\002, vhost:\002%s\002@\002%s\002 (%s - %s)"), counter, it->first.c_str(), hr->ident.c_str(), hr->host.c_str(), it->first.c_str(), do_strftime(hr->time).c_str());
				else
					source.Reply(_("#%d Nick:\002%s\002, vhost:\002%s\002 (%s - %s)"), counter, it->first.c_str(), hr->host.c_str(), it->first.c_str(), do_strftime(hr->time).c_str());
			}
			++counter;
		}
		source.Reply(_("Displayed all records (Count: \002%d\002)"), display_counter);

		return;
	}
 public:
	HSListBase(Module *creator, const Anope::string &cmd, int min, int max) : Command(creator, cmd, min, max)
	{
	}
};

class CommandHSWaiting : public HSListBase
{
 public:
	CommandHSWaiting(Module *creator) : HSListBase(creator, "hostserv/waiting", 0, 0)
	{
		this->SetDesc(_("Retrieves the vhost requests"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoList(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command retrieves the vhost requests"));

		return true;
	}
};

class HSRequest : public Module
{
	CommandHSRequest commandhsrequest;
	CommandHSActivate commandhsactive;
	CommandHSReject commandhsreject;
	CommandHSWaiting commandhswaiting;

 public:
	HSRequest(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandhsrequest(this), commandhsactive(this), commandhsreject(this), commandhswaiting(this)
	{
		this->SetAuthor("Anope");


		Implementation i[] = { I_OnDelNick, I_OnDatabaseRead, I_OnDatabaseWrite, I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	~HSRequest()
	{
		/* Clean up all open host requests */
		while (!Requests.empty())
		{
			delete Requests.begin()->second;
			Requests.erase(Requests.begin());
		}
	}

	void OnDelNick(NickAlias *na)
	{
		RequestMap::iterator it = Requests.find(na->nick);

		if (it != Requests.end())
		{
			delete it->second;
			Requests.erase(it);
		}
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("HS_REQUEST") && params.size() >= 5)
		{
			Anope::string vident = params[2].equals_ci("(null)") ? "" : params[2];
			my_add_host_request(params[1], vident, params[3], params[1], params[4].is_pos_number_only() ? convertTo<time_t>(params[4]) : 0);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const Anope::string &))
	{
		for (RequestMap::iterator it = Requests.begin(), it_end = Requests.end(); it != it_end; ++it)
		{
			HostRequest *hr = it->second;
			std::stringstream buf;
			buf << "HS_REQUEST " << it->first << " " << (hr->ident.empty() ? "(null)" : hr->ident) << " " << hr->host << " " << hr->time;
			Write(buf.str());
		}
	}

	void OnReload()
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
			
			NickAlias *na = findnick(o->name);
			if (!na)
				continue;

			Anope::string message = Anope::printf(_("[auto memo] vHost \002%s\002 has been requested by %s."), host.c_str(), source.u->GetMask().c_str());

			memoserv->Send(Config->HostServ, na->nick, message, true);
		}
}

void my_add_host_request(const Anope::string &nick, const Anope::string &vIdent, const Anope::string &vhost, const Anope::string &creator, time_t tmp_time)
{
	HostRequest *hr = new HostRequest;
	hr->ident = vIdent;
	hr->host = vhost;
	hr->time = tmp_time;
	RequestMap::iterator it = Requests.find(nick);
	if (it != Requests.end())
	{
		delete it->second;
		Requests.erase(it);
	}
	Requests.insert(std::make_pair(nick, hr));
}

void my_load_config()
{
	ConfigReader config;
	HSRequestMemoUser = config.ReadFlag("hs_request", "memouser", "no", 0);
	HSRequestMemoOper = config.ReadFlag("hs_request", "memooper", "no", 0);

	Log(LOG_DEBUG) << "[hs_request] Set config vars: MemoUser=" << HSRequestMemoUser << " MemoOper=" <<  HSRequestMemoOper;
}

MODULE_INIT(HSRequest)
