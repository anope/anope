#include "module.h"
#include "../extra/sql.h"
#include "../commands/os_session.h"

class DBMySQL : public Module, public Pipe
{
 private:
	Anope::string engine;
	Anope::string prefix;
	ServiceReference<SQLProvider> SQL;
	time_t lastwarn;
	bool ro;
	bool init;
	std::set<Reference<Serializable> > updated_items;

	bool CheckSQL()
	{
		if (SQL)
		{
			if (Anope::ReadOnly && this->ro)
			{
				Anope::ReadOnly = this->ro = false;
				Log() << "Found SQL again, going out of readonly mode...";
			}

			return true;
		}
		else
		{
			if (Anope::CurTime - Config->UpdateTimeout > lastwarn)
			{
				Log() << "Unable to locate SQL reference, going to readonly...";
				Anope::ReadOnly = this->ro = true;
				this->lastwarn = Anope::CurTime;
			}

			return false;
		}
	}

	bool CheckInit()
	{
		return init && SQL;
	}

	void RunQuery(const SQLQuery &query)
	{
		/* Can this be threaded? */
		this->RunQueryResult(query);
	}

	SQLResult RunQueryResult(const SQLQuery &query)
	{
		if (this->CheckSQL())
		{
			SQLResult res = SQL->RunQuery(query);
			if (!res.GetError().empty())
				Log(LOG_DEBUG) << "SQL-live got error " << res.GetError() << " for " + res.finished_query;
			else
				Log(LOG_DEBUG) << "SQL-live got " << res.Rows() << " rows for " << res.finished_query;
			return res;
		}
		throw SQLException("No SQL!");
	}

 public:
	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), SQL("", "")
	{
		this->lastwarn = 0;
		this->ro = false;
		this->init = false;

		Implementation i[] = { I_OnReload, I_OnShutdown, I_OnLoadDatabase, I_OnSerializableConstruct, I_OnSerializableDestruct, I_OnSerializeCheck, I_OnSerializableUpdate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	void OnNotify() anope_override
	{
		if (!this->CheckInit())
			return;

		for (std::set<Reference<Serializable> >::iterator it = this->updated_items.begin(), it_end = this->updated_items.end(); it != it_end; ++it)
		{
			Reference<Serializable> obj = *it;

			if (obj && this->SQL)
			{
				if (obj->IsCached())
					continue;
				obj->UpdateCache();

				Serialize::Type *s_type = obj->GetSerializableType();
				if (!s_type)
					continue;

				Serialize::Data data = obj->Serialize();

				std::vector<SQLQuery> create = this->SQL->CreateTable(this->prefix + s_type->GetName(), data);
				for (unsigned i = 0; i < create.size(); ++i)
					this->RunQueryResult(create[i]);

				SQLResult res = this->RunQueryResult(this->SQL->BuildInsert(this->prefix + s_type->GetName(), obj->id, data));
				if (obj->id != res.GetID())
				{
					/* In this case obj is new, so place it into the object map */
					obj->id = res.GetID();
					s_type->objects[obj->id] = obj;
				}
			}
		}

		this->updated_items.clear();
	}

	EventReturn OnLoadDatabase() anope_override
	{
		init = true;
		return EVENT_STOP;
	}

	void OnShutdown() anope_override
	{
		init = false;
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		this->engine = config.ReadValue("db_sql", "engine", "", 0);
		this->SQL = ServiceReference<SQLProvider>("SQLProvider", this->engine);
		this->prefix = config.ReadValue("db_sql", "prefix", "anope_db_", 0);
	}

	void OnSerializableConstruct(Serializable *obj) anope_override
	{
		if (!this->CheckInit())
			return;
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) anope_override
	{
		if (!this->CheckInit())	
			return;
		Serialize::Type *s_type = obj->GetSerializableType();
		if (!s_type)
			return;
		this->RunQuery("DELETE FROM `" + this->prefix + s_type->GetName() + "` WHERE `id` = " + stringify(obj->id));
		s_type->objects.erase(obj->id);
	}

	void OnSerializeCheck(Serialize::Type *obj) anope_override
	{
		if (!this->CheckInit() || obj->GetTimestamp() == Anope::CurTime)
			return;

		SQLQuery query("SELECT * FROM `" + this->prefix + obj->GetName() + "` WHERE (`timestamp` > " + this->SQL->FromUnixtime(obj->GetTimestamp()) + " OR `timestamp` IS NULL)");

		obj->UpdateTimestamp();

		SQLResult res = this->RunQueryResult(query);

		bool clear_null = false;
		for (int i = 0; i < res.Rows(); ++i)
		{
			const std::map<Anope::string, Anope::string> &row = res.Row(i);

			unsigned int id;
			try
			{
				id = convertTo<unsigned int>(res.Get(i, "id"));
			}
			catch (const ConvertException &)
			{
				Log(LOG_DEBUG) << "Unable to convert id from " << obj->GetName();
				continue;
			}

			if (res.Get(i, "timestamp").empty())
			{
				clear_null = true;
				std::map<unsigned int, Serializable *>::iterator it = obj->objects.find(id);
				if (it != obj->objects.end())
				{
					it->second->Destroy();
					obj->objects.erase(it);
				}
			}
			else
			{
				Serialize::Data data;

				for (std::map<Anope::string, Anope::string>::const_iterator it = row.begin(), it_end = row.end(); it != it_end; ++it)
					data[it->first] << it->second;

				Serializable *s = NULL;
				std::map<unsigned int, Serializable *>::iterator it = obj->objects.find(id);
				if (it != obj->objects.end())
					s = it->second;

				Serializable *new_s = obj->Unserialize(s, data);
				if (new_s)
				{
					// If s == new_s then s->id == new_s->id
					if (s != new_s)
					{
						new_s->id = id;
						obj->objects[id] = new_s;
						new_s->UpdateCache(); /* We know this is the most up to date copy */
					}
				}
				else
					s->Destroy();
			}
		}

		if (clear_null)
		{
			query = "DELETE FROM `" + this->prefix + obj->GetName() + "` WHERE `timestamp` IS NULL";
			this->RunQuery(query);
		}
	}

	void OnSerializableUpdate(Serializable *obj) anope_override
	{
		if (obj->IsTSCached())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}
};

MODULE_INIT(DBMySQL)

