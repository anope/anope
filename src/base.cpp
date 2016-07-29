/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
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

