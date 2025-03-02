/*
 *
 * (C) 2012-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/sql.h"

using namespace SQL;

class DBMySQL final
	: public Module
	, public Pipe
{
private:
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
			if (Anope::CurTime - Config->GetBlock("options").Get<time_t>("updatetimeout", "2m") > lastwarn)
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

	Anope::string GetTableName(Serialize::Type *s_type)
	{
		return this->prefix + s_type->GetName();
	}

	Result RunQuery(const Query &query)
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


		if (ModuleManager::FindFirstOf(DATABASE) != this)
			throw ModuleException("If db_sql_live is loaded it must be the first database module loaded.");
	}

	void OnNotify() override
	{
		if (!this->CheckInit())
			return;

		for (auto *obj : this->updated_items)
		{
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

				auto create = this->SQL->CreateTable(GetTableName(s_type), data);
				for (const auto &query : create)
					this->RunQuery(query);

				auto res = this->RunQuery(this->SQL->BuildInsert(GetTableName(s_type), obj->id, data));
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

	EventReturn OnLoadDatabase() override
	{
		init = true;
		return EVENT_STOP;
	}

	void OnShutdown() override
	{
		init = false;
	}

	void OnRestart() override
	{
		init = false;
	}

	void OnReload(Configuration::Conf &conf) override
	{
		Configuration::Block &block = conf.GetModule(this);
		this->SQL = ServiceReference<Provider>("SQL::Provider", block.Get<const Anope::string>("engine"));
		this->prefix = block.Get<const Anope::string>("prefix", "anope_db_");
	}

	void OnSerializableConstruct(Serializable *obj) override
	{
		if (!this->CheckInit())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}

	void OnSerializableDestruct(Serializable *obj) override
	{
		if (!this->CheckInit())
			return;
		Serialize::Type *s_type = obj->GetSerializableType();
		if (s_type)
		{
			if (obj->id > 0)
				this->RunQuery("DELETE FROM `" + GetTableName(s_type) + "` WHERE `id` = " + Anope::ToString(obj->id));
			s_type->objects.erase(obj->id);
		}
		this->updated_items.erase(obj);
	}

	void OnSerializeCheck(Serialize::Type *obj) override
	{
		if (!this->CheckInit() || obj->GetTimestamp() == Anope::CurTime)
			return;

		Anope::string sql = Anope::printf("SELECT * from `%s`", GetTableName(obj).c_str());
		if (obj->GetTimestamp())
			sql += Anope::printf(" WHERE (`timestamp` >= %s OR `timestamp` IS NULL)", this->SQL->FromUnixtime(obj->GetTimestamp()).c_str());

		Query query(sql);

		obj->UpdateTimestamp();

		Result res = this->RunQuery(query);

		bool clear_null = false;
		for (int i = 0; i < res.Rows(); ++i)
		{
			const std::map<Anope::string, Anope::string> &row = res.Row(i);



			auto oid = Anope::TryConvert<unsigned int>(res.Get(i, "id"));
			if (!oid.has_value())
			{
				Log(LOG_DEBUG) << "Unable to convert id from " << obj->GetName();
				continue;
			}

			auto id = oid.value();
			if (res.Get(i, "timestamp").empty())
			{
				clear_null = true;
				std::map<uint64_t, Serializable *>::iterator it = obj->objects.find(id);
				if (it != obj->objects.end())
					delete it->second; // This also removes this object from the map
			}
			else
			{
				Data data;

				for (const auto &[key, value] : row)
					data[key] << value;

				Serializable *s = NULL;
				std::map<uint64_t, Serializable *>::iterator it = obj->objects.find(id);
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

						/* The Unserialize operation is destructive so rebuild the data for UpdateCache.
						 * Also the old data may contain columns that we don't use, so we reserialize the
						 * object to know for sure our cache is consistent
						 */

						Data data2;
						new_s->Serialize(data2);
						new_s->UpdateCache(data2); /* We know this is the most up to date copy */
					}
				}
				else
				{
					if (!s)
						this->RunQuery("UPDATE `" + GetTableName(obj) + "` SET `timestamp` = " + this->SQL->FromUnixtime(obj->GetTimestamp()) + " WHERE `id` = " + Anope::ToString(id));
					else
						delete s;
				}
			}
		}

		if (clear_null)
		{
			query = "DELETE FROM `" + GetTableName(obj) + "` WHERE `timestamp` IS NULL";
			this->RunQuery(query);
		}
	}

	void OnSerializableUpdate(Serializable *obj) override
	{
		if (!this->CheckInit() || obj->IsTSCached())
			return;
		obj->UpdateTS();
		this->updated_items.insert(obj);
		this->Notify();
	}
};

MODULE_INIT(DBMySQL)
