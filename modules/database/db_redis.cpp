/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/redis.h"

using namespace Redis;

class DatabaseRedis;
static DatabaseRedis *me;

class Data final
	: public Serialize::Data
{
public:
	std::map<Anope::string, std::stringstream *> data;

	~Data() override
	{
		for (auto &[_, stream] : data)
			delete stream;
	}

	std::iostream &operator[](const Anope::string &key) override
	{
		std::stringstream *&stream = data[key];
		if (!stream)
			stream = new std::stringstream();
		return *stream;
	}

	size_t Hash() const override
	{
		size_t hash = 0;
		for (const auto &[_, value] : this->data)
			if (!value->str().empty())
				hash ^= Anope::hash_cs()(value->str());
		return hash;
	}
};

class TypeLoader final
	: public Interface
{
	Anope::string type;
public:
	TypeLoader(Module *creator, const Anope::string &t) : Interface(creator), type(t) { }

	void OnResult(const Reply &r) override;
};

class ObjectLoader final
	: public Interface
{
	Anope::string type;
	int64_t id;

public:
	ObjectLoader(Module *creator, const Anope::string &t, int64_t i) : Interface(creator), type(t), id(i) { }

	void OnResult(const Reply &r) override;
};

class IDInterface final
	: public Interface
{
	Reference<Serializable> o;
public:
	IDInterface(Module *creator, Serializable *obj) : Interface(creator), o(obj) { }

	void OnResult(const Reply &r) override;
};

class Deleter final
	: public Interface
{
	Anope::string type;
	int64_t id;
public:
	Deleter(Module *creator, const Anope::string &t, int64_t i) : Interface(creator), type(t), id(i) { }

	void OnResult(const Reply &r) override;
};

class Updater final
	: public Interface
{
	Anope::string type;
	int64_t id;
public:
	Updater(Module *creator, const Anope::string &t, int64_t i) : Interface(creator), type(t), id(i) { }

	void OnResult(const Reply &r) override;
};

class ModifiedObject final
	: public Interface
{
	Anope::string type;
	int64_t id;
public:
	ModifiedObject(Module *creator, const Anope::string &t, int64_t i) : Interface(creator), type(t), id(i) { }

	void OnResult(const Reply &r) override;
};

class SubscriptionListener final
	: public Interface
{
public:
	SubscriptionListener(Module *creator) : Interface(creator) { }

	void OnResult(const Reply &r) override;
};

class DatabaseRedis final
	: public Module
	, public Pipe
{
	SubscriptionListener sl;
	std::set<Serializable *> updated_items;

public:
	ServiceReference<Provider> redis;

	DatabaseRedis(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR), sl(this)
	{
		me = this;

	}

	/* Insert or update an object */
	void InsertObject(Serializable *obj)
	{
		Serialize::Type *t = obj->GetSerializableType();

		/* If there is no id yet for this object, get one */
		if (!obj->id)
			redis->SendCommand(new IDInterface(this, obj), "INCR id:" + t->GetName());
		else
		{
			Data data;
			t->Serialize(obj, data);

			if (obj->IsCached(data))
				return;

			obj->UpdateCache(data);

			std::vector<Anope::string> args;
			args.emplace_back("HGETALL");
			args.push_back("hash:" + t->GetName() + ":" + Anope::ToString(obj->id));

			/* Get object attrs to clear before updating */
			redis->SendCommand(new Updater(this, t->GetName(), obj->id), args);
		}
	}

	void OnNotify() override
	{
		for (auto *obj : this->updated_items)
		{
			this->InsertObject(obj);
		}

		this->updated_items.clear();
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		this->redis = ServiceReference<Provider>("Redis::Provider", block.Get<const Anope::string>("engine", "redis/main"));
	}

	EventReturn OnLoadDatabase() override
	{
		if (!redis)
		{
			Log(this) << "Unable to load database - unable to find redis provider";
			return EVENT_CONTINUE;
		}

		for (const auto &type_order : Serialize::Type::GetTypeOrder())
		{
			Serialize::Type *sb = Serialize::Type::Find(type_order);
			this->OnSerializeTypeCreate(sb);
		}

		while (!redis->IsSocketDead() && redis->BlockAndProcess());

		if (redis->IsSocketDead())
		{
			Log(this) << "I/O error while loading redis database - is it online?";
			return EVENT_CONTINUE;
		}

		redis->Subscribe(&this->sl, "__keyspace@*__:hash:*");

		return EVENT_STOP;
	}

	void OnSerializeTypeCreate(Serialize::Type *sb) override
	{
		if (!redis)
			return;

		std::vector<Anope::string> args;
		args.emplace_back("SMEMBERS");
		args.push_back("ids:" + sb->GetName());

		redis->SendCommand(new TypeLoader(this, sb->GetName()), args);
	}

	void OnSerializableConstruct(Serializable *obj) override
	{
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) override
	{
		Serialize::Type *t = obj->GetSerializableType();

		if (t == NULL)
		{
			/* This is probably the module providing the type unloading.
			 *
			 * The types get registered after the extensible container is
			 * registered so that unserialization on module load can insert
			 * into the extensible container. So, the type destructs prior to
			 * the extensible container, which then triggers this
			 */
			return;
		}

		std::vector<Anope::string> args;
		args.emplace_back("HGETALL");
		args.push_back("hash:" + t->GetName() + ":" + Anope::ToString(obj->id));

		/* Get all of the attributes for this object */
		redis->SendCommand(new Deleter(this, t->GetName(), obj->id), args);

		this->updated_items.erase(obj);
		t->objects.erase(obj->id);
		this->Notify();
	}

	void OnSerializableUpdate(Serializable *obj) override
	{
		this->updated_items.insert(obj);
		this->Notify();
	}
};

void TypeLoader::OnResult(const Reply &r)
{
	if (r.type != Reply::MULTI_BULK || !me->redis)
	{
		delete this;
		return;
	}

	for (auto *reply : r.multi_bulk)
	{
		if (reply->type != Reply::BULK)
			continue;

		auto i = Anope::TryConvert<int64_t>(reply->bulk);
		if (!i)
			continue;

		auto id = i.value();
		std::vector<Anope::string> args;
		args.emplace_back("HGETALL");
		args.push_back("hash:" + this->type + ":" + Anope::ToString(id));

		me->redis->SendCommand(new ObjectLoader(me, this->type, id), args);
	}

	delete this;
}

void ObjectLoader::OnResult(const Reply &r)
{
	Serialize::Type *st = Serialize::Type::Find(this->type);

	if (r.type != Reply::MULTI_BULK || r.multi_bulk.empty() || !me->redis || !st)
	{
		delete this;
		return;
	}

	Data data;

	for (unsigned i = 0; i + 1 < r.multi_bulk.size(); i += 2)
	{
		const Reply *key = r.multi_bulk[i],
			*value = r.multi_bulk[i + 1];

		data[key->bulk] << value->bulk;
	}

	Serializable *&obj = st->objects[this->id];
	obj = st->Unserialize(obj, data);
	if (obj)
	{
		obj->id = this->id;
		obj->UpdateCache(data);
	}

	delete this;
}

void IDInterface::OnResult(const Reply &r)
{
	if (!o || r.type != Reply::INT || !r.i)
	{
		delete this;
		return;
	}

	Serializable *&obj = o->GetSerializableType()->objects[r.i];
	if (obj)
		/* This shouldn't be possible */
		obj->id = 0;

	o->id = r.i;
	obj = o;

	/* Now that we have the id, insert this object for real */
	anope_dynamic_static_cast<DatabaseRedis *>(this->owner)->InsertObject(o);

	delete this;
}

void Deleter::OnResult(const Reply &r)
{
	if (r.type != Reply::MULTI_BULK || !me->redis || r.multi_bulk.empty())
	{
		delete this;
		return;
	}

	/* Transaction start */
	me->redis->StartTransaction();

	std::vector<Anope::string> args;
	args.emplace_back("DEL");
	args.push_back("hash:" + this->type + ":" + Anope::ToString(this->id));

	/* Delete hash object */
	me->redis->SendCommand(NULL, args);

	args.clear();
	args.emplace_back("SREM");
	args.push_back("ids:" + this->type);
	args.push_back(Anope::ToString(this->id));

	/* Delete id from ids set */
	me->redis->SendCommand(NULL, args);

	for (unsigned i = 0; i + 1 < r.multi_bulk.size(); i += 2)
	{
		const Reply *key = r.multi_bulk[i],
			*value = r.multi_bulk[i + 1];

		args.clear();
		args.emplace_back("SREM");
		args.push_back("value:" + this->type + ":" + key->bulk + ":" + value->bulk);
		args.push_back(Anope::ToString(this->id));

		/* Delete value -> object id */
		me->redis->SendCommand(NULL, args);
	}

	/* Transaction end */
	me->redis->CommitTransaction();

	delete this;
}

void Updater::OnResult(const Reply &r)
{
	Serialize::Type *st = Serialize::Type::Find(this->type);

	if (!st)
	{
		delete this;
		return;
	}

	Serializable *obj = st->objects[this->id];
	if (!obj)
	{
		delete this;
		return;
	}

	Data data;
	st->Serialize(obj, data);

	/* Transaction start */
	me->redis->StartTransaction();

	for (unsigned i = 0; i + 1 < r.multi_bulk.size(); i += 2)
	{
		const Reply *key = r.multi_bulk[i],
			*value = r.multi_bulk[i + 1];

		std::vector<Anope::string> args;
		args.emplace_back("SREM");
		args.push_back("value:" + this->type + ":" + key->bulk + ":" + value->bulk);
		args.push_back(Anope::ToString(this->id));

		/* Delete value -> object id */
		me->redis->SendCommand(NULL, args);
	}

	/* Add object id to id set for this type */
	std::vector<Anope::string> args;
	args.emplace_back("SADD");
	args.push_back("ids:" + this->type);
	args.push_back(Anope::ToString(obj->id));
	me->redis->SendCommand(NULL, args);

	args.clear();
	args.emplace_back("HMSET");
	args.push_back("hash:" + this->type + ":" + Anope::ToString(obj->id));

	for (const auto &[key, value] : data.data)
	{
		args.push_back(key);
		args.emplace_back(value->str());

		std::vector<Anope::string> args2;

		args2.emplace_back("SADD");
		args2.push_back("value:" + this->type + ":" + key + ":" + value->str());
		args2.push_back(Anope::ToString(obj->id));

		/* Add to value -> object id set */
		me->redis->SendCommand(NULL, args2);
	}

	++obj->redis_ignore;

	/* Add object */
	me->redis->SendCommand(NULL, args);

	/* Transaction end */
	me->redis->CommitTransaction();

	delete this;
}

void SubscriptionListener::OnResult(const Reply &r)
{
	/*
	 * [May 15 13:59:35.645839 2013] Debug: pmessage
	 * [May 15 13:59:35.645866 2013] Debug: __keyspace@*__:anope:hash:*
	 * [May 15 13:59:35.645880 2013] Debug: __keyspace@0__:anope:hash:type:id
	 * [May 15 13:59:35.645893 2013] Debug: hset
	 */
	if (r.multi_bulk.size() != 4)
		return;

	size_t sz = r.multi_bulk[2]->bulk.find(':');
	if (sz == Anope::string::npos)
		return;

	const Anope::string &key = r.multi_bulk[2]->bulk.substr(sz + 1),
				&op = r.multi_bulk[3]->bulk;

	sz = key.rfind(':');
	if (sz == Anope::string::npos)
		return;

	const Anope::string &id = key.substr(sz + 1);

	size_t sz2 = key.rfind(':', sz - 1);
	if (sz2 == Anope::string::npos)
		return;
	const Anope::string &type = key.substr(sz2 + 1, sz - sz2 - 1);

	Serialize::Type *s_type = Serialize::Type::Find(type);

	if (s_type == NULL)
		return;

	auto oid = Anope::TryConvert<Serializable::Id>(id);
	if (!oid.has_value())
		return;

	auto obj_id = oid.value();
	if (op == "hset" || op == "hdel")
	{
		Serializable *s = s_type->objects[obj_id];

		if (s && s->redis_ignore)
		{
			--s->redis_ignore;
			Log(LOG_DEBUG) << "redis: notify: got modify for object id " << obj_id << " of type " << type << ", but I am ignoring it";
		}
		else
		{
			Log(LOG_DEBUG) << "redis: notify: got modify for object id " << obj_id << " of type " << type;

			std::vector<Anope::string> args;
			args.emplace_back("HGETALL");
			args.push_back("hash:" + type + ":" + id);

			me->redis->SendCommand(new ModifiedObject(me, type, obj_id), args);
		}
	}
	else if (op == "del")
	{
		Serializable *&s = s_type->objects[obj_id];
		if (s == NULL)
			return;

		Log(LOG_DEBUG) << "redis: notify: deleting object id " << obj_id << " of type " << type;

		Data data;
		s_type->Serialize(s, data);

		/* Transaction start */
		me->redis->StartTransaction();

		for (const auto &[k, value] : data.data)
		{
			std::vector<Anope::string> args;
			args.emplace_back("SREM");
			args.push_back("value:" + type + ":" + k + ":" + value->str());
			args.push_back(id);

			/* Delete value -> object id */
			me->redis->SendCommand(NULL, args);
		}

		std::vector<Anope::string> args;
		args.emplace_back("SREM");
		args.push_back("ids:" + type);
		args.push_back(Anope::ToString(s->id));

		/* Delete object from id set */
		me->redis->SendCommand(NULL, args);

		/* Transaction end */
		me->redis->CommitTransaction();

		delete s;
		s = NULL;
	}
}

void ModifiedObject::OnResult(const Reply &r)
{
	Serialize::Type *st = Serialize::Type::Find(this->type);

	if (!st)
	{
		delete this;
		return;
	}

	Serializable *&obj = st->objects[this->id];

	/* Transaction start */
	me->redis->StartTransaction();

	/* Erase old object values */
	if (obj)
	{
		Data data;
		st->Serialize(obj, data);

		for (auto &[key, value] : data.data)
		{
			std::vector<Anope::string> args;
			args.emplace_back("SREM");
			args.push_back("value:" + st->GetName() + ":" + key + ":" + value->str());
			args.push_back(Anope::ToString(this->id));

			/* Delete value -> object id */
			me->redis->SendCommand(NULL, args);
		}
	}

	Data data;

	for (unsigned i = 0; i + 1 < r.multi_bulk.size(); i += 2)
	{
		const Reply *key = r.multi_bulk[i],
			*value = r.multi_bulk[i + 1];

		data[key->bulk] << value->bulk;
	}

	obj = st->Unserialize(obj, data);
	if (obj)
	{
		obj->id = this->id;
		obj->UpdateCache(data);

		/* Insert new object values */
		for (const auto &[key, value] : data.data)
		{
			std::vector<Anope::string> args;
			args.emplace_back("SADD");
			args.push_back("value:" + st->GetName() + ":" + key + ":" + value->str());
			args.push_back(Anope::ToString(obj->id));

			/* Add to value -> object id set */
			me->redis->SendCommand(NULL, args);
		}

		std::vector<Anope::string> args;
		args.emplace_back("SADD");
		args.push_back("ids:" + st->GetName());
		args.push_back(Anope::ToString(obj->id));

		/* Add to type -> id set */
		me->redis->SendCommand(NULL, args);
	}

	/* Transaction end */
	me->redis->CommitTransaction();

	delete this;
}

MODULE_INIT(DatabaseRedis)
