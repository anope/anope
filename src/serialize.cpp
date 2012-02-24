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

std::vector<Anope::string> SerializeType::type_order;
Anope::map<SerializeType *> SerializeType::types;
std::list<Serializable *> *Serializable::serizliable_items;

stringstream::stringstream() : std::stringstream(), type(Serialize::DT_TEXT), key(false), _max(0)
{
}

stringstream::stringstream(const stringstream &ss) : std::stringstream(ss.str()), type(Serialize::DT_TEXT), key(false), _max(0)
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

stringstream &stringstream::setType(Serialize::DataType t)
{
	this->type = t;
	return *this;
}

Serialize::DataType stringstream::getType() const
{
	return this->type;
}

stringstream &stringstream::setKey()
{
	this->key = true;
	return *this;
}

bool stringstream::getKey() const
{
	return this->key;
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

Serializable::Serializable()
{
	if (serizliable_items == NULL)
		serizliable_items = new std::list<Serializable *>();
	serizliable_items->push_back(this);
	this->s_iter = serizliable_items->end();
	--this->s_iter;
}

Serializable::Serializable(const Serializable &)
{
	serizliable_items->push_back(this);
	this->s_iter = serizliable_items->end();
	--this->s_iter;
}

Serializable::~Serializable()
{
	serizliable_items->erase(this->s_iter);
}

Serializable &Serializable::operator=(const Serializable &)
{
	return *this;
}

const std::list<Serializable *> &Serializable::GetItems()
{
	return *serizliable_items;
}

SerializeType::SerializeType(const Anope::string &n, unserialize_func f) : name(n), unserialize(f)
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

void SerializeType::Create(Serializable::serialized_data &data)
{
	this->unserialize(data);
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

