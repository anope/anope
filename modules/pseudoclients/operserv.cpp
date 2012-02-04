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

static BotInfo *OperServ;

class SGLineManager : public XLineManager
{
 public:
	SGLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sgline", 'G') { }

	void OnMatch(User *u, XLine *x)
	{
		if (u)
			u->Kill(Config->OperServ, x->Reason);
		this->Send(u, x);
	}

	void OnExpire(XLine *x)
	{
		if (Config->WallAkillExpire)
			ircdproto->SendGlobops(OperServ, "AKILL on %s has expired", x->Mask.c_str());
	}
	
	void Send(User *u, XLine *x)
	{
		try
		{
			if (!ircd->szline)
				throw SocketException("SZLine is not supported");
			else if (x->GetUser() != "*")
				throw SocketException("Can not ZLine a username");
			x->GetIP();
			ircdproto->SendSZLine(u, x);
		}
		catch (const SocketException &)
		{
			ircdproto->SendAkill(u, x);
		}
	}

	void SendDel(XLine *x)
	{
		try
		{
			if (!ircd->szline)
				throw SocketException("SZLine is not supported");
			else if (x->GetUser() != "*")
				throw SocketException("Can not ZLine a username");
			x->GetIP();
			ircdproto->SendSZLineDel(x);
		}
		catch (const SocketException &)
		{
			ircdproto->SendAkillDel(x);
		}
	}
};

class SQLineManager : public XLineManager
{
 public:
	SQLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sqline", 'Q') { }

	void OnMatch(User *u, XLine *x)
	{
		if (u)
		{
			Anope::string reason = "Q-Lined: " + x->Reason;
			u->Kill(Config->OperServ, reason);
		}

		this->Send(u, x);
	}

	void OnExpire(XLine *x)
	{
		if (Config->WallSQLineExpire)
			ircdproto->SendGlobops(OperServ, "SQLINE on \2%s\2 has expired", x->Mask.c_str());
	}

	void Send(User *u, XLine *x)
	{
		ircdproto->SendSQLine(u, x);
	}

	void SendDel(XLine *x)
	{
		ircdproto->SendSQLineDel(x);
	}

	bool CheckChannel(Channel *c)
	{
		if (ircd->chansqline)
			for (std::vector<XLine *>::const_iterator it = this->GetList().begin(), it_end = this->GetList().end(); it != it_end; ++it)
				if (Anope::Match(c->name, (*it)->Mask))
					return true;
		return false;
	}
};

class SNLineManager : public XLineManager
{
 public:
	SNLineManager(Module *creator) : XLineManager(creator, "xlinemanager/snline", 'N') { }

	void OnMatch(User *u, XLine *x)
	{
		if (u)
		{
			Anope::string reason = "G-Lined: " + x->Reason;
			u->Kill(Config->OperServ, reason);
		}
		this->Send(u, x);
	}

	void OnExpire(XLine *x)
	{
		if (Config->WallSNLineExpire)
			ircdproto->SendGlobops(OperServ, "SNLINE on \2%s\2 has expired", x->Mask.c_str());
	}

	void Send(User *u, XLine *x)
	{
		ircdproto->SendSGLine(u, x);
	}

	void SendDel(XLine *x)
	{
		ircdproto->SendSGLineDel(x);
	}

	XLine *Check(User *u)
	{
		for (unsigned i = this->GetList().size(); i > 0; --i)
		{
			XLine *x = this->GetList()[i - 1];

			if (x->Expires && x->Expires < Anope::CurTime)
			{
				this->OnExpire(x);
				this->DelXLine(x);
				continue;
			}

			if (Anope::Match(u->realname, x->Mask))
			{
				this->OnMatch(u, x);
				return x;
			}
		}

		return NULL;
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

		OperServ = findbot(Config->OperServ);
		if (OperServ == NULL)
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

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message)
	{
		if (Config->OSOpersOnly && !u->HasMode(UMODE_OPER) && bi->nick == Config->OperServ)
		{
			u->SendMessage(bi, ACCESS_DENIED);
			if (Config->WallBadOS)
				ircdproto->SendGlobops(OperServ, "Denied access to %s from %s!%s@%s (non-oper)", Config->OperServ.c_str(), u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str());
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnServerQuit(Server *server)
	{
		if (server->HasFlag(SERVER_JUPED))
			ircdproto->SendGlobops(OperServ, "Received SQUIT for juped server %s", server->GetName().c_str());
	}

	void OnUserModeSet(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
		{
			if (Config->WallOper)
				ircdproto->SendGlobops(OperServ, "\2%s\2 is now an IRC operator.", u->nick.c_str());
			Log(OperServ) << u->nick << " is now an IRC operator";
		}
	}

	void OnUserModeUnset(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
			Log(OperServ) << u->nick << " is no longer an IRC operator";
	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt)
	{
		if (u && !exempt)
			XLineManager::CheckAll(u);
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick)
	{
		if (ircd->sqline && !u->HasMode(UMODE_OPER))
			this->sqlines.Check(u);
	}

	EventReturn OnCheckKick(User *u, ChannelInfo *ci, bool &kick)
	{
		if (this->sqlines.CheckChannel(ci->c))
			kick = true;
		return EVENT_CONTINUE;
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->OperServ)
			return EVENT_CONTINUE;
		source.Reply(_("%s commands:"), Config->OperServ.c_str());
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OperServCore)

