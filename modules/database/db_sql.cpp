/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/sql.h"

using namespace SQL;

class SQLSQLInterface
	: public Interface
{
public:
	SQLSQLInterface(Module *o) : Interface(o) { }

	void OnResult(const Result &r) override
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const Result &r) override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
	}
};

class ResultSQLSQLInterface final
	: public SQLSQLInterface
{
	Reference<Serializable> obj;

public:
	ResultSQLSQLInterface(Module *o, Serializable *ob) : SQLSQLInterface(o), obj(ob) { }

	void OnResult(const Result &r) override
	{
		SQLSQLInterface::OnResult(r);
		if (r.GetID() > 0 && this->obj)
			this->obj->id = r.GetID();
		delete this;
	}

	void OnError(const Result &r) override
	{
		SQLSQLInterface::OnError(r);
		delete this;
	}
};

class DBSQL final
	: public Module
	, public Pipe
{
	ServiceReference<Provider> sql;
	SQLSQLInterface sqlinterface;
	Anope::string prefix;
	bool import;

	std::set<Serializable *> updated_items;
	bool shutting_down = false;
	bool loading_databases = false;
	bool loaded = false;
	bool imported = false;

	void RunBackground(const Query &q, Interface *iface = NULL)
	{
		if (!this->sql)
		{
			static time_t last_warn = 0;
			if (last_warn + 300 < Anope::CurTime)
			{
				last_warn = Anope::CurTime;
				Log(this) << "db_sql: Unable to execute query, is SQL (" << this->sql.GetServiceName() << ") configured correctly?";
			}
		}
		else if (!Anope::Quitting)
		{
			if (iface == NULL)
				iface = &this->sqlinterface;
			this->sql->Run(iface, q);
		}
		else
			this->sql->RunQuery(q);
	}

public:
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR), sql("", ""), sqlinterface(this)
	{


		if (ModuleManager::FindModule("db_sql_live") != NULL)
			throw ModuleException("db_sql can not be loaded after db_sql_live");
	}

	void OnNotify() override
	{
		for (auto *obj : this->updated_items)
		{
			if (this->sql)
			{
				Data data;
				obj->Serialize(data);

				if (obj->IsCached(data))
					continue;

				obj->UpdateCache(data);

				/* If we didn't load these objects and we don't want to import just update the cache and continue */
				if (!this->loaded && !this->imported && !this->import)
					continue;

				Serialize::Type *s_type = obj->GetSerializableType();
				if (!s_type)
					continue;

				std::vector<Query> create = this->sql->CreateTable(this->prefix + s_type->GetName(), data);
				Query insert = this->sql->BuildInsert(this->prefix + s_type->GetName(), obj->id, data);

				if (this->imported)
				{
					for (const auto &query : create)
						this->RunBackground(query);

					this->RunBackground(insert, new ResultSQLSQLInterface(this, obj));
				}
				else
				{
					for (const auto &query : create)
						this->sql->RunQuery(query);

					/* We are importing objects from another database module, so don't do asynchronous
					 * queries in case the core has to shut down, it will cut short the import
					 */
					Result r = this->sql->RunQuery(insert);
					if (r.GetID() > 0)
						obj->id = r.GetID();
				}
			}
		}

		this->updated_items.clear();
		this->imported = true;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->sql = ServiceReference<Provider>("SQL::Provider", block->Get<const Anope::string>("engine"));
		this->prefix = block->Get<const Anope::string>("prefix", "anope_db_");
		this->import = block->Get<bool>("import");
	}

	void OnShutdown() override
	{
		this->shutting_down = true;
		this->OnNotify();
	}

	void OnRestart() override
	{
		this->OnShutdown();
	}

	EventReturn OnLoadDatabase() override
	{
		if (!this->sql)
		{
			Log(this) << "Unable to load databases, is SQL (" << this->sql.GetServiceName() << ") configured correctly?";
			return EVENT_CONTINUE;
		}

		this->loading_databases = true;

		for (const auto &type_order : Serialize::Type::GetTypeOrder())
		{
			Serialize::Type *sb = Serialize::Type::Find(type_order);
			this->OnSerializeTypeCreate(sb);
		}

		this->loading_databases = false;
		this->loaded = true;

		return EVENT_STOP;
	}

	void OnSerializableConstruct(Serializable *obj) override
	{
		if (this->shutting_down || this->loading_databases)
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) override
	{
		if (this->shutting_down)
			return;
		Serialize::Type *s_type = obj->GetSerializableType();
		if (s_type && obj->id > 0)
			this->RunBackground("DELETE FROM `" + this->prefix + s_type->GetName() + "` WHERE `id` = " + Anope::ToString(obj->id));
		this->updated_items.erase(obj);
	}

	void OnSerializableUpdate(Serializable *obj) override
	{
		if (this->shutting_down || obj->IsTSCached())
			return;
		if (obj->id == 0)
			return; /* object is pending creation */
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializeTypeCreate(Serialize::Type *sb) override
	{
		if (!this->loading_databases && !this->loaded)
			return;

		Query query("SELECT * FROM `" + this->prefix + sb->GetName() + "`");
		Result res = this->sql->RunQuery(query);

		for (int j = 0; j < res.Rows(); ++j)
		{
			Data data;

			for (const auto &[key, value] : res.Row(j))
				data[key] << value;

			Serializable *obj = sb->Unserialize(NULL, data);
			if (obj)
			{
				auto oid = Anope::TryConvert<unsigned int>(res.Get(j, "id"));
				if (oid.has_value())
					obj->id = oid.value();
				else
					Log(this) << "Unable to convert id for object #" << j << " of type " << sb->GetName();

				/* The Unserialize operation is destructive so rebuild the data for UpdateCache.
				 * Also the old data may contain columns that we don't use, so we reserialize the
				 * object to know for sure our cache is consistent
				 */

				Data data2;
				obj->Serialize(data2);
				obj->UpdateCache(data2); /* We know this is the most up to date copy */
			}
		}
	}
};

MODULE_INIT(DBSQL)
