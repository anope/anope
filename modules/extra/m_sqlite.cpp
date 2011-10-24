/* RequiredLibraries: sqlite3 */

#include "module.h"
#include <sqlite3.h>
#include "sql.h"

/* SQLite3 API, based from InspiRCd */

/** A SQLite result
 */
class SQLiteResult : public SQLResult
{
 public:
	SQLiteResult(const SQLQuery &q, const Anope::string &fq) : SQLResult(q, fq)
	{
	}

	SQLiteResult(const SQLQuery &q, const Anope::string &fq, const Anope::string &err) : SQLResult(q, fq, err)
	{
	}

	void addRow(const std::map<Anope::string, Anope::string> &data)
	{
		this->entries.push_back(data);
	}
};

/** A SQLite database, there can be multiple
 */
class SQLiteService : public SQLProvider
{
	Anope::string database;

	sqlite3 *sql;

	Anope::string Escape(const Anope::string &query);

 public:
	SQLiteService(Module *o, const Anope::string &n, const Anope::string &d);

	~SQLiteService();

	void Run(SQLInterface *i, const SQLQuery &query);

	SQLResult RunQuery(const SQLQuery &query);

	SQLQuery CreateTable(const Anope::string &table, const SerializableBase::serialized_data &data);

	Anope::string BuildQuery(const SQLQuery &q);
};

class ModuleSQLite : public Module
{
	/* SQL connections */
	std::map<Anope::string, SQLiteService *> SQLiteServices;
 public:
	ModuleSQLite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this,  sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	~ModuleSQLite()
	{
		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end(); ++it)
			delete it->second;
		SQLiteServices.clear();
	}

	void OnReload()
	{
		ConfigReader config;
		int i, num;

		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end();)
		{
			const Anope::string &cname = it->first;
			SQLiteService *s = it->second;
			++it;

			for (i = 0, num = config.Enumerate("sqlite"); i < num; ++i)
				if (config.ReadValue("sqlite", "name", "sqlite/main", i) == cname)
					break;

			if (i == num)
			{
				Log(LOG_NORMAL, "sqlite") << "SQLite: Removing server connection " << cname;

				delete s;
				this->SQLiteServices.erase(cname);
			}
		}

		for (i = 0, num = config.Enumerate("sqlite"); i < num; ++i)
		{
			Anope::string connname = config.ReadValue("sqlite", "name", "sqlite/main", i);

			if (this->SQLiteServices.find(connname) == this->SQLiteServices.end())
			{
				Anope::string database = config.ReadValue("sqlite", "database", "anope", i);

				try
				{
					SQLiteService *ss = new SQLiteService(this, connname, database);
					this->SQLiteServices[connname] = ss;

					Log(LOG_NORMAL, "sqlite") << "SQLite: Successfully added database " << database;
				}
				catch (const SQLException &ex)
				{
					Log(LOG_NORMAL, "sqlite") << "SQLite: " << ex.GetReason();
				}
			}
		}
	}
};

SQLiteService::SQLiteService(Module *o, const Anope::string &n, const Anope::string &d)
: SQLProvider(o, n), database(d), sql(NULL)
{
	int db = sqlite3_open_v2(database.c_str(), &this->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	if (db != SQLITE_OK)
		throw SQLException("Unable to open SQLite database " + database + ": " + sqlite3_errmsg(this->sql));
}

SQLiteService::~SQLiteService()
{
	sqlite3_interrupt(this->sql);
	sqlite3_close(this->sql);
}

void SQLiteService::Run(SQLInterface *i, const SQLQuery &query)
{
	SQLResult res = this->RunQuery(query);
	if (!res.GetError().empty())
		i->OnError(res);
	else
		i->OnResult(res);
}

SQLResult SQLiteService::RunQuery(const SQLQuery &query)
{
	Anope::string real_query = this->BuildQuery(query);
	sqlite3_stmt *stmt;
	int err = sqlite3_prepare_v2(this->sql, real_query.c_str(), real_query.length(), &stmt, NULL);
	if (err != SQLITE_OK)
		return SQLiteResult(query, real_query, sqlite3_errmsg(this->sql));

	std::vector<Anope::string> columns;
	int cols = sqlite3_column_count(stmt);
	columns.resize(cols);
	for (int i = 0; i < cols; ++i)
		columns[i] = sqlite3_column_name(stmt, i);

	SQLiteResult result(query, real_query);

	do
	{
		err = sqlite3_step(stmt);
		if (err == SQLITE_ROW)
		{
			std::map<Anope::string, Anope::string> items;
			for (int i = 0; i < cols; ++i)
			{
				const char *data = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
				if (data && *data)
					items[columns[i]] = data;
			}
			result.addRow(items);
		}
	}
	while (err == SQLITE_ROW);

	if (err != SQLITE_DONE)
		return SQLiteResult(query, real_query, sqlite3_errmsg(this->sql));

	return result;
}

SQLQuery SQLiteService::CreateTable(const Anope::string &table, const SerializableBase::serialized_data &data)
{
	Anope::string query_text = "CREATE TABLE `" + table + "` (", key_buf;
	for (SerializableBase::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
	{
		query_text += "`" + it->first + "` ";
		if (it->second.getType() == Serialize::DT_INT)
			query_text += "int(11)";
		else if (it->second.getMax() > 0)
			query_text += "varchar(" + stringify(it->second.getMax()) + ")";
		else
			query_text += "text";
		query_text += ",";

		if (it->second.getKey())
		{
			if (key_buf.empty())
				key_buf = "UNIQUE (";
			key_buf += "`" + it->first + "`,";
		}
	}
	if (!key_buf.empty())
	{
		key_buf.erase(key_buf.end() - 1);
		key_buf += ")";
		query_text += " " + key_buf;
	}
	else
		query_text.erase(query_text.end() - 1);
	query_text += ")";

	return SQLQuery(query_text);
}

Anope::string SQLiteService::Escape(const Anope::string &query)
{
	char *e = sqlite3_mprintf("%q", query.c_str());
	Anope::string buffer = e;
	sqlite3_free(e);
	return buffer;
}

Anope::string SQLiteService::BuildQuery(const SQLQuery &q)
{
	Anope::string real_query = q.query;

	for (std::map<Anope::string, Anope::string>::const_iterator it = q.parameters.begin(), it_end = q.parameters.end(); it != it_end; ++it)
		real_query = real_query.replace_all_cs("@" + it->first + "@", "'" + this->Escape(it->second) + "'");

	return real_query;
}

MODULE_INIT(ModuleSQLite)

