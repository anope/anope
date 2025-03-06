/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

/* RequiredLibraries: mysqlclient */
/* RequiredWindowsLibraries: libmysql */

#include "module.h"
#include "modules/sql.h"

#ifdef WIN32
# include <mysql.h>
#else
# include <mysql/mysql.h>
#endif

#include <condition_variable>
#include <mutex>

using namespace SQL;

/** Non blocking threaded MySQL API, based loosely from InspIRCd's m_mysql.cpp
 *
 * This module spawns a single thread that is used to execute blocking MySQL queries.
 * When a module requests a query to be executed it is added to a list for the thread
 * (which never stops looping and sleeping) to pick up and execute, the result of which
 * is inserted in to another queue to be picked up by the main thread. The main thread
 * uses Pipe to become notified through the socket engine when there are results waiting
 * to be sent back to the modules requesting the query
 */

class MySQLService;

/** A query request
 */
struct QueryRequest final
{
	/* The connection to the database */
	MySQLService *service;
	/* The interface to use once we have the result to send the data back */
	Interface *sqlinterface;
	/* The actual query */
	Query query;

	QueryRequest(MySQLService *s, Interface *i, const Query &q) : service(s), sqlinterface(i), query(q) { }
};

/** A query result */
struct QueryResult final
{
	/* The interface to send the data back on */
	Interface *sqlinterface;
	/* The result */
	Result result;

	QueryResult(Interface *i, Result &r) : sqlinterface(i), result(r) { }
};

/** A MySQL result
 */
class MySQLResult final
	: public Result
{
	MYSQL_RES *res = nullptr;

public:
	MySQLResult(unsigned int i, const Query &q, const Anope::string &fq, MYSQL_RES *r) : Result(i, q, fq), res(r)
	{
		unsigned num_fields = res ? mysql_num_fields(res) : 0;

		/* It is not thread safe to log anything here using Log(this->owner) now :( */

		if (!num_fields)
			return;

		for (MYSQL_ROW row; (row = mysql_fetch_row(res));)
		{
			MYSQL_FIELD *fields = mysql_fetch_fields(res);

			if (fields)
			{
				std::map<Anope::string, Anope::string> items;

				for (unsigned field_count = 0; field_count < num_fields; ++field_count)
				{
					Anope::string column = (fields[field_count].name ? fields[field_count].name : "");
					Anope::string data = (row[field_count] ? row[field_count] : "");

					items[column] = data;
				}

				this->entries.push_back(items);
			}
		}
	}

	MySQLResult(const Query &q, const Anope::string &fq, const Anope::string &err) : Result(0, q, fq, err)
	{
	}

	~MySQLResult()
	{
		if (this->res)
			mysql_free_result(this->res);
	}
};

/** A MySQL connection, there can be multiple
 */
class MySQLService final
	: public Provider
{
	std::map<Anope::string, std::set<Anope::string> > active_schema;

	Anope::string database;
	Anope::string server;
	Anope::string user;
	Anope::string password;
	unsigned int port;
	Anope::string socket;

	MYSQL *sql = nullptr;

	/** Escape a query.
	 * Note the mutex must be held!
	 */
	Anope::string Escape(const Anope::string &query);

public:
	/* Locked by the SQL thread when a query is pending on this database,
	 * prevents us from deleting a connection while a query is executing
	 * in the thread
	 */
	std::mutex Lock;

	MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, unsigned int po, const Anope::string &so);

	~MySQLService();

	void Run(Interface *i, const Query &query) override;

	Result RunQuery(const Query &query) override;

	std::vector<Query> CreateTable(const Anope::string &table, const Data &data) override;

	Query BuildInsert(const Anope::string &table, unsigned int id, Data &data) override;

	Query GetTables(const Anope::string &prefix) override;

	void Connect();

	bool CheckConnection();

	Anope::string BuildQuery(const Query &q);

	Anope::string FromUnixtime(time_t) override;

	const char* GetColumnDefault(Serialize::DataType dt)
	{
		switch (dt)
		{
			case Serialize::DataType::BOOL:
			case Serialize::DataType::INT:
			case Serialize::DataType::UINT:
				return "0";
			case Serialize::DataType::FLOAT:
				return "0.0";
			case Serialize::DataType::TEXT:
				return "NULL";
		}
		return "NULL"; // Should never be reached
	}

	bool GetColumnNull(Serialize::DataType dt)
	{
		return dt == Serialize::DataType::TEXT;
	}

	const char* GetColumnType(Serialize::DataType dt)
	{
		switch (dt)
		{
			case Serialize::DataType::BOOL:
				return "TINYINT";
			case Serialize::DataType::FLOAT:
				return "DOUBLE";
			case Serialize::DataType::INT:
				return "BIGINT";
			case Serialize::DataType::TEXT:
				return "TEXT";
			case Serialize::DataType::UINT:
				return "BIGINT UNSIGNED";
		}
		return "TEXT"; // Should never be reached
	}

	Anope::string GetColumn(Serialize::DataType dt)
	{
		return Anope::printf("%s %s DEFAULT %s",
				GetColumnType(dt),
				GetColumnNull(dt) ? "NULL" : "NOT NULL",
				GetColumnDefault(dt));
	}
};

/** The SQL thread used to execute queries
 */
class DispatcherThread final
	: public Thread
{
public:
	std::condition_variable_any condvar;
	std::mutex mutex;

	DispatcherThread() : Thread() { }

	void Run() override;
};

class ModuleSQL;
static ModuleSQL *me;

class ModuleSQL final
	: public Module
	, public Pipe
{
	/* SQL connections */
	std::map<Anope::string, MySQLService *> MySQLServices;
public:
	/* Pending query requests */
	std::deque<QueryRequest> QueryRequests;
	/* Pending finished requests with results */
	std::deque<QueryResult> FinishedRequests;
	/* The thread used to execute queries */
	DispatcherThread *DThread;

	ModuleSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		me = this;

		DThread = new DispatcherThread();
		DThread->Start();

		Log(this) << "Module was compiled against MySQL version " << (MYSQL_VERSION_ID / 10000) << "." << (MYSQL_VERSION_ID / 100 % 100) << "." << (MYSQL_VERSION_ID % 100) << " and is running against version " << mysql_get_client_info();
	}

	~ModuleSQL()
	{
		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end(); ++it)
			delete it->second;
		MySQLServices.clear();

		DThread->SetExitState();
		DThread->condvar.notify_all();
		DThread->Join();
		delete DThread;
	}

	void OnReload(Configuration::Conf &conf) override
	{
		Configuration::Block &config = conf.GetModule(this);

		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end();)
		{
			const Anope::string &cname = it->first;
			MySQLService *s = it->second;
			int i;

			++it;

			for (i = 0; i < config.CountBlock("mysql"); ++i)
				if (config.GetBlock("mysql", i).Get<const Anope::string>("name", "mysql/main") == cname)
					break;

			if (i == config.CountBlock("mysql"))
			{
				Log(LOG_NORMAL, "mysql") << "MySQL: Removing server connection " << cname;

				delete s;
				this->MySQLServices.erase(cname);
			}
		}

		for (int i = 0; i < config.CountBlock("mysql"); ++i)
		{
			Configuration::Block &block = config.GetBlock("mysql", i);
			const Anope::string &connname = block.Get<const Anope::string>("name", "mysql/main");

			if (this->MySQLServices.find(connname) == this->MySQLServices.end())
			{
				const Anope::string &database = block.Get<const Anope::string>("database", "anope");
				const Anope::string &server = block.Get<const Anope::string>("server", "127.0.0.1");
				const Anope::string &user = block.Get<const Anope::string>("username", "anope");
				const Anope::string &password = block.Get<const Anope::string>("password");
				unsigned int port = block.Get<unsigned int>("port", "3306");
				const Anope::string &socket = block.Get<const Anope::string>("socket");

				try
				{
					auto *ss = new MySQLService(this, connname, database, server, user, password, port, socket);
					this->MySQLServices.emplace(connname, ss);

					Log(LOG_NORMAL, "mysql") << "MySQL: Successfully connected to server " << connname << " (" << (socket.empty() ? server + ":" + port : socket) << ")";
				}
				catch (const SQL::Exception &ex)
				{
					Log(LOG_NORMAL, "mysql") << "MySQL: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m) override
	{
		this->DThread->mutex.lock();

		for (unsigned i = this->QueryRequests.size(); i > 0; --i)
		{
			QueryRequest &r = this->QueryRequests[i - 1];

			if (r.sqlinterface && r.sqlinterface->owner == m)
			{
				if (i == 1)
				{
					r.service->Lock.lock();
					r.service->Lock.unlock();
				}

				this->QueryRequests.erase(this->QueryRequests.begin() + i - 1);
			}
		}

		this->DThread->mutex.unlock();

		this->OnNotify();
	}

	void OnNotify() override
	{
		this->DThread->mutex.lock();
		std::deque<QueryResult> finishedRequests = this->FinishedRequests;
		this->FinishedRequests.clear();
		this->DThread->mutex.unlock();

		for (const auto &qr : finishedRequests)
		{
			if (!qr.sqlinterface)
				throw SQL::Exception("NULL qr.sqlinterface in MySQLPipe::OnNotify() ?");

			if (qr.result.GetError().empty())
				qr.sqlinterface->OnResult(qr.result);
			else
				qr.sqlinterface->OnError(qr.result);
		}
	}
};

MySQLService::MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, unsigned int po, const Anope::string &so)
	: Provider(o, n)
	, database(d)
	, server(s)
	, user(u)
	, password(p)
	, port(po)
	, socket(so)
{
	Connect();
}

MySQLService::~MySQLService()
{
	me->DThread->mutex.lock();
	this->Lock.lock();
	mysql_close(this->sql);
	this->sql = NULL;

	for (unsigned i = me->QueryRequests.size(); i > 0; --i)
	{
		QueryRequest &r = me->QueryRequests[i - 1];

		if (r.service == this)
		{
			if (r.sqlinterface)
				r.sqlinterface->OnError(Result(0, r.query, "SQL Interface is going away"));
			me->QueryRequests.erase(me->QueryRequests.begin() + i - 1);
		}
	}
	this->Lock.unlock();
	me->DThread->mutex.unlock();
}

void MySQLService::Run(Interface *i, const Query &query)
{
	me->DThread->mutex.lock();
	me->QueryRequests.push_back(QueryRequest(this, i, query));
	me->DThread->mutex.unlock();
	me->DThread->condvar.notify_all();
}

Result MySQLService::RunQuery(const Query &query)
{
	this->Lock.lock();

	if (!this->CheckConnection())
	{
		this->Lock.unlock();
		return MySQLResult(query, query.query, mysql_error(this->sql));
	}

	Anope::string real_query = this->BuildQuery(query);
	if (!mysql_real_query(this->sql, real_query.c_str(), real_query.length()))
	{
		MYSQL_RES *res = mysql_store_result(this->sql);
		unsigned int id = mysql_insert_id(this->sql);

		/* because we enabled CLIENT_MULTI_RESULTS in our options
		 * a multiple statement or a procedure call can return
		 * multiple result sets.
		 * we must process them all before the next query.
		 */

		while (!mysql_next_result(this->sql))
			mysql_free_result(mysql_store_result(this->sql));

		this->Lock.unlock();
		return MySQLResult(id, query, real_query, res);
	}
	else
	{
		Anope::string error = mysql_error(this->sql);
		this->Lock.unlock();
		return MySQLResult(query, real_query, error);
	}
}

std::vector<Query> MySQLService::CreateTable(const Anope::string &table, const Data &data)
{
	std::vector<Query> queries;
	std::set<Anope::string> &known_cols = this->active_schema[table];

	if (known_cols.empty())
	{
		Log(LOG_DEBUG) << "mysql: Fetching columns for " << table;

		Result columns = this->RunQuery("SHOW COLUMNS FROM `" + table + "`");
		for (int i = 0; i < columns.Rows(); ++i)
		{

			const auto column = columns.Get(i, "Field");
			Log(LOG_DEBUG) << "mysql: Column #" << i << " for " << table << ": " << column;

			if (column == "id" || column == "timestamp")
				continue; // These columns are special and aren't part of the data.

			known_cols.insert(column);
			if (data.data.count(column) == 0)
			{
				Log(LOG_DEBUG) << "mysql: Column has been removed from the data set: " << column;
				continue;
			}

			// We know the column exists but is the type correct?
			auto update = false;
			const auto stype = data.GetType(column);

			auto coldef = columns.Get(i, "Default");
			if (coldef.empty())
				coldef = "NULL";

			const auto *newcoldef = GetColumnDefault(stype);
			if (!coldef.equals_ci(newcoldef))
			{
				Log(LOG_DEBUG) << "mysql: Updating the default of " << column << " from " << coldef << " to " << newcoldef;
				update = true;
			}

			const auto colnull = columns.Get(i, "Null");
			const auto newcolnull = GetColumnNull(stype) ? "YES" : "NO";
			if (!colnull.equals_ci(newcolnull))
			{
				Log(LOG_DEBUG) << "mysql: Updating the nullability of " << column << " from " << colnull << " to " << newcolnull;
				update = true;
			}

			const auto coltype = columns.Get(i, "Type");
			const auto *newcoltype = GetColumnType(stype);
			if (!coltype.equals_ci(newcoltype))
			{
				Log(LOG_DEBUG) << "mysql: Updating the type of " << column << " from " << coltype << " to " << newcoltype;
				update = true;
			}

			if (update)
			{
				// We can't just use MODIFY COLUMN here because the value may not
				// be valid and we may need to replace with the default.
				auto res = this->RunQuery(Anope::printf("ALTER TABLE `%s` ADD COLUMN `%s_new` %s; ",
					table.c_str(), column.c_str(), GetColumn(stype).c_str()));

				if (res)
				{
					res = this->RunQuery(Anope::printf("UPDATE IGNORE `%s` SET `%s_new` = %s; ",
						table.c_str(), column.c_str(), column.c_str()));
				}

				if (res)
				{
					res = this->RunQuery(Anope::printf("ALTER TABLE `%s` DROP COLUMN `%s`; ",
						table.c_str(), column.c_str()));
				}

				if (res)
				{
					res = this->RunQuery(Anope::printf("ALTER TABLE `%s` RENAME COLUMN `%s_new` TO `%s`; ",
						table.c_str(), column.c_str(), column.c_str()));
				}

				if (!res)
					Log(LOG_DEBUG) << "Failed to migrate the " << column << " column: " << res.GetError();
			}
		}
	}

	if (known_cols.empty())
	{
		Anope::string query_text = "CREATE TABLE `" + table + "` (`id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE,"
			" `timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP";
		for (const auto &[column, _] : data.data)
		{
			known_cols.insert(column);

			query_text += ", `" + column + "` " + GetColumn(data.GetType(column));
		}
		query_text += ", PRIMARY KEY (`id`), KEY `timestamp_idx` (`timestamp`)) ROW_FORMAT=DYNAMIC";
		queries.push_back(query_text);
	}
	else
	{
		for (const auto &[column, _] : data.data)
		{
			if (known_cols.count(column) > 0)
				continue;

			known_cols.insert(column);

			Anope::string query_text = "ALTER TABLE `" + table + "` ADD `" + column + "` " + GetColumn(data.GetType(column));

			queries.push_back(query_text);
		}
	}

	return queries;
}

Query MySQLService::BuildInsert(const Anope::string &table, unsigned int id, Data &data)
{
	/* Empty columns not present in the data set */
	for (const auto &known_col : this->active_schema[table])
	{
		if (data.data.count(known_col) == 0)
			data[known_col] << "";
	}

	Anope::string query_text = "INSERT INTO `" + table + "` (`id`";

	for (const auto &[field, _] : data.data)
		query_text += ",`" + field + "`";
	query_text += ") VALUES (" + Anope::ToString(id);
	for (const auto &[field, _] : data.data)
		query_text += ",@" + field + "@";
	query_text += ") ON DUPLICATE KEY UPDATE ";
	for (const auto &[field, _] : data.data)
		query_text += "`" + field + "`=VALUES(`" + field + "`),";
	query_text.erase(query_text.end() - 1);

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
			case Serialize::DataType::UINT:
			{
				if (buf.empty())
					buf = "DEFAULT";
				escape = false;
				break;
			}

			case Serialize::DataType::TEXT:
			{
				if (buf.empty())
				{
					buf = "DEFAULT";
					escape = false;
				}
				break;
			}
		}

		query.SetValue(field, buf, escape);
	}

	return query;
}

Query MySQLService::GetTables(const Anope::string &prefix)
{
	return Query("SHOW TABLES LIKE '" + prefix + "%';");
}

void MySQLService::Connect()
{
	this->sql = mysql_init(this->sql);

	const unsigned int timeout = 1;
	mysql_options(this->sql, MYSQL_OPT_CONNECT_TIMEOUT, reinterpret_cast<const char *>(&timeout));

	bool connect = mysql_real_connect(this->sql, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->database.c_str(), this->port, this->socket.empty() ? nullptr : this->socket.c_str(), CLIENT_MULTI_RESULTS);

	if (!connect)
		throw SQL::Exception("Unable to connect to MySQL service " + this->name + ": " + mysql_error(this->sql));

	// We force UTC so that FromUnixtime works as expected.
	SQL::Query tzquery("SET time_zone = '+00:00'");
	RunQuery(tzquery);

	if (this->socket.empty())
		Log(LOG_DEBUG) << "Successfully connected to MySQL service " << this->name << " at " << this->server << ":" << this->port;
	else
		Log(LOG_DEBUG) << "Successfully connected to MySQL service " << this->name << " at " << this->socket;
}


bool MySQLService::CheckConnection()
{
	if (!this->sql || mysql_ping(this->sql))
	{
		try
		{
			this->Connect();
		}
		catch (const SQL::Exception &)
		{
			return false;
		}
	}

	return true;
}

Anope::string MySQLService::Escape(const Anope::string &query)
{
	std::vector<char> buffer(query.length() * 2 + 1);
	mysql_real_escape_string(this->sql, &buffer[0], query.c_str(), query.length());
	return &buffer[0];
}

Anope::string MySQLService::BuildQuery(const Query &q)
{
	Anope::string real_query = q.query;

	for (const auto &[name, value] : q.parameters)
		real_query = real_query.replace_all_cs("@" + name + "@", (value.escape ? ("'" + this->Escape(value.data) + "'") : value.data));

	return real_query;
}

Anope::string MySQLService::FromUnixtime(time_t t)
{
	return "FROM_UNIXTIME(" + Anope::ToString(t) + ")";
}

void DispatcherThread::Run()
{
	this->mutex.lock();

	while (!this->GetExitState())
	{
		if (!me->QueryRequests.empty())
		{
			QueryRequest &r = me->QueryRequests.front();
			this->mutex.unlock();

			Result sresult = r.service->RunQuery(r.query);

			this->mutex.lock();
			if (!me->QueryRequests.empty() && me->QueryRequests.front().query == r.query)
			{
				if (r.sqlinterface)
					me->FinishedRequests.push_back(QueryResult(r.sqlinterface, sresult));
				me->QueryRequests.pop_front();
			}
		}
		else
		{
			if (!me->FinishedRequests.empty())
				me->Notify();
			this->condvar.wait(this->mutex);
		}
	}

	this->mutex.unlock();
}

MODULE_INIT(ModuleSQL)
