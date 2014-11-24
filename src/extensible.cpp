/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 *
 */

#include "extensible.h"

ExtensibleBase::ExtensibleBase(Module *m, const Anope::string &n) : ExtensibleBase(m, "Extensible", n)
{
}

ExtensibleBase::ExtensibleBase(Module *m, const Anope::string &t, const Anope::string &n) : Service(m, t, n)
{
}

ExtensibleBase::~ExtensibleBase()
{
}

Extensible::~Extensible()
{
	while (!extension_items.empty())
		extension_items[0]->Unset(this);
}

bool Extensible::HasExtOK(const Anope::string &name)
{
	ExtensibleRef<void *> ref(name);
	if (ref)
		return ref->HasExt(this);

	Log(LOG_DEBUG) << "HasExt for nonexistent type " << name << " on " << static_cast<const void *>(this);
	return false;
}

