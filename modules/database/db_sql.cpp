/*
 * (C) 2003-2011 Anope Team
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

	void OnResult(const SQLResult &r)
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const SQLResult &r)
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
	}
};

class DBSQL : public Module
{
	service_reference<SQLProvider, Base> sql;
	SQLSQLInterface sqlinterface;

	void RunBackground(const SQLQuery &q)
	{
		if (!quitting)
			this->sql->Run(&sqlinterface, q);
		else
			this->sql->RunQuery(q);
	}

	void DropTable(const Anope::string &table)
	{
		SQLQuery query("DROP TABLE `" + table + "`");
		this->RunBackground(query);
	}

	void AlterTable(const Anope::string &table, const SerializableBase::serialized_data &oldd, const SerializableBase::serialized_data &newd)
	{
		for (SerializableBase::serialized_data::const_iterator it = newd.begin(), it_end = newd.end(); it != it_end; ++it)
		{
			if (oldd.count(it->first) > 0)
				continue;

			Anope::string query_text = "ALTER TABLE `" + table + "` ADD `" + it->first + "` ";
			if (it->second.getType() == Serialize::DT_INT)
				query_text += "int(11)";
			else if (it->second.getMax() > 0)
				query_text += "varchar(" + stringify(it->second.getMax()) + ")";
			else
				query_text += "text";

			this->RunBackground(SQLQuery(query_text));
		}
	}

	void Insert(const Anope::string &table, const SerializableBase::serialized_data &data)
	{
		Anope::string query_text = "INSERT INTO `" + table + "` (";
		for (SerializableBase::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "`,";
		query_text.erase(query_text.end() - 1);
		query_text += ") VALUES (";
		for (SerializableBase::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "@" + it->first + "@,";
		query_text.erase(query_text.end() - 1);
		query_text += ")";

		SQLQuery query(query_text);
		for (SerializableBase::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunBackground(query);
	}
	
 public:
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sql(""), sqlinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnSaveDatabase, I_OnLoadDatabase };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload()
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->sql = engine;
	}

	EventReturn OnSaveDatabase()
	{
		if (!this->sql)
		{
			Log() << "db_sql: Unable to save databases, is SQL configured correctly?";
			return EVENT_CONTINUE;
		}

		std::map<Anope::string, SerializableBase::serialized_data> table_layout;
		for (std::list<SerializableBase *>::iterator it = serialized_items.begin(), it_end = serialized_items.end(); it != it_end; ++it)
		{
			SerializableBase *base = *it;
			SerializableBase::serialized_data data = base->serialize();

			if (data.empty())
				continue;

			if (table_layout.count(base->serialize_name()) == 0)
			{
				this->DropTable(base->serialize_name());
				this->RunBackground(this->sql->CreateTable(base->serialize_name(), data));
			}
			else
				this->AlterTable(base->serialize_name(), table_layout[base->serialize_name()], data);
			table_layout[base->serialize_name()] = data;

			this->Insert(base->serialize_name(), data);
		}
		return EVENT_CONTINUE;
	}

	EventReturn OnLoadDatabase()
	{
		if (!this->sql)
		{
			Log() << "db_sql: Unable to load databases, is SQL configured correctly?";
			return EVENT_CONTINUE;
		}

		for (std::vector<SerializableBase *>::iterator it = serialized_types.begin(), it_end = serialized_types.end(); it != it_end; ++it)
		{
			SerializableBase *sb = *it;

			SQLQuery query("SELECT * FROM `" + sb->serialize_name() + "`");
			SQLResult res = this->sql->RunQuery(query);
			for (int i = 0; i < res.Rows(); ++i)
			{
				SerializableBase::serialized_data data;
				const std::map<Anope::string, Anope::string> &row = res.Row(i);
				for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
					data[rit->first] << rit->second;

				sb->alloc(data);
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(DBSQL)

