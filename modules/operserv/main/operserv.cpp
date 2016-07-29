/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/help.h"
#include "modules/nickserv.h"

class SGLineManager : public XLineManager
{
 public:
	SGLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sgline", 'G') { }

	void OnMatch(User *u, XLine *x) override
	{
		this->Send(u, x);
	}

	void OnExpire(XLine *x) override
	{
		::Log(Config->GetClient("OperServ"), "expire/akill") << "AKILL on \002" << x->GetMask() << "\002 has expired";
	}

	void Send(User *u, XLine *x) override
	{
		IRCD->SendAkill(u, x);
	}

	void SendDel(XLine *x) override
	{
		IRCD->SendAkillDel(x);
	}

	bool Check(User *u, XLine *x) override
	{
		if (x->regex)
		{
			Anope::string uh = u->GetIdent() + "@" + u->host, nuhr = u->nick + "!" + uh + "#" + u->realname;
			return std::regex_match(uh.str(), *x->regex) || std::regex_match(nuhr.str(), *x->regex);
		}

		if (!x->GetNick().empty() && !Anope::Match(u->nick, x->GetNick()))
			return false;

		if (!x->GetUser().empty() && !Anope::Match(u->GetIdent(), x->GetUser()))
			return false;

		if (!x->GetReal().empty() && !Anope::Match(u->realname, x->GetReal()))
			return false;

		if (x->c && x->c->match(u->ip))
			return true;

		if (x->GetHost().empty() || Anope::Match(u->host, x->GetHost()) || Anope::Match(u->ip.addr(), x->GetHost()))
			return true;

		return false;
	}
};

class SQLineManager : public XLineManager
{
 public:
	SQLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sqline", 'Q') { }

	void OnMatch(User *u, XLine *x) override
	{
		this->Send(u, x);
	}

	void OnExpire(XLine *x) override
	{
		::Log(Config->GetClient("OperServ"), "expire/sqline") << "SQLINE on \002" << x->GetMask() << "\002 has expired";
	}

	void Send(User *u, XLine *x) override
	{
		if (!IRCD->CanSQLine)
		{
			if (!u)
				;
			else if (NickServ::service)
				NickServ::service->Collide(u, NULL);
			else
				u->Kill(Config->GetClient("OperServ"), "Q-Lined: " + x->GetReason());
		}
		else if (x->IsRegex())
			;
		else if (x->GetMask()[0] != '#' || IRCD->CanSQLineChannel)
			IRCD->SendSQLine(u, x);
	}

	void SendDel(XLine *x) override
	{
		if (!IRCD->CanSQLine || x->IsRegex())
			;
		else if (x->GetMask()[0] != '#' || IRCD->CanSQLineChannel)
			IRCD->SendSQLineDel(x);
	}

	bool Check(User *u, XLine *x) override
	{
		if (x->regex)
			return std::regex_match(u->nick.c_str(), *x->regex);
		return Anope::Match(u->nick, x->GetMask());
	}

	XLine *CheckChannel(Channel *c)
	{
		for (XLine *x : this->GetXLines())
		{
			if (x->regex)
			{
				if (std::regex_match(c->name.str(), *x->regex))
					return x;
			}
			else
			{
				if (x->GetMask().empty() || x->GetMask()[0] != '#')
					continue;

				if (Anope::Match(c->name, x->GetMask(), false, true))
					return x;
			}
		}
		return nullptr;
	}
};

class SNLineManager : public XLineManager
{
 public:
	SNLineManager(Module *creator) : XLineManager(creator, "xlinemanager/snline", 'N') { }

	void OnMatch(User *u, XLine *x) override
	{
		this->Send(u, x);
	}

	void OnExpire(XLine *x) override
	{
		::Log(Config->GetClient("OperServ"), "expire/snline") << "SNLINE on \002" << x->GetMask() << "\002 has expired";
	}

	void Send(User *u, XLine *x) override
	{
		if (IRCD->CanSNLine && !x->IsRegex())
			IRCD->SendSGLine(u, x);

		if (u)
			u->Kill(Config->GetClient("OperServ"), "SNLined: " + x->GetReason());
	}

	void SendDel(XLine *x) override
	{
		if (IRCD->CanSNLine && !x->IsRegex())
			IRCD->SendSGLineDel(x);
	}

	bool Check(User *u, XLine *x) override
	{
		if (x->regex)
			return std::regex_match(u->realname.str(), *x->regex);
		return Anope::Match(u->realname, x->GetMask(), false, true);
	}
};

class OperServCore : public Module
	, public EventHook<Event::BotPrivmsg>
	, public EventHook<Event::ServerQuit>
	, public EventHook<Event::UserModeSet>
	, public EventHook<Event::UserModeUnset>
	, public EventHook<Event::UserConnect>
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::CheckKick>
	, public EventHook<Event::Help>
	, public EventHook<Event::Log>
{
	Reference<ServiceBot> OperServ;
	SGLineManager sglines;
	SQLineManager sqlines;
	SNLineManager snlines;

 public:
	OperServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, EventHook<Event::BotPrivmsg>(this)
		, EventHook<Event::ServerQuit>(this)
		, EventHook<Event::UserModeSet>(this)
		, EventHook<Event::UserModeUnset>(this)
		, EventHook<Event::UserConnect>(this)
		, EventHook<Event::UserNickChange>(this)
		, EventHook<Event::CheckKick>(this)
		, EventHook<Event::Help>(this)
		, EventHook<Event::Log>(this)
		, sglines(this)
		, sqlines(this)
		, snlines(this)
	{

		/* Yes, these are in this order for a reason. Most violent->least violent. */
		XLineManager::RegisterXLineManager(&sglines);
		XLineManager::RegisterXLineManager(&sqlines);
		XLineManager::RegisterXLineManager(&snlines);
	}

	~OperServCore()
	{
		this->sglines.Clear();
		this->sqlines.Clear();
		this->snlines.Clear();

		XLineManager::UnregisterXLineManager(&sglines);
		XLineManager::UnregisterXLineManager(&sqlines);
		XLineManager::UnregisterXLineManager(&snlines);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &osnick = conf->GetModule(this)->Get<Anope::string>("client");

		if (osnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(osnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + osnick);

		OperServ = bi;
	}

	EventReturn OnBotPrivmsg(User *u, ServiceBot *bi, Anope::string &message) override
	{
		if (bi == OperServ && !u->HasMode("OPER") && Config->GetModule(this)->Get<bool>("opersonly"))
		{
			u->SendMessage(bi, "Access denied.");
			::Log(bi, "bados") << "Denied access to " << bi->nick << " from " << u->GetMask() << " (non-oper)";
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnServerQuit(Server *server) override
	{
		if (server->IsJuped())
			::Log(server, "squit", OperServ) << "Received SQUIT for juped server " << server->GetName();
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (mname == "OPER")
			::Log(u, "oper", OperServ) << "is now an IRC operator.";
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (mname == "OPER")
			::Log(u, "oper", OperServ) << "is no longer an IRC operator";
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (!u->Quitting() && !exempt)
			XLineManager::CheckAll(u);
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) override
	{
		if (!u->HasMode("OPER"))
			this->sqlines.CheckAllXLines(u);
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		XLine *x = this->sqlines.CheckChannel(c);
		if (x)
		{
			this->sqlines.OnMatch(u, x);
			reason = x->GetReason();
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *OperServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), OperServ->nick.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
	}

	void OnLog(::Log *l) override
	{
		if (l->type == LOG_SERVER)
			l->bi = OperServ;
	}
};

MODULE_INIT(OperServCore)

