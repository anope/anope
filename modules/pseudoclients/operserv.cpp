/* OperServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class SGLineManager : public XLineManager
{
 public:
	SGLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sgline", 'G') { }

	void OnMatch(User *u, XLine *x) anope_override
	{
		this->Send(u, x);
	}

	void OnExpire(const XLine *x) anope_override
	{
		Log(OperServ, "expire/akill") << "AKILL on \002" << x->mask << "\002 has expired";
	}
	
	void Send(User *u, XLine *x) anope_override
	{
		IRCD->SendAkill(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		IRCD->SendAkillDel(x);
	}

	bool Check(User *u, const XLine *x) anope_override
	{
		if (x->regex)
		{
			Anope::string uh = u->GetIdent() + "@" + u->host, nuhr = u->nick + "!" + uh + "#" + u->realname;
			if (x->regex->Matches(uh) || x->regex->Matches(nuhr))
				return true;

			return false;
		}

		if (!x->GetNick().empty() && !Anope::Match(u->nick, x->GetNick()))
			return false;

		if (!x->GetUser().empty() && !Anope::Match(u->GetIdent(), x->GetUser()))
			return false;

		if (!x->GetReal().empty() && !Anope::Match(u->realname, x->GetReal()))
			return false;

		if (x->GetHost().find('/') != Anope::string::npos)
		{
			try
			{
				cidr cidr_ip(x->GetHost());
				sockaddrs ip(u->ip);
				if (cidr_ip.match(ip))
					return true;
			}
			catch (const SocketException &) { }
		}

		if (x->GetHost().empty() || Anope::Match(u->host, x->GetHost()) || Anope::Match(u->ip, x->GetHost()))
			return true;

		return false;
	}
};

class SQLineManager : public XLineManager
{
 public:
	SQLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sqline", 'Q') { }

	void OnMatch(User *u, XLine *x) anope_override
	{
		this->Send(u, x);
	}

	void OnExpire(const XLine *x) anope_override
	{
		Log(OperServ, "expire/sqline") << "SQLINE on \002" << x->mask << "\002 has expired";
	}

	void Send(User *u, XLine *x) anope_override
	{
		IRCD->SendSQLine(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		IRCD->SendSQLineDel(x);
	}

	bool Check(User *u, const XLine *x) anope_override
	{
		if (x->regex)
			return x->regex->Matches(u->nick);
		return Anope::Match(u->nick, x->mask);
	}

	bool CheckChannel(Channel *c)
	{
		for (std::vector<XLine *>::const_iterator it = this->GetList().begin(), it_end = this->GetList().end(); it != it_end; ++it)
			if (Anope::Match(c->name, (*it)->mask, false, true))
				return true;
		return false;
	}
};

class SNLineManager : public XLineManager
{
 public:
	SNLineManager(Module *creator) : XLineManager(creator, "xlinemanager/snline", 'N') { }

	void OnMatch(User *u, XLine *x) anope_override
	{
		this->Send(u, x);
	}

	void OnExpire(const XLine *x) anope_override
	{
		Log(OperServ, "expire/snline") << "SNLINE on \002" << x->mask << "\002 has expired";
	}

	void Send(User *u, XLine *x) anope_override
	{
		IRCD->SendSGLine(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		IRCD->SendSGLineDel(x);
	}

	bool Check(User *u, const XLine *x) anope_override
	{
		if (x->regex)
			return x->regex->Matches(u->realname);
		return Anope::Match(u->realname, x->mask, false, true);
	}
};

class OperServCore : public Module
{
	SGLineManager sglines;
	SQLineManager sqlines;
	SNLineManager snlines;

 public:
	OperServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR),
		sglines(this), sqlines(this), snlines(this)
	{
		Implementation i[] = { I_OnReload, I_OnBotDelete, I_OnBotPrivmsg, I_OnServerQuit, I_OnUserModeSet, I_OnUserModeUnset, I_OnUserConnect, I_OnUserNickChange, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		/* Yes, these are in this order for a reason. Most violent->least violent. */
		XLineManager::RegisterXLineManager(&sglines);
		XLineManager::RegisterXLineManager(&sqlines);
		XLineManager::RegisterXLineManager(&snlines);
	}

	~OperServCore()
	{
		OperServ = NULL;

		this->sglines.Clear();
		this->sqlines.Clear();
		this->snlines.Clear();

		XLineManager::UnregisterXLineManager(&sglines);
		XLineManager::UnregisterXLineManager(&sqlines);
		XLineManager::UnregisterXLineManager(&snlines);
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		const Anope::string &osnick = conf->GetModule(this)->Get<const Anope::string &>("client");

		if (osnick.empty())
			throw ConfigException(this->name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(osnick, true);
		if (!bi)
			throw ConfigException(this->name + ": no bot named " + osnick);

		OperServ = bi;
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		if (bi == OperServ)
			OperServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_override
	{
		if (bi == OperServ && !u->HasMode("OPER") && Config->GetModule(this)->Get<bool>("opersonly"))
		{
			u->SendMessage(bi, ACCESS_DENIED);
			Log(OperServ, "bados") << "Denied access to " << bi->nick << " from " << u->GetMask() << " (non-oper)";
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnServerQuit(Server *server) anope_override
	{
		if (server->IsJuped())
			Log(server, "squit", OperServ) << "Received SQUIT for juped server " << server->GetName();
	}

	void OnUserModeSet(User *u, const Anope::string &mname) anope_override
	{
		if (mname == "OPER")
			Log(u, "oper", OperServ) << "is now an IRC operator.";
	}

	void OnUserModeUnset(User *u, const Anope::string &mname) anope_override
	{
		if (mname == "OPER")
			Log(u, "oper", OperServ) << "is no longer an IRC operator";
	}

	void OnUserConnect(User *u, bool &exempt) anope_override
	{
		if (!u->Quitting() && !exempt)
			XLineManager::CheckAll(u);
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		if (IRCD->CanSQLine && !u->HasMode("OPER"))
			this->sqlines.CheckAllXLines(u);
	}

	EventReturn OnCheckKick(User *u, ChannelInfo *ci, Anope::string &mask, Anope::string &reason) anope_override
	{
		return this->sqlines.CheckChannel(ci->c) ? EVENT_STOP : EVENT_CONTINUE;
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service != OperServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), OperServ->nick.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OperServCore)

