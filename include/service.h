/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "services.h"
#include "anope.h"
#include "modules.h"

class CoreExport Service : public virtual Base
{
	static Anope::map<Anope::map<Service *> > services;
 public:
 	static Service *FindService(const Anope::string &t, const Anope::string &n)
	{
		Anope::map<Anope::map<Service *> >::iterator it = services.find(t);
		if (it != services.end())
		{
			Anope::map<Service *>::iterator it2 = it->second.find(n);
			if (it2 != it->second.end())
				return it2->second;
		}

		return NULL;
	}

	static std::vector<Anope::string> GetServiceKeys(const Anope::string &t)
	{
		std::vector<Anope::string> keys;
		Anope::map<Anope::map<Service *> >::iterator it = services.find(t);
		if (it != services.end())
			for (Anope::map<Service *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
				keys.push_back(it2->first);
		return keys;
	}

	Module *owner;
	Anope::string type;
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
		Anope::map<Service *> &smap = services[this->type];
		if (smap.find(this->name) != smap.end())
			throw ModuleException("Service " + this->type + " with name " + this->name + " already exists");
		smap[this->name] = this;
	}

	void Unregister()
	{
		Anope::map<Service *> &smap = services[this->type];
		smap.erase(this->name);
		if (smap.empty())
			services.erase(this->type);
	}
};

template<typename T>
class service_reference : public dynamic_reference<T>
{
	Anope::string type;
	Anope::string name;

 public:
 	service_reference() : dynamic_reference<T>(NULL) { }

	service_reference(const Anope::string &t, const Anope::string &n) : dynamic_reference<T>(NULL), type(t), name(n)
	{
	}

	inline void operator=(const Anope::string &n)
	{
		this->name = n;
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
			this->ref = static_cast<T *>(Service::FindService(this->type, this->name));
			if (this->ref)
				this->ref->AddReference(this);
		}
		return this->ref;
	}
};

#endif // SERVICE_H

