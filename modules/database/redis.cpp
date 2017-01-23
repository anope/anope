/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
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

#include "module.h"
#include "modules/redis.h"

using namespace Redis;

class DatabaseRedis;
static DatabaseRedis *me;

class TypeLoader : public Interface
{
	Serialize::TypeBase *type;

 public:
	TypeLoader(Module *creator, Serialize::TypeBase *t) : Interface(creator), type(t) { }

	void OnResult(const Reply &r) override;
};

class ObjectLoader : public Interface
{
	Serialize::Object *obj;

 public:
	ObjectLoader(Module *creator, Serialize::Object *s) : Interface(creator), obj(s) { }

	void OnResult(const Reply &r) override;
};

class FieldLoader : public Interface
{
	Serialize::Object *obj;
	Serialize::FieldBase *field;

 public:
	FieldLoader(Module *creator, Serialize::Object *o, Serialize::FieldBase *f) : Interface(creator), obj(o), field(f) { }

	void OnResult(const Reply &) override;
};

class SubscriptionListener : public Interface
{
 public:
	SubscriptionListener(Module *creator) : Interface(creator) { }

	void OnResult(const Reply &r) override;
};

class DatabaseRedis : public Module
	, public EventHook<Event::LoadDatabase>
	, public EventHook<Event::SerializeEvents>
{
	SubscriptionListener sl;

 public:
	ServiceReference<Provider> redis;

	DatabaseRedis(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::LoadDatabase>(this)
		, EventHook<Event::SerializeEvents>(this)
		, sl(this)
	{
		me = this;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->redis = ServiceReference<Provider>(block->Get<Anope::string>("engine", "redis/main"));
	}

	EventReturn OnLoadDatabase() override
	{
		if (!redis)
			return EVENT_STOP;

		for (Serialize::TypeBase *type : Serialize::TypeBase::GetTypes())
			this->OnSerializeTypeCreate(type);

		while (redis->BlockAndProcess());

		redis->Subscribe(&this->sl, "anope");

		return EVENT_STOP;
	}

	void OnSerializeTypeCreate(Serialize::TypeBase *sb)
	{
		std::vector<Anope::string> args = { "SMEMBERS", "ids:" + sb->GetName() };

		redis->SendCommand(new TypeLoader(this, sb), args);
	}

	EventReturn OnSerializeList(Serialize::TypeBase *type, std::vector<Serialize::ID> &ids) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeFind(Serialize::TypeBase *type, Serialize::FieldBase *field, const Anope::string &value, Serialize::ID &id) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeGet(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &value) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeGetRefs(Serialize::Object *object, Serialize::TypeBase *type, std::vector<Serialize::Edge> &) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeDeref(Serialize::ID id, Serialize::TypeBase *type) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeGetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &type, Serialize::ID &value) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeSet(Serialize::Object *object, Serialize::FieldBase *field, const Anope::string &value) override
	{
		std::vector<Anope::string> args;

		redis->StartTransaction();

		const Anope::string &old = field->SerializeToString(object);
		args = { "SREM", "lookup:" + object->GetSerializableType()->GetName() + ":" + field->serialize_name + ":" + old, stringify(object->id) };
		redis->SendCommand(nullptr, args);

		// add object to type set
		args = { "SADD", "ids:" + object->GetSerializableType()->GetName(), stringify(object->id) };
		redis->SendCommand(nullptr, args);

		// add key to key set
		args = { "SADD", "keys:" + stringify(object->id), field->serialize_name };
		redis->SendCommand(nullptr, args);

		// set value
		args = { "SET", "values:" + stringify(object->id) + ":" + field->serialize_name, value };
		redis->SendCommand(nullptr, args);

		// lookup
		args = { "SADD", "lookup:" + object->GetSerializableType()->GetName() + ":" + field->serialize_name + ":" + value, stringify(object->id) };
		redis->SendCommand(nullptr, args);

		redis->CommitTransaction();

		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeSetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Serialize::Object *value) override
	{
		return OnSerializeSet(object, field, stringify(value->id));
	}

	EventReturn OnSerializeUnset(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		std::vector<Anope::string> args;

		redis->StartTransaction();

		const Anope::string &old = field->SerializeToString(object);
		args = { "SREM", "lookup:" + object->GetSerializableType()->GetName() + ":" + field->serialize_name + ":" + old, stringify(object->id) };
		redis->SendCommand(nullptr, args);

		// remove field from set
		args = { "SREM", "keys:" + stringify(object->id), field->serialize_name };
		redis->SendCommand(nullptr, args);

		redis->CommitTransaction();

		return EVENT_CONTINUE;
	}

	EventReturn OnSerializeUnsetSerializable(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		return OnSerializeUnset(object, field);
	}

	EventReturn OnSerializeHasField(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		return EVENT_CONTINUE;
	}

	EventReturn OnSerializableGetId(Serialize::ID &id) override
	{
		std::vector<Anope::string> args = { "INCR", "id" };

		auto f = [&](const Reply &r)
		{
			id = r.i;
		};

		FInterface inter(this, f);
		redis->SendCommand(&inter, args);
		while (redis->BlockAndProcess());
		return EVENT_ALLOW;
	}

	void OnSerializableCreate(Serialize::Object *) override
	{
	}

	void OnSerializableDelete(Serialize::Object *obj) override
	{
		std::vector<Anope::string> args;

		redis->StartTransaction();

		for (Serialize::FieldBase *field : obj->GetSerializableType()->GetFields())
		{
			Anope::string value = field->SerializeToString(obj);

			args = { "SREM", "lookup:" + obj->GetSerializableType()->GetName() + ":" + field->serialize_name + ":" + value, stringify(obj->id) };
			redis->SendCommand(nullptr, args);

			args = { "DEL", "values:" + stringify(obj->id) + ":" + field->serialize_name };
			redis->SendCommand(nullptr, args);

			args = { "SREM", "keys:" + stringify(obj->id), field->serialize_name };
			redis->SendCommand(nullptr, args);
		}

		args = { "SREM", "ids:" + obj->GetSerializableType()->GetName(), stringify(obj->id) };
		redis->SendCommand(nullptr, args);

		redis->CommitTransaction();
	}
};

void TypeLoader::OnResult(const Reply &r)
{
	if (r.type != Reply::MULTI_BULK || !me->redis)
	{
		delete this;
		return;
	}

	for (unsigned i = 0; i < r.multi_bulk.size(); ++i)
	{
		const Reply *reply = r.multi_bulk[i];

		if (reply->type != Reply::BULK)
			continue;

		int64_t id;
		try
		{
			id = convertTo<int64_t>(reply->bulk);
		}
		catch (const ConvertException &)
		{
			continue;
		}

		Serialize::Object *obj = type->Require(id);
		if (obj == nullptr)
		{
			Anope::Logger.Debug("Unable to require object #{0} of type {1}", id, type->GetName());
			continue;
		}

		std::vector<Anope::string> args = { "SMEMBERS", "keys:" + stringify(id) };

		me->redis->SendCommand(new ObjectLoader(me, obj), args);
	}

	delete this;
}

void ObjectLoader::OnResult(const Reply &r)
{
	if (r.type != Reply::MULTI_BULK || r.multi_bulk.empty() || !me->redis)
	{
		delete this;
		return;
	}

	Serialize::TypeBase *type = obj->GetSerializableType();

	for (Reply *reply : r.multi_bulk)
	{
		const Anope::string &key = reply->bulk;
		Serialize::FieldBase *field = type->GetField(key);

		if (field == nullptr)
			continue;

		std::vector<Anope::string> args = { "GET", "values:" + stringify(obj->id) + ":" + key };

		me->redis->SendCommand(new FieldLoader(me, obj, field), args);
	}

	delete this;
}

void FieldLoader::OnResult(const Reply &r)
{
	Anope::Logger.Debug2("Setting field {0} of object #{1} of type {2} to {3}", field->serialize_name, obj->id, obj->GetSerializableType()->GetName(), r.bulk);
	field->UnserializeFromString(obj, r.bulk);

	delete this;
}

void SubscriptionListener::OnResult(const Reply &r)
{
	/*
	 * message
	 * anope
	 * message
	 *
	 * set 4 email adam@anope.org
	 * unset 4 email
	 * create 4 NickCore
	 * delete 4
	 */

	const Anope::string &message = r.multi_bulk[2]->bulk;
	Anope::string command;
	spacesepstream sep(message);

	sep.GetToken(command);

	if (command == "set" || command == "unset")
	{
		Anope::string sid, key, value;

		sep.GetToken(sid);
		sep.GetToken(key);
		value = sep.GetRemaining();

		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(sid);
		}
		catch (const ConvertException &ex)
		{
			this->GetOwner()->logger.Debug("unable to get id for SL update key {0}", sid);
			return;
		}

		Serialize::Object *obj = Serialize::GetID(id);
		if (obj == nullptr)
		{
			this->GetOwner()->logger.Debug("message for unknown object #{0}", id);
			return;
		}

		Serialize::FieldBase *field = obj->GetSerializableType()->GetField(key);
		if (field == nullptr)
		{
			this->GetOwner()->logger.Debug("message for unknown field of object #{0}: {1}", id, key);
			return;
		}

		this->GetOwner()->logger.Debug2("Setting field {0} of object #{1} of type {2} to {3}",
				field->serialize_name, obj->id, obj->GetSerializableType()->GetName(), value);

		field->UnserializeFromString(obj, value);
	}
	else if (command == "create")
	{
		Anope::string sid, stype;

		sep.GetToken(sid);
		sep.GetToken(stype);

		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(sid);
		}
		catch (const ConvertException &ex)
		{
			this->GetOwner()->logger.Debug("unable to get id for SL update key {0}", sid);
			return;
		}

		Serialize::TypeBase *type = Serialize::TypeBase::Find(stype);
		if (type == nullptr)
		{
			this->GetOwner()->logger.Debug("message create for nonexistant type {0}", stype);
			return;
		}

		Serialize::Object *obj = type->Require(id);
		if (obj == nullptr)
		{
			this->GetOwner()->logger.Debug("require for message create type {0} id #{1} returned nullptr", type->GetName(), id);
			return;
		}
	}
	else if (command == "delete")
	{
		Anope::string sid;

		sep.GetToken(sid);

		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(sid);
		}
		catch (const ConvertException &ex)
		{
			this->GetOwner()->logger.Debug("unable to get id for SL update key {0}", sid);
			return;
		}

		Serialize::Object *obj = Serialize::GetID(id);
		if (obj == nullptr)
		{
			this->GetOwner()->logger.Debug("message for unknown object #{0}", id);
			return;
		}

		obj->Delete();
	}
	else
	{
		this->GetOwner()->logger.Debug("unknown message: {0}", message);
	}
}

MODULE_INIT(DatabaseRedis)
