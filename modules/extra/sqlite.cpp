/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

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
	std::vector<Query> CreateTable(const Anope::string &, Serialize::TypeBase *) override;
	std::vector<Query> AlterTable(const Anope::string &, Serialize::TypeBase *, Serialize::FieldBase *) override;
	std::vector<Query> CreateIndex(const Anope::string &table, const Anope::string &field) override;
	Query SelectFind(const Anope::string &table, const Anope::string &field) override;

	Query BeginTransaction() override;
	Query Commit() override;

	Serialize::ID GetID(const Anope::string &) override;

	Query GetTables(const Anope::string &prefix);

	Anope::string BuildQuery(const Query &q);

	static void Canonicalize(sqlite3_context *context, int argc, sqlite3_value **argv);
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

	int ret = sqlite3_create_function_v2(this->sql, "anope_canonicalize", 1, SQLITE_DETERMINISTIC, NULL, Canonicalize, NULL, NULL, NULL);
	if (ret != SQLITE_OK)
		Log(LOG_DEBUG) << "Unable to add anope_canonicalize function: " << sqlite3_errmsg(this->sql);
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
		const char *msg = sqlite3_errmsg(this->sql);

		Log(LOG_DEBUG) << "sqlite: error in query " << real_query << ": " << msg;

		return SQLiteResult(query, real_query, msg);
	}

	int id = sqlite3_last_insert_rowid(this->sql);
	SQLiteResult result(this->sql, id, query, real_query, stmt);

	sqlite3_finalize(stmt);

	if (!result.GetError().empty())
		Log(LOG_DEBUG) << "sqlite: error executing query " << real_query << ": " << result.GetError();
	else
		Log(LOG_DEBUG) << "sqlite: executed: " << real_query;

	return result;
}

std::vector<Query> SQLiteService::InitSchema(const Anope::string &prefix)
{
	std::vector<Query> queries;

	queries.push_back(Query("PRAGMA foreign_keys = ON"));

	Query t = "CREATE TABLE IF NOT EXISTS `" + prefix + "objects` (`id` PRIMARY KEY, `type`)";
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

std::vector<Query> SQLiteService::CreateTable(const Anope::string &prefix, Serialize::TypeBase *base)
{
	std::vector<Query> queries;

	if (active_schema.find(prefix + base->GetName()) == active_schema.end())
	{
		Anope::string query = "CREATE TABLE IF NOT EXISTS `" + prefix + base->GetName() + "` (";
		std::set<Anope::string> fields;

		for (Serialize::FieldBase *field : base->GetFields())
		{
			query += "`" + field->serialize_name + "`";
			fields.insert(field->serialize_name);

			if (field->object)
			{
				query += " REFERENCES " + prefix + "objects(id) ON DELETE ";

				if (field->depends)
				{
					query += "CASCADE";
				}
				else
				{
					query += "SET NULL";
				}

				query += " DEFERRABLE INITIALLY DEFERRED";
			}

			query += ",";
		}

		query += " `id` NOT NULL PRIMARY KEY, FOREIGN KEY (id) REFERENCES " + prefix + "objects(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED)";
		queries.push_back(query);

		active_schema[prefix + base->GetName()] = fields;
	}

	return queries;
}

std::vector<Query> SQLiteService::AlterTable(const Anope::string &prefix, Serialize::TypeBase *type, Serialize::FieldBase *field)
{
	const Anope::string &table = type->GetName();

	std::vector<Query> queries;
	std::set<Anope::string> &s = active_schema[prefix + table];

	if (!s.count(field->serialize_name))
	{
		Anope::string buf = "ALTER TABLE `" + prefix + table + "` ADD `" + field->serialize_name + "`";

		if (field->object)
		{
			buf += " REFERENCES " + prefix + "objects(id) ON DELETE ";

			if (field->depends)
			{
				buf += "CASCADE";
			}
			else
			{
				buf += "SET NULL";
			}

			buf += " DEFERRABLE INITIALLY DEFERRED";
		}

		queries.push_back(Query(buf));
		s.insert(field->serialize_name);
	}

	return queries;
}

std::vector<Query> SQLiteService::CreateIndex(const Anope::string &table, const Anope::string &field)
{
	std::vector<Query> queries;

	if (indexes[table].count(field))
		return queries;

	Query t = "CREATE INDEX IF NOT EXISTS idx_" + field + " ON `" + table + "` (anope_canonicalize(" + field + "))";
	queries.push_back(t);

	indexes[table].insert(field);

	return queries;
}

Query SQLiteService::SelectFind(const Anope::string &table, const Anope::string &field)
{
	return Query("SELECT `id` FROM `" + table + "` WHERE anope_canonicalize(`" + field + "`) = anope_canonicalize(@value@)");
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
	Query query = "SELECT `id` FROM `" + prefix + "objects` ORDER BY `id` DESC LIMIT 1";
	Serialize::ID id = 0;

	Result res = RunQuery(query);
	if (res.Rows())
	{
		id = convertTo<Serialize::ID>(res.Get(0, "id"));

		/* next id */
		++id;
	}

	/* OnSerializableCreate is called immediately after this which does the insert */

	return id;
}

Query SQLiteService::GetTables(const Anope::string &prefix)
{
	return Query("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE '" + prefix + "%';");
}

Anope::string SQLiteService::Escape(const Anope::string &query)
{
	std::vector<char> buffer(query.length() * 2 + 1);
	sqlite3_snprintf(buffer.size(), &buffer[0], "%q", query.c_str());
	return &buffer[0];
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

void SQLiteService::Canonicalize(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	sqlite3_value *arg = argv[0];
	const char *text = reinterpret_cast<const char *>(sqlite3_value_text(arg));

	if (text == nullptr)
		return;

	const Anope::string &result = Anope::transform(text);

	sqlite3_result_text(context, result.c_str(), -1, SQLITE_TRANSIENT);
}

MODULE_INIT(ModuleSQLite)

