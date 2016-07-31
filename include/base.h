/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "services.h"

/** The base class that most classes in Anope inherit from
 */
class CoreExport Base
{
	/* References to this base class */
	std::set<ReferenceBase *> *references;
 public:
	Base();
	virtual ~Base();

	/** Adds a reference to this object. Eg, when a Reference
	 * is created referring to this object this is called. It is used to
	 * cleanup references when this object is destructed.
	 */
	void AddReference(ReferenceBase *r);

	void DelReference(ReferenceBase *r);
};

class ReferenceBase
{
	static std::set<ReferenceBase *> *references;
 public:
	static void ResetAll();

	ReferenceBase();
	virtual ~ReferenceBase();
	virtual void Invalidate() anope_abstract;
	virtual void Reset() { }
};

/** Used to hold pointers to objects that may be deleted. A Reference will
 * no longer be valid once the object it refers is destructed.
 */
template<typename T>
class Reference : public ReferenceBase
{
 protected:
	T *ref;

	virtual void Check() { }

 public:
 	Reference() : ref(nullptr)
	{
	}

	Reference(T *obj) : ref(obj)
	{
		if (ref)
			ref->AddReference(this);
	}

	Reference(const Reference<T> &other) : ref(other.ref)
	{
		if (ref)
			ref->AddReference(this);
	}

	virtual ~Reference()
	{
		if (*this)
			this->ref->DelReference(this);
	}

	void Invalidate() override
	{
		ref = nullptr;
	}

	Reference<T>& operator=(const Reference<T> &other)
	{
		if (this != &other)
		{
			if (*this)
				this->ref->DelReference(this);

			this->ref = other.ref;

			if (*this)
				this->ref->AddReference(this);
		}
		return *this;
	}

	explicit operator bool()
	{
		Check();
		return this->ref != nullptr;
	}

	operator T*()
	{
		if (*this)
			return this->ref;
		return nullptr;
	}

	T* operator->()
	{
		if (*this)
			return this->ref;
		return nullptr;
	}

	T* operator*()
	{
		if (*this)
			return this->ref;
		return nullptr;
	}

	/** Note that we can't have an operator< that returns this->ref < other.ref
	 * because this function is used to sort objects in containers (such as set
	 * or map), and if the references themselves can change if the object they
	 * refer to is invalidated or changed, then this screws with the order that
	 * the objects would be in the container without properly adjusting the
	 * container, resulting in weird stuff.
	 *
	 * As such, we don't allow storing references in containers that require
	 * operator<, because they would not be able to compare what the references
	 * actually referred to.
	 */

	bool operator==(const Reference<T> &other)
	{
		if (*this)
			return this->ref == other;
		return false;
	}
};

