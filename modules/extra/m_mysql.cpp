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
 * is inserted in to another queue to be picked up my the main thread. The main thread
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
	SQLInterface *interface;
	/* The actual query */
	Anope::string query;

	QueryRequest(MySQLService *s, SQLInterface *i, const Anope::string &q) : service(s), interface(i), query(q) { }
};

struct QueryResult
{
	/* The interface to send the data back on */
	SQLInterface *interface;
	/* The result */
	SQLResult result;

	QueryResult(SQLInterface *i, SQLResult &r) : interface(i), result(r) { }
};

/** A MySQL result
 */
class MySQLResult : public SQLResult
{
	MYSQL_RES *res;

 public:
	MySQLResult(const Anope::string &q, MYSQL_RES *r) : SQLResult(q), res(r)
	{
		if (!res)
			return;

		unsigned num_fields = mysql_num_fields(res);

		if (!num_fields)
			return;

		Alog(LOG_DEBUG) << "SQL query returned " << num_fields << " fields";

		for (MYSQL_ROW row; (row = mysql_fetch_row(res));)
		{
			MYSQL_FIELD *fields = mysql_fetch_fields(res);

			if (fields)
			{
				std::map<Anope::string, Anope::string> items;

				for (unsigned field_count = 0; field_count < num_fields; ++field_count)
				{
					Alog(LOG_DEBUG) << "Field count " << field_count << " name is: " << (fields[field_count].name ? fields[field_count].name : "") << ", data is: " << (row[field_count] ? row[field_count] : "");
					Anope::string column = (fields[field_count].name ? fields[field_count].name : "");
					Anope::string data = (row[field_count] ? row[field_count] : "");

					items[column] = data;
				}

				this->entries.push_back(items);
			}
		}
	}

	MySQLResult(const Anope::string &q, const Anope::string &err) : SQLResult(q, err), res(NULL)
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

 public:
	/* Locked by the SQL thread when a query is pending on this database,
	 * prevents us from deleting a connection while a query is executing
	 * in the thread
	 */
	Mutex Lock;

	MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, int po);

	~MySQLService();

	void Run(SQLInterface *i, const Anope::string &query);

	SQLResult RunQuery(const Anope::string &query);

	const Anope::string Escape(const Anope::string &buf);

	void Connect();

	bool CheckConnection();
};

/** The SQL thread used to execute queries
 */
class DispatcherThread : public Thread, public Condition
{
 public:
	DispatcherThread() : Thread() { }

	void Run();
};

/** The pipe used by the SocketEngine to notify the main thread when
 * we have results from queries
 */
class MySQLPipe : public Pipe
{
 public:
	void OnNotify();
};

class ModuleSQL;
static ModuleSQL *me;
class ModuleSQL : public Module
{
 public:
	/* SQL connections */
	std::map<Anope::string, MySQLService *> MySQLServices;
	/* Pending query requests */
	std::deque<QueryRequest> QueryRequests;
	/* Pending finished requests with results */
	std::deque<QueryResult> FinishedRequests;
	/* The thread used to execute queries */
	DispatcherThread *DThread;
	/* Notify pipe */
	MySQLPipe *SQLPipe;

	ModuleSQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		me = this;

		Implementation i[] = { I_OnReload, I_OnModuleUnload };
		ModuleManager::Attach(i, this,  2);

		SQLPipe = new MySQLPipe();

		DThread = new DispatcherThread();
		threadEngine.Start(DThread);

		OnReload(true);
	}

	~ModuleSQL()
	{
		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end(); ++it)
			delete it->second;
		MySQLServices.clear();

		DThread->SetExitState();
		DThread->Wakeup();
		DThread->Join();

		delete SQLPipe;
	}

	void OnReload(bool startup)
	{
		ConfigReader config;
		int i, num;

		for (std::map<Anope::string, MySQLService *>::iterator it = this->MySQLServices.begin(); it != this->MySQLServices.end();)
		{
			const Anope::string cname = it->first;
			MySQLService *s = it->second;
			++it;

			for (i = 0, num = config.Enumerate("mysql"); i < num; ++i)
			{
				if (config.ReadValue("mysql", "name", "", i) == cname)
				{
					break;
				}
			}

			if (i == num)
			{
				Alog() << "MySQL: Removing server connection " << cname;

				delete s;
				this->MySQLServices.erase(cname);
			}
		}

		for (i = 0, num = config.Enumerate("mysql"); i < num; ++i)
		{
			Anope::string connname = config.ReadValue("mysql", "name", "main", i);

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

					Alog() << "MySQL: Sucessfully connected to server " << connname << " (" << server << ")";
				}
				catch (const SQLException &ex)
				{
					Alog() << "MySQL: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m)
	{
		this->DThread->Lock();

		for (unsigned i = this->QueryRequests.size(); i > 0; --i)
		{
			QueryRequest &r = this->QueryRequests[i];

			if (r.interface && r.interface->owner == m)
			{
				if (i == 0)
				{
					r.service->Lock.Lock();
					r.service->Lock.Unlock();
				}

				this->QueryRequests.erase(this->QueryRequests.begin() + i);
			}
		}

		this->DThread->Unlock();
	}
};

MySQLService::MySQLService(Module *o, const Anope::string &n, const Anope::string &d, const Anope::string &s, const Anope::string &u, const Anope::string &p, int po)
: SQLProvider(o, "mysql/" + n), database(d), server(s), user(u), password(p), port(po), sql(NULL)
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
		QueryRequest &r = me->QueryRequests[i];

		if (r.service == this)
		{
			if (r.interface)
				r.interface->OnError(SQLResult("", "SQL Interface is going away"));
			me->QueryRequests.erase(me->QueryRequests.begin() + i);
		}
	}
	this->Lock.Unlock();
	me->DThread->Unlock();
}

void MySQLService::Run(SQLInterface *i, const Anope::string &query)
{
	me->DThread->Lock();
	me->QueryRequests.push_back(QueryRequest(this, i, query));
	me->DThread->Unlock();
	me->DThread->Wakeup();
}

SQLResult MySQLService::RunQuery(const Anope::string &query)
{
	if (this->CheckConnection() && !mysql_real_query(this->sql, query.c_str(), query.length()))
	{
		MYSQL_RES *res = mysql_use_result(this->sql);

		return MySQLResult(query, res);
	}
	else
	{
		return MySQLResult(query, mysql_error(this->sql));
	}
}

const Anope::string MySQLService::Escape(const Anope::string &buf)
{
	char buffer[BUFSIZE];
	mysql_escape_string(buffer, buf.c_str(), buf.length());
	return buffer;
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

void DispatcherThread::Run()
{
	this->Lock();

	while (!this->GetExitState())
	{
		if (!me->QueryRequests.empty())
		{
			QueryRequest &r = me->QueryRequests.front();
			this->Unlock();

			r.service->Lock.Lock();
			SQLResult sresult = r.service->RunQuery(r.query);
			r.service->Lock.Unlock();

			this->Lock();
			if (me->QueryRequests.front().query == r.query)
			{
				if (r.interface)
					me->FinishedRequests.push_back(QueryResult(r.interface, sresult));
				me->QueryRequests.pop_front();
			}
		}
		else
		{
			if (!me->FinishedRequests.empty())
				me->SQLPipe->Notify();
			this->Wait();
		}
	}

	this->Unlock();
}

void MySQLPipe::OnNotify()
{
	me->DThread->Lock();

	for (std::deque<QueryResult>::const_iterator it = me->FinishedRequests.begin(), it_end = me->FinishedRequests.end(); it != it_end; ++it)
	{
		const QueryResult &qr = *it;

		if (!qr.interface)
			throw SQLException("NULL qr.interface in MySQLPipe::OnNotify() ?");

		if (qr.result.GetError().empty())
			qr.interface->OnResult(qr.result);
		else
			qr.interface->OnError(qr.result);
	}
	me->FinishedRequests.clear();

	me->DThread->Unlock();
}

MODULE_INIT(ModuleSQL)

