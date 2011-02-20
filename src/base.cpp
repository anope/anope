#include "services.h"
#include "modules.h"

Base::Base()
{
}

Base::~Base()
{
	for (std::set<dynamic_reference_base *>::iterator it = this->References.begin(), it_end = this->References.end(); it != it_end; ++it)
	{
		(*it)->Invalidate();
	}

	FOREACH_MOD(I_OnObjectDestroy, OnObjectDestroy(this));
}

void Base::AddReference(dynamic_reference_base *r)
{
	this->References.insert(r);
}

void Base::DelReference(dynamic_reference_base *r)
{
	this->References.erase(r);
}

