/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Adam <Adam@anope.org>
 * Copyright (C) 2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "services.h"
#include "service.h"
#include "logger.h"
#include "modules.h"

ServiceManager *ServiceManager::manager = nullptr;

void ServiceManager::Init()
{
	assert(manager == nullptr);
	manager = new ServiceManager();
}

ServiceManager *ServiceManager::Get()
{
	return manager;
}

void ServiceManager::Destroy()
{
	delete manager;
	manager = nullptr;
}

Service *ServiceManager::FindService(const Anope::string &name)
{
	for (Service *s : services)
		if (s->GetName() == name)
			return s;
	return nullptr;
}

Service *ServiceManager::FindService(const Anope::string &type, const Anope::string &name)
{
	for (Service *s : services)
		if (s->GetType() == type && s->GetName() == name)
			return s;
	return nullptr;
}

std::vector<Service *> ServiceManager::FindServices(const Anope::string &type)
{
	std::vector<Service *> v;

	for (Service *s : services)
		if (s->GetType() == type)
			v.push_back(s);

	return v;
}

void ServiceManager::Register(Service *service)
{
	// assert type
	if (service->GetType().empty())
		throw ModuleException("Service type must be non empty");

	if (service->GetName().empty() == false)
	{
		Service *s = FindService(service->GetType(), service->GetName());

		if (s != nullptr)
			throw ModuleException("Service of type " + service->GetType() + " with name " + service->GetName() + " already exists from " + service->GetOwner()->name);
	}

	Anope::Logger.Debug3("Service registered: {0} {1} address {2} by {3}", service->GetType(), service->GetName(), static_cast<void *>(this), service->GetOwner());

	services.push_back(service);

	LookupAll();
}

void ServiceManager::Unregister(Service *service)
{
	Anope::Logger.Debug3("Service unregistered: {0} {1} address {2} by {3}", service->GetType(), service->GetName(), static_cast<void *>(service), service->GetOwner());

	auto it = std::find(services.begin(), services.end(), service);
	if (it != services.end())
	{
		services.erase(it);

		LookupAll();
	}
}

void ServiceManager::Lookup(ServiceReferenceBase *reference)
{
	if (reference->GetName().empty())
	{
		std::vector<Service *> services = this->FindServices(reference->GetType());
		Anope::Logger.Debug3("Service lookup {0} type {1} name {2}: {3} services", static_cast<void *>(reference), reference->GetType(), reference->GetName(), services.size());
		reference->SetServices(services);
	}
	else
	{
		Service *service = this->FindService(reference->GetType(), reference->GetName());
		Anope::Logger.Debug3("Service lookup {0} type {1} name {2}: {3}", static_cast<void *>(reference), reference->GetType(), reference->GetName(), service);
		reference->SetService(service);
	}
}

void ServiceManager::LookupAll()
{
	for (ServiceReferenceBase *s : references)
		Lookup(s);
}

void ServiceManager::RegisterReference(ServiceReferenceBase *reference)
{
	Lookup(reference);
	this->references.push_back(reference);
}

void ServiceManager::UnregisterReference(ServiceReferenceBase *reference)
{
	auto it = std::find(this->references.begin(), this->references.end(), reference);
	if (it != this->references.end())
		this->references.erase(it);
}
