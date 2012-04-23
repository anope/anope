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

class DBSQL : public Module
{
	service_reference<SQLProvider> sql;
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

	void DropAll()
	{
		SQLResult r = this->sql->RunQuery(this->sql->GetTables());
		for (int i = 0; i < r.Rows(); ++i)
		{
			const std::map<Anope::string, Anope::string> &map = r.Row(i);
			for (std::map<Anope::string, Anope::string>::const_iterator it = map.begin(); it != map.end(); ++it)
				this->DropTable(it->second);
		}
	}

	void Insert(const Anope::string &table, const Serialize::Data &data)
	{
		Anope::string query_text = "INSERT INTO `" + table + "` (";
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "`,";
		query_text.erase(query_text.end() - 1);
		query_text += ") VALUES (";
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "@" + it->first + "@,";
		query_text.erase(query_text.end() - 1);
		query_text += ")";

		SQLQuery query(query_text);
		for (Serialize::Data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunBackground(query);
	}
	
 public:
	DBSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sql("", ""), sqlinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnSaveDatabase, I_OnLoadDatabase };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->sql = service_reference<SQLProvider>("SQLProvider", engine);
	}

	EventReturn OnSaveDatabase() anope_override
	{
		if (!this->sql)
		{
			Log() << "db_sql: Unable to save databases, is SQL configured correctly?";
			return EVENT_CONTINUE;
		}

		const std::list<Serializable *> &items = Serializable::GetItems();
		if (items.empty())
			return EVENT_CONTINUE;

		this->DropAll();

		for (std::list<Serializable *>::const_iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
		{
			Serializable *base = *it;
			Serialize::Data data = base->serialize();

			if (data.empty())
				continue;

			std::vector<SQLQuery> create_queries = this->sql->CreateTable(base->serialize_name(), data);
			for (unsigned i = 0; i < create_queries.size(); ++i)
				this->RunBackground(create_queries[i]);

			this->Insert(base->serialize_name(), data);
		}

		return EVENT_CONTINUE;
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

			SQLQuery query("SELECT * FROM `" + sb->GetName() + "`");
			SQLResult res = this->sql->RunQuery(query);
			for (int j = 0; j < res.Rows(); ++j)
			{
				Serialize::Data data;
				const std::map<Anope::string, Anope::string> &row = res.Row(j);
				for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
					data[rit->first] << rit->second;

				sb->Unserialize(NULL, data);
			}
		}

		return EVENT_STOP;
	}
};

MODULE_INIT(DBSQL)

