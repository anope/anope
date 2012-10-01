/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


#include "services.h"
#include "anope.h"
#include "serialize.h"
#include "modules.h"

std::vector<Anope::string> SerializeType::type_order;
Anope::map<SerializeType *> SerializeType::types;
std::list<Serializable *> *Serializable::serializable_items;

stringstream::stringstream() : std::stringstream(), type(Serialize::DT_TEXT), _max(0)
{
}

stringstream::stringstream(const stringstream &ss) : std::stringstream(ss.str()), type(Serialize::DT_TEXT), _max(0)
{
}

Anope::string stringstream::astr() const
{
	return this->str();
}

std::istream &stringstream::operator>>(Anope::string &val)
{
	val = this->str();
	return *this;
}

bool stringstream::operator==(const stringstream &other) const
{
	return this->astr() == other.astr();
}

bool stringstream::operator!=(const stringstream &other) const
{
	return !(*this == other);
}

stringstream &stringstream::setType(Serialize::DataType t)
{
	this->type = t;
	return *this;
}

Serialize::DataType stringstream::getType() const
{
	return this->type;
}

stringstream &stringstream::setMax(unsigned m)
{
	this->_max = m;
	return *this;
}

unsigned stringstream::getMax() const
{
	return this->_max;
}

Serializable::Serializable() : last_commit_time(0), id(0)
{
	throw CoreException("Default Serializable constructor?");
}

Serializable::Serializable(const Anope::string &serialize_type) : last_commit_time(0), id(0)
{
	if (serializable_items == NULL)
		serializable_items = new std::list<Serializable *>();
	serializable_items->push_back(this);

	this->s_type = SerializeType::Find(serialize_type);

	this->s_iter = serializable_items->end();
	--this->s_iter;

	FOREACH_MOD(I_OnSerializableConstruct, OnSerializableConstruct(this));
}

Serializable::Serializable(const Serializable &other) : last_commit_time(0), id(0)
{
	serializable_items->push_back(this);
	this->s_iter = serializable_items->end();
	--this->s_iter;

	this->s_type = other.s_type;

	FOREACH_MOD(I_OnSerializableConstruct, OnSerializableConstruct(this));
}

Serializable::~Serializable()
{
	serializable_items->erase(this->s_iter);
}

Serializable &Serializable::operator=(const Serializable &)
{
	return *this;
}

void Serializable::destroy()
{
	if (!this)
		return;

	FOREACH_MOD(I_OnSerializableDestruct, OnSerializableDestruct(this));

	delete this;
}

void Serializable::QueueUpdate()
{
	FOREACH_MOD(I_OnSerializableUpdate, OnSerializableUpdate(this));
}

bool Serializable::IsCached()
{
	return this->last_commit == this->serialize();
}

void Serializable::UpdateCache()
{
	this->last_commit = this->serialize();
}

bool Serializable::IsTSCached()
{
	return this->last_commit_time == Anope::CurTime;
}

void Serializable::UpdateTS()
{
	this->last_commit_time = Anope::CurTime;
}

SerializeType* Serializable::GetSerializableType() const
{
	return this->s_type;
}

const std::list<Serializable *> &Serializable::GetItems()
{
	return *serializable_items;
}

SerializeType::SerializeType(const Anope::string &n, unserialize_func f, Module *o)  : name(n), unserialize(f), owner(o), timestamp(0)
{
	type_order.push_back(this->name);
	types[this->name] = this;
}

SerializeType::~SerializeType()
{
	std::vector<Anope::string>::iterator it = std::find(type_order.begin(), type_order.end(), this->name);
	if (it != type_order.end())
		type_order.erase(it);
	types.erase(this->name);
}

const Anope::string &SerializeType::GetName()
{
	return this->name;
}

Serializable *SerializeType::Unserialize(Serializable *obj, Serialize::Data &data)
{
	return this->unserialize(obj, data);
}

void SerializeType::Check()
{
	FOREACH_MOD(I_OnSerializeCheck, OnSerializeCheck(this));
}

time_t SerializeType::GetTimestamp() const
{
	return this->timestamp;
}

void SerializeType::UpdateTimestamp()
{
	this->timestamp = Anope::CurTime;
}

Module* SerializeType::GetOwner() const
{
	return this->owner;
}

SerializeType *SerializeType::Find(const Anope::string &name)
{
	Anope::map<SerializeType *>::iterator it = types.find(name);
	if (it != types.end())
		return it->second;
	return NULL;
}

const std::vector<Anope::string> &SerializeType::GetTypeOrder()
{
	return type_order;
}

