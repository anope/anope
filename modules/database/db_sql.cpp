#include "module.h"
#include "modules/sql.h"

using namespace SQL;

class DBMySQL : public Module, public Pipe
	, public EventHook<Event::SerializeEvents>
{
 private:
	bool transaction = false;
	bool inited = false;
	Anope::string prefix;
	ServiceReference<Provider> SQL;

	Result Run(const Query &query)
	{
		if (!SQL)
			;//XXX

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
	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::SerializeEvents>()
	{
	}

	void OnNotify() override
	{
		Commit();
		Serialize::Clear();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->SQL = ServiceReference<Provider>("SQL::Provider", block->Get<const Anope::string>("engine"));
		this->prefix = block->Get<const Anope::string>("prefix", "anope_db_");
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

		for (Query &q : SQL->CreateTable(prefix, type->GetName()))
			Run(q);

		for (Query &q : SQL->AlterTable(prefix, type->GetName(), field->GetName(), false))
			Run(q);

		for (const Query &q : SQL->CreateIndex(prefix + type->GetName(), field->GetName()))
			Run(q);

		Query query("SELECT `id` FROM `" + prefix + type->GetName() + "` WHERE `" + field->GetName() + "` = @value@");
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

		Query query = "SELECT `" + field->GetName() + "` FROM `" + prefix + object->GetSerializableType()->GetName() + "` WHERE `id` = @id@";
		query.SetValue("id", object->id);
		Result res = Run(query);

		if (res.Rows() == 0)
			return false;

		v = res.GetValue(0, field->GetName());
		return true;
	}

 public:
	EventReturn OnSerializeGet(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &value) override
	{
		SQL::Result::Value v;

		if (!GetValue(object, field, v))
		{
			field->CacheMiss(object);
			return EVENT_CONTINUE;
		}

		value = v.value;
		return EVENT_ALLOW;
	}

	EventReturn OnSerializeGetRefs(Serialize::Object *object, Serialize::TypeBase *type, std::vector<Serialize::Edge> &edges) override
	{
		StartTransaction();

		edges.clear();

		Query query;
		if (type)
			query = "SELECT field," + prefix + "edges.id,other_id,j1.type,j2.type AS other_type FROM `" + prefix + "edges` "
				"JOIN `" + prefix + "objects` AS j1 ON " + prefix + "edges.id = j1.id "
				"JOIN `" + prefix + "objects` AS j2 ON " + prefix + "edges.other_id = j2.id "
				"WHERE "
				"	(" + prefix + "edges.id = @id@ AND j2.type = @other_type@) "
				"OR"
				"	(other_id = @id@ AND j1.type = @other_type@)";
		else
			query = "SELECT field," + prefix + "edges.id,other_id,j1.type,j2.type AS other_type FROM `" + prefix + "edges` "
				"JOIN `" + prefix + "objects` AS j1 ON " + prefix + "edges.id = j1.id "
				"JOIN `" + prefix + "objects` AS j2 ON " + prefix + "edges.other_id = j2.id "
				"WHERE " + prefix + "edges.id = @id@ OR other_id = @id@";

		query.SetValue("type", object->GetSerializableType()->GetName());
		query.SetValue("id", object->id);
		if (type)
			query.SetValue("other_type", type->GetName());

		Result res = Run(query);
		for (int i = 0; i < res.Rows(); ++i)
		{
			Serialize::ID id = convertTo<Serialize::ID>(res.Get(i, "id"));

			if (id == object->id)
			{
				// we want other type, this is my edge
				Anope::string t = res.Get(i, "other_type");
				Anope::string f = res.Get(i, "field");
				id = convertTo<Serialize::ID>(res.Get(i, "other_id"));

				//XXX sanity checks
				Serialize::FieldBase *obj_field = object->GetSerializableType()->GetField(f);

				Serialize::TypeBase *obj_type = Serialize::TypeBase::Find(t);
				Serialize::Object *other = obj_type->Require(id);

				edges.emplace_back(other, obj_field, true);
			}
			else
			{
				// edge to me
				Anope::string t = res.Get(i, "type");
				Anope::string f = res.Get(i, "field");

				//XXX sanity checks
				Serialize::TypeBase *obj_type = Serialize::TypeBase::Find(t);
				Serialize::FieldBase *obj_field = obj_type->GetField(f);
				Serialize::Object *other = obj_type->Require(id);

				// other type, other field,
				edges.emplace_back(other, obj_field, false);
			}
		}
		
		return EVENT_ALLOW;
	}

	EventReturn OnSerializeDeref(Serialize::ID id, Serialize::TypeBase *type) override
	{
		StartTransaction();

		Query query = "SELECT `id` FROM `" + prefix + type->GetName() + "` WHERE `id` = @id@";
		query.SetValue("id", id);
		Result res = Run(query);
		if (res.Rows() == 0)
			return EVENT_CONTINUE;
		return EVENT_ALLOW;
	}

	EventReturn OnSerializeGetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &type, Serialize::ID &value) override
	{
		StartTransaction();

		Query query = "SELECT `" + field->GetName() + "`,j1.type AS " + field->GetName() + "_type FROM `" + prefix + object->GetSerializableType()->GetName() + "` "
			"JOIN `" + prefix + "objects` AS j1 ON " + prefix + object->GetSerializableType()->GetName() + "." + field->GetName() + " = j1.id "
			"WHERE " + prefix + object->GetSerializableType()->GetName() + ".id = @id@";
		query.SetValue("id", object->id);
		Result res = Run(query);

		if (res.Rows() == 0)
			return EVENT_CONTINUE;

		type = res.Get(0, field->GetName() + "_type");
		try
		{
			value = convertTo<Serialize::ID>(res.Get(0, field->GetName()));
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

		for (Query &q : SQL->CreateTable(prefix, object->GetSerializableType()->GetName()))
			Run(q);

		for (Query &q : SQL->AlterTable(prefix, object->GetSerializableType()->GetName(), field->GetName(), is_object))
			Run(q);

		Query q;
		q.SetValue("id", object->id);
		if (value)
			q.SetValue(field->GetName(), *value);
		else
			q.SetNull(field->GetName());

		for (Query &q : SQL->Replace(prefix + object->GetSerializableType()->GetName(), q, { "id" }))
			Run(q);
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

			Query query;
			query.SetValue("field", field->GetName());
			query.SetValue("id", object->id);
			query.SetValue("other_id", value->id);

			for (Query &q : SQL->Replace(prefix + "edges", query, { "id", "field" }))
				Run(q);
		}
		else
		{
			DoSet(object, field, true, nullptr);

			Query query("DELETE FROM `" + prefix + "edges` WHERE `id` = @id@ AND `field` = @field@");
			query.SetValue("id", object->id);
			query.SetValue("field", field->GetName());
			Run(query);
		}

		return EVENT_STOP;
	}

	EventReturn OnSerializeUnset(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		DoSet(object, field, false, nullptr);
		field->CacheMiss(object);
		return EVENT_STOP;
	}

	EventReturn OnSerializeUnsetSerializable(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		DoSet(object, field, true, nullptr);
		field->CacheMiss(object);

		Query query("DELETE FROM `" + prefix + "edges` WHERE `id` = @id@ AND `field` = @field@");
		query.SetValue("id", object->id);
		query.SetValue("field", field->GetName());
		Run(query);

		return EVENT_STOP;
	}

	EventReturn OnSerializeHasField(Serialize::Object *object, Serialize::FieldBase *field) override
	{
		SQL::Result::Value v;

		return GetValue(object, field, v) && !v.null ? EVENT_STOP : EVENT_CONTINUE;
	}

	EventReturn OnSerializableGetId(Serialize::ID &id) override
	{
		StartTransaction();

		id = SQL->GetID(prefix);
		return EVENT_ALLOW;
	}

	void OnSerializableCreate(Serialize::Object *object) override
	{
		StartTransaction();

		Query q = Query("INSERT INTO `" + prefix + "objects` (`id`,`type`) VALUES (@id@, @type@)");
		q.SetValue("id", object->id);
		q.SetValue("type", object->GetSerializableType()->GetName());
		Run(q);
	}

	void OnSerializableDelete(Serialize::Object *object) override
	{
		StartTransaction();

		Query query("DELETE FROM `" + prefix + object->GetSerializableType()->GetName() + "` WHERE `id` = " + stringify(object->id));
		Run(query);
	}
};

MODULE_INIT(DBMySQL)

