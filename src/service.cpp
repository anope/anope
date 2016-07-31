/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Adam <Adam@anope.org>
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
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


