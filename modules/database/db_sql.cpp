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

	void AlterTable(const Anope::string &table, std::set<Anope::string> &data, const Serializable::serialized_data &newd)
	{
		for (Serializable::serialized_data::const_iterator it = newd.begin(), it_end = newd.end(); it != it_end; ++it)
		{
			if (data.count(it->first) > 0)
				continue;

			data.insert(it->first);

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

	void Insert(const Anope::string &table, const Serializable::serialized_data &data)
	{
		Anope::string query_text = "INSERT INTO `" + table + "` (";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "`,";
		query_text.erase(query_text.end() - 1);
		query_text += ") VALUES (";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "@" + it->first + "@,";
		query_text.erase(query_text.end() - 1);
		query_text += ")";

		SQLQuery query(query_text);
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
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

		const std::list<Serializable *> &items = Serializable::GetItems();
		if (items.empty())
			return EVENT_CONTINUE;

		this->DropAll();

		std::map<Anope::string, std::set<Anope::string> > table_layout;
		for (std::list<Serializable *>::const_iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
		{
			Serializable *base = *it;
			Serializable::serialized_data data = base->serialize();

			if (data.empty())
				continue;

			std::set<Anope::string> &layout = table_layout[base->GetSerializeName()];
			if (layout.empty())
			{
				this->RunBackground(this->sql->CreateTable(base->GetSerializeName(), data));

				for (Serializable::serialized_data::iterator it2 = data.begin(), it2_end = data.end(); it2 != it2_end; ++it2)
					layout.insert(it2->first);
			}
			else
				this->AlterTable(base->GetSerializeName(), layout, data);

			this->Insert(base->GetSerializeName(), data);
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

		const std::vector<Anope::string> type_order = SerializeType::GetTypeOrder();
		for (unsigned i = 0; i < type_order.size(); ++i)
		{
			SerializeType *sb = SerializeType::Find(type_order[i]);

			SQLQuery query("SELECT * FROM `" + sb->GetName() + "`");
			SQLResult res = this->sql->RunQuery(query);
			for (int j = 0; j < res.Rows(); ++j)
			{
				Serializable::serialized_data data;
				const std::map<Anope::string, Anope::string> &row = res.Row(j);
				for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
					data[rit->first] << rit->second;

				sb->Create(data);
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(DBSQL)

