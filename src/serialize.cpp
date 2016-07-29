/*
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */


#include "services.h"
#include "anope.h"
#include "serialize.h"
#include "modules.h"
#include "event.h"

using namespace Serialize;

std::unordered_map<ID, Object *> Serialize::objects;

std::vector<FieldBase *> Serialize::serializableFields;

std::multimap<Anope::string, Anope::string> Serialize::child_types;

static ID curid;


Object *Serialize::GetID(ID id)
{
	auto it = objects.find(id);
	if (it != objects.end())
		return it->second;
	return nullptr;
}

void Serialize::Clear()
{
	std::unordered_map<ID, Object *> o;
	objects.swap(o);

	for (const std::pair<ID, Object *> &p : o)
		delete p.second;
}

void Serialize::Unregister(Module *m)
{
	for (TypeBase *s : ServiceManager::Get()->FindServices<Serialize::TypeBase *>())
		if (s->GetOwner() == m)
			s->Unregister();

	for (FieldBase *field : serializableFields)
		if (field->GetOwner() == m)
			field->Unregister();
}

std::vector<Edge> Object::GetRefs(TypeBase *type)
{
	std::vector<Edge> refs;
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeGetRefs, this, type, refs);
	if (result == EVENT_ALLOW)
		return refs;

	if (type == nullptr)
	{
		refs.clear();
		for (const std::pair<TypeBase *, std::vector<Edge>> &p : edges)
		{
			const std::vector<Edge> &e = p.second;
			refs.insert(refs.end(), e.begin(), e.end());
		}
		return refs;
	}

	auto it = edges.find(type);
	if (it != edges.end())
		return it->second;
	return std::vector<Edge>();
}

Object::Object(TypeBase *type)
{
	ID i;
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializableGetId, i);
	if (result != EVENT_ALLOW)
	{
		while (GetID(++curid));
		i = curid;
	}

	id = i;
	objects[id] = this;

	this->s_type = type;

	type->objects.insert(this);

	Log(LOG_DEBUG_2) << "Creating object id #" << id << " address " << static_cast<void *>(this) << " type " << type->GetName();

	EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializableCreate, this);
}

Object::Object(TypeBase *type, ID i)
{
	this->id = i;
	objects[i] = this;

	this->s_type = type;

	type->objects.insert(this);

	Log(LOG_DEBUG_2) << "Creating object from id #" << id << " address " << static_cast<void *>(this) << " type " << type->GetName();
}

Object::~Object()
{
	Log(LOG_DEBUG_2) << "Destructing object id #" << id << " address " << static_cast<void *>(this) << " type " << s_type->GetName();

	/* Remove in memory edges */
 cont:
	for (const std::pair<TypeBase *, std::vector<Edge>> &p : edges)
		for (const Edge &edge : p.second)
		{
			if (!edge.direction)
			{
				Log(LOG_DEBUG_2) << "Removing edge from object id #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName() << " on field " << edge.field->serialize_name;
				edge.other->RemoveEdge(this, edge.field);
			}
			else
			{
				Log(LOG_DEBUG_2) << "Removing edge to object id #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName() << " on field " << edge.field->serialize_name;
				this->RemoveEdge(edge.other, edge.field);
			}
			goto cont;
		}

	objects.erase(id);
	s_type->objects.erase(this);
}

void Object::Delete()
{
	Log(LOG_DEBUG_2) << "Deleting object id #" << id << " type " << s_type->GetName();

	/* Delete dependant objects */
	for (const Edge &edge : GetRefs(nullptr))
	{
		Object *other = edge.other;
		FieldBase *field = edge.field;

		if (edge.direction)
			continue;

		if (field->depends)
		{
			Log(LOG_DEBUG_2) << "Deleting dependent object #" << other->id << " type " << other->GetSerializableType()->GetName() << " due to edge on " << field->serialize_name;
			other->Delete();
		}
		else
		{
			Log(LOG_DEBUG_2) << "Unsetting field " << field->serialize_name << " on object #" << other->id << " type " << other->GetSerializableType()->GetName();
			field->UnsetS(other);
		}
	}

	EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializableDelete, this);

	delete this;
}

void Object::AddEdge(Object *other, FieldBase *field)
{
	// field = the field on 'this' object
	this->edges[other->GetSerializableType()].emplace_back(other, field, true);
	// field = the field on the other object
	other->edges[this->GetSerializableType()].emplace_back(this, field, false);
}

void Object::RemoveEdge(Object *other, FieldBase *field)
{
	std::vector<Edge> &myedges = this->edges[other->GetSerializableType()];
	auto it = std::find(myedges.begin(), myedges.end(), Edge(other, field, true));
	if (it != myedges.end())
		myedges.erase(it);
	else
		Log(LOG_DEBUG_2) << "Unable to locate edge for removal on #" << this->id << " type " << s_type->GetName() << " -> #" << other->id << " type " << other->GetSerializableType()->GetName();

	std::vector<Edge> &theiredges = other->edges[this->GetSerializableType()];
	it = std::find(theiredges.begin(), theiredges.end(), Edge(this, field, false));
	if (it != theiredges.end())
		theiredges.erase(it);
	else
		Log(LOG_DEBUG_2) << "Unable to locate edge for removal on #" << this->id << " type " << s_type->GetName() << " <- #" << other->id << " type " << other->GetSerializableType()->GetName();
}

TypeBase::TypeBase(Module *o, const Anope::string &n) : Service(o, TypeBase::NAME, n), name(n), owner(o)
{
}

TypeBase::~TypeBase()
{
	if (!Serialize::GetObjects<Object *>(this->GetName()).empty())
		throw CoreException("Type destructing with objects still alive");
}

void TypeBase::Unregister()
{
	Log(LOG_DEBUG_2) << "Unregistering type " << this->GetName();

	for (Object *obj : GetObjects<Object *>(this->GetName()))
		obj->Delete();

	for (FieldBase *field : serializableFields)
	{
		if (field->serialize_type == this->GetName())
		{
			field->Unregister();
		}
	}
}

Serialize::FieldBase *TypeBase::GetField(const Anope::string &fname)
{
	/* is this too slow? */
	for (FieldBase *fb : ServiceManager::Get()->FindServices<FieldBase *>())
		if (fb->serialize_type == this->GetName() && fb->serialize_name == fname)
			return fb;

	Log(LOG_DEBUG_2) << "GetField() for unknown field " << fname << " on " << this->GetName();

	return nullptr;
}

std::vector<Serialize::FieldBase *> TypeBase::GetFields()
{
	std::vector<Serialize::FieldBase *> fields;

	for (FieldBase *fb : ServiceManager::Get()->FindServices<FieldBase *>())
		if (fb->serialize_type == this->GetName())
			fields.push_back(fb);

	return fields;
}

TypeBase *TypeBase::Find(const Anope::string &name)
{
	return ServiceManager::Get()->FindService<TypeBase *>(name);
}

FieldBase::FieldBase(Module *c, const Anope::string &n, const Anope::string &t, bool d)
	: Service(c, FieldBase::NAME)
	, serialize_type(t)
	, serialize_name(n)
	, depends(d)
{
	serializableFields.push_back(this);
}

FieldBase::~FieldBase()
{
	auto it = std::find(serializableFields.begin(), serializableFields.end(), this);
	if (it != serializableFields.end())
		serializableFields.erase(it);
}

void FieldBase::Unregister()
{
	Log(LOG_DEBUG_2) << "Unregistering field " << serialize_name << " on " << serialize_type;

	/* find edges on this field */
	for (Object *s : Serialize::GetObjects<Object *>(serialize_type))
	{
		for (const std::pair<TypeBase *, std::vector<Edge>> &p : s->edges)
			for (const Edge &edge : p.second)
				if (edge.direction && edge.field == this)
				{
					Log(LOG_DEBUG_2) << "Removing edge on #" << s->id << " type " << s->GetSerializableType()->GetName() << " -> #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName();
					s->RemoveEdge(edge.other, edge.field);

					goto cont;
				}
		cont:;
	}
}

void Serialize::SetParent(const Anope::string &child, const Anope::string &parent)
{
	child_types.insert(std::make_pair(parent, child));
}

std::vector<Serialize::TypeBase *> Serialize::GetTypes(const Anope::string &name)
{
	std::vector<Serialize::TypeBase *> v;

	Serialize::TypeBase *t = Serialize::TypeBase::Find(name);
	if (t != nullptr)
		v.push_back(t);
	else
		Log(LOG_DEBUG_2) << "GetTypes for unknown type " << name;

	auto its = child_types.equal_range(name);
	for (; its.first != its.second; ++its.first)
	{
		t = Serialize::TypeBase::Find(its.first->second);
		if (t != nullptr)
			v.push_back(t);
	}

	return v;
}
