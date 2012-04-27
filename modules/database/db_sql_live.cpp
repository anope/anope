#include "module.h"
#include "../extra/sql.h"
#include "../commands/os_session.h"

class DBMySQL : public Module, public Pipe
{
 private:
	Anope::string engine;
	Anope::string prefix;
	service_reference<SQLProvider> SQL;
	time_t lastwarn;
	bool ro;
	bool init;
	std::set<dynamic_reference<Serializable> > updated_items;

	bool CheckSQL()
	{
		if (SQL)
		{
			if (readonly && this->ro)
			{
				readonly = this->ro = false;
				const BotInfo *bi = findbot(Config->OperServ);
				if (bi)
					ircdproto->SendGlobops(bi, "Found SQL again, going out of readonly mode...");
			}

			return true;
		}
		else
		{
			if (Anope::CurTime - Config->UpdateTimeout > lastwarn)
			{
				const BotInfo *bi = findbot(Config->OperServ);
				if (bi)
					ircdproto->SendGlobops(bi, "Unable to locate SQL reference, going to readonly...");
				readonly = this->ro = true;
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
				Log(LOG_DEBUG) << "SQlive got error " << res.GetError() << " for " + res.finished_query;
			else
				Log(LOG_DEBUG) << "SQLive got " << res.Rows() << " rows for " << res.finished_query;
			return res;
		}
		throw SQLException("No SQL!");
	}

	SQLQuery BuildInsert(const Anope::string &table, unsigned int id, const Serialize::Data &data)
	{
		if (this->SQL)
		{
			std::vector<SQLQuery> create_queries = this->SQL->CreateTable(table, data);
			for (unsigned i = 0; i < create_queries.size(); ++i)
				this->RunQuery(create_queries[i]);
		}

		Anope::string query_text = "INSERT INTO `" + table + "` (`id`";
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += ",`" + it->first + "`";
		query_text += ") VALUES (" + stringify(id);
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += ",@" + it->first + "@";
		query_text += ") ON DUPLICATE KEY UPDATE ";
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "`=VALUES(`" + it->first + "`),";
		query_text.erase(query_text.end() - 1);

		SQLQuery query(query_text);
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		return query;
	}

 public:
	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), SQL("", "")
	{
		this->lastwarn = 0;
		this->ro = false;
		this->init = false;

		Implementation i[] = { I_OnReload, I_OnShutdown, I_OnLoadDatabase, I_OnSerializableConstruct, I_OnSerializableDestruct, I_OnSerializePtrAssign, I_OnSerializeCheck, I_OnSerializableUpdate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	void OnNotify() anope_override
	{
		if (!this->CheckInit())
			return;

		for (std::set<dynamic_reference<Serializable> >::iterator it = this->updated_items.begin(), it_end = this->updated_items.end(); it != it_end; ++it)
		{
			dynamic_reference<Serializable> obj = *it;

			if (obj)
			{
				if (obj->IsCached())
					continue;
				obj->UpdateCache();

				static std::set<Serializable *> working_objects; // XXX
				if (working_objects.count(obj))
					continue;
				working_objects.insert(obj);

				SQLResult res = this->RunQueryResult(BuildInsert(this->prefix + obj->serialize_name(), obj->id, obj->serialize()));
				if (res.GetID() > 0)
					obj->id = res.GetID();
				SerializeType *stype = SerializeType::Find(obj->serialize_name());
				if (stype)
					stype->objects.erase(obj->id);

				working_objects.erase(obj);
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
		this->SQL = service_reference<SQLProvider>("SQLProvider", this->engine);
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
		this->RunQuery("DELETE FROM `" + this->prefix + obj->serialize_name() + "` WHERE `id` = " + stringify(obj->id));
		SerializeType *stype = SerializeType::Find(obj->serialize_name());
		if (stype)
			stype->objects.erase(obj->id);
	}

	void OnSerializePtrAssign(Serializable *obj) anope_override
	{
		SerializeType *stype = SerializeType::Find(obj->serialize_name());
		if (stype == NULL || !this->CheckInit() || stype->GetTimestamp() == Anope::CurTime)
			return;

		if (obj->IsCached())
			return;
		obj->UpdateCache();

		SQLResult res = this->RunQueryResult("SELECT * FROM `" + this->prefix + obj->serialize_name() + "` WHERE `id` = " + stringify(obj->id));

		if (res.Rows() == 0)
			obj->destroy();
		else
		{
			const std::map<Anope::string, Anope::string> &row = res.Row(0);

			if (res.Get(0, "timestamp").empty())
			{
				obj->destroy();
				stype->objects.erase(obj->id);
			}
			else
			{
				Serialize::Data data;

				for (std::map<Anope::string, Anope::string>::const_iterator it = row.begin(), it_end = row.end(); it != it_end; ++it)
					data[it->first] << it->second;

				if (stype->Unserialize(obj, data) == NULL)
					obj->destroy();
			}
		}
	}

	void OnSerializeCheck(SerializeType *obj) anope_override
	{
		if (!this->CheckInit() || obj->GetTimestamp() == Anope::CurTime)
			return;

		SQLQuery query("SELECT * FROM `" + this->prefix + obj->GetName() + "` WHERE (`timestamp` > FROM_UNIXTIME(@ts@) OR `timestamp` IS NULL)");
		query.setValue("ts", obj->GetTimestamp());

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
					it->second->destroy();
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
					new_s->id = id;
					obj->objects[id] = new_s;
				}
				else
					s->destroy();
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
		this->updated_items.insert(obj);
		this->Notify();
	}
};

MODULE_INIT(DBMySQL)

