/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#pragma once

#include "services.h"
#include "anope.h"
#include "base.h"

/** Anything that inherits from this class can be referred to
 * using ServiceReference. Any interfaces provided by modules,
 * such as commands, use this. This is also used for modules
 * that publish a service (m_ssl_openssl, etc).
 */
class CoreExport Service : public virtual Base
{
	static std::map<Anope::string, std::map<Anope::string, Service *> > *Services;
	static std::map<Anope::string, std::map<Anope::string, Anope::string> > *Aliases;

	static void check();

	static Service *FindService(const std::map<Anope::string, Service *> &services, const std::map<Anope::string, Anope::string> *aliases, const Anope::string &n);

 public:
 	static Service *FindService(const Anope::string &t, const Anope::string &n);

	static std::vector<Anope::string> GetServiceKeys(const Anope::string &t);

	static void AddAlias(const Anope::string &t, const Anope::string &n, const Anope::string &v);

	static void DelAlias(const Anope::string &t, const Anope::string &n);

	Module *owner;
	/* Service type, which should be the class name (eg "Command") */
	Anope::string type;
	/* Service name, commands are usually named service/command */
	Anope::string name;

	Service(Module *o, const Anope::string &t, const Anope::string &n);

	Service(const Service &) = delete;

	virtual ~Service();

	void Register();

	void Unregister();
};

/** Like Reference, but used to refer to Services.
 */
template<typename T>
class ServiceReference : public Reference<T>
{
	Anope::string type;
	Anope::string name;
	bool need_check = true;

 public:
 	ServiceReference() { }

	ServiceReference(const Anope::string &t, const Anope::string &n) : type(t), name(n)
	{
	}

	const Anope::string &GetName() const
	{
		return name;
	}

	void operator=(const Anope::string &n)
	{
		this->name = n;
	}

	void Check() override
	{
		if (need_check)
		{
			need_check = false;

			this->ref = static_cast<T *>(Service::FindService(this->type, this->name));
			if (this->ref)
			{
				this->ref->AddReference(this);
				this->OnAcquire();
			}
		}
	}

	void Reset() override
	{
		need_check = true;
	}

	virtual void OnAcquire() { }
};

class ServiceAlias
{
	Anope::string t, f;
 public:
	ServiceAlias(const Anope::string &type, const Anope::string &from, const Anope::string &to);
	~ServiceAlias();
};

