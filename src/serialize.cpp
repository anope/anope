/*
 *
 * (C) 2003-2025 Anope Team
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
#include "account.h"
#include "bots.h"
#include "regchannel.h"
#include "xline.h"
#include "access.h"

using namespace Serialize;

std::vector<Anope::string> Type::TypeOrder;
std::map<Anope::string, Type *> Serialize::Type::Types;
std::list<Serializable *> *Serializable::SerializableItems;

void Serialize::RegisterTypes()
{
	static NickCore::Type nc;
	static NickAlias::Type na;
	static BotInfo::Type bi;
	static ChannelInfo::Type ci;
	static ChanAccess::Type access;
	static AutoKick::Type akick;
	static Memo::Type memo;
	static XLine::Type xline;
}

void Serialize::CheckTypes()
{
	for (const auto &[_, t] : Serialize::Type::GetTypes())
	{
		t->Check();
	}
}

Serializable::Serializable(const Anope::string &serialize_type)
{
	if (SerializableItems == NULL)
		SerializableItems = new std::list<Serializable *>();
	SerializableItems->push_back(this);

	this->s_type = Type::Find(serialize_type);

	this->s_iter = SerializableItems->end();
	--this->s_iter;

	FOREACH_MOD(OnSerializableConstruct, (this));
}

Serializable::Serializable(const Serializable &other)
{
	SerializableItems->push_back(this);
	this->s_iter = SerializableItems->end();
	--this->s_iter;

	this->s_type = other.s_type;

	FOREACH_MOD(OnSerializableConstruct, (this));
}

Serializable::~Serializable()
{
	FOREACH_MOD(OnSerializableDestruct, (this));

	SerializableItems->erase(this->s_iter);
}

Serializable &Serializable::operator=(const Serializable &)
{
	return *this;
}

void Serializable::QueueUpdate()
{
	/* Schedule updater */
	FOREACH_MOD(OnSerializableUpdate, (this));

	/* Check for modifications now - this can delete this object! */
	FOREACH_MOD(OnSerializeCheck, (this->GetSerializableType()));
}

bool Serializable::IsCached(Serialize::Data &data)
{
	return this->last_commit == data.Hash();
}

void Serializable::UpdateCache(Serialize::Data &data)
{
	this->last_commit = data.Hash();
}

bool Serializable::IsTSCached()
{
	return this->last_commit_time == Anope::CurTime;
}

void Serializable::UpdateTS()
{
	this->last_commit_time = Anope::CurTime;
}

const std::list<Serializable *> &Serializable::GetItems()
{
	return *SerializableItems;
}

Serialize::DataType Serialize::Data::GetType(const Anope::string &key) const
{
	auto it = this->types.find(key);
	if (it != this->types.end())
		return it->second;
	return Serialize::DataType::TEXT;
}

void Serialize::Data::SetType(const Anope::string &key, Serialize::DataType dt)
{
	this->types[key] = dt;
}

Type::Type(const Anope::string &n, Module *o)
	: name(n)
	, owner(o)
{
	TypeOrder.push_back(this->name);
	Types[this->name] = this;

	FOREACH_MOD(OnSerializeTypeCreate, (this));
}

Type::~Type()
{
	/* null the type of existing serializable objects of this type */
	if (Serializable::SerializableItems != NULL)
	{
		for (auto *s : *Serializable::SerializableItems)
		{
			if (s->s_type == this)
				s->s_type = NULL;
		}
	}

	std::vector<Anope::string>::iterator it = std::find(TypeOrder.begin(), TypeOrder.end(), this->name);
	if (it != TypeOrder.end())
		TypeOrder.erase(it);
	Types.erase(this->name);
}

void Type::Check()
{
	FOREACH_MOD(OnSerializeCheck, (this));
}

void Type::UpdateTimestamp()
{
	this->timestamp = Anope::CurTime;
}

Type *Serialize::Type::Find(const Anope::string &name)
{
	std::map<Anope::string, Type *>::iterator it = Types.find(name);
	if (it != Types.end())
		return it->second;
	return NULL;
}
