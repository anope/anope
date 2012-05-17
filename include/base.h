/*
 *
 * Copyright (C) 2008-2011 Adam <Adam@anope.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef BASE_H
#define BASE_H

#include "services.h"

/** The base class that most classes in Anope inherit from
 */
class CoreExport Base
{
	/* References to this base class */
	std::set<dynamic_reference_base *> References;
 public:
	Base();
	virtual ~Base();
	void AddReference(dynamic_reference_base *r);
	void DelReference(dynamic_reference_base *r);
};

class dynamic_reference_base
{
 protected:
	bool invalid;
 public:
	dynamic_reference_base() : invalid(false) { }
	dynamic_reference_base(const dynamic_reference_base &other) : invalid(other.invalid) { }
	virtual ~dynamic_reference_base() { }
	inline void Invalidate() { this->invalid = true; }
};

template<typename T>
class dynamic_reference : public dynamic_reference_base
{
 protected:
	T *ref;
 public:
 	dynamic_reference() : ref(NULL)
	{
	}

	dynamic_reference(T *obj) : ref(obj)
	{
		if (ref)
			ref->AddReference(this);
	}

	dynamic_reference(const dynamic_reference<T> &other) : dynamic_reference_base(other), ref(other.ref)
	{
		if (operator bool())
			ref->AddReference(this);
	}

	virtual ~dynamic_reference()
	{
		if (operator bool())
			ref->DelReference(this);
	}

	/* We explicitly call operator bool here in several places to prevent other
	 * operators, such operator T*, from being called instead, which will mess
	 * with any class inheriting from this that overloads this operator.
	 */
	virtual operator bool()
	{
		if (!this->invalid)
			return this->ref != NULL;
		return false;
	}

	inline operator T*()
	{
		if (operator bool())
			return this->ref;
		return NULL;
	}

	inline T* operator->()
	{
		if (operator bool())
			return this->ref;
		return NULL;
	}

	inline void operator=(T *newref)
	{
		if (operator bool())
			this->ref->DelReference(this);
		this->ref = newref;
		this->invalid = false;
		if (operator bool())
			this->ref->AddReference(this);
	}

	inline bool operator<(const dynamic_reference<T> &other) const
	{
		return this < &other;
	}

	inline bool operator==(const dynamic_reference<T> &other)
	{
		if (!this->invalid)
			return this->ref == other;
		return false;
	}
};

#endif // BASE_H

