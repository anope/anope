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

std::vector<NewsItem *> News;

std::vector<std::bitset<32> > DefCon;
bool DefConModesSet = false;
/* Defcon modes mlocked on */
Flags<ChannelModeName, CMODE_END * 2> DefConModesOn(ChannelModeNameStrings);
/* Defcon modes mlocked off */
Flags<ChannelModeName, CMODE_END * 2> DefConModesOff(ChannelModeNameStrings);
/* Map of Modesa and Params for DefCon */
std::map<ChannelModeName, Anope::string> DefConModesOnParams;

XLineManager *SGLine = NULL, *SZLine = NULL, *SQLine = NULL, *SNLine = NULL;

void os_init()
{
	if (!Config->s_OperServ.empty())
	{
		ModuleManager::LoadModuleList(Config->OperServCoreModules);

		/* Yes, these are in this order for a reason. Most violent->least violent. */
		XLineManager::RegisterXLineManager(SGLine = new SGLineManager());
		XLineManager::RegisterXLineManager(SZLine = new SZLineManager());
		XLineManager::RegisterXLineManager(SQLine = new SQLineManager());
		XLineManager::RegisterXLineManager(SNLine = new SNLineManager());
	}
}

bool SetDefConParam(ChannelModeName Name, const Anope::string &buf)
{
	return DefConModesOnParams.insert(std::make_pair(Name, buf)).second;
}

bool GetDefConParam(ChannelModeName Name, Anope::string &buf)
{
	std::map<ChannelModeName, Anope::string>::iterator it = DefConModesOnParams.find(Name);

	buf.clear();

	if (it != DefConModesOnParams.end())
	{
		buf = it->second;
		return true;
	}

	return false;
}

void UnsetDefConParam(ChannelModeName Name)
{
	std::map<ChannelModeName, Anope::string>::iterator it = DefConModesOnParams.find(Name);

	if (it != DefConModesOnParams.end())
		DefConModesOnParams.erase(it);
}

/** Check if a certain defcon option is currently in affect
 * @param Level The level
 * @return true and false
 */
bool CheckDefCon(DefconLevel Level)
{
	if (Config->DefConLevel)
		return DefCon[Config->DefConLevel][Level];
	return false;
}

/** Check if a certain defcon option is in a certain defcon level
 * @param level The defcon level
 * @param Level The defcon level name
 * @return true or false
 */
bool CheckDefCon(int level, DefconLevel Level)
{
	return DefCon[level][Level];
}

/** Add a defcon level option to a defcon level
 * @param level The defcon level
 * @param Level The defcon level option
 */
void AddDefCon(int level, DefconLevel Level)
{
	DefCon[level][Level] = true;
}

/** Remove a defcon level option from a defcon level
 * @param level The defcon level
 * @param Level The defcon level option
 */
void DelDefCon(int level, DefconLevel Level)
{
	DefCon[level][Level] = false;
}

void server_global(const Server *s, const Anope::string &message)
{
	if (Config->s_GlobalNoticer.empty())
		return;

	/* Do not send the notice to ourselves our juped servers */
	if (s != Me && !s->HasFlag(SERVER_JUPED))
		notice_server(Config->s_GlobalNoticer, s, "%s", message.c_str());

	if (!s->GetLinks().empty())
	{
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			server_global(s->GetLinks()[i], message);
	}
}

void oper_global(const Anope::string &nick, const char *fmt, ...)
{
	va_list args;
	char msg[2048]; /* largest valid message is 512, this should cover any global */

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	if (!nick.empty() && !Config->AnonymousGlobal)
	{
		Anope::string rmsg = "[" + nick + "] " + msg;
		server_global(Me->GetLinks().front(), rmsg);
	}
	else
		server_global(Me->GetLinks().front(), msg);
}

/**************************************************************************/

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

void XLineManager::Burst()
{
	for (std::list<XLineManager *>::iterator it = XLineManagers.begin(), it_end = XLineManagers.end(); it != it_end; ++it)
	{
		XLineManager *xlm = *it;
		
		for (std::vector<XLine *>::const_iterator it2 = xlm->GetList().begin(), it2_end = xlm->GetList().end(); it2 != it2_end; ++it2)
			xlm->Send(*it2);
	}
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
 * @param bi The bot error replies should be sent from
 * @param u The user adding the XLine
 * @param mask The mask of the XLine
 * @param expires When this should expire
 * @param reaosn The reason
 * @return A pointer to the XLine
 */
XLine *XLineManager::Add(BotInfo *bi, User *u, const Anope::string &mask, time_t expires, const Anope::string &reason)
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
					return x;
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

XLine *SGLineManager::Add(BotInfo *bi, User *u, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	if (mask.find('!') != Anope::string::npos)
	{
		if (bi && u)
			u->SendMessage(bi, OPER_AKILL_NO_NICK);
		return NULL;
	}

	if (mask.find('@') == Anope::string::npos)
	{
		if (bi && u)
			u->SendMessage(bi, BAD_USERHOST_MASK);
		return NULL;
	}

	if (mask.find_first_not_of("~@.*?") == Anope::string::npos)
	{
		if (bi && u)
			u->SendMessage(bi, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				u->SendMessage(bi, OPER_AKILL_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				u->SendMessage(bi, OPER_EXPIRY_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				u->SendMessage(bi, OPER_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return NULL;
	}

	Anope::string realreason = reason;
	if (u && Config->AddAkiller)
		realreason = "[" + u->nick + "] " + reason;

	XLine *x = new XLine(mask, u ? u->nick : (OperServ ? OperServ->nick : "OperServ"), expires, realreason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddAkill, OnAddAkill(u, x));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (UplinkSock && Config->AkillOnAdd)
		this->Send(x);

	return x;
}

void SGLineManager::Del(XLine *x)
{
	ircdproto->SendAkillDel(x);
}

void SGLineManager::OnMatch(User *u, XLine *x)
{
	ircdproto->SendAkill(x);
}

void SGLineManager::OnExpire(XLine *x)
{
	if (Config->WallAkillExpire)
		ircdproto->SendGlobops(OperServ, "AKILL on %s has expired", x->Mask.c_str());
}

void SGLineManager::Send(XLine *x)
{
	ircdproto->SendAkill(x);
}

XLine *SNLineManager::Add(BotInfo *bi, User *u, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	if (!mask.empty() && mask.find_first_not_of("*?") == Anope::string::npos)
	{
		if (bi && u)
			u->SendMessage(bi, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				u->SendMessage(bi, OPER_SNLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				u->SendMessage(bi, OPER_EXPIRY_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				u->SendMessage(bi, OPER_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return NULL;
	}

	XLine *x = new XLine(mask, u ? u->nick : (OperServ ? OperServ->nick : "OperServ"), expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SNLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (Config->KillonSNline && !ircd->sglineenforce)
	{
		Anope::string rreason = "G-Lined: " + reason;

		patricia_tree<User *, ci::ci_char_traits>::iterator uit(UserListByNick);
		for (bool next = uit.next(); next;)
		{
			User *user = *uit;
			next = uit.next();

			if (!user->HasMode(UMODE_OPER) && user->server != Me && Anope::Match(user->realname, x->Mask))
				kill_user(Config->ServerName, user, rreason);
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
	this->Send(x);
	Anope::string reason = "G-Lined: " + x->Reason;
	kill_user(Config->s_OperServ, u, reason);
}

void SNLineManager::OnExpire(XLine *x)
{
	if (Config->WallSNLineExpire)
		ircdproto->SendGlobops(OperServ, "SNLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SNLineManager::Send(XLine *x)
{
	ircdproto->SendSGLine(x);
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

XLine *SQLineManager::Add(BotInfo *bi, User *u, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	if (mask.find_first_not_of("*") == Anope::string::npos)
	{
		if (bi && u)
			u->SendMessage(OperServ, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	if (mask[0] == '#' && !ircd->chansqline)
	{
		if (bi && u)
			u->SendMessage(OperServ, OPER_SQLINE_CHANNELS_UNSUPPORTED);
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				u->SendMessage(bi, OPER_SQLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				u->SendMessage(bi, OPER_EXPIRY_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				u->SendMessage(bi, OPER_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return NULL;
	}

	XLine *x = new XLine(mask, u ? u->nick : (OperServ ? OperServ->nick : "OperServ"), expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SQLINE));
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
			patricia_tree<User *, ci::ci_char_traits>::iterator uit(UserListByNick);
			for (bool next = uit.next(); next;)
			{
				User *user = *uit;
				next = uit.next();

				if (!user->HasMode(UMODE_OPER) && user->server != Me && Anope::Match(user->nick, x->Mask))
					kill_user(Config->ServerName, user, rreason);
			}
		}
	}

	if (UplinkSock)
		this->Send(x);

	return x;
}

void SQLineManager::Del(XLine *x)
{
	ircdproto->SendSQLineDel(x);
}

void SQLineManager::OnMatch(User *u, XLine *x)
{
	ircdproto->SendSQLine(x);

	Anope::string reason = "Q-Lined: " + x->Reason;
	kill_user(Config->s_OperServ, u, reason);
}

void SQLineManager::OnExpire(XLine *x)
{
	if (Config->WallSQLineExpire)
		ircdproto->SendGlobops(OperServ, "SQLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SQLineManager::Send(XLine *x)
{
	ircdproto->SendSQLine(x);
}

bool SQLineManager::Check(Channel *c)
{
	if (ircd->chansqline && SQLine)
	{
		for (std::vector<XLine *>::const_iterator it = SGLine->GetList().begin(), it_end = SGLine->GetList().end(); it != it_end; ++it)
		{
			XLine *x = *it;

			if (Anope::Match(c->name, x->Mask))
				return true;
		}
	}

	return false;
}

XLine *SZLineManager::Add(BotInfo *bi, User *u, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	if (mask.find('!') != Anope::string::npos || mask.find('@') != Anope::string::npos)
	{
		u->SendMessage(OperServ, OPER_SZLINE_ONLY_IPS);
		return NULL;
	}

	if (mask.find_first_not_of("*?") == Anope::string::npos)
	{
		u->SendMessage(OperServ, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				u->SendMessage(bi, OPER_SZLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				u->SendMessage(bi, OPER_EXPIRY_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				u->SendMessage(bi, OPER_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return NULL;
	}

	XLine *x = new XLine(mask, u ? u->nick : (OperServ ? OperServ->nick : "OperServ"), expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SZLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (UplinkSock)
		this->Send(x);

	return x;
}

void SZLineManager::Del(XLine *x)
{
	ircdproto->SendSZLineDel(x);
}

void SZLineManager::OnMatch(User *u, XLine *x)
{
	ircdproto->SendSZLine(x);
}

void SZLineManager::OnExpire(XLine *x)
{
	if (Config->WallSZLineExpire)
		ircdproto->SendGlobops(OperServ, "SZLINE on \2%s\2 has expired", x->Mask.c_str());
}

void SZLineManager::Send(XLine *x)
{
	ircdproto->SendSZLine(x);
}

