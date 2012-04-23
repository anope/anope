/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
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
		Log(findbot(Config->OperServ), "expire/akill") << "AKILL on \2" << x->Mask << "\2 has expired";
	}
	
	void Send(User *u, XLine *x) anope_override
	{
		ircdproto->SendAkill(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		try
		{
			if (!ircd->szline)
				throw SocketException("SZLine is not supported");
			else if (x->GetUser() != "*")
				throw SocketException("Can not ZLine a username");
			sockaddrs(x->GetHost());
			ircdproto->SendSZLineDel(x);
		}
		catch (const SocketException &)
		{
			ircdproto->SendAkillDel(x);
		}
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

		if (!x->GetHost().empty())
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

		if (x->GetHost().empty() || Anope::Match(u->host, x->GetHost()))
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
		Log(findbot(Config->OperServ), "expire/sqline") << "SQLINE on \2" << x->Mask << "\2 has expired";
	}

	void Send(User *u, XLine *x) anope_override
	{
		ircdproto->SendSQLine(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		ircdproto->SendSQLineDel(x);
	}

	bool Check(User *u, const XLine *x) anope_override
	{
		if (x->regex)
			return x->regex->Matches(u->nick);
		return Anope::Match(u->nick, x->Mask);
	}

	bool CheckChannel(Channel *c)
	{
		for (std::vector<XLine *>::const_iterator it = this->GetList().begin(), it_end = this->GetList().end(); it != it_end; ++it)
			if (Anope::Match(c->name, (*it)->Mask, false, true))
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
		Log(findbot(Config->OperServ), "expire/snline") << "SNLINE on \2" << x->Mask << "\2 has expired";
	}

	void Send(User *u, XLine *x) anope_override
	{
		ircdproto->SendSGLine(u, x);
	}

	void SendDel(XLine *x) anope_override
	{
		ircdproto->SendSGLineDel(x);
	}

	bool Check(User *u, const XLine *x) anope_override
	{
		if (x->regex)
			return x->regex->Matches(u->realname);
		return Anope::Match(u->realname, x->Mask, false, true);
	}
};

class OperServCore : public Module
{
	SGLineManager sglines;
	SQLineManager sqlines;
	SNLineManager snlines;

 public:
	OperServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		sglines(this), sqlines(this), snlines(this)
	{
		this->SetAuthor("Anope");

		if (!findbot(Config->OperServ))
			throw ModuleException("No bot named " + Config->OperServ);

		Implementation i[] = { I_OnBotPrivmsg, I_OnServerQuit, I_OnUserModeSet, I_OnUserModeUnset, I_OnUserConnect, I_OnUserNickChange, I_OnPreHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

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

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_override
	{
		if (Config->OSOpersOnly && !u->HasMode(UMODE_OPER) && bi->nick == Config->OperServ)
		{
			u->SendMessage(bi, ACCESS_DENIED);
			Log(findbot(Config->OperServ), "bados") << "Denied access to " << Config->OperServ << " from " << u->GetMask() << " (non-oper)";
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnServerQuit(Server *server) anope_override
	{
		if (server->HasFlag(SERVER_JUPED))
			Log(server, "squit", findbot(Config->OperServ)) << "Received SQUIT for juped server " << server->GetName();
	}

	void OnUserModeSet(User *u, UserModeName Name) anope_override
	{
		if (Name == UMODE_OPER)
			Log(u, "oper", findbot(Config->OperServ)) << "is now an IRC operator.";
	}

	void OnUserModeUnset(User *u, UserModeName Name) anope_override
	{
		if (Name == UMODE_OPER)
			Log(u, "oper", findbot(Config->OperServ)) << "is no longer an IRC operator";
	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt) anope_override
	{
		if (u && !exempt)
			XLineManager::CheckAll(u);
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		if (ircd->sqline && !u->HasMode(UMODE_OPER))
			this->sqlines.CheckAllXLines(u);
	}

	EventReturn OnCheckKick(User *u, ChannelInfo *ci, bool &kick) anope_override
	{
		if (this->sqlines.CheckChannel(ci->c))
			kick = true;
		return EVENT_CONTINUE;
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.owner->nick != Config->OperServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Config->OperServ.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OperServCore)

