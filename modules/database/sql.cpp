/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "modules/sql.h"

using namespace SQL;

class DBSQL : public Module, public Pipe
	, public EventHook<Event::SerializeEvents>
{
 private:
	bool transaction = false;
	bool inited = false;
	Anope::string prefix;
	ServiceReference<Provider> SQL;
	std::unordered_multimap<Serialize::Object *, Serialize::FieldBase *> cache;

	void CacheMiss(Serialize::Object *object, Serialize::FieldBase *field)
	{
		cache.insert(std::make_pair(object, field));
	}

	bool IsCacheMiss(Serialize::Object *object, Serialize::FieldBase *field)
	{
		auto range = cache.equal_range(object);
		for (auto it = range.first; it != range.second; ++it)
			if (it->second == field)
				return true;
		return false;
	}

	Result Run(const Query &query)
	{
		if (!SQL)
			return Result();

		if (!inited)
		{
			inited = true;
			for (const Query &q : SQL->InitSchema(prefix))
				SQL->RunQuery(q);
		}

		return SQL->RunQuery(query);
	}

	void StartTransaction()
	{
		if (!SQL || transaction)
			return;

		Run(SQL->BeginTransaction());

		transaction = true;
		Notify();
	}

	void Commit()
	{
		if (!SQL || !transaction)
			return;

		Run(SQL->Commit());

		transaction = false;
	}

 public:
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::SerializeEvents>(this)
	{
	}

	void OnNotify() override
	{
		Commit();
		Serialize::Clear();
		cache.clear();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->SQL = ServiceReference<Provider>(block->Get<Anope::string>("engine"));
		this->prefix = block->Get<Anope::string>("prefix", "anope_db_");
		inited = false;
	}

	EventReturn OnSerializeList(Serialize::TypeBase *type, std::vector<Serialize::ID> &ids) override
	{
		StartTransaction();

		ids.clear();

		Query query = "SELECT `id` FROM `" + prefix + type->GetName() + "`";
		Result res = Run(query);
		for (int i = 0; i < res.Rows(); ++i)
		{
			Serialize::ID id = convertTo<Serialize::ID>(res.Get(i, "id"));
			ids.push_back(id);
		}

		return EVENT_ALLOW;
	}

	EventReturn OnSerializeFind(Serialize::TypeBase *type, Serialize::FieldBase *field, const Anope::string &value, Serialize::ID &id) override
	{
		if (!SQL)
			return EVENT_CONTINUE;

		StartTransaction();

		for (Query &q : SQL->CreateTable(prefix, type))
			Run(q);

		for (Query &q : SQL->AlterTable(prefix, type, field))
			Run(q);

		for (const Query &q : SQL->CreateIndex(prefix + type->GetName(), field->serialize_name))
			Run(q);

		Query query = SQL->SelectFind(prefix + type->GetName(), field->serialize_name);

		query.SetValue("value", value);
		Result res = Run(query);
		if (res.Rows())
			try
			{
				id = convertTo<Serialize::ID>(res.Get(0, "id"));
				return EVENT_ALLOW;
			}
			catch (const ConvertException &)
			{
			}

		return EVENT_CONTINUE;
	}

 private:
	bool GetValue(Serialize::Object *object, Serialize::FieldBase *field, SQL::Result::Value &v)
	{
		StartTransaction();

		Query query = "SELECT `" + field->serialize_name + "` FROM `" + prefix + object->GetSerializableType()->GetName() + "` WHERE `id` = @id@";
		query.SetValue("id", object->id, false);
		Result res = Run(query);

		if (res.Rows() == 0)
			return false;

		v = res.GetValue(0, field->serialize_name);
		return true;
	}

	void GetRefs(Serialize::Object *object, Serialize::TypeBase *type, std::vector<Serialize::Edge> &edges)
	{
		for (Serialize::FieldBase *field : type->GetFields())
		{
			if (field->object)
			{
				Anope::string table = prefix + type->GetName();

				Query query = "SELECT " + table + ".id FROM " + table +
					" INNER JOIN " + prefix + "objects AS o ON " +
					table + "." + field->serialize_name + " = o.id "
					"WHERE o.id = @id@";

				query.SetValue("id", object->id, false);

				Result res = Run(query);
				for (int i = 0; i < res.Rows(); ++i)
				{
					Serialize::ID id = convertTo<Serialize::ID>(res.Get(i, "id"));

					Serialize::Object *other = type->Require(id);
					if (other == nullptr)
					{
						Log(LOG_DEBUG) << "Unable to require id " << id << " type " << type->GetName();
						continue;
					}

					// other type, other field, direction
					edges.emplace_back(other, field, false);
				}
			}
		}
	}

 public:
	EventReturn OnSerializeGet(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &value) override
	{
		SQL::Result::Value v;

		if (IsCacheMiss(object, field))
			return EVENT_CONTINUE;

		if (!GetValue(object, field, v))
		{
			CacheMiss(object, field);
			return EVENT_CONTINUE;
		}

		value = v.value;
		return EVENT_ALLOW;
	}

	EventReturn OnSerializeGetRefs(Serialize::Object *object, Serialize::TypeBase *type, std::vector<Serialize::Edge> &edges) override
	{
		StartTransaction();

		edges.clear();

		if (type == nullptr)
		{
			for (Serialize::TypeBase *type : Serialize::TypeBase::GetTypes())
				GetRefs(object, type, edges);
		}
		else
		{
			GetRefs(object, type, edges);
		}

		return EVENT_ALLOW;
	}

	EventReturn OnSerializeDeref(Serialize::ID id, Serialize::TypeBase *type) override
	{
		StartTransaction();

		Query query = "SELECT `id` FROM `" + prefix + type->GetName() + "` WHERE `id` = @id@";
		query.SetValue("id", id, false);
		Result res = Run(query);
		if (res.Rows() == 0)
			return EVENT_CONTINUE;
		return EVENT_ALLOW;
	}

	EventReturn OnSerializeGetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &type, Serialize::ID &value) override
	{
		StartTransaction();

		Query query = "SELECT `" + field->serialize_name + "`,j1.type AS " + field->serialize_name + "_type FROM `" + prefix + object->GetSerializableType()->GetName() + "` "
			"JOIN `" + prefix + "objects` AS j1 ON " + prefix + object->GetSerializableType()->GetName() + "." + field->serialize_name + " = j1.id "
			"WHERE " + prefix + object->GetSerializableType()->GetName() + ".id = @id@";
		query.SetValue("id", object->id, false);
		Result res = Run(query);

		if (res.Rows() == 0)
			return EVENT_CONTINUE;

		type = res.Get(0, field->serialize_name + "_type");
		try
		{
			value = convertTo<Serialize::ID>(res.Get(0, field->serialize_name));
		}
		catch (const ConvertException &ex)
		{
			return EVENT_STOP;
		}

		return EVENT_ALLOW;
	}

 private:
	void DoSet(Serialize::Object *object, Serialize::FieldBase *field, bool is_object, const Anope::string *value)
	{
		if (!SQL)
			return;

		StartTransaction();

		for (Query &q : SQL->CreateTable(prefix, object->GetSerializableType()))
			Run(q);

		for (Query &q : SQL->AlterTable(prefix, object->GetSerializableType(), field))
			Run(q);

		Query q;
		q.SetValue("id", object->id, false);
		if (value)
			q.SetValue(field->serialize_name, *value, !is_object);
		else
			q.SetNull(field->serialize_name);

		for (Query &q2 : SQL->Replace(prefix + object->GetSerializableType()->GetName(), q, { "id" }))
			Run(q2);
	}

 public:
	EventReturn OnSerializeSet(Serialize::Object *object, Serialize::FieldBase *field, const Anope::string &value) override
	{
		DoSet(object, field, false, &value);
		return EVENT_STOP;
	}

	EventReturn OnSerializeSetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Serialize::Object *value) override
	{
		if (!SQL)
			return EVENT_CONTINUE;

		StartTransaction();

		if (value)
		{
			Anope::string v = stringify(value->id);
			DoSet(object, field, true, &v);
		}
		else
		{
			DoSet(object, field, true, nullptr);
		}

		return EVENT_STOP;
	}

	EventReturn OnSerializeUnset(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		DoSet(object, field, false, nullptr);
		return EVENT_STOP;
	}

	EventReturn OnSerializeUnsetSerializable(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		DoSet(object, field, true, nullptr);
		return EVENT_STOP;
	}

	EventReturn OnSerializeHasField(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		if (field->object)
		{
			Anope::string type;
			Serialize::ID id;

			EventReturn er = OnSerializeGetSerializable(object, field, type, id);

			if (er != EVENT_ALLOW)
				return EVENT_CONTINUE;

			field->UnserializeFromString(object, type + ":" + stringify(id));
			return EVENT_STOP;
		}
		else
		{
			SQL::Result::Value v;

			if (!GetValue(object, field, v))
				return EVENT_CONTINUE;

			if (v.null)
				return EVENT_CONTINUE;

			field->UnserializeFromString(object, v.value);
			return EVENT_STOP;
		}
	}

	EventReturn OnSerializableGetId(Serialize::ID &id) override
	{
		if (!SQL)
			return EVENT_CONTINUE;

		StartTransaction();

		id = SQL->GetID(prefix);
		return EVENT_ALLOW;
	}

	void OnSerializableCreate(Serialize::Object *object) override
	{
		StartTransaction();

		Query q = Query("INSERT INTO `" + prefix + "objects` (`id`,`type`) VALUES (@id@, @type@)");
		q.SetValue("id", object->id, false);
		q.SetValue("type", object->GetSerializableType()->GetName());
		Run(q);
	}

	void OnSerializableDelete(Serialize::Object *object) override
	{
		StartTransaction();

		Query query("DELETE FROM `" + prefix + "objects` WHERE `id` = " + stringify(object->id));
		Run(query);
	}
};

MODULE_INIT(DBSQL)

