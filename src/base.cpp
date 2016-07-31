/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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
#include "anope.h"
#include "service.h"
#include "base.h"

std::set<ReferenceBase *> *ReferenceBase::references = NULL;

ReferenceBase::ReferenceBase()
{
	if (references == NULL)
		references = new std::set<ReferenceBase *>();
	references->insert(this);
}

ReferenceBase::~ReferenceBase()
{
	references->erase(this);
}

void ReferenceBase::ResetAll()
{
	if (references)
		for (ReferenceBase *b : *references)
			b->Reset();
}

Base::Base() : references(NULL)
{
}

Base::~Base()
{
	if (this->references != NULL)
	{
		for (std::set<ReferenceBase *>::iterator it = this->references->begin(), it_end = this->references->end(); it != it_end; ++it)
			(*it)->Invalidate();
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

