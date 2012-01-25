/* OperServ functions.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "oper.h"

/* List of XLine managers we check users against in XLineManager::CheckAll */
std::list<XLineManager *> XLineManager::XLineManagers;
std::map<Anope::string, XLine *, ci::less> XLineManager::XLinesByUID;

XLine::XLine(const Anope::string &mask, const Anope::string &reason, const Anope::string &uid) : Mask(mask), Created(0), Expires(0), Reason(reason), UID(uid)
{
	manager = NULL;
}

XLine::XLine(const Anope::string &mask, const Anope::string &by, const time_t expires, const Anope::string &reason, const Anope::string &uid) : Mask(mask), By(by), Created(Anope::CurTime), Expires(expires), Reason(reason), UID(uid)
{
	manager = NULL;
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

sockaddrs XLine::GetIP() const
{
	sockaddrs addr;
	addr.pton(this->GetHost().find(':') != Anope::string::npos ? AF_INET6 : AF_INET, this->GetHost());
	return addr;
}

Anope::string XLine::serialize_name() const
{
	return "XLine";
}

Serializable::serialized_data XLine::serialize()
{
	serialized_data data;	

	data["mask"] << this->Mask;
	data["by"] << this->By;
	data["created"] << this->Created;
	data["expires"] << this->Expires;
	data["reason"] << this->Reason;
	data["uid"] << this->UID;
	if (this->manager)
		data["manager"] << this->manager->name;

	return data;
}

void XLine::unserialize(serialized_data &data)
{
	service_reference<XLineManager> xlm("XLineManager", data["manager"].astr());
	if (!xlm)
		return;

	time_t expires;
	data["expires"] >> expires;
	XLine *xl = new XLine(data["mask"].astr(), data["by"].astr(), expires, data["reason"].astr(), data["uid"].astr());
	data["created"] >> xl->Created;
	xl->manager = xlm;

	xlm->AddXLine(xl);
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

Anope::string XLineManager::GenerateUID()
{
	Anope::string id;
	for (int i = 0; i < 10; ++i)
	{
		char c;
		do
			c = getrandom8();
		while (!isupper(c) && !isdigit(c));
		id += c;
	}

	return id;
}

/** Constructor
 */
XLineManager::XLineManager(Module *creator, const Anope::string &xname, char t) : Service(creator, "XLineManager", xname), type(t)
{
}

/** Destructor
 * Clears all XLines in this XLineManager
 */
XLineManager::~XLineManager()
{
	this->Clear();
}

/** The type of xline provided by this service
 * @return The type
 */
const char &XLineManager::Type()
{
	return this->type;
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
	x->manager = this;
	if (!x->UID.empty())
		XLinesByUID[x->UID] = x;
	this->XLines.push_back(x);
}

/** Delete an entry from this XLineManager
 * @param x The entry
 * @return true if the entry was found and deleted, else false
 */
bool XLineManager::DelXLine(XLine *x)
{
	std::vector<XLine *>::iterator it = std::find(this->XLines.begin(), this->XLines.end(), x);

	if (!x->UID.empty())
		XLinesByUID.erase(x->UID);

	if (it != this->XLines.end())
	{
		this->SendDel(x);

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
	for (unsigned i = 0; i < this->XLines.size(); ++i)
	{
		if (!this->XLines[i]->UID.empty())
			XLinesByUID.erase(this->XLines[i]->UID);
		delete this->XLines[i];
	}
	this->XLines.clear();
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
	std::map<Anope::string, XLine *, ci::less>::iterator it = XLinesByUID.find(mask);
	if (it != XLinesByUID.end() && (it->second->manager == NULL || it->second->manager == this))
		return it->second;
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
			this->DelXLine(x);
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

