/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

/* RequiredLibraries: sqlite3 */
/* RequiredWindowsLibraries: sqlite3 */

#include "module.h"
#include "modules/sql.h"
#include <sqlite3.h>

using namespace SQL;

/* SQLite3 API, based from InspIRCd */

/** A SQLite result
 */
class SQLiteResult final
	: public Result
{
public:
	SQLiteResult(unsigned int i, const Query &q, const Anope::string &fq) : Result(i, q, fq)
	{
	}

	SQLiteResult(const Query &q, const Anope::string &fq, const Anope::string &err) : Result(0, q, fq, err)
	{
	}

	void AddRow(const std::map<Anope::string, Anope::string> &data)
	{
		this->entries.push_back(data);
	}
};

/** A SQLite database, there can be multiple
 */
class SQLiteService final
	: public Provider
{
	std::map<Anope::string, std::set<Anope::string> > active_schema;

	Anope::string database;

	sqlite3 *sql = nullptr;

	Anope::string Escape(const Anope::string &query);

public:
	SQLiteService(Module *o, const Anope::string &n, const Anope::string &d);

	~SQLiteService();

	void Run(Interface *i, const Query &query) override;

	Result RunQuery(const Query &query) override;

	std::vector<Query> CreateTable(const Anope::string &table, const Data &data) override;

	Query BuildInsert(const Anope::string &table, unsigned int id, Data &data) override;

	Query GetTables(const Anope::string &prefix) override;

	Anope::string BuildQuery(const Query &q);

	Anope::string FromUnixtime(time_t) override;

	Anope::string GetColumnType(Serialize::DataType dt)
	{
		switch (dt)
		{
			case Serialize::DataType::BOOL:
				return "INTEGER";

			case Serialize::DataType::FLOAT:
				return "REAL";

			case Serialize::DataType::INT:
				return "INTEGER";

			case Serialize::DataType::TEXT:
				return "TEXT";

			// SQLite lacks support for 64-bit unsigned integers so we have to
			// store them as text columns instead.
			case Serialize::DataType::UINT:
				return "TEXT";
		}

		return "TEXT"; // Should never be reached
	}
};

class ModuleSQLite final
	: public Module
{
	/* SQL connections */
	std::map<Anope::string, SQLiteService *> SQLiteServices;
public:
	ModuleSQLite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		Log() << "Module was compiled against SQLite version " << SQLITE_VERSION << " and is running against version " << sqlite3_libversion();
	}

	~ModuleSQLite()
	{
		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end(); ++it)
			delete it->second;
		SQLiteServices.clear();
	}

	void OnReload(Configuration::Conf &conf) override
	{
		Configuration::Block &config = conf.GetModule(this);

		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end();)
		{
			const Anope::string &cname = it->first;
			SQLiteService *s = it->second;
			int i, num;
			++it;

			for (i = 0, num = Config->CountBlock("sqlite"); i < num; ++i)
				if (config.GetBlock("sqlite", i).Get<const Anope::string>("name", "sqlite/main") == cname)
					break;

			if (i == num)
			{
				Log(LOG_NORMAL, "sqlite") << "SQLite: Removing server connection " << cname;

				delete s;
				this->SQLiteServices.erase(cname);
			}
		}

		for (int i = 0; i < Config->CountBlock("sqlite"); ++i)
		{
			Configuration::Block &block = config.GetBlock("sqlite", i);
			Anope::string connname = block.Get<const Anope::string>("name", "sqlite/main");

			if (this->SQLiteServices.find(connname) == this->SQLiteServices.end())
			{
				auto database = Anope::ExpandData(block.Get<const Anope::string>("database", "anope"));
				try
				{
					auto *ss = new SQLiteService(this, connname, database);
					this->SQLiteServices[connname] = ss;

					Log(LOG_NORMAL, "sqlite") << "SQLite: Successfully added database " << database;
				}
				catch (const SQL::Exception &ex)
				{
					Log(LOG_NORMAL, "sqlite") << "SQLite: " << ex.GetReason();
				}
			}
		}
	}
};

SQLiteService::SQLiteService(Module *o, const Anope::string &n, const Anope::string &d)
: Provider(o, n), database(d)
{
	int db = sqlite3_open_v2(database.c_str(), &this->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	if (db != SQLITE_OK)
	{
		Anope::string exstr = "Unable to open SQLite database " + database;
		if (this->sql)
		{
			exstr += ": ";
			exstr += sqlite3_errmsg(this->sql);
			sqlite3_close(this->sql);
		}
		throw SQL::Exception(exstr);
	}
}

SQLiteService::~SQLiteService()
{
	sqlite3_interrupt(this->sql);
	sqlite3_close(this->sql);
}

void SQLiteService::Run(Interface *i, const Query &query)
{
	Result res = this->RunQuery(query);
	if (!res.GetError().empty())
		i->OnError(res);
	else
		i->OnResult(res);
}

Result SQLiteService::RunQuery(const Query &query)
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

	SQLiteResult result(0, query, real_query);

	while ((err = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		std::map<Anope::string, Anope::string> items;
		for (int i = 0; i < cols; ++i)
		{
			const char *data = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
			if (data && *data)
				items[columns[i]] = data;
		}
		result.AddRow(items);
	}

	result.id = sqlite3_last_insert_rowid(this->sql);

	sqlite3_finalize(stmt);

	if (err != SQLITE_DONE)
		return SQLiteResult(query, real_query, sqlite3_errmsg(this->sql));

	// GCC and clang disagree about whether this should be a move >:(
#ifdef __clang__
	return std::move(result);
#else
	return result;
#endif
}

std::vector<Query> SQLiteService::CreateTable(const Anope::string &table, const Data &data)
{
	std::vector<Query> queries;
	std::set<Anope::string> &known_cols = this->active_schema[table];

	if (known_cols.empty())
	{
		Log(LOG_DEBUG) << "sqlite: Fetching columns for " << table;

		Result columns = this->RunQuery("PRAGMA table_info(" + table + ")");
		for (int i = 0; i < columns.Rows(); ++i)
		{
			const Anope::string &column = columns.Get(i, "name");

			Log(LOG_DEBUG) << "sqlite: Column #" << i << " for " << table << ": " << column;
			known_cols.insert(column);
		}
	}

	if (known_cols.empty())
	{
		Anope::string query_text = "CREATE TABLE `" + table + "` (`id` INTEGER PRIMARY KEY, `timestamp` timestamp DEFAULT CURRENT_TIMESTAMP";

		for (const auto &[column, _] : data.data)
		{
			known_cols.insert(column);

			query_text += ", `" + column + "` " + GetColumnType(data.GetType(column));
		}

		query_text += ")";

		queries.push_back(query_text);

		query_text = "CREATE UNIQUE INDEX `" + table + "_id_idx` ON `" + table + "` (`id`)";
		queries.push_back(query_text);

		query_text = "CREATE INDEX `" + table + "_timestamp_idx` ON `" + table + "` (`timestamp`)";
		queries.push_back(query_text);

		query_text = "CREATE TRIGGER `" + table + "_trigger` AFTER UPDATE ON `" + table + "` FOR EACH ROW BEGIN UPDATE `" + table + "` SET `timestamp` = CURRENT_TIMESTAMP WHERE `id` = `old.id`; end;";
		queries.push_back(query_text);
	}
	else
	{
		for (const auto &[column, _] : data.data)
		{
			if (known_cols.count(column) > 0)
				continue;

			known_cols.insert(column);

			Anope::string query_text = "ALTER TABLE `" + table + "` ADD `" + column + "` " + GetColumnType(data.GetType(column));;

			queries.push_back(query_text);
		}
	}

	return queries;
}

Query SQLiteService::BuildInsert(const Anope::string &table, unsigned int id, Data &data)
{
	/* Empty columns not present in the data set */
	for (const auto &known_col : this->active_schema[table])
	{
		if (known_col != "id" && known_col != "timestamp" && data.data.count(known_col) == 0)
			data[known_col] << "";
	}

	Anope::string query_text = "REPLACE INTO `" + table + "` (";
	if (id > 0)
		query_text += "`id`,";
	for (const auto &[field, _] : data.data)
		query_text += "`" + field + "`,";
	query_text.erase(query_text.length() - 1);
	query_text += ") VALUES (";
	if (id > 0)
		query_text += Anope::ToString(id) + ",";
	for (const auto &[field, _] : data.data)
		query_text += "@" + field + "@,";
	query_text.erase(query_text.length() - 1);
	query_text += ")";

	Query query(query_text);
	for (auto &[field, value] : data.data)
	{
		Anope::string buf;
		*value >> buf;

		auto escape = true;
		switch (data.GetType(field))
		{
			case Serialize::DataType::BOOL:
			case Serialize::DataType::FLOAT:
			case Serialize::DataType::INT:
			{
				if (buf.empty())
					buf = "0";
				escape = false;
				break;
			}

			case Serialize::DataType::TEXT:
			case Serialize::DataType::UINT:
			{
				if (buf.empty())
				{
					buf = "NULL";
					escape = false;
				}
				break;
			}
		}

		query.SetValue(field, buf, escape);
	}

	return query;
}

Query SQLiteService::GetTables(const Anope::string &prefix)
{
	return Query("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE '" + prefix + "%';");
}

Anope::string SQLiteService::Escape(const Anope::string &query)
{
	char *e = sqlite3_mprintf("%q", query.c_str());
	Anope::string buffer = e;
	sqlite3_free(e);
	return buffer;
}

Anope::string SQLiteService::BuildQuery(const Query &q)
{
	Anope::string real_query = q.query;

	for (const auto &[name, value] : q.parameters)
		real_query = real_query.replace_all_cs("@" + name + "@", (value.escape ? ("'" + this->Escape(value.data) + "'") : value.data));

	return real_query;
}

Anope::string SQLiteService::FromUnixtime(time_t t)
{
	return "datetime('" + Anope::ToString(t) + "', 'unixepoch')";
}

MODULE_INIT(ModuleSQLite)
