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

#include "extensible.h"

static std::set<ExtensibleBase *> extensible_items;

ExtensibleBase::ExtensibleBase(Module *m, const Anope::string &n) : Service(m, "Extensible", n)
{
	extensible_items.insert(this);
}

ExtensibleBase::~ExtensibleBase()
{
	extensible_items.erase(this);
}

Extensible::~Extensible()
{
	UnsetExtensibles();
}

void Extensible::UnsetExtensibles()
{
	while (!extension_items.empty())
		(*extension_items.begin())->Unset(this);
}

bool Extensible::HasExt(const Anope::string &name) const
{
	ExtensibleRef<void *> ref(name);
	if (ref)
		return ref->HasExt(this);

	Log(LOG_DEBUG) << "HasExt for nonexistent type " << name << " on " << static_cast<const void *>(this);
	return false;
}

void Extensible::ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data)
{
	for (std::set<ExtensibleBase *>::iterator it = e->extension_items.begin(); it != e->extension_items.end(); ++it)
	{
		ExtensibleBase *eb = *it;
		eb->ExtensibleSerialize(e, s, data);
	}
}

void Extensible::ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data)
{
	for (auto *extensible_item : extensible_items)
	{
		extensible_item->ExtensibleUnserialize(e, s, data);
	}
}

template<>
bool *Extensible::Extend(const Anope::string &name, const bool &what)
{
	ExtensibleRef<bool> ref(name);
	if (ref)
		return ref->Set(this);

	Log(LOG_DEBUG) << "Extend for nonexistent type " << name << " on " << static_cast<void *>(this);
	return NULL;
}
