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
			throw ModuleException("Service of type " + service->GetType() + " with name " + service->GetName() + " already exists");
	}

	Log(LOG_DEBUG_2) << "Service registered: " << service->GetType() << " " << service->GetName() << " address " << static_cast<void *>(this) << " by " << service->GetOwner();

	services.push_back(service);

	LookupAll();
}

void ServiceManager::Unregister(Service *service)
{
	Log(LOG_DEBUG_2) <<  "Service unregistered: " << service->GetType() << " " << service->GetName() << " address " << static_cast<void *>(service) << " by " << service->GetOwner();

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
		Log(LOG_DEBUG_2) << "Service lookup " << static_cast<void *>(reference) << " type " << reference->GetType() << " name " << reference->GetName() << ": " << services.size() << " services";
		reference->SetServices(services);
	}
	else
	{
		Service *service = this->FindService(reference->GetType(), reference->GetName());
		Log(LOG_DEBUG_2) << "Service lookup " << static_cast<void *>(reference) << " type " << reference->GetType() << " name " << reference->GetName() << ": " << service;
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
