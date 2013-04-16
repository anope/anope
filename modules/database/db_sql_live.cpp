#include "module.h"
#include "sql.h"
#include "../commands/os_session.h"

using namespace SQL;

class DBMySQL : public Module, public Pipe
{
 private:
	Anope::string engine;
	Anope::string prefix;
	ServiceReference<Provider> SQL;
	time_t lastwarn;
	bool ro;
	bool init;
	std::set<Serializable *> updated_items;

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

	void RunQuery(const Query &query)
	{
		/* Can this be threaded? */
		this->RunQueryResult(query);
	}

	Result RunQueryResult(const Query &query)
	{
		if (this->CheckSQL())
		{
			Result res = SQL->RunQuery(query);
			if (!res.GetError().empty())
				Log(LOG_DEBUG) << "SQL-live got error " << res.GetError() << " for " + res.finished_query;
			else
				Log(LOG_DEBUG) << "SQL-live got " << res.Rows() << " rows for " << res.finished_query;
			return res;
		}
		throw SQL::Exception("No SQL!");
	}

 public:
	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR), SQL("", "")
	{
		this->lastwarn = 0;
		this->ro = false;
		this->init = false;

		Implementation i[] = { I_OnReload, I_OnShutdown, I_OnLoadDatabase, I_OnSerializableConstruct, I_OnSerializableDestruct, I_OnSerializeCheck, I_OnSerializableUpdate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		if (ModuleManager::FindFirstOf(DATABASE) != this)
			throw ModuleException("If db_sql_live is loaded it must be the first database module loaded.");
	}

	void OnNotify() anope_override
	{
		if (!this->CheckInit())
			return;

		for (std::set<Serializable *>::iterator it = this->updated_items.begin(), it_end = this->updated_items.end(); it != it_end; ++it)
		{
			Serializable *obj = *it;

			if (obj && this->SQL)
			{
				Data data;
				obj->Serialize(data);

				if (obj->IsCached(data))
					continue;

				obj->UpdateCache(data);

				Serialize::Type *s_type = obj->GetSerializableType();
				if (!s_type)
					continue;

				std::vector<Query> create = this->SQL->CreateTable(this->prefix + s_type->GetName(), data);
				for (unsigned i = 0; i < create.size(); ++i)
					this->RunQueryResult(create[i]);

				Result res = this->RunQueryResult(this->SQL->BuildInsert(this->prefix + s_type->GetName(), obj->id, data));
				if (res.GetID() && obj->id != res.GetID())
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

	void OnReload(ServerConfig *conf, ConfigReader &reader) anope_override
	{
		this->engine = reader.ReadValue("db_sql", "engine", "", 0);
		this->SQL = ServiceReference<Provider>("SQL::Provider", this->engine);
		this->prefix = reader.ReadValue("db_sql", "prefix", "anope_db_", 0);
	}

	void OnSerializableConstruct(Serializable *obj) anope_override
	{
		if (!this->CheckInit())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) anope_override
	{
		if (!this->CheckInit())	
			return;
		Serialize::Type *s_type = obj->GetSerializableType();
		if (s_type)
		{
			if (obj->id > 0)
				this->RunQuery("DELETE FROM `" + this->prefix + s_type->GetName() + "` WHERE `id` = " + stringify(obj->id));
			s_type->objects.erase(obj->id);
		}
		this->updated_items.erase(obj);
	}

	void OnSerializeCheck(Serialize::Type *obj) anope_override
	{
		if (!this->CheckInit() || obj->GetTimestamp() == Anope::CurTime)
			return;

		Query query("SELECT * FROM `" + this->prefix + obj->GetName() + "` WHERE (`timestamp` > " + this->SQL->FromUnixtime(obj->GetTimestamp()) + " OR `timestamp` IS NULL)");

		obj->UpdateTimestamp();

		Result res = this->RunQueryResult(query);

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
					delete it->second; // This also removes this object from the map
			}
			else
			{
				Data data;

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

						Data data2;
						/* The Unserialize operation is destructive so rebuild the data for UpdateCache */
						for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
							if (rit->first != "id" && rit->first != "timestamp")
								data2[rit->first] << rit->second;
						new_s->UpdateCache(data2); /* We know this is the most up to date copy */
					}
				}
				else
				{
					delete s;
				}
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
		if (!this->CheckInit() || obj->IsTSCached())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}
};

MODULE_INIT(DBMySQL)

