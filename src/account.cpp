// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "services.h"
#include "account.h"
#include "modules.h"
#include "users.h"
#include "protocol.h"
#include "regchannel.h"

std::set<IdentifyRequest *> IdentifyRequest::Requests;

IdentifyRequest::IdentifyRequest(Module *o, const Anope::string &acc, const Anope::string &pass, const Anope::string &ip)
	: owner(o)
	, account(acc)
	, password(pass)
	, address(ip)
{
	Requests.insert(this);
}

IdentifyRequest::~IdentifyRequest()
{
	Requests.erase(this);
}

void IdentifyRequest::Hold(Module *m)
{
	holds.insert(m);
}

void IdentifyRequest::Release(Module *m)
{
	holds.erase(m);
	if (holds.empty() && dispatched)
	{
		if (!success)
			this->OnFail();
		delete this;
	}
}

void IdentifyRequest::Success(Module *m, NickAlias *na)
{
	if (!success)
	{
		this->OnSuccess(na);
		success = true;
	}
}

void IdentifyRequest::Dispatch()
{
	if (holds.empty())
	{
		if (!success)
			this->OnFail();
		delete this;
	}
	else
		dispatched = true;
}

void IdentifyRequest::ModuleUnload(Module *m)
{
	for (auto it = Requests.begin(), it_end = Requests.end(); it != it_end;)
	{
		IdentifyRequest *ir = *it;
		++it;

		ir->holds.erase(m);
		if (ir->holds.empty() && ir->dispatched)
		{
			if (!ir->success)
				ir->OnFail();
			delete ir;
			continue;
		}

		if (ir->owner == m)
		{
			if (!ir->success)
				ir->OnFail();
			delete ir;
		}
	}
}
