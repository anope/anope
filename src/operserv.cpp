/* OperServ functions.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "language.h"

std::vector<NewsItem *> News;

std::vector<std::bitset<32> > DefCon;
bool DefConModesSet = false;
/* Defcon modes mlocked on */
Flags<ChannelModeName> DefConModesOn;
/* Defcon modes mlocked off */
Flags<ChannelModeName> DefConModesOff;
/* Map of Modesa and Params for DefCon */
std::map<ChannelModeName, std::string> DefConModesOnParams;

XLineManager *SGLine, *SZLine, *SQLine, *SNLine;

void os_init()
{
	ModuleManager::LoadModuleList(Config.OperServCoreModules);

	/* Yes, these are in this order for a reason. Most violent->least violent. */
	XLineManager::RegisterXLineManager(SGLine = new SGLineManager());
	XLineManager::RegisterXLineManager(SZLine = new SZLineManager());
	XLineManager::RegisterXLineManager(SQLine = new SQLineManager());
	XLineManager::RegisterXLineManager(SNLine = new SNLineManager());
}

void operserv(User *u, const std::string &buf)
{
	if (!u || buf.empty())
		return;

	Alog() << Config.s_OperServ << ": " << u->nick << ": " <<  buf;

	if (buf.find("\1PING ", 0, 6) != std::string::npos && buf[buf.length() - 1] == '\1')
	{
		std::string command = buf;
		command.erase(command.begin());
		command.erase(command.end());
		ircdproto->SendCTCP(OperServ, u->nick.c_str(), "%s", command.c_str());
	}
	else
		mod_run_cmd(OperServ, u, buf);
}

bool SetDefConParam(ChannelModeName Name, std::string &buf)
{
	return DefConModesOnParams.insert(std::make_pair(Name, buf)).second;
}

bool GetDefConParam(ChannelModeName Name, std::string &buf)
{
	std::map<ChannelModeName, std::string>::iterator it = DefConModesOnParams.find(Name);

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
	std::map<ChannelModeName, std::string>::iterator it = DefConModesOnParams.find(Name);

	if (it != DefConModesOnParams.end())
		DefConModesOnParams.erase(it);
}

/** Check if a certain defcon option is currently in affect
 * @param Level The level
 * @return true and false
 */
bool CheckDefCon(DefconLevel Level)
{
	if (Config.DefConLevel)
		return DefCon[Config.DefConLevel][Level];
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

void server_global(Server *s, const std::string &message)
{
	/* Do not send the notice to ourselves our juped servers */
	if (s != Me && !s->HasFlag(SERVER_JUPED))
		notice_server(Config.s_GlobalNoticer, s, "%s", message.c_str());

	if (s->GetLinks())
	{
		for (std::list<Server *>::const_iterator it = s->GetLinks()->begin(), it_end = s->GetLinks()->end(); it != it_end; ++it)
			server_global(*it, message);
	}
}

void oper_global(char *nick, const char *fmt, ...)
{
	va_list args;
	char msg[2048]; /* largest valid message is 512, this should cover any global */

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	if (nick && !Config.AnonymousGlobal)
	{
		std::string rmsg = std::string("[") + nick + std::string("] ") + msg;
		server_global(Me->GetUplink(), rmsg.c_str());
	}
	else
		server_global(Me->GetUplink(), msg);

}

/**************************************************************************/

/* List of XLine managers we check users against in XLineManager::CheckAll */
std::list<XLineManager *> XLineManager::XLineManagers;

XLine::XLine(const ci::string &mask, const std::string &reason) : Mask(mask), Reason(reason)
{
	Expires = Created = 0;
}

XLine::XLine(const ci::string &mask, const ci::string &by, const time_t expires, const std::string &reason) : Mask(mask), By(by), Created(time(NULL)), Expires(expires), Reason(reason)
{
}

ci::string XLine::GetNick() const
{
	size_t nick_t = Mask.find('!');

	if (nick_t == ci::string::npos)
		return "";

	return Mask.substr(0, nick_t - 1);
}

ci::string XLine::GetUser() const
{
	size_t user_t = Mask.find('!'), host_t = Mask.find('@');

	if (user_t == ci::string::npos)
		return Mask.substr(0, host_t);
	else if (host_t != ci::string::npos)
		return Mask.substr((user_t != ci::string::npos ? user_t + 1 : 0), host_t);
	else
		return "";
}

ci::string XLine::GetHost() const
{
	size_t host_t = Mask.find('@');

	if (host_t == ci::string::npos)
		return Mask;
	else
		return Mask.substr(host_t + 1);
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
	Clear();
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
			ret.second = x;;
			break;
		}
	}

	return ret;
}

/** Get the number of XLines in this XLineManager
 * @return The number of XLines
 */
const size_t XLineManager::GetCount() const
{
	return XLines.size();
}

/** Get the XLine list
  * @return The list
  */
const std::deque<XLine *>& XLineManager::GetList() const
{
	return XLines;
}

/** Add an entry to this XLineManager
 * @param x The entry
 */
void XLineManager::AddXLine(XLine *x)
{
	XLines.push_back(x);
}

/** Delete an entry from this XLineManager
 * @param x The entry
 * @return true if the entry was found and deleted, else false
 */
bool XLineManager::DelXLine(XLine *x)
{
	std::deque<XLine *>::iterator it = std::find(XLines.begin(), XLines.end(), x);

	if (it != XLines.end())
	{
		delete x;
		XLines.erase(it);

		return true;
	}

	return false;
}

/** Gets an entry by index
  * @param index The index
  * @return The XLine, or NULL if the index is out of bounds
  */
XLine *XLineManager::GetEntry(unsigned index) const
{
	if (index >= XLines.size())
		return NULL;

	return XLines[index];
}

/** Clear the XLine list
 */
void XLineManager::Clear()
{
	for (std::deque<XLine *>::iterator it = XLines.begin(), it_end = XLines.end(); it != it_end; ++it)
		delete *it;
	XLines.clear();
}

/** Add an entry to this XLine Manager
 * @param bi The bot error replies should be sent from
 * @param u The user adding the XLine
 * @param mask The mask of the XLine
 * @param expires When this should expire
 * @param reaosn The reason
 * @return A pointer to the XLine
 */
XLine *XLineManager::Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason)
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
std::pair<int, XLine *> XLineManager::CanAdd(const ci::string &mask, time_t expires)
{
	std::pair<int, XLine *> ret;
	
	ret.first = 0;
	ret.second = NULL;

	for (unsigned i = 0, end = GetCount(); i < end; ++i)
	{
		XLine *x = GetEntry(i);
		ret.second = x;

		if (x->Mask == mask)
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
			--i;
		}
	}

	return ret;
}

/** Checks if this list has an entry
 * @param mask The mask
 * @return The XLine the user matches, or NULL
 */
XLine *XLineManager::HasEntry(const ci::string &mask) const
{
	for (unsigned i = 0, end = XLines.size(); i < end; ++i)
	{
		XLine *x = XLines[i];

		if (x->Mask == mask)
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
	const time_t now = time(NULL);

	for (std::deque<XLine *>::iterator it = XLines.begin(), it_end = XLines.end(); it != it_end; ++it)
	{
		XLine *x = *it;

		if (x->Expires && x->Expires < now)
		{
			OnExpire(x);
			delete x;
			it = XLines.erase(it);
			--it;
			continue;
		}

		if (!x->GetNick().empty() && !Anope::Match(u->nick.c_str(), x->GetNick()))
			continue;

		if (!x->GetUser().empty() && !Anope::Match(u->GetIdent().c_str(), x->GetUser()))
			continue;

		if (x->GetHost().empty() || (u->hostip && Anope::Match(u->hostip, x->GetHost())) || Anope::Match(u->host, x->GetHost()) || (!u->chost.empty() && Anope::Match(u->chost.c_str(), x->GetHost())) || (u->vhost && Anope::Match(u->vhost, x->GetHost())))
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

XLine *SGLineManager::Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason)
{
	if (mask.find('!') != ci::string::npos)
	{
		if (bi && u)
			notice_lang(bi->nick.c_str(), u, OPER_AKILL_NO_NICK);
		return NULL;
	}

	if (mask.find('@') == ci::string::npos)
	{
		if (bi && u)
			notice_lang(bi->nick.c_str(), u, BAD_USERHOST_MASK);
		return NULL;
	}

	if (strspn(mask.c_str(), "~@.*?") == mask.length())
	{
		if (bi && u)
			notice_lang(bi->nick.c_str(), u, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				notice_lang(bi->nick.c_str(), u, OPER_AKILL_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				notice_lang(bi->nick.c_str(), u, OPER_AKILL_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				notice_lang(bi->nick.c_str(), u, OPER_AKILL_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return canAdd.second;
	}

	std::string realreason = reason;
	if (u && Config.AddAkiller)
		realreason = "[" + u->nick + "]" + reason;

	XLine *x = new XLine(mask, u ? u->nick.c_str() : "", expires, realreason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddAkill, OnAddAkill(u, x));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (Config.AkillOnAdd)
		ircdproto->SendAkill(x);

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
	if (Config.WallAkillExpire)
		ircdproto->SendGlobops(OperServ, "AKILL on %s has expired", x->Mask.c_str());
}

XLine *SNLineManager::Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason)
{
	if (!mask.empty() && strspn(mask.c_str(), "*?") == mask.length())
	{
		if (bi && u)
			notice_lang(bi->nick.c_str(), u, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				notice_lang(bi->nick.c_str(), u, OPER_SNLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				notice_lang(bi->nick.c_str(), u, OPER_SNLINE_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				notice_lang(bi->nick.c_str(), u, OPER_SNLINE_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return canAdd.second;
	}

	XLine *x = new XLine(mask, u ? u->nick.c_str() : "", expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SNLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (Config.KillonSNline && !ircd->sglineenforce)
	{
		std::string rreason = "G-Lined: " + reason;

		for (user_map::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; )
		{
			User *user = it->second;
			++it;

			if (!is_oper(user) && Anope::Match(user->realname, x->Mask))
				kill_user(Config.ServerName, user->nick, rreason.c_str());
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
	ircdproto->SendSGLine(x);

	std::string reason = "G-Lined: " + x->Reason;
	kill_user(Config.s_OperServ, u->nick, reason.c_str());
}

void SNLineManager::OnExpire(XLine *x)
{
	if (Config.WallSNLineExpire)
		ircdproto->SendGlobops(OperServ, "SNLINE on \2%s\2 has expired", x->Mask.c_str());
}

XLine *SQLineManager::Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason)
{
	if (strspn(mask.c_str(), "*") == mask.length())
	{
		if (bi && u)
			notice_lang(Config.s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	if (mask[0] == '#' && !ircd->chansqline)
	{
		if (bi && u)
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_CHANNELS_UNSUPPORTED);
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				notice_lang(bi->nick.c_str(), u, OPER_SQLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				notice_lang(bi->nick.c_str(), u, OPER_SQLINE_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				notice_lang(bi->nick.c_str(), u, OPER_SQLINE_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return canAdd.second;
	}

	XLine *x = new XLine(mask, u ? u->nick.c_str() : "", expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SQLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	if (Config.KillonSQline)
	{
		std::string rreason = "Q-Lined: " + reason;

		if (mask[0] == '#')
		{
			for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
			{
				Channel *c = cit->second;

				if (!Anope::Match(c->name.c_str(), mask))
					continue;
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
				{
					UserContainer *uc = *it;
					++it;

					if (is_oper(uc->user))
						continue;
					c->Kick(NULL, uc->user, "%s", reason.c_str());
				}
			}
		}
		else
		{
			for (user_map::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; )
			{
				User *user = it->second;
				++it;

				if (!is_oper(user) && Anope::Match(user->nick.c_str(), x->Mask))
					kill_user(Config.ServerName, user->nick, rreason.c_str());
			}
		}
	}

	ircdproto->SendSQLine(x);

	return x;
}

void SQLineManager::Del(XLine *x)
{
	ircdproto->SendSQLineDel(x);
}

void SQLineManager::OnMatch(User *u, XLine *x)
{
	ircdproto->SendSQLine(x);

	char reason[300];
	snprintf(reason, sizeof(reason), "Q-Lined: %s", x->Reason.c_str());
	kill_user(Config.s_OperServ, u->nick, reason);
}

void SQLineManager::OnExpire(XLine *x)
{
	if (Config.WallSQLineExpire)
		ircdproto->SendGlobops(OperServ, "SQLINE on \2%s\2 has expired", x->Mask.c_str());
}

bool SQLineManager::Check(Channel *c)
{
	if (ircd->chansqline && SQLine)
	{
		for (std::deque<XLine *>::const_iterator it = SGLine->GetList().begin(), it_end = SGLine->GetList().end(); it != it_end; ++it)
		{
			XLine *x = *it;

			if (Anope::Match(c->name.c_str(), x->Mask))
				return true;
		}
	}

	return false;
}

XLine *SZLineManager::Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason)
{
	if (mask.find('!') != ci::string::npos || mask.find('@') != ci::string::npos)
	{
		notice_lang(Config.s_OperServ, u, OPER_SZLINE_ONLY_IPS);
		return NULL;
	}

	if (strspn(mask.c_str(), "*?") == mask.length())
	{
		notice_lang(Config.s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask.c_str());
		return NULL;
	}

	std::pair<int, XLine *> canAdd = this->CanAdd(mask, expires);
	if (canAdd.first)
	{
		if (bi && u)
		{
			if (canAdd.first == 1)
				notice_lang(bi->nick.c_str(), u, OPER_SZLINE_EXISTS, canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				notice_lang(bi->nick.c_str(), u, OPER_SZLINE_CHANGED, canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				notice_lang(bi->nick.c_str(), u, OPER_SZLINE_ALREADY_COVERED, mask.c_str(), canAdd.second->Mask.c_str());
		}

		return canAdd.second;
	}

	XLine *x = new XLine(mask, u ? u->nick.c_str() : "", expires, reason);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, X_SZLINE));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete x;
		return NULL;
	}

	this->AddXLine(x);

	ircdproto->SendSZLine(x);

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
	if (Config.WallSZLineExpire)
		ircdproto->SendGlobops(OperServ, "SZLINE on \2%s\2 has expired", x->Mask.c_str());
}
