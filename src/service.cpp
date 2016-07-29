/*
 *
 * (C) 2014-2016 Anope Team
 * (C) 2016 Adam <Adam@anope.org>
 * 
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */


#include "services.h"
#include "service.h"
#include "modules/nickserv.h"
#include "modules/chanserv.h"
#include "modules/memoserv.h"

NickServ::NickServService *NickServ::service = nullptr;
ChanServ::ChanServService *ChanServ::service = nullptr;
MemoServ::MemoServService *MemoServ::service = nullptr;

ServiceReferenceBase::ServiceReferenceBase(const Anope::string &_type, const Anope::string &_name) : type(_type), name(_name)
{
	ServiceManager::Get()->RegisterReference(this);
}

ServiceReferenceBase::ServiceReferenceBase(const Anope::string &_type) : ServiceReferenceBase(_type, "")
{
}

ServiceReferenceBase::~ServiceReferenceBase()
{
	ServiceManager::Get()->UnregisterReference(this);
}

void ServiceReferenceBase::SetService(Service *service)
{
	if (service == nullptr)
		this->services.clear();
	else
		this->services = { service };
}

void ServiceReferenceBase::SetServices(const std::vector<Service *> &s)
{
	this->services = s;
}

Service::Service(Module *o, const Anope::string &t, const Anope::string &n) : owner(o), type(t), name(n)
{
	ServiceManager::Get()->Register(this);
}

Service::~Service()
{
	ServiceManager::Get()->Unregister(this);
}

ServiceAlias::ServiceAlias(const Anope::string &type, const Anope::string &from, const Anope::string &to) : t(type), f(from)
{
	ServiceManager::Get()->AddAlias(type, from, to);
}

ServiceAlias::~ServiceAlias()
{
	ServiceManager::Get()->DelAlias(t, f);
}


