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
#include "users.h"
#include "extern.h"
#include "sockets.h"
#include "regexpr.h"
#include "config.h"
#include "commands.h"

/* List of XLine managers we check users against in XLineManager::CheckAll */
std::list<XLineManager *> XLineManager::XLineManagers;
serialize_checker<std::multimap<Anope::string, XLine *, ci::less> > XLineManager::XLinesByUID("XLine");

void XLine::InitRegex()
{
	if (!Config->RegexEngine.empty() && this->Mask.length() >= 2 && this->Mask[0] == '/' && this->Mask[this->Mask.length() - 1] == '/')
	{
		Anope::string stripped_mask = this->Mask.substr(1, this->Mask.length() - 2);

		service_reference<RegexProvider> provider("Regex", Config->RegexEngine);
		if (provider)
		{
			try
			{
				this->regex = provider->Compile(stripped_mask);
			}
			catch (const RegexException &ex)
			{
				Log(LOG_DEBUG) << ex.GetReason();
			}
		}
	}
}

XLine::XLine(const Anope::string &mask, const Anope::string &reason, const Anope::string &uid) : Serializable("XLine"), Mask(mask), By(Config->OperServ), Created(0), Expires(0), Reason(reason), UID(uid)
{
	regex = NULL;
	manager = NULL;

	this->InitRegex();
}

XLine::XLine(const Anope::string &mask, const Anope::string &by, const time_t expires, const Anope::string &reason, const Anope::string &uid) : Serializable("XLine"), Mask(mask), By(by), Created(Anope::CurTime), Expires(expires), Reason(reason), UID(uid)
{
	regex = NULL;
	manager = NULL;

	this->InitRegex();
}

XLine::~XLine()
{
	delete regex;
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
		if (user_t != Anope::string::npos && host_t > user_t)
			return this->Mask.substr(user_t + 1, host_t - user_t - 1);
		else
			return this->Mask.substr(0, host_t);
	}
	else
		return "";
}

Anope::string XLine::GetHost() const
{
	size_t host_t = this->Mask.find('@'), real_t = this->Mask.find('#');

	if (host_t != Anope::string::npos)
	{
		if (real_t != Anope::string::npos && real_t > host_t)
			return this->Mask.substr(host_t + 1, real_t - host_t - 1);
		else
			return this->Mask.substr(host_t + 1);
	}
	else
		return "";
}

Anope::string XLine::GetReal() const
{
	size_t real_t = this->Mask.find('#');

	if (real_t != Anope::string::npos)
		return this->Mask.substr(real_t + 1);
	else
		return "";
}

Anope::string XLine::GetReason() const
{
	Anope::string r = this->Reason;
	if (Config->AddAkiller && !this->By.empty())
		r = "[" + this->By + "] " + r;
	if (!this->UID.empty())
		r += " (ID: " + this->UID + ")";
	return r;
}

bool XLine::HasNickOrReal() const
{
	bool r = this->GetNick().find_first_not_of("?*") != Anope::string::npos;
	r = r || this->GetReal().find_first_not_of("?*") != Anope::string::npos;
	return r;
}

bool XLine::IsRegex() const
{
	return !this->Mask.empty() && this->Mask[0] == '/' && this->Mask[this->Mask.length() - 1] == '/';
}

Serialize::Data XLine::serialize() const
{
	Serialize::Data data;	

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

Serializable* XLine::unserialize(Serializable *obj, Serialize::Data &data)
{
	service_reference<XLineManager> xlm("XLineManager", data["manager"].astr());
	if (!xlm)
		return NULL;

	XLine *xl;
	if (obj)
	{
		xl = anope_dynamic_static_cast<XLine *>(obj);
		data["mask"] >> xl->Mask;
		data["by"] >> xl->By;
		data["reason"] >> xl->Reason;
		data["uid"] >> xl->UID;

		if (xlm != xl->manager)
		{
			xl->manager->DelXLine(xl);
			xlm->AddXLine(xl);
		}
	}
	else
	{
		time_t expires;
		data["expires"] >> expires;
		xl = new XLine(data["mask"].astr(), data["by"].astr(), expires, data["reason"].astr(), data["uid"].astr());
		xlm->AddXLine(xl);
	}

	data["created"] >> xl->Created;
	xl->manager = xlm;

	return xl;
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
void XLineManager::CheckAll(User *u)
{
	for (std::list<XLineManager *>::iterator it = XLineManagers.begin(), it_end = XLineManagers.end(); it != it_end; ++it)
	{
		XLineManager *xlm = *it;

		XLine *x = xlm->CheckAllXLines(u);

		if (x)
		{
			xlm->OnMatch(u, x);
			break;
		}
	}
}

Anope::string XLineManager::GenerateUID()
{
	Anope::string id;
	int count = 0;
	while (id.empty() || XLinesByUID->count(id) > 0)
	{
		if (++count > 10)
		{
			id.clear();
			Log(LOG_DEBUG) << "Unable to generate XLine UID";
			break;
		}

		for (int i = 0; i < 10; ++i)
		{
			char c;
			do
				c = (rand() % 75) + 48;
			while (!isupper(c) && !isdigit(c));
			id += c;
		}
	}

	return id;
}

/** Constructor
 */
XLineManager::XLineManager(Module *creator, const Anope::string &xname, char t) : Service(creator, "XLineManager", xname), type(t), XLines("XLine")
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
	return this->XLines->size();
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
	if (!x->UID.empty())
		XLinesByUID->insert(std::make_pair(x->UID, x));
	this->XLines->push_back(x);
	x->manager = this;
}

/** Delete an entry from this XLineManager
 * @param x The entry
 * @return true if the entry was found and deleted, else false
 */
bool XLineManager::DelXLine(XLine *x)
{
	std::vector<XLine *>::iterator it = std::find(this->XLines->begin(), this->XLines->end(), x);

	if (!x->UID.empty())
	{
		std::multimap<Anope::string, XLine *, ci::less>::iterator it2 = XLinesByUID->find(x->UID), it3 = XLinesByUID->upper_bound(x->UID);
		for (; it2 != XLinesByUID->end() && it2 != it3; ++it2)
			if (it2->second == x)
			{
				XLinesByUID->erase(it2);
				break;
			}
	}

	if (it != this->XLines->end())
	{
		this->SendDel(x);

		x->destroy();
		this->XLines->erase(it);

		return true;
	}

	return false;
}

/** Gets an entry by index
  * @param index The index
  * @return The XLine, or NULL if the index is out of bounds
  */
XLine* XLineManager::GetEntry(unsigned index)
{
	if (index >= this->XLines->size())
		return NULL;

	XLine *x = this->XLines->at(index);
	x->QueueUpdate();
	return x;
}

/** Clear the XLine vector
 */
void XLineManager::Clear()
{
	for (unsigned i = 0; i < this->XLines->size(); ++i)
	{
		XLine *x = this->XLines->at(i);
		if (!x->UID.empty())
			XLinesByUID->erase(x->UID);
		x->destroy();
	}
	this->XLines->clear();
}

/** Checks if a mask can/should be added to the XLineManager
 * @param source The source adding the mask.
 * @param mask The mask
 * @param expires When the mask would expire
 * @param reason the reason
 * @return true if the mask can be added
 */
bool XLineManager::CanAdd(CommandSource &source, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	for (unsigned i = this->GetCount(); i > 0; --i)
	{
		XLine *x = this->GetEntry(i - 1);

		if (x->Mask.equals_ci(mask))
		{
			if (!x->Expires || x->Expires >= expires)
			{
				if (x->Reason != reason)
				{
					x->Reason = reason;
					source.Reply(_("Reason for %s updated."), x->Mask.c_str());
				}
				else
					source.Reply(_("%s already exists."), mask.c_str());
			}
			else
			{
				x->Expires = expires;
				if (x->Reason != reason)
				{
					x->Reason = reason;
					source.Reply(_("Expiry and reason updated for %s."), x->Mask.c_str());
				}
				else
					source.Reply(_("Expiry for %s updated."), x->Mask.c_str());
			}

			return false;
		}
		else if (Anope::Match(mask, x->Mask) && (!x->Expires || x->Expires >= expires))
		{
			source.Reply(_("%s is already covered by %s."), mask.c_str(), x->Mask.c_str());
			return false;
		}
		else if (Anope::Match(x->Mask, mask) && (!expires || x->Expires <= expires))
		{
			source.Reply(_("Removing %s because %s covers it."), x->Mask.c_str(), mask.c_str());
			this->DelXLine(x);
		}
	}

	return true;
}

/** Checks if this list has an entry
 * @param mask The mask
 * @return The XLine the user matches, or NULL
 */
XLine* XLineManager::HasEntry(const Anope::string &mask)
{
	std::multimap<Anope::string, XLine *, ci::less>::iterator it = XLinesByUID->find(mask);
	if (it != XLinesByUID->end())
		for (std::multimap<Anope::string, XLine *, ci::less>::iterator it2 = XLinesByUID->upper_bound(mask); it != it2; ++it)
			if (it->second->manager == NULL || it->second->manager == this)
			{
				it->second->QueueUpdate();
				return it->second;
			}
	for (unsigned i = 0, end = this->XLines->size(); i < end; ++i)
	{
		XLine *x = this->XLines->at(i);

		if (x->Mask.equals_ci(mask))
		{
			x->QueueUpdate();
			return x;
		}
	}

	return NULL;
}

/** Check a user against all of the xlines in this XLineManager
 * @param u The user
 * @return The xline the user marches, if any.
 */
XLine *XLineManager::CheckAllXLines(User *u)
{
	for (unsigned i = this->XLines->size(); i > 0; --i)
	{
		XLine *x = this->XLines->at(i - 1);

		if (x->Expires && x->Expires < Anope::CurTime)
		{
			this->OnExpire(x);
			this->DelXLine(x);
			continue;
		}

		if (this->Check(u, x))
		{
			this->OnMatch(u, x);
			return x;
		}
	}

	return NULL;
}

/** Called when an XLine expires
 * @param x The xline
 */
void XLineManager::OnExpire(const XLine *x)
{
}

