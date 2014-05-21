#include "module.h"
#include "modules/sql.h"

struct SQLOper : Oper
{
	SQLOper(const Anope::string &n, OperType *o) : Oper(n, o) { }
};

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

		BotInfo *OperServ = Config->GetClient("OperServ");
		if (opertype.empty())
		{
			if (user->Account() && user->Account()->o && dynamic_cast<SQLOper *>(user->Account()->o))
			{
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
			user->Account()->o = new SQLOper(user->Account()->display, ot);
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
	}

	~ModuleSQLOper()
	{
		for (nickcore_map::const_iterator it = NickCoreList->begin(), it_end = NickCoreList->end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (nc->o && dynamic_cast<SQLOper *>(nc->o))
			{
				delete nc->o;
				nc->o = NULL;
			}
		}
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		Configuration::Block *config = conf->GetModule(this);

		this->engine = config->Get<const Anope::string>("engine");
		this->query = config->Get<const Anope::string>("query");

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
		q.SetValue("i", u->ip.addr());

		this->SQL->Run(new SQLOperResult(this, u), q);

		Log(LOG_DEBUG) << "m_sql_oper: Checking authentication for " << u->Account()->display;
	}
};

MODULE_INIT(ModuleSQLOper)
