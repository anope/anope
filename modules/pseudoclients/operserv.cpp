/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	XLine *Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
	{
		XLine *x = new XLine(mask, creator, expires, reason);

		this->AddXLine(x);

		if (UplinkSock && Config->AkillOnAdd)
			this->Send(NULL, x);

		return x;
	}

	void Del(XLine *x)
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
};

class SQLineManager : public XLineManager
{
 public:
	SQLineManager(Module *creator) : XLineManager(creator, "xlinemanager/sqline", 'Q') { }

	XLine *Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
	{
		XLine *x = new XLine(mask, creator, expires, reason);

		this->AddXLine(x);

		if (Config->KillonSQline)
		{
			Anope::string rreason = "Q-Lined: " + reason;

			if (mask[0] == '#')
			{
				for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
				{
					Channel *c = cit->second;

					if (!Anope::Match(c->name, mask))
						continue;
					for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
					{
						UserContainer *uc = *it;
						++it;

						if (uc->user->HasMode(UMODE_OPER) || uc->user->server == Me)
							continue;
						c->Kick(NULL, uc->user, "%s", reason.c_str());
					}
				}
			}
			else
			{
				for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end();)
				{
					User *user = it->second;
					++it;

					if (!user->HasMode(UMODE_OPER) && user->server != Me && Anope::Match(user->nick, x->Mask))
						user->Kill(Config->ServerName, rreason);
				}
			}
		}

		if (UplinkSock)
			this->Send(NULL, x);

		return x;
	}

	void Del(XLine *x)
	{
		ircdproto->SendSQLineDel(x);
	}

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

	XLine *Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
	{
		XLine *x = new XLine(mask, creator, expires, reason);

		this->AddXLine(x);

		if (Config->KillonSNline && !ircd->sglineenforce)
		{
			Anope::string rreason = "G-Lined: " + reason;

			for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end();)
			{
				User *user = it->second;
				++it;

				if (!user->HasMode(UMODE_OPER) && user->server != Me && Anope::Match(user->realname, x->Mask))
					user->Kill(Config->ServerName, rreason);
			}
		}

		return x;
	}

	void Del(XLine *x)
	{
		ircdproto->SendSGLineDel(x);
	}

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

	XLine *Check(User *u)
	{
		for (unsigned i = this->XLines.size(); i > 0; --i)
		{
			XLine *x = this->XLines[i - 1];

			if (x->Expires && x->Expires < Anope::CurTime)
			{
				this->OnExpire(x);
				this->Del(x);
				delete x;
				this->XLines.erase(XLines.begin() + i - 1);
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

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message)
	{
		if (Config->OSOpersOnly && !u->HasMode(UMODE_OPER) && bi->nick == Config->OperServ)
		{
			u->SendMessage(bi, ACCESS_DENIED);
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

	void OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->OperServ)
			return;
		source.Reply(_("%s commands:"), Config->OperServ.c_str());
	}
};

MODULE_INIT(OperServCore)

