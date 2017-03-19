/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "base.h"
#include "extensible.h"
#include "event.h"

namespace Serialize
{
	class Object;

	class TypeBase;
	
	class FieldBase;
	template<typename> class FieldTypeBase;
	template<typename, typename> class CommonFieldBase;
	template<typename, typename> class Field;
	template<typename, typename> class ObjectField;

	template<typename> class Storage;

	template<typename T, typename> class Type;
	template<typename T> class Reference;

	extern std::vector<FieldBase *> serializableFields;
	
	extern Object *GetID(Serialize::TypeBase *type, ID id);

	template<typename T>
	inline T GetObject();

	template<typename T>
	inline std::vector<T> GetObjects_(TypeBase *);

	template<typename T>
	inline std::vector<T> GetObjects(const Anope::string &name);

	template<typename T>
	inline std::vector<T> GetObjects();

	template<typename T>
	inline T New();

	extern void GC();

	extern void Unregister(Module *);

	struct Edge
	{
		Object *other;
		FieldBase *field;
		bool direction;

		Edge(Object *o, FieldBase *f, bool d) : other(o), field(f), direction(d) { }

		bool operator==(const Edge &e) const
		{
			return other == e.other && field == e.field && direction == e.direction;
		}
	};

	extern std::multimap<Anope::string, Anope::string> child_types;

	extern void SetParent(const Anope::string &child, const Anope::string &parent);

	extern std::vector<Serialize::TypeBase *> GetTypes(const Anope::string &name);
}

class CoreExport Serialize::Object : public Extensible, public virtual Base
{
 private:
	friend class Serialize::FieldBase;

	/* The type of item this object is */
	TypeBase *s_type;

	std::map<TypeBase *, std::vector<Edge>> edges;

	std::vector<Edge> GetEdges(TypeBase *);

 public:
	static constexpr const char *NAME = "object";

	Object(TypeBase *type);
	Object(TypeBase *type, ID);

	virtual ~Object();

	virtual void Delete();

	/* Unique ID for this object */
	ID id;

	void AddEdge(Object *other, FieldBase *field);

	void RemoveEdge(Object *other, FieldBase *field);

	/**
	 * Get an object of type T that this object references.
	 */
	template<typename T>
	T GetRef()
	{
		std::vector<T> t = GetRefs<T>();
		return !t.empty() ? t[0] : nullptr;
	}
	
	/**
	 * Gets all objects of type T that this object references
	 */
	template<typename T>
	std::vector<T> GetRefs();

	std::vector<Object *> GetRefs(Serialize::TypeBase *);

	/**
	 * Get the value of a field on this object.
	 */
	template<
		typename Type,
		template<typename, typename> class Field, // Field type being read
		typename TypeImpl,
		typename T // type of the Extensible
	>
	T Get(Field<TypeImpl, T> Type::*field)
	{
		static_assert(std::is_base_of<Object, TypeImpl>::value, "");
		static_assert(std::is_base_of<Serialize::TypeBase, Type>::value, "");

		Type *t = static_cast<Type *>(s_type);
		Field<TypeImpl, T>& f = t->*field;
		return f.GetField(f.Upcast(this));
	}

	/**
	 * Get the value of a field on this object. Allows specifying return
	 * type if the return type can't be inferred.
	 */
	template<typename Ret, typename Field, typename Type>
	Ret Get(Field Type::*field)
	{
		static_assert(std::is_base_of<Serialize::TypeBase, Type>::value, "");

		Type *t = static_cast<Type *>(s_type);
		Field& f = t->*field;
		return f.GetField(f.Upcast(this));
	}

	/**
	 * Set the value of a field on this object
	 */
	template<
		typename Type,
		template<typename, typename> class Field,
		typename TypeImpl,
		typename T
	>
	void Set(Field<TypeImpl, T> Type::*field, const T& value)
	{
		static_assert(std::is_base_of<Object, TypeImpl>::value, "");
		static_assert(std::is_base_of<Serialize::TypeBase, Type>::value, "");

		Type *t = static_cast<Type *>(s_type);
		Field<TypeImpl, T>& f = t->*field;
		f.SetField(f.Upcast(this), value);
	}

	/**
	 * Set the value of a field on this object
	 */
	template<typename Field, typename Type, typename T>
	void Set(Field Type::*field, const T& value)
	{
		static_assert(std::is_base_of<Serialize::TypeBase, Type>::value, "");

		Type *t = static_cast<Type *>(s_type);
		Field& f = t->*field;
		f.SetField(f.Upcast(this), value);
	}

	/** Get the type of serializable object this is
	 * @return The serializable object type
	 */
	TypeBase* GetSerializableType() const { return this->s_type; }

	/** Set the value of a field on this object, by field name
	 */
	template<typename T>
	void SetS(const Anope::string &name, const T &what);

	/** Unset a field on this object, by field name
	 */
	template<typename T>
	void UnsetS(const Anope::string &name);

	/** Test if a field is set. Only useful with extensible fields,
	 * which can unset (vs set to the default value)
	 */
	bool HasFieldS(const Anope::string &name);

	/** Whether or not the object can be garbage collected.
	 * Should be true unless your object holds in-memory only
	 * state.
	 */
	virtual bool CanGC() { return true; }

	void Wipe();
};

class CoreExport Serialize::TypeBase : public Service
{
	Anope::string name;

	Module *owner = nullptr;

 public:
	static constexpr const char *NAME = "typebase";
	
	std::set<Object *> objects;

	TypeBase(Module *owner, const Anope::string &n);

	void Unregister();

	/** Gets the name for this type
	 * @return The name, eg "NickAlias"
	 */
	const Anope::string &GetName() { return this->name; }

	/** Create a new object of this type.
	 */
	virtual Object *Create() anope_abstract;

	/** Get or otherwise create an object of this type
	 * with the given ID.
	 */
	virtual Object *Require(Serialize::ID) anope_abstract;

	/** Find a field on this type
	 */
	FieldBase *GetField(const Anope::string &name);

	/** Get all fields of this type
	 */
	std::vector<FieldBase *> GetFields();

	Module* GetOwner() const { return this->owner; }

	static TypeBase *Find(const Anope::string &name);

	static std::vector<TypeBase *> GetTypes();
};

template<typename T, typename Base = Serialize::TypeBase>
class Serialize::Type : public Base
{
 public:
	Type(Module *module) : Base(module, T::NAME) { }
	Type(Module *module, const Anope::string &name) : Base(module, name) { }

	Object *Create() override
	{
		return new T(this);
	}

	Object *Require(Serialize::ID id) override
	{
		return RequireID(id);
	}

	T* RequireID(ID id)
	{
		Object *s = Serialize::GetID(this, id);
		if (s == nullptr)
			return new T(this, id);

		if (s->GetSerializableType() != this)
		{
			Anope::Logger.Debug("Mismatch for required id {0}, is of type {1} but wants {2}", id, s->GetSerializableType()->GetName(), this->GetName());
			return nullptr;
		}

		return static_cast<T *>(s);
	}
};

/** A reference to a serializable object. Serializable objects may not always
 * exist in memory at all times, like if they exist in an external database,
 * so you can't hold pointers to them. Instead, hold a Serialize::Reference
 * which will properly fetch the object on demand from the underlying database
 * system.
 */
template<typename T>
class Serialize::Reference
{
 protected:
	bool valid = false;
	TypeBase *type;
	/* ID of the object which we reference */
	ID id;

 public:
	Serialize::Reference<T>& operator=(T* obj)
	{
		if (obj != nullptr)
		{
			type = obj->GetSerializableType();
			id = obj->id;
			valid = true;
		}
		else
		{
			valid = false;
		}
		return *this;
	}

	explicit operator bool() const
	{
		return Dereference() != nullptr;
	}

	operator T*() const { return Dereference(); }

	T* operator*() const { return Dereference(); }
	T* operator->() const { return Dereference(); }

 private:
	T* Dereference() const
	{
		if (!valid)
			return nullptr;

		Object *targ = GetID(type, id);
		if (targ != nullptr && targ->GetSerializableType() == type)
			return anope_dynamic_static_cast<T*>(targ);

		EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeDeref, id, type);
		if (result == EVENT_ALLOW)
			return anope_dynamic_static_cast<T *>(type->Require(id));

		return nullptr;
	}
};

/** A field, associated with a type.
 */
class Serialize::FieldBase : public Service
{
 public:
	static constexpr const char *NAME = "fieldbase";

	Anope::string serialize_type; // type the field is on
	Anope::string serialize_name; // field name

	/** For fields which reference other Objects. If true, when
	 * the object the field references gets deleted, this object
	 * gets deleted too.
	 */
	bool depends = false;

	bool is_object = false;

	FieldBase(Module *, const Anope::string &, const Anope::string &, bool);
	virtual ~FieldBase();
	void Unregister();

	/** Serialize value of this field on the given object to string form
	 */
	virtual Anope::string SerializeToString(Object *s) anope_abstract;

	/** Unserialize value of this field on the given object from string form
	 */
	virtual void UnserializeFromString(Object *s, const Anope::string &) anope_abstract;

	/** Test if the given object has the given field, only usefil for extensible fields
	 */
	virtual bool HasFieldS(Object *) anope_abstract;

	/** Unset this field on the given object
	 */
	virtual void UnsetS(Object *) anope_abstract;

	virtual void Uncache(Object *) anope_abstract;

	virtual Anope::string GetTypeName() { return ""; }
};

template<typename T>
class Serialize::FieldTypeBase : public FieldBase
{
 public:
	using FieldBase::FieldBase;

	/** Set this field to the given value on the given object.
	 */
	virtual void SetFieldS(Object *, const T &) anope_abstract;
};

/** Base class for serializable fields and serializable object fields.
 */
template<
	typename TypeImpl, // Serializable type
	typename T // actual type of field
>
class Serialize::CommonFieldBase : public FieldTypeBase<T>
{
	static_assert(std::is_base_of<Object, TypeImpl>::value, "");

	/** Extensible storage for value of fields. Only used if field
	 * isn't set. Note extensible fields can be "unset", where field 
	 * pointers are never unset, but are T().
	 */
	ExtensibleItem<T> *ext = nullptr;

	/** Storage pointer in the TypeImpl object for this field.
	 */
	Serialize::Storage<T> TypeImpl::*storage = nullptr;

 protected:
	/** Set the value of a field in storage
	 */
	void Set_(TypeImpl *object, const T &value)
	{
		if (storage != nullptr)
		{
			Serialize::Storage<T> &s = object->*storage;
			s.t = value;
			s.cached = true;
		}
		else if (ext != nullptr)
			ext->Set(object, value);
		else
			throw CoreException("No field or ext");
	}

	/* Get the value of a field from storage
	 */
	T* Get_(TypeImpl *object)
	{
		if (storage != nullptr)
			return &(object->*storage).t;
		else if (ext != nullptr)
			return ext->Get(object);
		else
			throw CoreException("No field or ext");
	}

	/** Unset a field from storage
	 */
	void Unset_(TypeImpl *object)
	{
		if (storage != nullptr)
		{
			Serialize::Storage<T> &s = object->*storage;
			s.t = T();
			s.cached = false;
		}
		else if (ext != nullptr)
			ext->Unset(object);
		else
			throw CoreException("No field or ext");
	}

	/** Check is a field is set. Only useful for
	 * extensible storage. Returns true for field storage.
	 */
	bool HasField_(TypeImpl *object)
	{
		if (storage != nullptr)
			return true;
		else if (ext != nullptr)
			return ext->HasExt(object);
		else
			throw CoreException("No field or ext");
	}

	bool Cached_(TypeImpl *object)
	{
		if (storage != nullptr)
			return (object->*storage).cached;
		else if (ext != nullptr)
			return ext->HasExt(object);
		else
			throw CoreException("No field or ext");
	}

 public:
	CommonFieldBase(Module *creator, const Anope::string &n, bool d)
		: FieldTypeBase<T>(creator, n, TypeImpl::NAME, d)
	{
		ext = new ExtensibleItem<T>(creator, TypeImpl::NAME, n);
	}

	CommonFieldBase(Serialize::TypeBase *t,
			const Anope::string &n,
			Serialize::Storage<T> TypeImpl::*s,
			bool d)
		: FieldTypeBase<T>(t->GetOwner(), n, t->GetName(), d)
		, storage(s)
	{
	}

	~CommonFieldBase()
	{
		delete ext;
	}

	/** Get the value of this field on the given object
	 */
	virtual T GetField(TypeImpl *) anope_abstract;

	/** Unset this field on the given object
	 */
	virtual void UnsetField(TypeImpl *) anope_abstract;

	void UnsetS(Object *s) override
	{
		UnsetField(Upcast(s));
	}

	bool HasFieldS(Object *s) override
	{
		return HasField(Upcast(s));
	}

	/** Cast a serializable object of type Object to type TypeImpl,
	 * if appropriate
	 */
	TypeImpl* Upcast(Object *s)
	{
		if (this->serialize_type != s->GetSerializableType()->GetName())
		{
			return nullptr;
		}

		return anope_dynamic_static_cast<TypeImpl *>(s);
	}

	bool HasField(TypeImpl *s)
	{
		EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeHasField, s, this);
		if (result != EVENT_CONTINUE)
			return true;

		return this->HasField_(s);
	}

	void Uncache(Object *s) override
	{
		Unset_(Upcast(s));
	}
};

/** Class for all fields that aren't to other serializable objects
 */
template<typename TypeImpl, typename T>
class Serialize::Field : public CommonFieldBase<TypeImpl, T>
{
 public:
	Field(Module *creator, const Anope::string &n) : CommonFieldBase<TypeImpl, T>(creator, n, false)
	{
	}

	Field(TypeBase *t, const Anope::string &n, Serialize::Storage<T> TypeImpl::*f) : CommonFieldBase<TypeImpl, T>(t, n, f, false)
	{
	}

	T GetField(TypeImpl *s) override
	{
		T* t = this->Get_(s);

		// If this field is cached
		if (t && this->Cached_(s))
			return *t;

		// Query modules
		Anope::string value;
		EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeGet, s, this, value);
		if (result == EVENT_ALLOW)
		{
			// module returned us data, so we unserialize it
			T t2 = this->Unserialize(value);

			// set & cache
			OnSet(s, t2);
			this->Set_(s, t2);

			return t2;
		}

		return t ? *t : T();
	}

	void SetFieldS(Object *s, const T &value) override
	{
		SetField(this->Upcast(s), value);
	}

	/**
	 * Override to hook to changes in the internally
	 * cached value
	 */
	virtual void OnSet(TypeImpl *s, const T &value) { }

	void SetField(TypeImpl *s, const T &value)
	{
		Anope::string strvalue = this->Serialize(value);
		EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeSet, s, this, strvalue);

		OnSet(s, value);
		this->Set_(s, value);
	}

	void UnsetField(TypeImpl *s) override
	{
		EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeUnset, s, this);

		this->Unset_(s);
	}

	Anope::string Serialize(const T& t)
	{
		try
		{
			return stringify(t);
		}
		catch (const ConvertException &)
		{
			return "";
		}
	}

	T Unserialize(const Anope::string &str)
	{
		if (str.empty())
			return T();

		try
		{
			return convertTo<T>(str);
		}
		catch (const ConvertException &)
		{
			return T();
		}
	}

	Anope::string SerializeToString(Object *s) override
	{
		T t = GetField(this->Upcast(s));
		return this->Serialize(t);
	}

	void UnserializeFromString(Object *s, const Anope::string &v) override
	{
		T t = this->Unserialize(v);
		SetField(this->Upcast(s), t);
	}

	void Set(Extensible *obj, const T &value)
	{
		TypeImpl *s = anope_dynamic_static_cast<TypeImpl *>(obj);
		SetField(s, value);
	}

	void Unset(Extensible *obj)
	{
		TypeImpl *s = anope_dynamic_static_cast<TypeImpl *>(obj);
		this->UnsetField(s);
	}

	T Get(Extensible *obj)
	{
		TypeImpl *s = anope_dynamic_static_cast<TypeImpl *>(obj);
		return this->GetField(s);
	}

	bool HasExt(Extensible *obj)
	{
		TypeImpl *s = anope_dynamic_static_cast<TypeImpl *>(obj);
		return this->HasField(s);
	}
};

/** Class for all fields that contain a reference to another serializable object.
 */
template<typename TypeImpl, typename T>
class Serialize::ObjectField : public CommonFieldBase<TypeImpl, T>
{
 public:
	ObjectField(Module *creator, const Anope::string &n) : CommonFieldBase<TypeImpl, T>(creator, n, false)
	{
		this->is_object = true;
	}

	ObjectField(TypeBase *t, const Anope::string &n, Serialize::Storage<T> TypeImpl::*field, bool d = false) : CommonFieldBase<TypeImpl, T>(t, n, field, d)
	{
		this->is_object = true;
	}

	T GetField(TypeImpl *s)
	{
		T *t = this->Get_(s);

		if (t && this->Cached_(s))
			return *t;

		Anope::string type;
		ID sid;
		EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeGetSerializable, s, this, type, sid);
		if (result != EVENT_CONTINUE)
		{
			Serialize::TypeBase *base = Serialize::TypeBase::Find(type);
			if (base == nullptr)
			{
				Anope::Logger.Debug2("OnSerializeGetSerializable returned unknown type {0}", type);
				return nullptr;
			}

			T t2 = result == EVENT_ALLOW ? static_cast<T>(base->Require(sid)) : nullptr;

			OnSet(s, t2);
			this->Set_(s, t2);

			return t2;
		}

		return t ? *t : T();
	}

	void SetFieldS(Object *s, const T &value) override
	{
		SetField(this->Upcast(s), value);
	}

	virtual void OnSet(TypeImpl *s, T value) { }

	void SetField(TypeImpl *s, T value)
	{
		EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeSetSerializable, s, this, value);

		T *old = this->Get_(s);
		if (old != nullptr && *old != nullptr)
			s->RemoveEdge(*old, this);

		OnSet(s, value);
		this->Set_(s, value);

		if (value != nullptr)
			s->AddEdge(value, this);
	}

	void UnsetField(TypeImpl *s) override
	{
		EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeUnsetSerializable, s, this);

		this->Unset_(s);
	}

	Anope::string SerializeToString(Object *s) override
	{
		T t = GetField(this->Upcast(s));
		return this->Serialize(t);
	}

	void UnserializeFromString(Object *s, const Anope::string &v) override
	{
		T t = this->Unserialize(v);
		SetField(this->Upcast(s), t);
	}

	Anope::string Serialize(const T& t)
	{
		return t ? t->GetSerializableType()->GetName() + ":" + stringify(t->id) : "";
	}

	T Unserialize(const Anope::string &str)
	{
		size_t c = str.rfind(':');
		if (c == Anope::string::npos)
			return nullptr;

		Anope::string type = str.substr(0, c);
		ID id;
		try
		{
			id = convertTo<ID>(str.substr(c + 1));
		}
		catch (const ConvertException &)
		{
			return nullptr;
		}

		TypeBase *t = TypeBase::Find(type);
		if (!t)
		{
			return nullptr;
		}

		return anope_dynamic_static_cast<T>(t->Require(id));
	}

	Anope::string GetTypeName() override
	{
		const char* const name = std::remove_pointer<T>::type::NAME;
		return name;
	}
};

template<typename T>
class Serialize::Storage
{
 public:
	T t = T();
	bool cached = false;
};

template<typename T>
T Serialize::GetObject()
{
	std::vector<T> v = GetObjects<T>();
	return v.empty() ? nullptr : v[0];
}

template<typename T>
std::vector<T> Serialize::GetObjects_(TypeBase *type)
{
	std::vector<T> o;
	std::vector<ID> ids;
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeList, type, ids);
	if (result == EVENT_ALLOW)
	{
		for (ID id : ids)
		{
			Object *s = type->Require(id);
			if (s)
				o.push_back(anope_dynamic_static_cast<T>(s));
		}
		return o;
	}

	std::transform(type->objects.begin(), type->objects.end(), std::back_inserter(o), [](Object *e) { return anope_dynamic_static_cast<T>(e); });
	return o;
}

template<typename T>
std::vector<T> Serialize::GetObjects(const Anope::string &name)
{
	std::vector<T> objs;

	for (TypeBase *t : GetTypes(name))
		for (T obj : GetObjects_<T>(t))
			objs.push_back(obj);

	return objs;
}

template<typename T>
std::vector<T> Serialize::GetObjects()
{
	const char* const name = std::remove_pointer<T>::type::NAME;
	return GetObjects<T>(name);
}

template<typename T>
T Serialize::New()
{
	const char* const name = std::remove_pointer<T>::type::NAME;
	Serialize::TypeBase *type = TypeBase::Find(name);

	if (type == nullptr)
	{
		Anope::Logger.Debug("Serialize::New with unknown type {0}", name);
		return nullptr;
	}

	return static_cast<T>(type->Create());
}

inline std::vector<Serialize::Object *> Serialize::Object::GetRefs(Serialize::TypeBase *type)
{
	std::vector<Serialize::TypeBase *> types = GetTypes(type->GetName());
	std::vector<Object *> objs;

	if (types.empty())
	{
		Anope::Logger.Debug("GetRefs for unknown type on #{0} type {1} named {2}", this->id, s_type->GetName(), type->GetName());
		return objs;
	}

	for (Serialize::TypeBase *t : types)
		for (const Serialize::Edge &edge : GetEdges(t))
			if (!edge.direction)
				objs.push_back(edge.other);

	return objs;
}

template<typename T>
std::vector<T> Serialize::Object::GetRefs()
{
	const char* const name = std::remove_pointer<T>::type::NAME;
	std::vector<Serialize::TypeBase *> types = GetTypes(name);
	std::vector<T> objs;

	if (types.empty())
	{
		Anope::Logger.Debug("GetRefs for unknown type on #{0} type {1} named {2}", this->id, s_type->GetName(), name);
		return objs;
	}

	for (Serialize::TypeBase *t : types)
		for (const Serialize::Edge &edge : GetEdges(t))
			if (!edge.direction)
				objs.push_back(anope_dynamic_static_cast<T>(edge.other));

	return objs;
}

template<typename T>
void Serialize::Object::SetS(const Anope::string &name, const T &what)
{
	FieldBase *field = s_type->GetField(name);
	if (field == nullptr)
	{
		Anope::Logger.Debug("Set for unknown field {0} on {1}", name, s_type->GetName());
		return;
	}

	FieldTypeBase<T> *fieldt = static_cast<FieldTypeBase<T> *>(field);
	fieldt->SetFieldS(this, what);
}

template<typename T>
void Serialize::Object::UnsetS(const Anope::string &name)
{
	FieldBase *field = s_type->GetField(name);
	if (field == nullptr)
	{
		Anope::Logger.Debug("Unset for unknown field {0} on {1}", name, s_type->GetName());
		return;
	}

	FieldTypeBase<T> *fieldt = static_cast<FieldTypeBase<T> *>(field);
	fieldt->UnsetS(this);
}

inline bool Serialize::Object::HasFieldS(const Anope::string &name)
{
	FieldBase *field = s_type->GetField(name);
	if (field == nullptr)
	{
		Anope::Logger.Debug("HasField for unknown field {0} on {1}", name, s_type->GetName());
		return false;
	}

	FieldTypeBase<void *> *fieldt = static_cast<FieldTypeBase<void *> *>(field);
	return fieldt->HasFieldS(this);
}

