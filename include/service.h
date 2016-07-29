/*
 *
 * (C) 2003-2014 Anope Team
 * (C) 2016 Adam <Adam@anope.org>
 *
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
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
	Module *owner;
	/* Service type, which should be the class name (eg "Command") */
	Anope::string type;
	/* Service name, commands are usually named service/command */
	Anope::string name;

 public:
	Service(Module *o, const Anope::string &t, const Anope::string &n);
	
	Service(Module *o, const Anope::string &t) : Service(o, t, "") { }

	virtual ~Service();

	Module *GetOwner() const { return owner; }

	const Anope::string &GetType() const { return type; }

	const Anope::string &GetName() const { return name; }
};

class ServiceReferenceBase
{
	Anope::string type, name;

 protected:
	std::vector<Service *> services;

 public:

	ServiceReferenceBase() = default;

	ServiceReferenceBase(const Anope::string &_type, const Anope::string &_name);
	ServiceReferenceBase(const Anope::string &_type);
	
	virtual ~ServiceReferenceBase();
	
	explicit operator bool() const
	{
		return !this->services.empty();
	}

	const Anope::string &GetType() const { return type; }
	const Anope::string &GetName() const { return name; }

	Service *GetService() const { return services.empty() ? nullptr : services[0]; }
	const std::vector<Service *> &GetServices() const { return services; }
	void SetService(Service *service);
	void SetServices(const std::vector<Service *> &s);
};

template<typename T>
class ServiceReference : public ServiceReferenceBase
{
	static_assert(std::is_base_of<Service, T>::value, "");

 public:
	ServiceReference() : ServiceReferenceBase(T::NAME) { }
	ServiceReference(const Anope::string &n) : ServiceReferenceBase(T::NAME, n) { }

	operator T*() const
	{
		return static_cast<T*>(GetService());
	}

	T* operator->() const
	{
		return static_cast<T*>(GetService());
	}

	T* operator*() const
	{
		return static_cast<T*>(GetService());
	}
};

template<typename T>
class ServiceReferenceList : public ServiceReferenceBase
{
	static_assert(std::is_base_of<Service, T>::value, "");

 public:
	using ServiceReferenceBase::ServiceReferenceBase;

	std::vector<T *> GetServices() const
	{
		std::vector<T *> out;
		std::transform(services.begin(), services.end(), std::back_inserter(out), [](Service *e) { return static_cast<T *>(e); });
		return out;
	}
};

class ServiceManager
{
	std::vector<ServiceReferenceBase *> references; // managed references
	std::vector<Service *> services; // all registered services

	/* Lookup services for a reference */
	void Lookup(ServiceReferenceBase *reference);

	/* Lookup services for all managed references */
	void LookupAll();

	std::vector<Service *> FindServices(const Anope::string &type);

	static ServiceManager *manager;

 public:
	Service *FindService(const Anope::string &name);
	Service *FindService(const Anope::string &type, const Anope::string &name);

	template<typename T>
	std::vector<T> FindServices()
	{
		static_assert(std::is_base_of<Service, typename std::remove_pointer<T>::type>::value, "");
		const char *type = std::remove_pointer<T>::type::NAME;
		std::vector<Service *> s = FindServices(type);
		std::vector<T> out;
		std::transform(s.begin(), s.end(), std::back_inserter(out), [](Service *e) { return static_cast<T>(e); });
		return out;
	}

	template<typename T>
	T FindService(const Anope::string &name)
	{
		static_assert(std::is_base_of<Service, typename std::remove_pointer<T>::type>::value, "");
		const char *type = std::remove_pointer<T>::type::NAME;
		return static_cast<T>(FindService(type, name));
	}

	void AddAlias(const Anope::string &t, const Anope::string &n, const Anope::string &v);

	void DelAlias(const Anope::string &t, const Anope::string &n);

	void Register(Service *service);

	void Unregister(Service *service);

	void RegisterReference(ServiceReferenceBase *reference);

	void UnregisterReference(ServiceReferenceBase *reference);

	static void Init();
	static ServiceManager *Get();
	static void Destroy();
};

class ServiceAlias
{
	Anope::string t, f;
 public:
	ServiceAlias(const Anope::string &type, const Anope::string &from, const Anope::string &to);
	~ServiceAlias();
};
