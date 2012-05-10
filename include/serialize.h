/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <sstream>

#include "anope.h"
#include "base.h"

namespace Serialize
{
 	enum DataType
	{
		DT_TEXT,
		DT_INT
	};
}

class stringstream : public std::stringstream
{
 private:
	Serialize::DataType type;
	unsigned _max;

 public:
	stringstream();
	stringstream(const stringstream &ss);
	Anope::string astr() const;

	template<typename T> std::istream &operator>>(T &val)
	{
		std::istringstream is(this->str());
		is >> val;
		return *this;
	}
	std::istream &operator>>(Anope::string &val);

	bool operator==(const stringstream &other) const;
	bool operator!=(const stringstream &other) const;

	stringstream &setType(Serialize::DataType t);
	Serialize::DataType getType() const;
	stringstream &setMax(unsigned m);
	unsigned getMax() const;
};

namespace Serialize
{
	typedef std::map<Anope::string, stringstream> Data;
}

extern void RegisterTypes();

class CoreExport Serializable : public virtual Base
{
 private:
	static std::list<Serializable *> *serializable_items;

 private:
	std::list<Serializable *>::iterator s_iter;
	Serialize::Data last_commit;
	time_t last_commit_time;

 protected:
	Serializable();
	Serializable(const Serializable &);

	virtual ~Serializable();

	Serializable &operator=(const Serializable &);

 public:
	unsigned int id;

	void destroy();

	void QueueUpdate();

	bool IsCached();
	void UpdateCache();

	bool IsTSCached();
	void UpdateTS();

	virtual const Anope::string serialize_name() const = 0;
	virtual Serialize::Data serialize() const = 0;

	static const std::list<Serializable *> &GetItems();
};

class CoreExport SerializeType
{
	typedef Serializable* (*unserialize_func)(Serializable *obj, Serialize::Data &);

	static std::vector<Anope::string> type_order;
	static Anope::map<SerializeType *> types;

	Anope::string name;
	unserialize_func unserialize;
	time_t timestamp;

 public:
	std::map<unsigned int, Serializable *> objects;

	SerializeType(const Anope::string &n, unserialize_func f);
	~SerializeType();

	const Anope::string &GetName();

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data);

	void Check();

	time_t GetTimestamp() const;
	void UpdateTimestamp();

	static SerializeType *Find(const Anope::string &name);

	static const std::vector<Anope::string> &GetTypeOrder();
};

template<typename T>
class serialize_checker
{
	Anope::string name;
	T obj;

 public:
	serialize_checker(const Anope::string &n) : name(n) { }

	inline const T* operator->() const
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return &this->obj;
	}
	inline T* operator->()
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return &this->obj;
	}

	inline const T& operator*() const
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return this->obj;
	}
	inline T& operator*()
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return this->obj;
	}

	inline operator const T&() const
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return this->obj;
	}
	inline operator T&()
	{
		static SerializeType *type = SerializeType::Find(this->name);
		if (type)
			type->Check();
		return this->obj;
	}
};

#include "modules.h"

template<typename T>
class serialize_obj : public dynamic_reference_base
{
 protected:
	T *ref;

 public:
 	serialize_obj() : ref(NULL)
	{
	}

	serialize_obj(T *obj) : ref(obj)
	{
		if (obj)
			obj->AddReference(this);
	}

	serialize_obj(const serialize_obj<T> &other) : ref(other.ref)
	{
		if (*this)
			this->ref->AddReference(this);
	}

	~serialize_obj()
	{
		if (*this)
			this->ref->DelReference(this);
	}

	inline operator bool() const
	{
		if (!this->invalid)
			return this->ref != NULL;
		return NULL;
	}

	inline void operator=(T *newref)
	{
		if (*this)
			this->ref->DelReference(this);

		this->ref = newref;
		this->invalid = false;

		if (newref)
			this->ref->AddReference(this);
	}

	inline operator T*() const
	{
		if (!this->invalid)
		{
			if (this->ref)
			{
				FOREACH_MOD(I_OnSerializePtrAssign, OnSerializePtrAssign(this->ref));
			}
			return this->ref;
		}
		return NULL;
	}

	inline T* operator*() const
	{
		if (!this->invalid)
		{
			if (this->ref)
			{
				FOREACH_MOD(I_OnSerializePtrAssign, OnSerializePtrAssign(this->ref));
			}
			return this->ref;
		}
		return NULL;
	}

	inline T* operator->() const
	{
		if (!this->invalid)
		{
			if (this->ref)
			{
				FOREACH_MOD(I_OnSerializePtrAssign, OnSerializePtrAssign(this->ref));
			}
			return this->ref;
		}
		return NULL;
	}
};

#endif // SERIALIZE_H
