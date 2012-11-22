/* XLine functions.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "services.h"
#include "modules.h"
#include "xline.h"
#include "users.h"
#include "sockets.h"
#include "regexpr.h"
#include "config.h"
#include "commands.h"

/* List of XLine managers we check users against in XLineManager::CheckAll */
std::list<XLineManager *> XLineManager::XLineManagers;
Serialize::Checker<std::multimap<Anope::string, XLine *, ci::less> > XLineManager::XLinesByUID("XLine");

void XLine::InitRegex()
{
	if (!Config->RegexEngine.empty() && this->mask.length() >= 2 && this->mask[0] == '/' && this->mask[this->mask.length() - 1] == '/')
	{
		Anope::string stripped_mask = this->mask.substr(1, this->mask.length() - 2);

		ServiceReference<RegexProvider> provider("Regex", Config->RegexEngine);
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

XLine::XLine(const Anope::string &ma, const Anope::string &r, const Anope::string &uid) : Serializable("XLine"), mask(ma), by(Config->OperServ), created(0), expires(0), reason(r), id(uid)
{
	regex = NULL;
	manager = NULL;

	this->InitRegex();
}

XLine::XLine(const Anope::string &ma, const Anope::string &b, const time_t ex, const Anope::string &r, const Anope::string &uid) : Serializable("XLine"), mask(ma), by(b), created(Anope::CurTime), expires(ex), reason(r), id(uid)
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
	size_t nick_t = this->mask.find('!');

	if (nick_t == Anope::string::npos)
		return "";

	return this->mask.substr(0, nick_t);
}

Anope::string XLine::GetUser() const
{
	size_t user_t = this->mask.find('!'), host_t = this->mask.find('@');

	if (host_t != Anope::string::npos)
	{
		if (user_t != Anope::string::npos && host_t > user_t)
			return this->mask.substr(user_t + 1, host_t - user_t - 1);
		else
			return this->mask.substr(0, host_t);
	}
	else
		return "";
}

Anope::string XLine::GetHost() const
{
	size_t host_t = this->mask.find('@'), real_t = this->mask.find('#');

	if (host_t != Anope::string::npos)
	{
		if (real_t != Anope::string::npos && real_t > host_t)
			return this->mask.substr(host_t + 1, real_t - host_t - 1);
		else
			return this->mask.substr(host_t + 1);
	}
	else
		return "";
}

Anope::string XLine::GetReal() const
{
	size_t real_t = this->mask.find('#');

	if (real_t != Anope::string::npos)
		return this->mask.substr(real_t + 1);
	else
		return "";
}

Anope::string XLine::GetReason() const
{
	Anope::string r = this->reason;
	if (Config->AddAkiller && !this->by.empty())
		r = "[" + this->by + "] " + r;
	if (!this->id.empty())
		r += " (ID: " + this->id + ")";
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
	return !this->mask.empty() && this->mask[0] == '/' && this->mask[this->mask.length() - 1] == '/';
}

Serialize::Data XLine::Serialize() const
{
	Serialize::Data data;	

	data["mask"] << this->mask;
	data["by"] << this->by;
	data["created"] << this->created;
	data["expires"] << this->expires;
	data["reason"] << this->reason;
	data["uid"] << this->id;
	if (this->manager)
		data["manager"] << this->manager->name;

	return data;
}

Serializable* XLine::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ServiceReference<XLineManager> xlm("XLineManager", data["manager"].astr());
	if (!xlm)
		return NULL;

	XLine *xl;
	if (obj)
	{
		xl = anope_dynamic_static_cast<XLine *>(obj);
		data["mask"] >> xl->mask;
		data["by"] >> xl->by;
		data["reason"] >> xl->reason;
		data["uid"] >> xl->id;

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

	data["created"] >> xl->created;
	xl->manager = xlm;

	return xl;
}

void XLineManager::RegisterXLineManager(XLineManager *xlm)
{
	XLineManagers.push_back(xlm);
}

void XLineManager::UnregisterXLineManager(XLineManager *xlm)
{
	std::list<XLineManager *>::iterator it = std::find(XLineManagers.begin(), XLineManagers.end(), xlm);

	if (it != XLineManagers.end())
		XLineManagers.erase(it);
}

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

XLineManager::XLineManager(Module *creator, const Anope::string &xname, char t) : Service(creator, "XLineManager", xname), type(t), xlines("XLine")
{
}

XLineManager::~XLineManager()
{
	this->Clear();
}

const char &XLineManager::Type()
{
	return this->type;
}

size_t XLineManager::GetCount() const
{
	return this->xlines->size();
}

const std::vector<XLine *> &XLineManager::GetList() const
{
	return this->xlines;
}

void XLineManager::AddXLine(XLine *x)
{
	if (!x->id.empty())
		XLinesByUID->insert(std::make_pair(x->id, x));
	this->xlines->push_back(x);
	x->manager = this;
}

bool XLineManager::DelXLine(XLine *x)
{
	std::vector<XLine *>::iterator it = std::find(this->xlines->begin(), this->xlines->end(), x);

	if (!x->id.empty())
	{
		std::multimap<Anope::string, XLine *, ci::less>::iterator it2 = XLinesByUID->find(x->id), it3 = XLinesByUID->upper_bound(x->id);
		for (; it2 != XLinesByUID->end() && it2 != it3; ++it2)
			if (it2->second == x)
			{
				XLinesByUID->erase(it2);
				break;
			}
	}

	if (it != this->xlines->end())
	{
		this->SendDel(x);

		x->Destroy();
		this->xlines->erase(it);

		return true;
	}

	return false;
}

XLine* XLineManager::GetEntry(unsigned index)
{
	if (index >= this->xlines->size())
		return NULL;

	XLine *x = this->xlines->at(index);
	x->QueueUpdate();
	return x;
}

void XLineManager::Clear()
{
	for (unsigned i = 0; i < this->xlines->size(); ++i)
	{
		XLine *x = this->xlines->at(i);
		if (!x->id.empty())
			XLinesByUID->erase(x->id);
		x->Destroy();
	}
	this->xlines->clear();
}

bool XLineManager::CanAdd(CommandSource &source, const Anope::string &mask, time_t expires, const Anope::string &reason)
{
	for (unsigned i = this->GetCount(); i > 0; --i)
	{
		XLine *x = this->GetEntry(i - 1);

		if (x->mask.equals_ci(mask))
		{
			if (!x->expires || x->expires >= expires)
			{
				if (x->reason != reason)
				{
					x->reason = reason;
					source.Reply(_("Reason for %s updated."), x->mask.c_str());
				}
				else
					source.Reply(_("%s already exists."), mask.c_str());
			}
			else
			{
				x->expires = expires;
				if (x->reason != reason)
				{
					x->reason = reason;
					source.Reply(_("Expiry and reason updated for %s."), x->mask.c_str());
				}
				else
					source.Reply(_("Expiry for %s updated."), x->mask.c_str());
			}

			return false;
		}
		else if (Anope::Match(mask, x->mask) && (!x->expires || x->expires >= expires))
		{
			source.Reply(_("%s is already covered by %s."), mask.c_str(), x->mask.c_str());
			return false;
		}
		else if (Anope::Match(x->mask, mask) && (!expires || x->expires <= expires))
		{
			source.Reply(_("Removing %s because %s covers it."), x->mask.c_str(), mask.c_str());
			this->DelXLine(x);
		}
	}

	return true;
}

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
	for (unsigned i = 0, end = this->xlines->size(); i < end; ++i)
	{
		XLine *x = this->xlines->at(i);

		if (x->mask.equals_ci(mask))
		{
			x->QueueUpdate();
			return x;
		}
	}

	return NULL;
}

XLine *XLineManager::CheckAllXLines(User *u)
{
	for (unsigned i = this->xlines->size(); i > 0; --i)
	{
		XLine *x = this->xlines->at(i - 1);

		if (x->expires && x->expires < Anope::CurTime)
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

void XLineManager::OnExpire(const XLine *x)
{
}

