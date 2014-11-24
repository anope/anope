/* RequiredLibraries: mysqlclient */
/* RequiredWindowsLibraries: libmysql */

#include "module.h"
#include "modules/sql.h"
#define NO_CLIENT_LONG_LONG
#ifdef WIN32
# include <mysql.h>
#else
# include <mysql/mysql.h>
#endif

using namespace SQL;

/** Non blocking threaded MySQL API, based loosely from InspIRCd's m_mysql.cpp
 *
 * This module spawns a single thread that is used to execute blocking MySQL queries.
 * When a module requests a query to be executed it is added to a list for the thread
 * (which never stops looping and sleeing) to pick up and execute, the result of which
 * is inserted in to another queue to be picked up by the main thread. The main thread
 * uses Pipe to become notified through the socket engine when there are results waiting
 * to be sent back to the modules requesting the query
 */

class MySQLService;

/** A query request
 */
struct QueryRequest
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
struct QueryResult
{
	/* The interface to send the data back on */
	Interface *sqlinterface;
	/* The result */
	Result result;

	QueryResult(Interface *i, Result &r) : sqlinterface(i), result(r) { }
};

/** A MySQL result
 */
class MySQLResult : public Result
{
	MYSQL_RES *res;

 public:
	MySQLResult(unsigned int i, const Query &q, const Anope::string &fq, MYSQL_RES *r) : Result(i, q, fq), res(r)
	{
		if (!res)
			return;

		unsigned num_fields = mysql_num_fields(res);
		MYSQL_FIELD *fields = mysql_fetch_fields(res);

		/* It is not thread safe to log anything here using Log(this->owner) now :( */

		if (!num_fields || !fields)
			return;

		for (unsigned field_count = 0; field_count < num_fields; ++field_count)
			columns.push_back(fields[field_count].name ? fields[field_count].name : "");

		for (MYSQL_ROW row; (row = mysql_fetch_row(res));)
		{
			std::vector<Value> values;

			for (unsigned field_count = 0; field_count < num_fields; ++field_count)
			{
				const char *data = row[field_count];

				Value v;
				v.null = !data;
				v.value = data ? data : "";
				values.push_back(v);
			}

			this->values.push_back(values);
		}
	}

	MySQLResult(const Query &q, const Anope::string &fq, const Anope::string &err) : Result(0, q, fq, err), res(NULL)
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
class MySQLService : public Provider
{
	std::map<Anope::string, std::set<Anope::string> > active_schema, indexes;

	Anope::string database;
	Anope::string server;
	Anope::string user;
	Anope::string password;
	int port;

	MYSQL *sql;

	/** Escape a query.
	 * Note the mutex must be held!
	 */
	Anope::string Escape(const Anope::string &query);

 public:
	/* Locked by the SQL thread when a query is pending on this database,
	 * prevents us from deleting a connection while a query is executing
	 * in the thread
	 */
	Mutex Lock;

	MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, int po);

	~MySQLService();

	void Run(Interface *i, const Query &query) override;

	Result RunQuery(const Query &query) override;

	std::vector<Query> InitSchema(const Anope::string &prefix) override;
	std::vector<Query> Replace(const Anope::string &table, const Query &, const std::set<Anope::string> &) override;
	std::vector<Query> CreateTable(const Anope::string &prefix, const Anope::string &table) override;
	std::vector<Query> AlterTable(const Anope::string &, const Anope::string &table, const Anope::string &field, bool) override;
	std::vector<Query> CreateIndex(const Anope::string &table, const Anope::string &field) override;

	Query BeginTransaction() override;
	Query Commit() override;

	Serialize::ID GetID(const Anope::string &) override;

	Query GetTables(const Anope::string &prefix) override;

	void Connect();

	bool CheckConnection();

	Anope::string BuildQuery(const Query &q);
};

/** The SQL thread used to execute queries
 */
class DispatcherThread : public Thread, public Condition
{
 public:
	DispatcherThread() : Thread() { }

	void Run() override;
};

class ModuleSQL;
static ModuleSQL *me;
class ModuleSQL : public Module
	, public Pipe
	, public EventHook<Event::ModuleUnload>
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
		, EventHook<Event::ModuleUnload>("OnModuleUnload")
	{
		me = this;


		DThread = new DispatcherThread();
		DThread->Start();
	}

	~ModuleSQL()
	{
		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end(); ++it)
			delete it->second;
		MySQLServices.clear();

		DThread->SetExitState();
		DThread->Wakeup();
		DThread->Join();
		delete DThread;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *config = conf->GetModule(this);

		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end();)
		{
			const Anope::string &cname = it->first;
			MySQLService *s = it->second;
			int i;

			++it;

			for (i = 0; i < config->CountBlock("mysql"); ++i)
				if (config->GetBlock("mysql", i)->Get<const Anope::string>("name", "mysql/main") == cname)
					break;

			if (i == config->CountBlock("mysql"))
			{
				Log(LOG_NORMAL, "mysql") << "MySQL: Removing server connection " << cname;

				delete s;
				this->MySQLServices.erase(cname);
			}
		}

		for (int i = 0; i < config->CountBlock("mysql"); ++i)
		{
			Configuration::Block *block = config->GetBlock("mysql", i);
			const Anope::string &connname = block->Get<const Anope::string>("name", "mysql/main");

			if (this->MySQLServices.find(connname) == this->MySQLServices.end())
			{
				const Anope::string &database = block->Get<const Anope::string>("database", "anope");
				const Anope::string &server = block->Get<const Anope::string>("server", "127.0.0.1");
				const Anope::string &user = block->Get<const Anope::string>("username", "anope");
				const Anope::string &password = block->Get<const Anope::string>("password");
				int port = block->Get<int>("port", "3306");

				try
				{
					MySQLService *ss = new MySQLService(this, connname, database, server, user, password, port);
					this->MySQLServices.insert(std::make_pair(connname, ss));

					Log(LOG_NORMAL, "mysql") << "MySQL: Successfully connected to server " << connname << " (" << server << ")";
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
		this->DThread->Lock();

		for (unsigned i = this->QueryRequests.size(); i > 0; --i)
		{
			QueryRequest &r = this->QueryRequests[i - 1];

			if (r.sqlinterface && r.sqlinterface->owner == m)
			{
				if (i == 1)
				{
					r.service->Lock.Lock();
					r.service->Lock.Unlock();
				}

				this->QueryRequests.erase(this->QueryRequests.begin() + i - 1);
			}
		}

		this->DThread->Unlock();

		this->OnNotify();
	}

	void OnNotify() override
	{
		this->DThread->Lock();
		std::deque<QueryResult> finishedRequests = this->FinishedRequests;
		this->FinishedRequests.clear();
		this->DThread->Unlock();

		for (std::deque<QueryResult>::const_iterator it = finishedRequests.begin(), it_end = finishedRequests.end(); it != it_end; ++it)
		{
			const QueryResult &qr = *it;

			if (!qr.sqlinterface)
				throw SQL::Exception("NULL qr.sqlinterface in MySQLPipe::OnNotify() ?");

			if (qr.result.GetError().empty())
				qr.sqlinterface->OnResult(qr.result);
			else
				qr.sqlinterface->OnError(qr.result);
		}
	}
};

MySQLService::MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, int po)
: Provider(o, n), database(d), server(s), user(u), password(p), port(po), sql(NULL)
{
	Connect();
}

MySQLService::~MySQLService()
{
	me->DThread->Lock();
	this->Lock.Lock();
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
	this->Lock.Unlock();
	me->DThread->Unlock();
}

void MySQLService::Run(Interface *i, const Query &query)
{
	me->DThread->Lock();
	me->QueryRequests.push_back(QueryRequest(this, i, query));
	me->DThread->Unlock();
	me->DThread->Wakeup();
}

Result MySQLService::RunQuery(const Query &query)
{
	this->Lock.Lock();

	Anope::string real_query = this->BuildQuery(query);

	if (this->CheckConnection() && !mysql_real_query(this->sql, real_query.c_str(), real_query.length()))
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

		this->Lock.Unlock();
		return MySQLResult(id, query, real_query, res);
	}
	else
	{
		Anope::string error = mysql_error(this->sql);
		this->Lock.Unlock();
		return MySQLResult(query, real_query, error);
	}
}

std::vector<Query> MySQLService::InitSchema(const Anope::string &prefix)
{
	std::vector<Query> queries;

	Query t = "CREATE TABLE IF NOT EXISTS `" + prefix + "id` ("
		"`id` bigint(20) NOT NULL"
		") ENGINE=InnoDB";
	queries.push_back(t);

	t = "CREATE TABLE IF NOT EXISTS `" + prefix + "objects` (`id` bigint(20) NOT NULL PRIMARY KEY, `type` varchar(256)) ENGINE=InnoDB";
	queries.push_back(t);

	t = "CREATE TABLE IF NOT EXISTS `" + prefix + "edges` ("
		"`id` bigint(20) NOT NULL,"
		"`field` varchar(64) NOT NULL,"
		"`other_id` bigint(20) NOT NULL,"
		"PRIMARY KEY (`id`, `field`),"
		"KEY `other` (`other_id`),"
		"CONSTRAINT `edges_id_fk` FOREIGN KEY (`id`) REFERENCES `" + prefix + "objects` (`id`),"
		"CONSTRAINT `edges_other_id_fk` FOREIGN KEY (`other_id`) REFERENCES `" + prefix + "objects` (`id`)"
		") ENGINE=InnoDB";
	queries.push_back(t);

	return queries;
}

std::vector<Query> MySQLService::Replace(const Anope::string &table, const Query &q, const std::set<Anope::string> &keys)
{
	std::vector<Query> queries;

	Anope::string query_text = "INSERT INTO `" + table + "` (";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		query_text += "`" + p.first + "`,";
	query_text.erase(query_text.length() - 1);
	query_text += ") VALUES (";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		query_text += "@" + p.first + "@,";
	query_text.erase(query_text.length() - 1);
	query_text += ") ON DUPLICATE KEY UPDATE ";
	for (const std::pair<Anope::string, QueryData> &p : q.parameters)
		if (!keys.count(p.first))
			query_text += "`" + p.first + "` = VALUES(`" + p.first + "`),";
	query_text.erase(query_text.length() - 1);

	Query query(query_text);
	query.parameters = q.parameters;

	queries.push_back(query);

	return queries;
}

std::vector<Query> MySQLService::CreateTable(const Anope::string &prefix, const Anope::string &table)
{
	std::vector<Query> queries;

	if (active_schema.find(prefix + table) == active_schema.end())
	{
		Query t = "CREATE TABLE IF NOT EXISTS `" + prefix + table + "` (`id` bigint(20) NOT NULL, PRIMARY KEY (`id`)) ENGINE=InnoDB";
		queries.push_back(t);

		t = "ALTER TABLE `" + prefix + table + "` "
			"ADD CONSTRAINT `" + table + "_id_fk` FOREIGN KEY (`id`) REFERENCES `" + prefix + "objects` (`id`)";
		queries.push_back(t);

		active_schema[prefix + table];
	}

	return queries;
}

std::vector<Query> MySQLService::AlterTable(const Anope::string &prefix, const Anope::string &table, const Anope::string &field, bool object)
{
	std::vector<Query> queries;
	std::set<Anope::string> &s = active_schema[prefix + table];

	if (!s.count(field))
	{
		Query column;
		if (!object)
			column = "ALTER TABLE `" + prefix + table + "` ADD COLUMN `" + field + "` TINYTEXT";
		else
			column = "ALTER TABLE `" + prefix + table + "` "
				"ADD COLUMN `" + field + "` bigint(20), "
				"ADD CONSTRAINT `" + table + "_" + field + "_fk` FOREIGN KEY (`" + field + "`) REFERENCES `" + prefix + "objects` (`id`)";
		queries.push_back(column);
		s.insert(field);
	}

	return queries;
}

std::vector<Query> MySQLService::CreateIndex(const Anope::string &table, const Anope::string &field)
{
	std::vector<Query> queries;

	if (indexes[table].count(field))
		return queries;

	Query t = "ALTER TABLE `" + table + "` ADD KEY `idx_" + field + "` (`" + field + "`(512))";
	queries.push_back(t);

	indexes[table].insert(field);

	return queries;
}

Query MySQLService::BeginTransaction()
{
	return Query("START TRANSACTION WITH CONSISTENT SNAPSHOT");
}

Query MySQLService::Commit()
{
	return Query("COMMIT");
}

Serialize::ID MySQLService::GetID(const Anope::string &prefix)
{
	Query query("SELECT `id` FROM `" + prefix + "id` FOR UPDATE");
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

Query MySQLService::GetTables(const Anope::string &prefix)
{
	return Query("SHOW TABLES LIKE '" + prefix + "%';");
}

void MySQLService::Connect()
{
	this->sql = mysql_init(this->sql);

	const unsigned int timeout = 1;
	mysql_options(this->sql, MYSQL_OPT_CONNECT_TIMEOUT, reinterpret_cast<const char *>(&timeout));

	bool connect = mysql_real_connect(this->sql, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->database.c_str(), this->port, NULL, CLIENT_MULTI_RESULTS);

	if (!connect)
		throw SQL::Exception("Unable to connect to MySQL service " + this->name + ": " + mysql_error(this->sql));

	Log(LOG_DEBUG) << "Successfully connected to MySQL service " << this->name << " at " << this->server << ":" << this->port;
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

void DispatcherThread::Run()
{
	this->Lock();

	while (!this->GetExitState())
	{
		if (!me->QueryRequests.empty())
		{
			QueryRequest &r = me->QueryRequests.front();
			this->Unlock();

			Result sresult = r.service->RunQuery(r.query);

			this->Lock();
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
			this->Wait();
		}
	}

	this->Unlock();
}

MODULE_INIT(ModuleSQL)

