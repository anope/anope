/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */


#include "services.h"
#include "service.h"

void Service::check()
{
	if (Services || Aliases)
		return;

	Services = new std::map<Anope::string, std::map<Anope::string, Service *> >();
	Aliases = new std::map<Anope::string, std::map<Anope::string, Anope::string> >;
}

Service *Service::FindService(const std::map<Anope::string, Service *> &services, const std::map<Anope::string, Anope::string> *aliases, const Anope::string &n)
{
	std::map<Anope::string, Service *>::const_iterator it = services.find(n);
	if (it != services.end())
		return it->second;

	if (aliases != NULL)
	{
		std::map<Anope::string, Anope::string>::const_iterator it2 = aliases->find(n);
		if (it2 != aliases->end())
			return FindService(services, aliases, it2->second);
	}

	return NULL;
}

Service *Service::FindService(const Anope::string &t, const Anope::string &n)
{
	check();

	std::map<Anope::string, std::map<Anope::string, Service *> >::const_iterator it = (*Services).find(t);
	if (it == (*Services).end())
		return NULL;

	std::map<Anope::string, std::map<Anope::string, Anope::string> >::const_iterator it2 = (*Aliases).find(t);
	if (it2 != (*Aliases).end())
		return FindService(it->second, &it2->second, n);

	return FindService(it->second, NULL, n);
}

std::vector<Anope::string> Service::GetServiceKeys(const Anope::string &t)
{
	check();

	std::vector<Anope::string> keys;
	std::map<Anope::string, std::map<Anope::string, Service *> >::iterator it = (*Services).find(t);
	if (it != (*Services).end())
		for (std::map<Anope::string, Service *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			keys.push_back(it2->first);
	return keys;
}

void Service::AddAlias(const Anope::string &t, const Anope::string &n, const Anope::string &v)
{
	check();

	std::map<Anope::string, Anope::string> &smap = (*Aliases)[t];
	smap[n] = v;
}

void Service::DelAlias(const Anope::string &t, const Anope::string &n)
{
	check();

	std::map<Anope::string, Anope::string> &smap = (*Aliases)[t];
	smap.erase(n);
	if (smap.empty())
		(*Aliases).erase(t);
}

Service::Service(Module *o, const Anope::string &t, const Anope::string &n) : owner(o), type(t), name(n)
{
	this->Register();
}

Service::~Service()
{
	this->Unregister();
}

void Service::Register()
{
	check();

	std::map<Anope::string, Service *> &smap = (*Services)[this->type];
	if (smap.find(this->name) != smap.end())
		throw ModuleException("Service " + this->type + " with name " + this->name + " already exists");
	smap[this->name] = this;

	ReferenceBase::ResetAll();
}

void Service::Unregister()
{
	check();

	std::map<Anope::string, Service *> &smap = (*Services)[this->type];
	smap.erase(this->name);
	if (smap.empty())
		(*Services).erase(this->type);
}

ServiceAlias::ServiceAlias(const Anope::string &type, const Anope::string &from, const Anope::string &to) : t(type), f(from)
{
	Service::AddAlias(type, from, to);
}

ServiceAlias::~ServiceAlias()
{
	Service::DelAlias(t, f);
}


