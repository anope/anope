/*
 * Anope IRC Services
 *
 * Copyright (C) 2009-2016 Anope Team <team@anope.org>
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

#pragma once

#include "anope.h"
#include "service.h"
#include "logger.h"

class Extensible;

class CoreExport ExtensibleBase : public Service
{
 protected:
	std::map<Extensible *, void *> items;

	ExtensibleBase(Module *m, const Anope::string &n);
	ExtensibleBase(Module *m, const Anope::string &t, const Anope::string &n);

 public:
	virtual void Unset(Extensible *obj) anope_abstract;
	
	static constexpr const char *NAME = "Extensible";
};

class CoreExport Extensible
{
 public:
	std::vector<ExtensibleBase *> extension_items;

	virtual ~Extensible();

	template<typename T> T* GetExt(const Anope::string &name);
	bool HasExtOK(const Anope::string &name);

	template<typename T> T* Extend(const Anope::string &name, const T &what);

	template<typename T> void Shrink(const Anope::string &name);
};

template<typename T>
class ExtensibleItem : public ExtensibleBase
{
 public:
	ExtensibleItem(Module *m, const Anope::string &n) : ExtensibleBase(m, n) { }
	ExtensibleItem(Module *m, const Anope::string &t, const Anope::string &n) : ExtensibleBase(m, t, n) { }

	~ExtensibleItem()
	{
		while (!items.empty())
		{
			std::map<Extensible *, void *>::iterator it = items.begin();
			Extensible *obj = it->first;
			T *value = static_cast<T *>(it->second);

			auto it2 = std::find(obj->extension_items.begin(), obj->extension_items.end(), this);
			if (it2 != obj->extension_items.end())
				obj->extension_items.erase(it2);
			items.erase(it);

			delete value;
		}
	}

	T* Set(Extensible *obj, const T &value)
	{
		T* t = new T(value);
		Unset(obj);

		items[obj] = t;
		obj->extension_items.push_back(this);

		return t;
	}

	void Unset(Extensible *obj) override
	{
		T *value = Get(obj);

		items.erase(obj);
		auto it = std::find(obj->extension_items.begin(), obj->extension_items.end(), this);
		if (it != obj->extension_items.end())
			obj->extension_items.erase(it);

		delete value;
	}

	T* Get(Extensible *obj)
	{
		std::map<Extensible *, void *>::const_iterator it = items.find(obj);
		if (it != items.end())
			return static_cast<T *>(it->second);
		return nullptr;
	}

	bool HasExt(Extensible *obj)
	{
		return items.find(obj) != items.end();
	}

	T* Require(Extensible *obj)
	{
		T* t = Get(obj);
		if (t)
			return t;

		return Set(obj, T());
	}
};

template<typename T>
struct ExtensibleRef : ServiceReference<ExtensibleItem<T>>
{
	ExtensibleRef(const Anope::string &n) : ServiceReference<ExtensibleItem<T>>(n) { }
};

template<typename T>
T* Extensible::GetExt(const Anope::string &name)
{
	ExtensibleRef<T> ref(name);
	if (ref)
		return ref->Get(this);

	Log(LOG_DEBUG) << "GetExt for nonexistent type " << name << " on " << static_cast<const void *>(this);
	return NULL;
}

template<typename T>
T* Extensible::Extend(const Anope::string &name, const T &what)
{
	ExtensibleRef<T> ref(name);
	if (ref)
	{
		ref->Set(this, what);
		return ref->Get(this);
	}

	Log(LOG_DEBUG) << "Extend for nonexistent type " << name << " on " << static_cast<void *>(this);
	return NULL;
}

template<typename T>
void Extensible::Shrink(const Anope::string &name)
{
	ExtensibleRef<T> ref(name);
	if (ref)
		ref->Unset(this);
	else
		Log(LOG_DEBUG) << "Shrink for nonexistent type " << name << " on " << static_cast<void *>(this);
}

