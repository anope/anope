/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "../extra/sql.h"

class SQLSQLInterface : public SQLInterface
{
 public:
	SQLSQLInterface(Module *o) : SQLInterface(o) { }

	void OnResult(const SQLResult &r) anope_override
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const SQLResult &r) anope_override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
	}
};

class ResultSQLSQLInterface : public SQLSQLInterface
{
	dynamic_reference<Serializable> obj;

public:
	ResultSQLSQLInterface(Module *o, Serializable *ob) : SQLSQLInterface(o), obj(ob) { }

	void OnResult(const SQLResult &r) anope_override
	{
		SQLSQLInterface::OnResult(r);
		if (r.GetID() > 0 && this->obj)
			this->obj->id = r.GetID();
		delete this;
	}

	void OnError(const SQLResult &r) anope_override
	{
		SQLSQLInterface::OnError(r);
		delete this;
	}
};

class DBSQL : public Module, public Pipe
{
	service_reference<SQLProvider> sql;
	SQLSQLInterface sqlinterface;
	Anope::string prefix;
	std::set<dynamic_reference<Serializable> > updated_items;

	void RunBackground(const SQLQuery &q, SQLInterface *iface = NULL)
	{
		if (!this->sql)
		{
			static time_t last_warn = 0;
			if (last_warn + 300 < Anope::CurTime)
			{
				last_warn = Anope::CurTime;
				Log() << "db_sql: Unable to execute query, is SQL configured correctly?";
			}
		}
		else if (!quitting)
		{
			if (iface == NULL)
				iface = &this->sqlinterface;
			this->sql->Run(iface, q);
		}
		else
			this->sql->RunQuery(q);
	}

 public:
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sql("", ""), sqlinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnLoadDatabase, I_OnSerializableConstruct, I_OnSerializableDestruct, I_OnSerializableUpdate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}
	
	void OnNotify() anope_override
	{
		for (std::set<dynamic_reference<Serializable> >::iterator it = this->updated_items.begin(), it_end = this->updated_items.end(); it != it_end; ++it)
		{
			dynamic_reference<Serializable> obj = *it;

			if (obj && this->sql)
			{
				if (obj->IsCached())
					continue;
				obj->UpdateCache();

				const Serialize::Data &data = obj->serialize();

				std::vector<SQLQuery> create = this->sql->CreateTable(this->prefix + obj->serialize_name(), data);
				for (unsigned i = 0; i < create.size(); ++i)
					this->RunBackground(create[i]);


				SQLQuery insert = this->sql->BuildInsert(this->prefix + obj->serialize_name(), obj->id, data);
				this->RunBackground(insert, new ResultSQLSQLInterface(this, obj));
			}
		}

		this->updated_items.clear();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->sql = service_reference<SQLProvider>("SQLProvider", engine);
		this->prefix = config.ReadValue("db_sql", "prefix", "anope_db_", 0);
	}

	EventReturn OnLoadDatabase() anope_override
	{
		if (!this->sql)
		{
			Log() << "db_sql: Unable to load databases, is SQL configured correctly?";
			return EVENT_CONTINUE;
		}

		const std::vector<Anope::string> type_order = SerializeType::GetTypeOrder();
		for (unsigned i = 0; i < type_order.size(); ++i)
		{
			SerializeType *sb = SerializeType::Find(type_order[i]);

			SQLQuery query("SELECT * FROM `" + this->prefix + sb->GetName() + "`");
			SQLResult res = this->sql->RunQuery(query);

			for (int j = 0; j < res.Rows(); ++j)
			{
				Serialize::Data data;

				const std::map<Anope::string, Anope::string> &row = res.Row(j);
				for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
					data[rit->first] << rit->second;

				Serializable *obj = sb->Unserialize(NULL, data);
				try
				{
					if (obj)
						obj->id = convertTo<unsigned int>(res.Get(j, "id"));
				}
				catch (const ConvertException &)
				{
					Log() << "db_sql: Unable to convert id for object #" << j << " of type " << sb->GetName();
				}
			}
		}

		return EVENT_STOP;
	}

	void OnSerializableConstruct(Serializable *obj) anope_override
	{
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) anope_override
	{
		this->RunBackground("DELETE FROM `" + this->prefix + obj->serialize_name() + "` WHERE `id` = " + stringify(obj->id));
	}

	void OnSerializableUpdate(Serializable *obj) anope_override
	{
		this->updated_items.insert(obj);
		this->Notify();
	}
};

MODULE_INIT(DBSQL)

