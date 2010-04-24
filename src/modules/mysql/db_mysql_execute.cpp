/* RequiredLibraries: mysqlpp */

#include "db_mysql.h"
#define HASH(nick)      (((nick)[0]&31)<<5 | ((nick)[1]&31))

class FakeNickCore : public NickCore
{
 public:
 	FakeNickCore() : NickCore("-SQLUser")
	{
		if (this->next)
			this->next->prev = this->prev;
		if (this->prev)
			this->prev->next = this->next;
		else
			nclists[HASH(this->display)] = this->next;
	}

	~FakeNickCore()
	{
		insert_core(this);
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
		this->server = serv_uplink; // XXX Need a good way to set this to ourself

		if (this->prev)
			this->prev->next = this->next;
		else
			userlist[HASH(this->nick)] = this->next;
		if (this->next)
			this->next->prev = this->prev;
		--usercnt;
	}

	~FakeUser()
	{
		this->server = serv_uplink; // XXX Need a good way to set this to ourself

		User **list = &userlist[HASH(this->nick)];
		this->next = *list;
		if (*list)
			(*list)->prev = this;
		*list = this;
		++usercnt;
		nc = NULL;
	}

	void SetNewNick(const std::string &newnick) { this->nick = newnick; }

	void SendMessage(const std::string &, const char *, ...) { }
	void SendMessage(const std::string &, const std::string &) { }

	NickCore *Account() const { return nc; }
	const bool IsIdentified() const { return nc ? true : false; }
} SQLUser;

class SQLTimer : public Timer
{
 public:
	SQLTimer() : Timer(Me->Delay, time(NULL), true)
	{
		mysqlpp::Query query(Me->Con);
		query << "TRUNCATE TABLE `anope_commands`";
		ExecuteQuery(query);
	}

	void Tick(time_t)
	{
		mysqlpp::Query query(Me->Con);
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

				// XXX this whole strtok thing needs to die
				char *cmdbuf = sstrdup(qres[i]["command"].c_str());
				char *cmd = strtok(cmdbuf, " ");
				mod_run_cmd(bi->nick, u, bi->cmdTable, cmd);
				delete [] cmdbuf;

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
		TimerManager::DelTimer(_SQLTimer);
	}
};

MODULE_INIT(DBMySQLExecute)
