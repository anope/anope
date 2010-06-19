/* RequiredLibraries: mysqlpp */

#include "db_mysql.h"

class FakeNickCore : public NickCore
{
 public:
 	FakeNickCore() : NickCore("-SQLUser")
	{
		NickCoreList.erase(this->display);
	}

	~FakeNickCore()
	{
		NickCoreList[this->display] = this;
		Users.clear();
	}

 	bool IsServicesOper() const { return true; }
	bool HasCommand(const std::string &) const { return true; }
	bool HasPriv(const std::string &) const { return true; }
} SQLCore;

class FakeUser : public User
{
 public:
 	FakeUser() : User("-SQLUser", "")
	{
		this->SetIdent("SQL");
		this->host = sstrdup(Config.ServerName);
		this->realname = sstrdup("Fake SQL User");
		this->hostip = sstrdup("255.255.255.255");
		this->vhost = NULL;
		this->server = Me;

		UserListByNick.erase("-SQLUser");
		--usercnt;
	}

	~FakeUser()
	{
		UserListByNick["-SQLUser"] = this;
		++usercnt;

		nc = NULL;
	}

	void SetNewNick(const std::string &newnick) { this->nick = newnick; }

	void SendMessage(const std::string &, const char *, ...) { }
	void SendMessage(const std::string &, const std::string &) { }

	NickCore *Account() const { return nc; }
	const bool IsIdentified(bool) const { return nc ? true : false; }
	const bool IsRecognized(bool) const { return true; }
} SQLUser;

class SQLTimer : public Timer
{
 public:
	SQLTimer() : Timer(me->Delay, time(NULL), true)
	{
		mysqlpp::Query query(me->Con);
		query << "TRUNCATE TABLE `anope_commands`";
		ExecuteQuery(query);
	}

	void Tick(time_t)
	{
		mysqlpp::Query query(me->Con);
		mysqlpp::StoreQueryResult qres;

		query << "SELECT * FROM `anope_commands`";
		qres = StoreQuery(query);

		if (qres && qres.num_rows())
		{
			for (size_t i = 0; i < qres.num_rows(); ++i)
			{
				User *u;
				NickAlias *na = NULL;
				bool logout = false;

				/* If they want -SQLUser to execute the command, use it */
				if (qres[i]["nick"] == "-SQLUser")
				{
					u = &SQLUser;
					u->SetNewNick("-SQLUser");
					u->Login(&SQLCore);
					logout = true;
				}
				else
				{
					/* See if the nick they want to execute the command is registered */
					na = findnick(SQLAssign(qres[i]["nick"]));
					if (na)
					{
						/* If it is and someone is online using that nick, use them */
						if (!na->nc->Users.empty())
							u = na->nc->Users.front();
						/* Make a fake nick and use that logged in as the nick we want to use */
						else
						{
							u = &SQLUser;
							u->SetNewNick(SQLAssign(qres[i]["nick"]));
							u->Login(na->nc);
							logout = true;
						}
					}
					else
					{
						/* Check if someone is online using the nick now */
						u = finduser(SQLAssign(qres[i]["nick"]));
						/* If they arent make a fake user, and use them */
						if (!u)
						{
							u = &SQLUser;
							u->SetNewNick(SQLAssign(qres[i]["nick"]));
							u->Logout();
							logout = true;
						}
					}
				}

				BotInfo *bi = findbot(SQLAssign(qres[i]["service"]));
				if (!bi)
				{
					Alog() << "Warning: SQL command for unknown service " << qres[i]["service"];
					continue;
				}

				mod_run_cmd(bi, u, qres[i]["command"].c_str());

				if (logout)
					u->Logout();
			}

			query << "TRUNCATE TABLE `anope_commands`";
			ExecuteQuery(query);
		}
	}
};

class DBMySQLExecute : public DBMySQL
{
	SQLTimer *_SQLTimer;
 public:
	DBMySQLExecute(const std::string &modname, const std::string &creator) : DBMySQL(modname, creator)
	{
		_SQLTimer = new SQLTimer();
	}
	
	~DBMySQLExecute()
	{
		delete _SQLTimer;
	}
};

MODULE_INIT(DBMySQLExecute)
