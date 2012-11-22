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
	static Type nc("NickCore", NickCore::Unserialize), na("NickAlias", NickAlias::Unserialize), bi("BotInfo", BotInfo::Unserialize),
		ci("ChannelInfo", ChannelInfo::Unserialize), access("ChanAccess", ChanAccess::Unserialize), logsetting("LogSetting", LogSetting::Unserialize),
		modelock("ModeLock", ModeLock::Unserialize), akick("AutoKick", AutoKick::Unserialize), badword("BadWord", BadWord::Unserialize),
		memo("Memo", Memo::Unserialize), xline("XLine", XLine::Unserialize);
}

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

stringstream &stringstream::SetType(Serialize::DataType t)
{
	this->type = t;
	return *this;
}

DataType Serialize::stringstream::GetType() const
{
	return this->type;
}

stringstream &stringstream::SetMax(unsigned m)
{
	this->_max = m;
	return *this;
}

unsigned stringstream::GetMax() const
{
	return this->_max;
}

Serializable::Serializable() : last_commit_time(0), id(0)
{
	throw CoreException("Default Serializable constructor?");
}

Serializable::Serializable(const Anope::string &serialize_type) : last_commit_time(0), id(0)
{
	if (SerializableItems == NULL)
		SerializableItems = new std::list<Serializable *>();
	SerializableItems->push_back(this);

	this->s_type = Type::Find(serialize_type);

	this->s_iter = SerializableItems->end();
	--this->s_iter;

	FOREACH_MOD(I_OnSerializableConstruct, OnSerializableConstruct(this));
}

Serializable::Serializable(const Serializable &other) : last_commit_time(0), id(0)
{
	SerializableItems->push_back(this);
	this->s_iter = SerializableItems->end();
	--this->s_iter;

	this->s_type = other.s_type;

	FOREACH_MOD(I_OnSerializableConstruct, OnSerializableConstruct(this));
}

Serializable::~Serializable()
{
	SerializableItems->erase(this->s_iter);
}

Serializable &Serializable::operator=(const Serializable &)
{
	return *this;
}

void Serializable::Destroy()
{
	if (!this)
		return;

	FOREACH_MOD(I_OnSerializableDestruct, OnSerializableDestruct(this));

	delete this;
}

void Serializable::QueueUpdate()
{
	/* Check for modifications now */
	FOREACH_MOD(I_OnSerializeCheck, OnSerializeCheck(this->GetSerializableType()));
	/* Schedule updater */
	FOREACH_MOD(I_OnSerializableUpdate, OnSerializableUpdate(this));
}

bool Serializable::IsCached()
{
	return this->last_commit == this->Serialize();
}

void Serializable::UpdateCache()
{
	this->last_commit = this->Serialize();
}

bool Serializable::IsTSCached()
{
	return this->last_commit_time == Anope::CurTime;
}

void Serializable::UpdateTS()
{
	this->last_commit_time = Anope::CurTime;
}

Type* Serializable::GetSerializableType() const
{
	return this->s_type;
}

const std::list<Serializable *> &Serializable::GetItems()
{
	return *SerializableItems;
}

Type::Type(const Anope::string &n, unserialize_func f, Module *o)  : name(n), unserialize(f), owner(o), timestamp(0)
{
	TypeOrder.push_back(this->name);
	Types[this->name] = this;
}

Type::~Type()
{
	std::vector<Anope::string>::iterator it = std::find(TypeOrder.begin(), TypeOrder.end(), this->name);
	if (it != TypeOrder.end())
		TypeOrder.erase(it);
	Types.erase(this->name);
}

const Anope::string &Type::GetName()
{
	return this->name;
}

Serializable *Type::Unserialize(Serializable *obj, Serialize::Data &data)
{
	return this->unserialize(obj, data);
}

void Type::Check()
{
	FOREACH_MOD(I_OnSerializeCheck, OnSerializeCheck(this));
}

time_t Type::GetTimestamp() const
{
	return this->timestamp;
}

void Type::UpdateTimestamp()
{
	this->timestamp = Anope::CurTime;
}

Module* Type::GetOwner() const
{
	return this->owner;
}

Type *Serialize::Type::Find(const Anope::string &name)
{
	std::map<Anope::string, Type *>::iterator it = Types.find(name);
	if (it != Types.end())
		return it->second;
	return NULL;
}

const std::vector<Anope::string> &Type::GetTypeOrder()
{
	return TypeOrder;
}

