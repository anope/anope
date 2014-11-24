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


#include "services.h"
#include "anope.h"
#include "serialize.h"
#include "modules.h"
#include "event.h"

using namespace Serialize;

std::map<Anope::string, TypeBase *> Serialize::Types;

std::unordered_map<ID, Object *> Serialize::objects;

std::vector<FieldBase *> Serialize::serializableFields;

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
	for (FieldBase *field : serializableFields)
		if (field->creator == m)
			field->Unregister();

	for (const std::pair<Anope::string, TypeBase *> &p : Types)
	{
		TypeBase *s = p.second;

		if (s->GetOwner() == m)
			s->Unregister();
	}
}

std::vector<Edge> Object::GetRefs(TypeBase *type)
{
	std::vector<Edge> refs;
	EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeGetRefs, this, type, refs);
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
	EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializableGetId, i);
	if (result != EVENT_ALLOW)
	{
		while (GetID(++curid));
		i = curid;
	}

	id = i;
	objects[id] = this;

	this->s_type = type;

	type->objects.insert(this);

	Log(LOG_DEBUG) << "Creating object id #" << id << " type " << type->GetName();

	Event::OnSerialize(&Event::SerializeEvents::OnSerializableCreate, this);
}

Object::Object(TypeBase *type, ID i)
{
	this->id = i;
	objects[i] = this;

	this->s_type = type;

	type->objects.insert(this);

	Log(LOG_DEBUG) << "Creating object from id #" << id << " type " << type->GetName();
}

Object::~Object()
{
	Log(LOG_DEBUG) << "Destructing object id #" << id << " type " << s_type->GetName();

	/* Remove in memory edges */
 cont:
	for (const std::pair<TypeBase *, std::vector<Edge>> &p : edges)
		for (const Edge &edge : p.second)
		{
			if (!edge.direction)
			{
				Log(LOG_DEBUG_2) << "Removing edge from object id #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName() << " on field " << edge.field->GetName();
				edge.other->RemoveEdge(this, edge.field);
			}
			else
			{
				Log(LOG_DEBUG_2) << "Removing edge to object id #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName() << " on field " << edge.field->GetName();
				this->RemoveEdge(edge.other, edge.field);
			}
			goto cont;
		}

	objects.erase(id);
	s_type->objects.erase(this);
}

void Object::Delete()
{
	Log(LOG_DEBUG) << "Deleting object id #" << id << " type " << s_type->GetName();

	/* Delete dependant objects */
	for (const Edge &edge : GetRefs(nullptr))
	{
		Object *other = edge.other;
		FieldBase *field = edge.field;

		if (edge.direction)
			continue;

		if (field->depends)
		{
			Log(LOG_DEBUG_2) << "Deleting dependent object #" << other->id << " type " << other->GetSerializableType()->GetName() << " due to edge on " << field->GetName();
			other->Delete();
		}
		else
		{
			Log(LOG_DEBUG_2) << "Unsetting field " << field->GetName() << " on object #" << other->id << " type " << other->GetSerializableType()->GetName();
			field->UnsetS(other);
		}
	}

	Event::OnSerialize(&Event::SerializeEvents::OnSerializableDelete, this);

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

TypeBase::TypeBase(Module *o, const Anope::string &n) : Service(o, "Serialize::Type", n), name(n), owner(o)
{
	Types[this->name] = this;
}

TypeBase::~TypeBase()
{
	if (!Serialize::GetObjects<Object *>(this).empty())
		throw CoreException("Type destructing with objects still alive");

	if (parent)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end())
			parent->children.erase(it);
	}

	for (TypeBase *child : children)
		child->parent = nullptr;

	Types.erase(this->name);
}

void TypeBase::Unregister()
{
	Log(LOG_DEBUG_2) << "Unregistering type " << this->GetName();

	for (Object *obj : GetObjects<Object *>(this))
		obj->Delete();

 cont:
	for (FieldBase *field : fields)
	{
		field->Unregister();
		goto cont;
	}
}

void TypeBase::SetParent(TypeBase *other)
{
	parent = other;
	other->children.push_back(this);
}

Serialize::FieldBase *TypeBase::GetField(const Anope::string &fname)
{
	for (FieldBase *f : fields)
		if (f->GetName() == fname)
			return f;
	return nullptr;
}

std::vector<TypeBase *> TypeBase::GetSubTypes()
{
	std::vector<TypeBase *> v;

	v.push_back(this);

	for (TypeBase *b : children)
	{
		std::vector<TypeBase *> c = b->GetSubTypes();
		v.insert(v.end(), c.begin(), c.end());
	}

	return v;
}

TypeBase *TypeBase::Find(const Anope::string &name)
{
	std::map<Anope::string, TypeBase *>::iterator it = Types.find(name);
	if (it != Types.end())
		return it->second;
	return nullptr;
}

const std::map<Anope::string, Serialize::TypeBase *>& TypeBase::GetTypes()
{
	return Types;
}

void Serialize::FieldBase::Listener::OnAcquire()
{
	TypeBase *t = *this;

	if (base->unregister)
		return;

	Log(LOG_DEBUG_2) << "Acquired type reference to " << t->GetName() << " for field " << base->GetName();

	auto it = std::find(t->fields.begin(), t->fields.end(), base);
	if (it == t->fields.end())
		t->fields.push_back(base);
}

FieldBase::FieldBase(Module *c, const Anope::string &n, const ServiceReference<TypeBase> &t, bool d) : type(this, t), creator(c), name(n), depends(d)
{
	serializableFields.push_back(this);

	type.Check();
}

FieldBase::~FieldBase()
{
	if (std::find(type->fields.begin(), type->fields.end(), this) != type->fields.end())
		Log(LOG_DEBUG) << "Destructing field " << this->GetName() << " on " << type->GetName() << " while still registered!";

	auto it = std::find(serializableFields.begin(), serializableFields.end(), this);
	if (it != serializableFields.end())
		serializableFields.erase(it);
}

void FieldBase::Unregister()
{
	unregister = true;

	Log(LOG_DEBUG_2) << "Unregistering field " << this->GetName() << " on " << type->GetName();

	/* find edges on this field */
	for (Object *s : Serialize::GetObjects<Object *>(type))
	{
		for (const std::pair<TypeBase *, std::vector<Edge>> &p : s->edges)
			for (const Edge &edge : p.second)
				if (edge.direction && edge.field == this)
				{
					Log(LOG_DEBUG_2) << "Removing edge on #" << s->id << " <-> #" << edge.other->id << " type " << edge.other->GetSerializableType()->GetName();
					s->RemoveEdge(edge.other, edge.field);
					goto cont;
				}
		cont:;
	}

	auto it = std::find(type->fields.begin(), type->fields.end(), this);
	if (it != type->fields.end())
		type->fields.erase(it);
}
