/* OperServ functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "operserv.h"

XLineManager *SGLine = NULL, *SZLine = NULL, *SQLine = NULL, *SNLine = NULL;

/* List of XLine managers we check users against in XLineManager::CheckAll */
std::list<XLineManager *> XLineManager::XLineManagers;

XLine::XLine(const Anope::string &mask, const Anope::string &reason) : Mask(mask), Created(0), Expires(0), Reason(reason)
{
}

XLine::XLine(const Anope::string &mask, const Anope::string &by, const time_t expires, const Anope::string &reason) : Mask(mask), By(by), Created(Anope::CurTime), Expires(expires), Reason(reason)
{
}

Anope::string XLine::GetNick() const
{
	size_t nick_t = this->Mask.find('!');

	if (nick_t == Anope::string::npos)
		return "";

	return this->Mask.substr(0, nick_t);
}

Anope::string XLine::GetUser() const
{
	size_t user_t = this->Mask.find('!'), host_t = this->Mask.find('@');

	if (host_t != Anope::string::npos)
	{
		if (user_t != Anope::string::npos)
			return this->Mask.substr(user_t + 1, host_t - user_t - 1);
		else
			return this->Mask.substr(0, host_t);
	}
	else
		return "";
}

Anope::string XLine::GetHost() const
{
	size_t host_t = this->Mask.find('@');

	if (host_t == Anope::string::npos)
		return this->Mask;
	else
		return this->Mask.substr(host_t + 1);
}

/** Constructor
 */
XLineManager::XLineManager()
{
}

/** Destructor
 * Clears all XLines in this XLineManager
 */
XLineManager::~XLineManager()
{
	this->Clear();
}

 /** Register a XLineManager, places it in XLineManagers for use in XLineManager::CheckAll
  * It is important XLineManagers are registered in the proper order. Eg, if you had one akilling
  * clients and one handing them free olines, you would want the akilling one first. This way if a client
  * matches an entry on both of the XLineManagers, they would be akilled.
  * @param xlm THe XLineManager
  */
void XLineManager::RegisterXLineManager(XLineManager *xlm)
{
	XLineManagers.push_back(xlm);
}

/** Unregister a XLineManager
 * @param xlm The XLineManager
 */
void XLineManager::UnregisterXLineManager(XLineManager *xlm)
{
	std::list<XLineManager *>::iterator it = std::find(XLineManagers.begin(), XLineManagers.end(), xlm);

	if (it != XLineManagers.end())
		XLineManagers.erase(it);
}

/* Check a user against all known XLineManagers
 * @param u The user
 * @return A pair of the XLineManager the user was found in and the XLine they matched, both may be NULL for no match
 */
std::pair<XLineManager *, XLine *> XLineManager::CheckAll(User *u)
{
	std::pair<XLineManager *, XLine *> ret;

	ret.first = NULL;
	ret.second = NULL;

	for (std::list<XLineManager *>::iterator it = XLineManagers.begin(), it_end = XLineManagers.end(); it != it_end; ++it)
	{
		XLineManager *xlm = *it;

		XLine *x = xlm->Check(u);

		if (x)
		{
			ret.first = xlm;
			ret.second = x;
			break;
		}
	}

	return ret;
}

/** Get the number of XLines in this XLineManager
 * @return The number of XLines
 */
size_t XLineManager::GetCount() const
{
	return this->XLines.size();
}

/** Get the XLine vector
  * @return The vecotr
  */
const std::vector<XLine *> &XLineManager::GetList() const
{
	return this->XLines;
}

/** Add an entry to this XLineManager
 * @param x The entry
 */
void XLineManager::AddXLine(XLine *x)
{
	this->XLines.push_back(x);
}

/** Delete an entry from this XLineManager
 * @param x The entry
 * @return true if the entry was found and deleted, else false
 */
bool XLineManager::DelXLine(XLine *x)
{
	std::vector<XLine *>::iterator it = std::find(this->XLines.begin(), this->XLines.end(), x);

	if (it != this->XLines.end())
	{
		this->Del(x);

		delete x;
		this->XLines.erase(it);

		return true;
	}

	return false;
}

/** Gets an entry by index
  * @param index The index
  * @return The XLine, or NULL if the index is out of bounds
  */
XLine *XLineManager::GetEntry(unsigned index)
{
	if (index >= this->XLines.size())
		return NULL;

	return this->XLines[index];
}

/** Clear the XLine vector
 */
void XLineManager::Clear()
{
	while (!this->XLines.empty())
		this->DelXLine(this->XLines.front());
}

/** Add an entry to this XLine Manager
 * @param mask The mask of the XLine
 * @param creator The creator of the XLine
 * @param expires When this should expire
 * @param reaosn The reason
 * @return A pointer to the XLine
 */
XLine *XLineManager::Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
{
	return NULL;
}

/** Delete an XLine, eg, remove it from the IRCd.
 * @param x The xline
 */
void XLineManager::Del(XLine *x)
{
}

/** Checks if a mask can/should be added to the XLineManager
 * @param mask The mask
 * @param expires When the mask would expire
 * @return A pair of int and XLine*.
 * 1 - Mask already exists
 * 2 - Mask already exists, but the expiry time was changed
 * 3 - Mask is already covered by another mask
 * In each case the XLine it matches/is covered by is returned in XLine*
 */
std::pair<int, XLine *> XLineManager::CanAdd(const Anope::string &mask, time_t expires)
{
	std::pair<int, XLine *> ret;

	ret.first = 0;
	ret.second = NULL;

	for (unsigned i = this->GetCount(); i > 0; --i)
	{
		XLine *x = this->GetEntry(i - 1);
		ret.second = x;

		if (x->Mask.equals_ci(mask))
		{
			if (!x->Expires || x->Expires >= expires)
			{
				ret.first = 1;
				break;
			}
			else
			{
				x->Expires = expires;

				ret.first = 2;
				break;
			}
		}
		else if (Anope::Match(mask, x->Mask) && (!x->Expires || x->Expires >= expires))
		{
			ret.first = 3;
			break;
		}
		else if (Anope::Match(x->Mask, mask) && (!expires || x->Expires <= expires))
		{
			this->DelXLine(x);
		}
	}

	return ret;
}

/** Checks if this list has an entry
 * @param mask The mask
 * @return The XLine the user matches, or NULL
 */
XLine *XLineManager::HasEntry(const Anope::string &mask)
{
	for (unsigned i = 0, end = this->XLines.size(); i < end; ++i)
	{
		XLine *x = this->XLines[i];

		if (x->Mask.equals_ci(mask))
			return x;
	}

	return NULL;
}

/** Check a user against all of the xlines in this XLineManager
 * @param u The user
 * @return The xline the user marches, if any. Also calls OnMatch()
 */
XLine *XLineManager::Check(User *u)
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

		if (!x->GetNick().empty() && !Anope::Match(u->nick, x->GetNick()))
			continue;

		if (!x->GetUser().empty() && !Anope::Match(u->GetIdent(), x->GetUser()))
			continue;

		if (u->ip() && !x->GetHost().empty())
		{
			try
			{
				cidr cidr_ip(x->GetHost());
				if (cidr_ip.match(u->ip))
				{
					OnMatch(u, x);
					return x;
				}
			}
			catch (const SocketException &) { }
		}

		if (x->GetHost().empty() || (Anope::Match(u->host, x->GetHost()) || (!u->chost.empty() && Anope::Match(u->chost, x->GetHost())) || (!u->vhost.empty() && Anope::Match(u->vhost, x->GetHost()))))
		{
			OnMatch(u, x);
			return x;
		}
	}

	return NULL;
}

/** Called when a user matches a xline in this XLineManager
 * @param u The user
 * @param x The XLine they match
 */
void XLineManager::OnMatch(User *u, XLine *x)
{
}

/** Called when an XLine expires
 * @param x The xline
 */
void XLineManager::OnExpire(XLine *x)
{
}

XLine *SGLineManager::Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
{
	Anope::string realreason = reason;
	if (!creator.empty() && Config->AddAkiller)
		realreason = "[" + creator + "] " + reason;

	XLine *x = new XLine(mask, creator, expires, realreason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddAkill, OnAddAkill(x));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (UplinkSock && Config->AkillOnAdd)
		this->Send(NULL, x);

	return x;
}

void SGLineManager::Del(XLine *x)
{
	ircdproto->SendAkillDel(x);
}

void SGLineManager::OnMatch(User *u, XLine *x)
{
	if (u)
		u->Kill(Config->s_OperServ, x->Reason);
	ircdproto->SendAkill(u, x);
}

void SGLineManager::OnExpire(XLine *x)
{
	if (Config->WallAkillExpire && operserv)
		ircdproto->SendGlobops(operserv->Bot(), "AKILL on %s has expired", x->Mask.c_str());
}

void SGLineManager::Send(User *u, XLine *x)
{
	ircdproto->SendAkill(u, x);
}

XLine *SNLineManager::Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
{
	XLine *x = new XLine(mask, creator, expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(x, X_SNLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

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

void SNLineManager::Del(XLine *x)
{
	ircdproto->SendSGLineDel(x);
}

void SNLineManager::OnMatch(User *u, XLine *x)
{
	if (u)
	{
		Anope::string reason = "G-Lined: " + x->Reason;
		u->Kill(Config->s_OperServ, reason);
	}
	this->Send(u, x);
}

void SNLineManager::OnExpire(XLine *x)
{
	if (Config->WallSNLineExpire && operserv)
		ircdproto->SendGlobops(operserv->Bot(), "SNLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SNLineManager::Send(User *u, XLine *x)
{
	ircdproto->SendSGLine(u, x);
}

XLine *SNLineManager::Check(User *u)
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

XLine *SQLineManager::Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
{
	XLine *x = new XLine(mask, creator, expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(x, X_SQLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

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

void SQLineManager::Del(XLine *x)
{
	ircdproto->SendSQLineDel(x);
}

void SQLineManager::OnMatch(User *u, XLine *x)
{
	if (u)
	{
		Anope::string reason = "Q-Lined: " + x->Reason;
		u->Kill(Config->s_OperServ, reason);
	}

	this->Send(u, x);
}

void SQLineManager::OnExpire(XLine *x)
{
	if (Config->WallSQLineExpire && operserv)
		ircdproto->SendGlobops(operserv->Bot(), "SQLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SQLineManager::Send(User *u, XLine *x)
{
	ircdproto->SendSQLine(u, x);
}

bool SQLineManager::Check(Channel *c)
{
	if (ircd->chansqline && SQLine)
	{
		for (std::vector<XLine *>::const_iterator it = SQLine->GetList().begin(), it_end = SQLine->GetList().end(); it != it_end; ++it)
		{
			XLine *x = *it;

			if (Anope::Match(c->name, x->Mask))
				return true;
		}
	}

	return false;
}

XLine *SZLineManager::Add(const Anope::string &mask, const Anope::string &creator, time_t expires, const Anope::string &reason)
{
	XLine *x = new XLine(mask, creator, expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(x, X_SZLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (UplinkSock)
		this->Send(NULL, x);

	return x;
}

void SZLineManager::Del(XLine *x)
{
	ircdproto->SendSZLineDel(x);
}

void SZLineManager::OnMatch(User *u, XLine *x)
{
	if (u)
	{
		Anope::string reason = "Z-Lined: " + x->Reason;
		u->Kill(Config->s_OperServ, reason);
	}

	ircdproto->SendSZLine(u, x);
}

void SZLineManager::OnExpire(XLine *x)
{
	if (Config->WallSZLineExpire && operserv)
		ircdproto->SendGlobops(operserv->Bot(), "SZLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SZLineManager::Send(User *u, XLine *x)
{
	ircdproto->SendSZLine(u, x);
}

