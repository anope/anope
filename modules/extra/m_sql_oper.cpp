#include "module.h"
#include "sql.h"

class SQLOperResult : public SQL::Interface
{
	Reference<User> user;

	struct SQLOperResultDeleter
	{
		SQLOperResult *res;
		SQLOperResultDeleter(SQLOperResult *r) : res(r) { }
		~SQLOperResultDeleter() { delete res; }
	};

 public:
	SQLOperResult(Module *m, User *u) : SQL::Interface(m), user(u) { }

	void OnResult(const SQL::Result &r) anope_override
	{
		SQLOperResultDeleter d(this);

		if (!user || !user->Account() || r.Rows() == 0)
			return;

		Anope::string opertype;
		try
		{
			opertype = r.Get(0, "opertype");
		}
		catch (const SQL::Exception &)
		{
			return;
		}

		Log(LOG_DEBUG) << "m_sql_oper: Got result for " << user->nick << ", opertype " << opertype;

		Anope::string modes;
		try
		{
			modes = r.Get(0, "modes");
		}
		catch (const SQL::Exception &) { }

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
				user->RemoveMode(OperServ, "OPER"); // Probably not set, just incase
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

		if (!user->HasMode("OPER"))
		{
			IRCD->SendOper(user);

			if (!modes.empty())
				user->SetModes(OperServ, "%s", modes.c_str());
		}
	}

	void OnError(const SQL::Result &r) anope_override
	{
		SQLOperResultDeleter d(this);
		Log(this->owner) << "m_sql_oper: Error executing query " << r.GetQuery().query << ": " << r.GetError();
	}
};

class ModuleSQLOper : public Module
{
	Anope::string engine;
	Anope::string query;

	ServiceReference<SQL::Provider> SQL;

 public:
	ModuleSQLOper(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{

		Implementation i[] = { I_OnReload, I_OnNickIdentify };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->engine = config.ReadValue("m_sql_oper", "engine", "", 0);
		this->query = config.ReadValue("m_sql_oper", "query", "", 0);

		this->SQL = ServiceReference<SQL::Provider>("SQL::Provider", this->engine);
	}

	void OnNickIdentify(User *u) anope_override
	{
		if (!this->SQL)
		{
			Log() << "Unable to find SQL engine";
			return;
		}

		SQL::Query q(this->query);
		q.SetValue("a", u->Account()->display);
		q.SetValue("i", u->ip);

		this->SQL->Run(new SQLOperResult(this, u), q);

		Log(LOG_DEBUG) << "m_sql_oper: Checking authentication for " << u->Account()->display;
	}
};

MODULE_INIT(ModuleSQLOper)
