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
	static Memo::Type memo;
	static XLine::Type xline;
	CreateTypes();
}

void Serialize::CreateTypes()
{
	for (const auto &type : Serialize::Type::GetTypeOrder())
	{
		auto *s_type = Serialize::Type::Find(type);
		if (s_type)
			s_type->Create();
	}
}

void Serialize::CheckTypes()
{
	for (const auto &type : Serialize::Type::GetTypeOrder())
	{
		auto *s_type = Serialize::Type::Find(type);
		if (s_type)
			s_type->Check();
	}
}

Serializable::Serializable(const Anope::string &serialize_type)
	: s_name(serialize_type)
	, s_type(Type::Find(serialize_type))
{
	if (SerializableItems == NULL)
		SerializableItems = new std::list<Serializable *>();

	SerializableItems->push_back(this);
	this->s_iter = std::prev(SerializableItems->end());

	if (!s_type)
		Log(LOG_DEBUG) << "Created orphan " << this->s_name << " object: " << this;

	FOREACH_MOD(OnSerializableConstruct, (this));
}

Serializable::Serializable(const Serializable &other)
	: s_name(other.s_name)
	, s_type(other.s_type)
{
	SerializableItems->push_back(this);
	this->s_iter = std::prev(SerializableItems->end());

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
	FOREACH_MOD(OnSerializeTypeCheck, (this->GetSerializableType()));
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

	if (Serializable::SerializableItems == nullptr)
		return;

	// Rehook objects from before a reload to this type.
	for (auto *s : *Serializable::SerializableItems)
	{
		if (s->s_type != nullptr || s->s_name != n)
			continue;

		Log(LOG_DEBUG) << "Adopting " << s->s_name << " object: " << this;
		s->s_type = this;
	}
}

Type::~Type()
{
	auto it = std::find(TypeOrder.begin(), TypeOrder.end(), this->name);
	if (it != TypeOrder.end())
		TypeOrder.erase(it);
	Types.erase(this->name);

	if (Serializable::SerializableItems == nullptr)
		return;

	// Orphan objects of this type. They will be rehooked later if reloaded.
	for (auto *s : *Serializable::SerializableItems)
	{
		if (s->s_type != this)
			continue;

		Log(LOG_DEBUG) << "Orphaning " << s->s_name << " object: " << this;
		s->s_type = nullptr;
	}
}

void Type::Create()
{
	if (created)
		return;

	FOREACH_MOD(OnSerializeTypeCreate, (this));
	created = true;
}

void Type::Check()
{
	FOREACH_MOD(OnSerializeTypeCheck, (this));
}

void Type::UpdateTimestamp()
{
	this->timestamp = Anope::CurTime;
}

Type *Serialize::Type::Find(const Anope::string &name)
{
	auto it = Types.find(name);
	if (it != Types.end())
		return it->second;
	return NULL;
}
