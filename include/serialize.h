// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#pragma once

#include <sstream>

#include "anope.h"
#include "base.h"

/** Names of serialization types implemented in the core. */
#define BOTINFO_TYPE     "BotInfo"
#define CHANACCESS_TYPE  "ChanAccess"
#define CHANNELINFO_TYPE "ChannelInfo"
#define MEMO_TYPE        "Memo"
#define NICKALIAS_TYPE   "NickAlias"
#define NICKCORE_TYPE    "NickCore"
#define XLINE_TYPE       "XLine"

namespace Serialize
{
	enum class DataType
		: uint8_t
	{
		BOOL,
		FLOAT,
		INT,
		TEXT,
		UINT,
	};

	extern void RegisterTypes();
	extern void CheckTypes();
	extern void CreateTypes();

	class Data;
	class Type;
	template<typename T> class Checker;
	template<typename T> class Reference;
}

/** A serializable object. Serializable objects can be serialized into
 * abstract data types (Serialize::Data), and then reconstructed or
 * updated later at any time.
 */
class CoreExport Serializable
	: public virtual Base
{
private:
	/* A list of every serializable item in Anope.
	 * Some of these are static and constructed at runtime,
	 * so this list must be on the heap, as it is not always
	 * constructed before other objects are if it isn't.
	 */
	static std::list<Serializable *> *SerializableItems;
	friend class Serialize::Type;
	/* The type of item this object is */
	const Anope::string s_name;
	Serialize::Type *s_type;
	/* Iterator into serializable_items */
	std::list<Serializable *>::iterator s_iter;
	/* The hash of the last serialized form of this object committed to the database */
	size_t last_commit = 0;
	/* The last time this object was committed to the database */
	time_t last_commit_time = 0;

protected:
	Serializable(const Anope::string &serialize_type);
	Serializable(const Serializable &);

	Serializable &operator=(const Serializable &);

public:
	using Id = uint64_t;
	virtual ~Serializable();

	/* Unique ID (per type, not globally) for this object */
	Id object_id = 0;

	/* Only used by redis, to ignore updates */
	unsigned short redis_ignore = 0;

	/** Marks the object as potentially being updated "soon".
	 */
	void QueueUpdate();

	bool IsCached(Serialize::Data &);
	void UpdateCache(Serialize::Data &);

	bool IsTSCached();
	void UpdateTS();

	/** Get the type of serializable object this is
	 * @return The serializable object type
	 */
	Serialize::Type *GetSerializableType() const { return this->s_type; }

	static const std::list<Serializable *> &GetItems();
};

class CoreExport Serialize::Data
{
protected:
	/** The specified data types of known fields. */
	std::map<Anope::string, Serialize::DataType> types;

	Data() = default;

	/** Internal method for loading data from the database.
	 * @param key The field to get the value of.
	 * @param value The location to store the retrieved value.
	 */
	virtual bool LoadInternal(const Anope::string &key, Anope::string &value) = 0;

	/** Internal method for storing data in the database.
	 * @param key The field to set the value of.
	 * @param value The value of the field.
	 */
	virtual bool StoreInternal(const Anope::string &key, const Anope::string &value) = 0;

	/** Sets the data type of the specified field. This is called automatically from \ref Store.
	 * @param key The field to specify the data type for.
	 * @param dt The data type of the field.
	 */
	void SetType(const Anope::string &key, Serialize::DataType dt);

public:
	virtual ~Data() = default;

	/** Retrieves the data type for the specified field. If the field does not have a data type
	 * specified then it will default to TEXT.
	 * @param key The field to retrieve the data type for.
	 */
	Serialize::DataType GetType(const Anope::string &key) const;

	/** Retrieves a unique hash for the data set. */
	virtual size_t Hash() const { throw CoreException("Not supported"); }

	/** Loads the value of a specific field.
	 * @param key The field to get the value of.
	 * @param def The default value if none is set.
	 */
	template <typename T = Anope::string>
	T Load(const Anope::string &key, T def = T())
	{
		T out;
		if (!TryLoad(key, out))
			out = def;
		return out;
	}

	/** Stores the value of a specific field, automatically setting its type.
	 * @param key The field to set the value of.
	 * @param value The value of the field.
	 */
	template <typename T>
	void Store(const Anope::string &key, const T &value)
	{
		using Type = std::remove_cv_t<std::remove_reference_t<T>>;

		if constexpr (std::is_same_v<Type, bool>)
			SetType(key, DataType::BOOL);
		else if constexpr (std::is_floating_point_v<Type>)
			SetType(key, DataType::FLOAT);
		else if constexpr (std::is_integral_v<Type> && std::is_signed_v<Type>)
			SetType(key, DataType::INT);
		else if constexpr (std::is_integral_v<Type> && std::is_unsigned_v<Type>)
			SetType(key, DataType::UINT);

		StoreInternal(key, Anope::ToString(value));
	}

	/** Tries to load the value of a specific field.
	 * @param key The field to get the value of.
	 * @param out The location to store the retrieved value.
	 */
	template <typename T = Anope::string>
	bool TryLoad(const Anope::string &key, T &out)
	{
		Anope::string out_str;
		if (!LoadInternal(key, out_str))
			return false;

		auto out_opt = Anope::TryConvert<T>(out_str);
		if (!out_opt)
			return false;

		out = out_opt.value();
		return true;
	}
};

/* A serializable type. There should be a single instance of a subclass of this
 * for each subclass of Serializable as this is what is used to serialize and
 * deserialize data from the database.
 */
class CoreExport Serialize::Type
	: public Base
{
private:
	/** Whether modules have been informed about the creation of this type yet. */
	bool created = false;

	/** The name of this type in the database (e.g. NickAlias). */
	const Anope::string name;

	/** The module which owns this type, or nullptr if it belongs to the core.
	 * Some database backends use this to put third-party module data into their
	 * own database.
	 */
	Module *owner;

	/** The time at which this type was last synchronised with the database.
	 * Only used by live database backends like db_sql_live.
	 */
	time_t timestamp = 0;

	/* The names of currently registered types in order of registration. */
	static std::vector<Anope::string> TypeOrder;

	/** The currently registered types. */
	static std::map<Anope::string, Serialize::Type *> Types;

protected:
	/** Creates a new serializable type.
	 * @param n The name of the type . This should match the value passed in the
	 *          constructor of the equivalent Serializable type.
	 * @param o The module which owns this type, or nullptr if it belongs to the
	 *          core.
	 */
	Type(const Anope::string &n, Module *o = nullptr);

public:
	/* Map of Serializable objects of this type keyed by their object id. */
	std::map<Serializable::Id, Serializable *> objects;

	/** Destroys a serializable type. */
	~Type();

	/** Informs modules about the creation of this type. */
	void Create();

	/** Checks for and applies any pending object updates for this type. */
	void Check();

	/** Attempts to find a serializable type with the specified name.
	 * @param n The name of the serializable type to find.
	 */
	static Serialize::Type *Find(const Anope::string &n);

	/** Retrieves the name of this type in the database (e.g. NickAlias). */
	inline const auto &GetName() const { return this->name; }

	/** Retrieves the module which owns this type, or nullptr if it belongs to
	 * the core. Some database backends use this to put third-party module data
	 * into their own database.
	 */
	inline auto *GetOwner() const { return this->owner; }

	/** Retrieves the time at which this type was last synchronised with the
	 * database. Only used by live database backends like db_sql_live.
	 */
	inline auto GetTimestamp() const { return this->timestamp; };

	/** Retrieves the names of currently registered types in order of
	 * registration.
	 */
	inline static const auto &GetTypeOrder() { return TypeOrder; }

	/** Retrieves the currently registered types. */
	inline static const auto &GetTypes() { return Types; }

	/** Serializes the specified object to the database.
	 * @param obj The object to serialise. This is guaranteed to be the correct
	 *            type so you can cast it without any checks.
	 * @param data The database to serialize to.
	 */
	virtual void Serialize(Serializable *obj, Serialize::Data &data) const = 0;

	/** Unserializes the specified object from the database.
	 * @param obj The object to unserialize into. If the object has not been
	 *            unserialized yet this will be nullptr. This is guaranteed to
	 *            be the correct type so you can cast it without any checks.
	 * @param data The database to unserialize from.
	 * @return The object specified in obj or a new object it if was nullptr.
	 */
	virtual Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const = 0;

	/** Updates the time at which this type was last synchronised with the
	 * database to the current time. Only used by live database backends like
	 * db_sql_live.
	 */
	void UpdateTimestamp();
};

/** Should be used to hold lists and other objects of a specific type,
 * but not a specific object. Used for ensuring that any access to
 * this object type is always up to date. These are usually constructed
 * at run time, before main is called, so no types are registered. This
 * is why there are static Serialize::Type* variables in every function.
 */
template<typename T>
class Serialize::Checker
{
	Anope::string name;
	T obj;
	mutable ::Reference<Serialize::Type> type = nullptr;

	inline void Check() const
	{
		if (!type)
			type = Serialize::Type::Find(this->name);
		if (type)
			type->Check();
	}

public:
	Checker(const Anope::string &n) : name(n) { }

	inline const T *operator->() const
	{
		this->Check();
		return &this->obj;
	}
	inline T *operator->()
	{
		this->Check();
		return &this->obj;
	}

	inline const T &operator*() const
	{
		this->Check();
		return this->obj;
	}
	inline T &operator*()
	{
		this->Check();
		return this->obj;
	}

	inline operator const T&() const
	{
		this->Check();
		return this->obj;
	}
	inline operator T&()
	{
		this->Check();
		return this->obj;
	}
};

/** Used to hold references to serializable objects. Reference should always be
 * used when holding references to serializable objects for extended periods of time
 * to ensure that the object it refers to it always up to date. This also behaves like
 * Reference in that it will invalidate itself if the object it refers to is
 * destructed.
 */
template<typename T>
class Serialize::Reference final
	: public ReferenceBase
{
protected:
	T *ref = nullptr;

public:
	Reference() = default;

	Reference(T *obj) : ref(obj)
	{
		if (obj)
			obj->AddReference(this);
	}

	Reference(const Reference<T> &other) : ReferenceBase(other), ref(other.ref)
	{
		if (ref && !invalid)
			this->ref->AddReference(this);
	}

	~Reference()
	{
		if (ref && !invalid)
			this->ref->DelReference(this);
	}

	inline T *Get(bool update = true) const
	{
		if (!this->invalid)
		{
			if (this->ref && update)
				this->ref->QueueUpdate(); // This can invalidate me
			if (!this->invalid)
				return this->ref;
		}
		return nullptr;
	}

	inline Reference<T>& operator=(const Reference<T> &other)
	{
		if (this != &other)
		{
			if (ref && !invalid)
				this->ref->DelReference(this);

			this->ref = other.ref;
			this->invalid = other.invalid;

			if (ref && !invalid)
				this->ref->AddReference(this);
		}
		return *this;
	}

	inline operator bool() const
	{
		if (!this->invalid)
			return this->ref != NULL;
		return false;
	}

	inline operator T *() const
	{
		return Get(true);
	}

	inline T *operator*() const
	{
		return Get(true);
	}

	inline T *operator->() const
	{
		return Get(true);
	}
};
