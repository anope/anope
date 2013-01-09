/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "../extra/sql.h"

using namespace SQL;

class SQLSQLInterface : public Interface
{
 public:
	SQLSQLInterface(Module *o) : Interface(o) { }

	void OnResult(const Result &r) anope_override
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const Result &r) anope_override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
	}
};

class ResultSQLSQLInterface : public SQLSQLInterface
{
	Reference<Serializable> obj;

public:
	ResultSQLSQLInterface(Module *o, Serializable *ob) : SQLSQLInterface(o), obj(ob) { }

	void OnResult(const Result &r) anope_override
	{
		SQLSQLInterface::OnResult(r);
		if (r.GetID() > 0 && this->obj)
			this->obj->id = r.GetID();
		delete this;
	}

	void OnError(const Result &r) anope_override
	{
		SQLSQLInterface::OnError(r);
		delete this;
	}
};

class DBSQL : public Module, public Pipe
{
	ServiceReference<Provider> sql;
	SQLSQLInterface sqlinterface;
	Anope::string prefix;
	std::set<Reference<Serializable> > updated_items;
	bool shutting_down;
	bool loading_databases;
	bool loaded;

	void RunBackground(const Query &q, Interface *iface = NULL)
	{
		if (!this->sql)
		{
			static time_t last_warn = 0;
			if (last_warn + 300 < Anope::CurTime)
			{
				last_warn = Anope::CurTime;
				Log(this) << "db_sql: Unable to execute query, is SQL configured correctly?";
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
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sql("", ""), sqlinterface(this), shutting_down(false), loading_databases(false), loaded(false)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnShutdown, I_OnRestart, I_OnLoadDatabase, I_OnSerializableConstruct, I_OnSerializableDestruct, I_OnSerializableUpdate, I_OnSerializeTypeCreate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnNotify() anope_override
	{
		for (std::set<Reference<Serializable> >::iterator it = this->updated_items.begin(), it_end = this->updated_items.end(); it != it_end; ++it)
		{
			Reference<Serializable> obj = *it;

			if (obj && this->sql)
			{
				Data *data = new Data();
				obj->Serialize(*data);

				if (obj->IsCached(data))
				{
					delete data;
					continue;
				}

				obj->UpdateCache(data);

				Serialize::Type *s_type = obj->GetSerializableType();
				if (!s_type)
					continue;

				std::vector<Query> create = this->sql->CreateTable(this->prefix + s_type->GetName(), *data);
				for (unsigned i = 0; i < create.size(); ++i)
					this->RunBackground(create[i]);

				Query insert = this->sql->BuildInsert(this->prefix + s_type->GetName(), obj->id, *data);
				this->RunBackground(insert, new ResultSQLSQLInterface(this, obj));
			}
		}

		this->updated_items.clear();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->sql = ServiceReference<Provider>("SQL::Provider", engine);
		this->prefix = config.ReadValue("db_sql", "prefix", "anope_db_", 0);
	}

	void OnShutdown() anope_override
	{
		this->shutting_down = true;
		this->OnNotify();
	}

	void OnRestart() anope_override
	{
		this->OnShutdown();
	}

	EventReturn OnLoadDatabase() anope_override
	{
		if (!this->sql)
		{
			Log(this) << "Unable to load databases, is SQL configured correctly?";
			return EVENT_CONTINUE;
		}

		this->loading_databases = true;

		const std::vector<Anope::string> type_order = Serialize::Type::GetTypeOrder();
		for (unsigned i = 0; i < type_order.size(); ++i)
		{
			Serialize::Type *sb = Serialize::Type::Find(type_order[i]);
			this->OnSerializeTypeCreate(sb);
		}

		this->loading_databases = false;
		this->loaded = true;

		return EVENT_STOP;
	}

	void OnSerializableConstruct(Serializable *obj) anope_override
	{
		if (this->shutting_down || this->loading_databases)
			return;
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) anope_override
	{
		Serialize::Type *s_type = obj->GetSerializableType();
		if (s_type)
			this->RunBackground("DELETE FROM `" + this->prefix + s_type->GetName() + "` WHERE `id` = " + stringify(obj->id));
	}

	void OnSerializableUpdate(Serializable *obj) anope_override
	{
		if (this->shutting_down || obj->IsTSCached())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializeTypeCreate(Serialize::Type *sb) anope_override
	{
		if (!this->loading_databases && !this->loaded)
			return;

		Query query("SELECT * FROM `" + this->prefix + sb->GetName() + "`");
		Result res = this->sql->RunQuery(query);

		for (int j = 0; j < res.Rows(); ++j)
		{
			Data *data = new Data();

			const std::map<Anope::string, Anope::string> &row = res.Row(j);
			for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
				(*data)[rit->first] << rit->second;

			Serializable *obj = sb->Unserialize(NULL, *data);
			try
			{
				if (obj)
					obj->id = convertTo<unsigned int>(res.Get(j, "id"));
			}
			catch (const ConvertException &)
			{
				Log(this) << "Unable to convert id for object #" << j << " of type " << sb->GetName();
			}

			if (obj)
				obj->UpdateCache(data); /* We know this is the most up to date copy */
			else
				delete data;
		}
	}
};

MODULE_INIT(DBSQL)

