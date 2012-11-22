/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "anope.h"
#include "service.h"

std::map<Anope::string, std::map<Anope::string, Service *> > Service::Services;
std::map<Anope::string, std::map<Anope::string, Anope::string> > Service::Aliases;

Base::Base()
{
}

Base::~Base()
{
	for (std::set<ReferenceBase *>::iterator it = this->references.begin(), it_end = this->references.end(); it != it_end; ++it)
	{
		(*it)->Invalidate();
	}
}

void Base::AddReference(ReferenceBase *r)
{
	this->references.insert(r);
}

void Base::DelReference(ReferenceBase *r)
{
	this->references.erase(r);
}

