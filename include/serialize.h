/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
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
	class AbstractType;
	
	class FieldBase;
	template<typename> class FieldTypeBase;
	template<typename, typename> class CommonFieldBase;
	template<typename, typename> class Field;
	template<typename, typename> class ObjectField;

	template<typename T, typename> class Type;
	template<typename T> class Reference;
	template<typename T> class TypeReference;

	extern std::map<Anope::string, Serialize::TypeBase *> Types;
	// by id
	extern std::unordered_map<ID, Object *> objects;
	extern std::vector<FieldBase *> serializableFields;
	
	extern Object *GetID(ID id);

	template<typename T>
	inline std::vector<T> GetObjects(Serialize::TypeBase *type);

	extern void Clear();

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
}

class CoreExport Serialize::Object : public Extensible, public virtual Base
{
 private:
	friend class Serialize::FieldBase;

	/* The type of item this object is */
	TypeBase *s_type;

	std::map<TypeBase *, std::vector<Edge>> edges;

	std::vector<Serialize::Edge> GetRefs(TypeBase *);

 protected:
	Object(TypeBase *type);
	Object(TypeBase *type, ID);

 public:
	virtual ~Object();

	virtual void Delete();

	/* Unique ID for this object */
	ID id;

	void AddEdge(Object *other, FieldBase *field);

	void RemoveEdge(Object *other, FieldBase *field);

	template<typename T>
	T GetRef(TypeBase *type)
	{
		std::vector<T> t = GetRefs<T>(type);
		return !t.empty() ? t[0] : nullptr;
	}

	template<typename T>
	std::vector<T> GetRefs(TypeBase *type);

	template<
		typename Type, // inherits from Serialize::Type
		template<typename, typename> class Field,
		typename TypeImpl, // implementation of Type
		typename T // type of the Extensible
	>
	T Get(Field<TypeImpl, T> Type::*field)
	{
		Type *t = static_cast<Type *>(s_type);
		Field<TypeImpl, T>& f = t->*field;
		return f.GetField(f.Upcast(this));
	}

	/* the return value can't be inferred by the compiler so we supply this function to allow us to specify it */
	template<typename Ret, typename Field, typename Type>
	Ret Get(Field Type::*field)
	{
		Type *t = static_cast<Type *>(s_type);
		Field& f = t->*field;
		return f.GetField(f.Upcast(this));
	}

	template<
		typename Type,
		template<typename, typename> class Field,
		typename TypeImpl,
		typename T
	>
	void Set(Field<TypeImpl, T> Type::*field, const T& value)
	{
		Type *t = static_cast<Type *>(s_type);
		Field<TypeImpl, T>& f = t->*field;
		f.SetField(f.Upcast(this), value);
	}

	template<typename Field, typename Type, typename T>
	void Set(Field Type::*field, const T& value)
	{
		Type *t = static_cast<Type *>(s_type);
		Field& f = t->*field;
		f.SetField(f.Upcast(this), value);
	}

	/** Get the type of serializable object this is
	 * @return The serializable object type
	 */
	TypeBase* GetSerializableType() const { return this->s_type; }

	template<typename T>
	void SetS(const Anope::string &name, const T &what);

	template<typename T>
	void UnsetS(const Anope::string &name);

	bool HasFieldS(const Anope::string &name);
};

class CoreExport Serialize::TypeBase : public Service
{
	Anope::string name;

	TypeBase *parent = nullptr;
	std::vector<TypeBase *> children;

	/* Owner of this type. Used for placing objects of this type in separate databases
	 * based on what module, if any, owns it.
	 */
	Module *owner;

 public:
	std::vector<FieldBase *> fields;
	std::set<Object *> objects;

	TypeBase(Module *owner, const Anope::string &n);
	~TypeBase();

	void Unregister();

 protected:
	void SetParent(TypeBase *other);

 public:
	/** Gets the name for this type
	 * @return The name, eg "NickAlias"
	 */
	const Anope::string &GetName() { return this->name; }

	template<typename T>
	std::vector<T> List()
	{
		std::vector<ID> ids;
		EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeList, this, ids);
		if (result == EVENT_ALLOW)
		{
			std::vector<T> o;
			for (ID id : ids)
			{
				Object *s = Require(id);
				if (s)
					o.push_back(anope_dynamic_static_cast<T>(s));
			}
			return o;
		}

		return GetObjects<T>(this);
	}

	virtual Object *Create() anope_abstract;
	virtual Object *Require(Serialize::ID) anope_abstract;

	FieldBase *GetField(const Anope::string &name);

	Module* GetOwner() const { return this->owner; }

	std::vector<TypeBase *> GetSubTypes();

	static TypeBase *Find(const Anope::string &name);

	static const std::map<Anope::string, TypeBase *>& GetTypes();
};

class Serialize::AbstractType : public Serialize::TypeBase
{
 public:
	using Serialize::TypeBase::TypeBase;

	Object *Create() override
	{
		return nullptr;
	}

	Object *Require(ID id) override
	{
		return nullptr;
	}
};

template<typename T, typename Base = Serialize::TypeBase>
class Serialize::Type : public Base
{
 public:
	using Base::Base;

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
		Object *s = Serialize::GetID(id);
		if (s == nullptr)
			return new T(this, id);

		if (s->GetSerializableType() != this)
			return nullptr;

		return static_cast<T *>(s);
	}
};

template<typename T>
class Serialize::TypeReference : public ServiceReference<Serialize::TypeBase>
{
 public:
	TypeReference(const Anope::string &name) : ServiceReference<Serialize::TypeBase>("Serialize::Type", name) { }

	T* Create()
	{
		TypeBase *t = *this;
		return static_cast<T *>(t->Create());
	}

	TypeBase *ToType()
	{
		TypeBase *t = *this;
		return t;
	}
};

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

		Object *targ = GetID(id);
		if (targ != nullptr && targ->GetSerializableType() == type)
			return anope_dynamic_static_cast<T*>(targ);

		EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeDeref, id, type);
		if (result == EVENT_ALLOW)
			return anope_dynamic_static_cast<T *>(type->Require(id));

		return nullptr;
	}
};

class Serialize::FieldBase
{
 public:
	struct Listener : ServiceReference<TypeBase>
	{
		FieldBase *base;

		Listener(FieldBase *b, const ServiceReference<TypeBase> &t) : ServiceReference<TypeBase>(t), base(b) { }

		void OnAcquire() override;
	} type;

	Module *creator;
	Anope::string name;
	bool depends;
	bool unregister = false;

	FieldBase(Module *, const Anope::string &, const ServiceReference<Serialize::TypeBase> &, bool);
	virtual ~FieldBase();
	void Unregister();

	const Anope::string &GetName() { return name; }
	virtual Anope::string SerializeToString(Object *s) anope_abstract;
	virtual void UnserializeFromString(Object *s, const Anope::string &) anope_abstract;
	virtual void CacheMiss(Object *) anope_abstract;

	virtual bool HasFieldS(Object *) anope_abstract;
	virtual void UnsetS(Object *) anope_abstract;
};

template<typename T>
class Serialize::FieldTypeBase : public FieldBase
{
 public:
	using FieldBase::FieldBase;

	virtual void SetFieldS(Object *, const T &) anope_abstract;
};

template<typename TypeImpl, typename T>
class Serialize::CommonFieldBase : public FieldTypeBase<T>
{
 protected:
	ExtensibleItem<T> ext;

 public:
	CommonFieldBase(Serialize::TypeBase *t, const Anope::string &n, bool depends)
		: FieldTypeBase<T>(t->GetOwner(), n, ServiceReference<Serialize::TypeBase>("Serialize::Type", t->GetName()), depends)
		, ext(t->GetOwner(), t->GetName(), n)
	{
	}

	CommonFieldBase(Module *creator, Serialize::TypeReference<TypeImpl> &typeref, const Anope::string &n, bool depends)
		: FieldTypeBase<T>(creator, n, typeref, depends)
		, ext(creator, typeref.GetName(), n)
	{
	}

	void CacheMiss(Object *s) override
	{
		ext.Set(s, T());
	}

	virtual void UnsetField(TypeImpl *) anope_abstract;

	void UnsetS(Object *s) override
	{
		UnsetField(Upcast(s));
	}

	TypeImpl* Upcast(Object *s)
	{
		if (this->type != s->GetSerializableType())
			return nullptr;

		return anope_dynamic_static_cast<TypeImpl *>(s);
	}

	bool HasFieldS(Object *s) override
	{
		return HasField(Upcast(s));
	}

	bool HasField(TypeImpl *s)
	{
		EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeHasField, s, this);
		return result != EVENT_CONTINUE;
	}
};

template<typename TypeImpl, typename T>
class Serialize::Field : public CommonFieldBase<TypeImpl, T>
{
 public:
	Field(TypeBase *t, const Anope::string &n) : CommonFieldBase<TypeImpl, T>(t, n, false)
	{
	}

	Field(Module *creator, TypeReference<TypeImpl> &typeref, const Anope::string &n) : CommonFieldBase<TypeImpl, T>(creator, typeref, n, false)
	{
	}

	T GetField(TypeImpl *s)
	{
		T* t = this->ext.Get(s);
		if (t)
			return *t;

		Anope::string value;
		EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeGet, s, this, value);
		if (result == EVENT_ALLOW)
		{
			// module returned us data, so we unserialize it
			T t2 = this->Unserialize(value);

			this->ext.Set(s, t2);

			return t2;
		}

		return T();
	}

	void SetFieldS(Object *s, const T &value) override
	{
		SetField(this->Upcast(s), value);
	}

	virtual void SetField(TypeImpl *s, const T &value)
	{
		Anope::string strvalue = this->Serialize(value);
		Event::OnSerialize(&Event::SerializeEvents::OnSerializeSet, s, this, strvalue);

		this->ext.Set(s, value);
	}

	void UnsetField(TypeImpl *s)
	{
		Event::OnSerialize(&Event::SerializeEvents::OnSerializeUnset, s, this);

		this->ext.Unset(s);
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
		if (str.empty()) // for caching not set
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

template<typename TypeImpl, typename T>
class Serialize::ObjectField : public CommonFieldBase<TypeImpl, T>
{
 public:
	ObjectField(TypeBase *t, const Anope::string &n, bool d = false) : CommonFieldBase<TypeImpl, T>(t, n, d)
	{
	}

	T GetField(TypeImpl *s)
	{
		T *t = this->ext.Get(s);
		if (t)
			return *t;

		Anope::string type;
		ID sid;
		EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeGetSerializable, s, this, type, sid);
		if (result != EVENT_CONTINUE)
		{
			Serialize::TypeBase *base = Serialize::TypeBase::Find(type);
			if (base == nullptr)
				return nullptr;

			T t2 = result == EVENT_ALLOW ? static_cast<T>(base->Require(sid)) : nullptr;

			this->ext.Set(s, t2);

			return t2;
		}

		return T();
	}

	void SetFieldS(Object *s, const T &value) override
	{
		SetField(this->Upcast(s), value);
	}

	virtual void SetField(TypeImpl *s, T value)
	{
		Event::OnSerialize(&Event::SerializeEvents::OnSerializeSetSerializable, s, this, value);

		T *old = this->ext.Get(s);
		if (old != nullptr && *old != nullptr)
			s->RemoveEdge(*old, this);

		this->ext.Set(s, value);

		if (value != nullptr)
			s->AddEdge(value, this);
	}

	void UnsetField(TypeImpl *s)
	{
		Event::OnSerialize(&Event::SerializeEvents::OnSerializeUnsetSerializable, s, this);

		this->ext.Unset(s);
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
};

template<typename T>
std::vector<T> Serialize::GetObjects(Serialize::TypeBase *type)
{
	std::vector<T> objs;

	for (TypeBase *t : type->GetSubTypes())
		for (Object *s : t->objects)
			objs.push_back(anope_dynamic_static_cast<T>(s));

	return objs;
}

template<typename T>
std::vector<T> Serialize::Object::GetRefs(Serialize::TypeBase *type)
{
	std::vector<T> objs;

	if (type == nullptr)
	{
		Log(LOG_DEBUG) << "GetRefs for unknown type on #" << this->id << " type " << s_type->GetName();
		return objs;
	}

	for (Serialize::TypeBase *t : type->GetSubTypes())
		for (const Serialize::Edge &edge : GetRefs(t))
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
		Log(LOG_DEBUG) << "Set for unknown field " << name << " on " << s_type->GetName();
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
		Log(LOG_DEBUG) << "Unset for unknown field " << name << " on " << s_type->GetName();
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
		Log(LOG_DEBUG) << "HasField for unknown field " << name << " on " << s_type->GetName();
		return false;
	}

	FieldTypeBase<void *> *fieldt = static_cast<FieldTypeBase<void *> *>(field);
	return fieldt->HasFieldS(this);
}

