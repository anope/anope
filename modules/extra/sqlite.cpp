/* RequiredLibraries: sqlite3 */
/* RequiredWindowsLibraries: sqlite3 */

#include "module.h"
#include "modules/sql.h"
#include <sqlite3.h>

using namespace SQL;

/* SQLite3 API, based from InspiRCd */

/** A SQLite result
 */
class SQLiteResult : public Result
{
 public:
	SQLiteResult(sqlite3 *sql, unsigned int id, const Query &q, const Anope::string &fq, sqlite3_stmt *stmt) : Result(id, q, fq)
	{
		int cols = sqlite3_column_count(stmt);
		for (int i = 0; i < cols; ++i)
			this->columns.push_back(sqlite3_column_name(stmt, i));

		int err;
		while ((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			std::vector<SQL::Result::Value> values;

			for (int i = 0; i < cols; ++i)
			{
				const char *data = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));

				Value v;
				v.null = !data;
				v.value = data ? data : "";
				values.push_back(v);
			}

			this->values.push_back(values);
		}

		if (err != SQLITE_DONE)
		{
			error = sqlite3_errmsg(sql);
		}
	}

	SQLiteResult(const Query &q, const Anope::string &fq, const Anope::string &err) : Result(0, q, fq, err)
	{
	}
};

/** A SQLite database, there can be multiple
 */
class SQLiteService : public Provider
{
	std::map<Anope::string, std::set<Anope::string> > active_schema, indexes;

	Anope::string database;

	sqlite3 *sql;

	Anope::string Escape(const Anope::string &query);

 public:
	SQLiteService(Module *o, const Anope::string &n, const Anope::string &d);

	~SQLiteService();

	void Run(Interface *i, const Query &query) override;

	Result RunQuery(const Query &query);

	std::vector<Query> InitSchema(const Anope::string &prefix) override;
	std::vector<Query> Replace(const Anope::string &table, const Query &, const std::set<Anope::string> &) override;
	std::vector<Query> CreateTable(const Anope::string &, const Anope::string &table) override;
	std::vector<Query> AlterTable(const Anope::string &, const Anope::string &table, const Anope::string &field, bool) override;
	std::vector<Query> CreateIndex(const Anope::string &table, const Anope::string &field) override;

	Query BeginTransaction() override;
	Query Commit() override;

	Serialize::ID GetID(const Anope::string &) override;

	Query GetTables(const Anope::string &prefix);

	Anope::string BuildQuery(const Query &q);
};

class ModuleSQLite : public Module
{
	/* SQL connections */
	std::map<Anope::string, SQLiteService *> SQLiteServices;

 public:
	ModuleSQLite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
	}

	~ModuleSQLite()
	{
		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end(); ++it)
			delete it->second;
		SQLiteServices.clear();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *config = conf->GetModule(this);

		for (std::map<Anope::string, SQLiteService *>::iterator it = this->SQLiteServices.begin(); it != this->SQLiteServices.end();)
		{
			const Anope::string &cname = it->first;
			SQLiteService *s = it->second;
			int i, num;
			++it;

			for (i = 0, num = config->CountBlock("sqlite"); i < num; ++i)
				if (config->GetBlock("sqlite", i)->Get<Anope::string>("name", "sqlite/main") == cname)
					break;

			if (i == num)
			{
				Log(LOG_NORMAL, "sqlite") << "SQLite: Removing server connection " << cname;

				delete s;
				this->SQLiteServices.erase(cname);
			}
		}

		for (int i = 0; i < config->CountBlock("sqlite"); ++i)
		{
			Configuration::Block *block = config->GetBlock("sqlite", i);
			Anope::string connname = block->Get<Anope::string>("name", "sqlite/main");

			if (this->SQLiteServices.find(connname) == this->SQLiteServices.end())
			{
				Anope::string database = Anope::DataDir + "/" + block->Get<Anope::string>("database", "anope");

				try
				{
					SQLiteService *ss = new SQLiteService(this, connname, database);
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
: Provider(o, n), database(d), sql(NULL)
{
	int db = sqlite3_open_v2(database.c_str(), &this->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	if (db != SQLITE_OK)
		throw SQL::Exception("Unable to open SQLite database " + database + ": " + sqlite3_errmsg(this->sql));
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
	{
		return SQLiteResult(query, real_query, sqlite3_errmsg(this->sql));
	}

	int id = sqlite3_last_insert_rowid(this->sql);
	SQLiteResult result(this->sql, id, query, real_query, stmt);

	sqlite3_finalize(stmt);

	return result;
}

std::vector<Query> SQLiteService::InitSchema(const Anope::string &prefix)
{
	std::vector<Query> queries;

	Query t = "CREATE TABLE IF NOT EXISTS `" + prefix + "id` ("
		"`id`"
		")";
	queries.push_back(t);

	t = "CREATE TABLE IF NOT EXISTS `" + prefix + "objects` (`id` PRIMARY KEY, `type`)";
	queries.push_back(t);

	t = "CREATE TABLE IF NOT EXISTS `" + prefix + "edges` ("
		"`id`,"
		"`field`,"
		"`other_id`,"
		"PRIMARY KEY (`id`, `field`)"
		")";
	queries.push_back(t);

	t = "CREATE INDEX IF NOT EXISTS idx_edge ON `" + prefix + "edges` (other_id)";
	queries.push_back(t);

	return queries;
}

std::vector<Query> SQLiteService::Replace(const Anope::string &table, const Query &q, const std::set<Anope::string> &keys)
{
	std::vector<Query> queries;

	Anope::string query_text = "INSERT OR IGNORE INTO `" + table + "` (";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		query_text += "`" + p.first + "`,";
	query_text.erase(query_text.length() - 1);
	query_text += ") VALUES (";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		query_text += "@" + p.first + "@,";
	query_text.erase(query_text.length() - 1);
	query_text += ")";

	Query query(query_text);
	query.parameters = q.parameters;
	queries.push_back(query);

	query_text = "UPDATE `" + table + "` SET ";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		if (!keys.count(p.first))
			query_text += "`" + p.first + "` = @" + p.first + "@,";
	query_text.erase(query_text.length() - 1);
	unsigned int i = 0;
	for (const Anope::string &key : keys)
	{
		if (!i++)
			query_text += " WHERE ";
		else
			query_text += " AND ";
		query_text += "`" + key + "` = @" + key + "@";
	}

	query = query_text;
	query.parameters = q.parameters;
	queries.push_back(query);

	return queries;
}

std::vector<Query> SQLiteService::CreateTable(const Anope::string &prefix, const Anope::string &table)
{
	std::vector<Query> queries;

	if (active_schema.find(prefix + table) == active_schema.end())
	{
		Query t = "CREATE TABLE IF NOT EXISTS `" + prefix + table + "` (`id` bigint(20) NOT NULL, PRIMARY KEY (`id`))";
		queries.push_back(t);

		active_schema[prefix + table];
	}

	return queries;
}

std::vector<Query> SQLiteService::AlterTable(const Anope::string &prefix, const Anope::string &table, const Anope::string &field, bool)
{
	std::vector<Query> queries;
	std::set<Anope::string> &s = active_schema[prefix + table];

	if (!s.count(field))
	{
		Query t =  "ALTER TABLE `" + prefix + table + "` ADD `" + field + "` COLLATE NOCASE";
		queries.push_back(t);
		s.insert(field);
	}

	return queries;
}

std::vector<Query> SQLiteService::CreateIndex(const Anope::string &table, const Anope::string &field)
{
	std::vector<Query> queries;

	if (indexes[table].count(field))
		return queries;

	Query t = "CREATE INDEX IF NOT EXISTS idx_" + field + " ON `" + table + "` (" + field + ")";
	queries.push_back(t);

	indexes[table].insert(field);

	return queries;
}

Query SQLiteService::BeginTransaction()
{
	return Query("BEGIN TRANSACTION");
}

Query SQLiteService::Commit()
{
	return Query("COMMIT");
}

Serialize::ID SQLiteService::GetID(const Anope::string &prefix)
{
	/* must be in a deferred or reserved transaction here for atomic row update */

	Query query("SELECT `id` FROM `" + prefix + "id`");
	Serialize::ID id;

	Result res = RunQuery(query);
	if (res.Rows())
	{
		id = convertTo<Serialize::ID>(res.Get(0, "id"));

		Query update_query("UPDATE `" + prefix + "id` SET `id` = `id` + 1");
		RunQuery(update_query);
	}
	else
	{
		id = 0;

		Query insert_query("INSERT INTO `" + prefix + "id` (id) VALUES(@id@)");
		insert_query.SetValue("id", 1);
		RunQuery(insert_query);
	}

	return id;
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

	for (std::map<Anope::string, QueryData>::const_iterator it = q.parameters.begin(), it_end = q.parameters.end(); it != it_end; ++it)
	{
		const QueryData& qd = it->second;
		Anope::string replacement;

		if (qd.null)
			replacement = "NULL";
		else if (!qd.escape)
			replacement = qd.data;
		else
			replacement = "'" + this->Escape(qd.data) + "'";

		real_query = real_query.replace_all_cs("@" + it->first + "@", replacement);
	}

	return real_query;
}

MODULE_INIT(ModuleSQLite)

