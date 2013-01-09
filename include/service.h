/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "services.h"
#include "anope.h"
#include "modules.h"

/** Anything that inherits from this class can be referred to
 * using ServiceReference. Any interfaces provided by modules,
 * such as commands, use this. This is also used for modules
 * that publish a service (m_ssl, etc).
 */
class CoreExport Service : public virtual Base
{
	static std::map<Anope::string, std::map<Anope::string, Service *> > Services;
	static std::map<Anope::string, std::map<Anope::string, Anope::string> > Aliases;
 public:
 	static Service *FindService(const Anope::string &t, const Anope::string &n)
	{
		std::map<Anope::string, std::map<Anope::string, Service *> >::iterator it = Services.find(t);
		if (it != Services.end())
		{
			Anope::string name = n;

			std::map<Anope::string, std::map<Anope::string, Anope::string> >::iterator it2 = Aliases.find(t);
			if (it2 != Aliases.end())
			{
				std::map<Anope::string, Anope::string>::iterator it3 = it2->second.find(n);
				if (it3 != it2->second.end())
					name = it3->second;
			}

			std::map<Anope::string, Service *>::iterator it4 = it->second.find(name);
			if (it4 != it->second.end())
				return it4->second;
		}

		return NULL;
	}

	static std::vector<Anope::string> GetServiceKeys(const Anope::string &t)
	{
		std::vector<Anope::string> keys;
		std::map<Anope::string, std::map<Anope::string, Service *> >::iterator it = Services.find(t);
		if (it != Services.end())
			for (std::map<Anope::string, Service *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
				keys.push_back(it2->first);
		return keys;
	}

	static void AddAlias(const Anope::string &t, const Anope::string &n, const Anope::string &v)
	{
		std::map<Anope::string, Anope::string> &smap = Aliases[t];
		smap[n] = v;
	}

	static void DelAlias(const Anope::string &t, const Anope::string &n)
	{
		std::map<Anope::string, Anope::string> &smap = Aliases[t];
		smap.erase(n);
		if (smap.empty())
			Aliases.erase(t);
	}

	Module *owner;
	/* Service type, which should be the class name (eg "Command") */
	Anope::string type;
	/* Service name, commands are usually named service/command */
	Anope::string name;

	Service(Module *o, const Anope::string &t, const Anope::string &n) : owner(o), type(t), name(n)
	{
		this->Register();
	}

	virtual ~Service()
	{
		this->Unregister();
	}

	void Register()
	{
		std::map<Anope::string, Service *> &smap = Services[this->type];
		if (smap.find(this->name) != smap.end())
			throw ModuleException("Service " + this->type + " with name " + this->name + " already exists");
		smap[this->name] = this;
	}

	void Unregister()
	{
		std::map<Anope::string, Service *> &smap = Services[this->type];
		smap.erase(this->name);
		if (smap.empty())
			Services.erase(this->type);
	}
};

/** Like Reference, but used to refer to Services.
 */
template<typename T>
class ServiceReference : public Reference<T>
{
	Anope::string type;
	Anope::string name;

 public:
 	ServiceReference() { }

	ServiceReference(const Anope::string &t, const Anope::string &n) : type(t), name(n)
	{
	}

	inline void operator=(const Anope::string &n)
	{
		this->name = n;
		this->invalid = true;
	}

	operator bool() anope_override
	{
		if (this->invalid)
		{
			this->invalid = false;
			this->ref = NULL;
		}
		if (!this->ref)
		{
			/* This really could be dynamic_cast in every case, except for when a module
			 * creates its own service type (that other modules must include the header file
			 * for), as the core is not compiled with it so there is no RTTI for it.
			 */
			this->ref = static_cast<T *>(Service::FindService(this->type, this->name));
			if (this->ref)
				this->ref->AddReference(this);
		}
		return this->ref;
	}
};

class ServiceAlias
{
	Anope::string t, f;
 public:
	ServiceAlias(const Anope::string &type, const Anope::string &from, const Anope::string &to) : t(type), f(from)
	{
		Service::AddAlias(type, from, to);
	}

	~ServiceAlias()
	{
		Service::DelAlias(t, f);
	}
};

#endif // SERVICE_H

