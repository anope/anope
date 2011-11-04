/* RequiredLibraries: mysqlclient */

#include "module.h"
#define NO_CLIENT_LONG_LONG
#include <mysql/mysql.h>
#include "sql.h"

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
	SQLInterface *sqlinterface;
	/* The actual query */
	SQLQuery query;

	QueryRequest(MySQLService *s, SQLInterface *i, const SQLQuery &q) : service(s), sqlinterface(i), query(q) { }
};

/** A query result */
struct QueryResult
{
	/* The interface to send the data back on */
	SQLInterface *sqlinterface;
	/* The result */
	SQLResult result;

	QueryResult(SQLInterface *i, SQLResult &r) : sqlinterface(i), result(r) { }
};

/** A MySQL result
 */
class MySQLResult : public SQLResult
{
	MYSQL_RES *res;

 public:
	MySQLResult(const SQLQuery &q, const Anope::string &fq, MYSQL_RES *r) : SQLResult(q, fq), res(r)
	{
		unsigned num_fields = res ? mysql_num_fields(res) : 0;

		Log(LOG_DEBUG) << "SQL query " << this->finished_query << " returned " << num_fields << " fields";

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
					Log(LOG_DEBUG) << "Field count " << field_count << " name is: " << (fields[field_count].name ? fields[field_count].name : "") << ", data is: " << (row[field_count] ? row[field_count] : "");
					Anope::string column = (fields[field_count].name ? fields[field_count].name : "");
					Anope::string data = (row[field_count] ? row[field_count] : "");

					items[column] = data;
				}

				this->entries.push_back(items);
			}
		}
	}

	MySQLResult(const SQLQuery &q, const Anope::string &fq, const Anope::string &err) : SQLResult(q, fq, err), res(NULL)
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
class MySQLService : public SQLProvider
{
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

	void Run(SQLInterface *i, const SQLQuery &query);

	SQLResult RunQuery(const SQLQuery &query);

	SQLQuery CreateTable(const Anope::string &table, const SerializableBase::serialized_data &data);

	SQLQuery GetTables();

	void Connect();

	bool CheckConnection();

	Anope::string BuildQuery(const SQLQuery &q);
};

/** The SQL thread used to execute queries
 */
class DispatcherThread : public Thread, public Condition
{
 public:
	DispatcherThread() : Thread() { }

	void Run();
};

class ModuleSQL;
static ModuleSQL *me;
class ModuleSQL : public Module, public Pipe
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

	ModuleSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		me = this;

		Implementation i[] = { I_OnReload, I_OnModuleUnload };
		ModuleManager::Attach(i, this,  2);

		DThread = new DispatcherThread();
		DThread->Start();

		OnReload();
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

	void OnReload()
	{
		ConfigReader config;
		int i, num;

		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end();)
		{
			const Anope::string &cname = it->first;
			MySQLService *s = it->second;
			++it;

			for (i = 0, num = config.Enumerate("mysql"); i < num; ++i)
			{
				if (config.ReadValue("mysql", "name", "main", i) == cname)
				{
					break;
				}
			}

			if (i == num)
			{
				Log(LOG_NORMAL, "mysql") << "MySQL: Removing server connection " << cname;

				delete s;
				this->MySQLServices.erase(cname);
			}
		}

		for (i = 0, num = config.Enumerate("mysql"); i < num; ++i)
		{
			Anope::string connname = config.ReadValue("mysql", "name", "mysql/main", i);

			if (this->MySQLServices.find(connname) == this->MySQLServices.end())
			{
				Anope::string database = config.ReadValue("mysql", "database", "anope", i);
				Anope::string server = config.ReadValue("mysql", "server", "127.0.0.1", i);
				Anope::string user = config.ReadValue("mysql", "username", "anope", i);
				Anope::string password = config.ReadValue("mysql", "password", "", i);
				int port = config.ReadInteger("mysql", "port", "3306", i, true);

				try
				{
					MySQLService *ss = new MySQLService(this, connname, database, server, user, password, port);
					this->MySQLServices.insert(std::make_pair(connname, ss));

					Log(LOG_NORMAL, "mysql") << "MySQL: Successfully connected to server " << connname << " (" << server << ")";
				}
				catch (const SQLException &ex)
				{
					Log(LOG_NORMAL, "mysql") << "MySQL: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m)
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

	void OnNotify()
	{
		this->DThread->Lock();
		std::deque<QueryResult> finishedRequests = this->FinishedRequests;
		this->FinishedRequests.clear();
		this->DThread->Unlock();

		for (std::deque<QueryResult>::const_iterator it = finishedRequests.begin(), it_end = finishedRequests.end(); it != it_end; ++it)
		{
			const QueryResult &qr = *it;

			if (!qr.sqlinterface)
				throw SQLException("NULL qr.sqlinterface in MySQLPipe::OnNotify() ?");

			if (qr.result.GetError().empty())
				qr.sqlinterface->OnResult(qr.result);
			else
				qr.sqlinterface->OnError(qr.result);
		}
	}
};

MySQLService::MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, int po)
: SQLProvider(o, n), database(d), server(s), user(u), password(p), port(po), sql(NULL)
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
				r.sqlinterface->OnError(SQLResult(r.query, "SQL Interface is going away"));
			me->QueryRequests.erase(me->QueryRequests.begin() + i - 1);
		}
	}
	this->Lock.Unlock();
	me->DThread->Unlock();
}

void MySQLService::Run(SQLInterface *i, const SQLQuery &query)
{
	me->DThread->Lock();
	me->QueryRequests.push_back(QueryRequest(this, i, query));
	me->DThread->Unlock();
	me->DThread->Wakeup();
}

SQLResult MySQLService::RunQuery(const SQLQuery &query)
{
	this->Lock.Lock();

	Anope::string real_query = this->BuildQuery(query);

	if (this->CheckConnection() && !mysql_real_query(this->sql, real_query.c_str(), real_query.length()))
	{
		MYSQL_RES *res = mysql_use_result(this->sql);

		this->Lock.Unlock();
		return MySQLResult(query, real_query, res);
	}
	else
	{
		Anope::string error = mysql_error(this->sql);
		this->Lock.Unlock();
		return MySQLResult(query, real_query, error);
	}
}

SQLQuery MySQLService::CreateTable(const Anope::string &table, const SerializableBase::serialized_data &data)
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
				key_buf = "UNIQUE KEY `ukey` (";
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

SQLQuery MySQLService::GetTables()
{
	return SQLQuery("SHOW TABLES");
}

void MySQLService::Connect()
{
	this->sql = mysql_init(this->sql);

	const unsigned int timeout = 1;
	mysql_options(this->sql, MYSQL_OPT_CONNECT_TIMEOUT, reinterpret_cast<const char *>(&timeout));

	bool connect = mysql_real_connect(this->sql, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->database.c_str(), this->port, NULL, 0);

	if (!connect)
		throw SQLException("Unable to connect to SQL service " + this->name + ": " + mysql_error(this->sql));
}


bool MySQLService::CheckConnection()
{
	if (!this->sql || mysql_ping(this->sql))
	{
		try
		{
			this->Connect();
		}
		catch (const SQLException &)
		{
			return false;
		}
	}

	return true;
}

Anope::string MySQLService::Escape(const Anope::string &query)
{
	char buffer[BUFSIZE];
	mysql_real_escape_string(this->sql, buffer, query.c_str(), query.length());
	return buffer;
}

Anope::string MySQLService::BuildQuery(const SQLQuery &q)
{
	Anope::string real_query = q.query;

	for (std::map<Anope::string, Anope::string>::const_iterator it = q.parameters.begin(), it_end = q.parameters.end(); it != it_end; ++it)
		real_query = real_query.replace_all_cs("@" + it->first + "@", "'" + this->Escape(it->second) + "'");

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

			SQLResult sresult = r.service->RunQuery(r.query);

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

