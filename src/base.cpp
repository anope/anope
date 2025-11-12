// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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
#include "anope.h"
#include "service.h"

std::map<Anope::string, std::map<Anope::string, Service *> > Service::Services;
std::map<Anope::string, std::map<Anope::string, Anope::string> > Service::Aliases;

Base::~Base()
{
	if (this->references != NULL)
	{
		for (auto *reference : *this->references)
			reference->Invalidate();
		delete this->references;
	}
}

void Base::AddReference(ReferenceBase *r)
{
	if (this->references == NULL)
		this->references = new std::set<ReferenceBase *>();
	this->references->insert(r);
}

void Base::DelReference(ReferenceBase *r)
{
	if (this->references != NULL)
	{
		this->references->erase(r);
		if (this->references->empty())
		{
			delete this->references;
			this->references = NULL;
		}
	}
}
