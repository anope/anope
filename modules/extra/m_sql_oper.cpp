#include "module.h"
#include "sql.h"

class SQLOperResult : public SQLInterface
{
	dynamic_reference<User> user;

	struct SQLOperResultDeleter
	{
		SQLOperResult *res;
		SQLOperResultDeleter(SQLOperResult *r) : res(r) { }
		~SQLOperResultDeleter() { delete res; }
	};

 public:
	SQLOperResult(Module *m, User *u) : SQLInterface(m), user(u) { }

	void OnResult(const SQLResult &r) anope_override
	{
		SQLOperResultDeleter d(this);

		if (!user || !user->Account() || r.Rows() == 0)
			return;

		Anope::string opertype;
		try
		{
			opertype = r.Get(0, "opertype");
		}
		catch (const SQLException &)
		{
			return;
		}

		Log(LOG_DEBUG) << "m_sql_oper: Got result for " << user->nick << ", opertype " << opertype;

		Anope::string modes;
		try
		{
			modes = r.Get(0, "modes");
		}
		catch (const SQLException &) { }

		if (opertype.empty())
		{
			if (user->Account() && user->Account()->o && !user->Account()->o->config)
			{
				std::vector<Oper *>::iterator it = std::find(Config->Opers.begin(), Config->Opers.end(), user->Account()->o);
				if (it != Config->Opers.end())
					Config->Opers.erase(it);
				delete user->Account()->o;
				user->Account()->o = NULL;
				Log(this->owner) << "m_sql_oper: Removed services operator from " << user->nick << " (" << user->Account()->display << ")";
				user->RemoveMode(findbot(Config->OperServ), UMODE_OPER); // Probably not set, just incase
			}
			return;
		}

		OperType *ot = OperType::Find(opertype);
		if (ot == NULL)
		{
			Log(this->owner) << "m_sql_oper: Oper " << user->nick << " has type " << opertype << ", but this opertype does not exist?";
			return;
		}

		if (!user->Account()->o || user->Account()->o->ot != ot)
		{
			Log(this->owner) << "m_sql_oper: Tieing oper " << user->nick << " to type " << opertype;
			user->Account()->o = new Oper(user->Account()->display, ot);
			Config->Opers.push_back(user->Account()->o);
		}

		if (!user->HasMode(UMODE_OPER))
		{
			ircdproto->SendOper(user);

			if (!modes.empty())
				user->SetModes(findbot(Config->OperServ), "%s", modes.c_str());
		}
	}

	void OnError(const SQLResult &r) anope_override
	{
		SQLOperResultDeleter d(this);
		Log(this->owner) << "m_sql_oper: Error executing query " << r.GetQuery().query << ": " << r.GetError();
	}
};

class ModuleSQLOper : public Module
{
	Anope::string engine;
	Anope::string query;

	service_reference<SQLProvider> SQL;

 public:
	ModuleSQLOper(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNickIdentify };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->engine = config.ReadValue("m_sql_oper", "engine", "", 0);
		this->query = config.ReadValue("m_sql_oper", "query", "", 0);

		this->SQL = service_reference<SQLProvider>("SQLProvider", this->engine);
	}

	void OnNickIdentify(User *u) anope_override
	{
		if (!this->SQL)
		{
			Log() << "Unable to find SQL engine";
			return;
		}

		SQLQuery q(this->query);
		q.setValue("a", u->Account()->display);
		q.setValue("i", u->ip);

		this->SQL->Run(new SQLOperResult(this, u), q);

		Log(LOG_DEBUG) << "m_sql_oper: Checking authentication for " << u->Account()->display;
	}
};

MODULE_INIT(ModuleSQLOper)
