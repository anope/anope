/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
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

#include "extensible.h"

ExtensibleBase::ExtensibleBase(Module *m, const Anope::string &n) : ExtensibleBase(m, "Extensible", n)
{
}

ExtensibleBase::ExtensibleBase(Module *m, const Anope::string &t, const Anope::string &n) : Service(m, t, n)
{
}

Extensible::~Extensible()
{
	while (!extension_items.empty())
		(*extension_items.begin())->Unset(this);
}

bool Extensible::HasExtOK(const Anope::string &name)
{
	ExtensibleRef<void *> ref(name);
	if (ref)
		return ref->HasExt(this);

	Log(LOG_DEBUG) << "HasExt for nonexistent type " << name << " on " << static_cast<const void *>(this);
	return false;
}

