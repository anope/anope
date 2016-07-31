/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2016 Anope Team <team@anope.org>
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

#include "identifyrequest.h"

IdentifyRequestImpl::IdentifyRequestImpl(NickServ::IdentifyRequestListener *li, Module *o, const Anope::string &acc, const Anope::string &pass) : NickServ::IdentifyRequest(li, o, acc, pass)
{
	std::set<NickServ::IdentifyRequest *> &requests = NickServ::service->GetIdentifyRequests();
	requests.insert(this);
}

IdentifyRequestImpl::~IdentifyRequestImpl()
{
	std::set<NickServ::IdentifyRequest *> &requests = NickServ::service->GetIdentifyRequests();
	requests.erase(this);
	delete l;
}

void IdentifyRequestImpl::Hold(Module *m)
{
	holds.insert(m);
}

void IdentifyRequestImpl::Release(Module *m)
{
	holds.erase(m);
	if (holds.empty() && dispatched)
	{
		if (!success)
			l->OnFail(this);
		delete this;
	}
}

void IdentifyRequestImpl::Success(Module *m)
{
	if (!success)
	{
		l->OnSuccess(this);
		success = true;
	}
}

void IdentifyRequestImpl::Dispatch()
{
	if (holds.empty())
	{
		if (!success)
			l->OnFail(this);
		delete this;
	}
	else
		dispatched = true;
}

void IdentifyRequestImpl::Unload(Module *m)
{
	if (this->GetOwner() != m)
		return;

	if (!success)
		l->OnFail(this);
	delete this;
}

