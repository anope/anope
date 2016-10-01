/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "module.h"
#include "modules/sql.h"

class SQLOperResult : public SQL::Interface
{
	Reference<User> user;

	struct SQLOperResultDeleter
	{
		SQLOperResult *res;
		SQLOperResultDeleter(SQLOperResult *r) : res(r) { }
		~SQLOperResultDeleter() { delete res; }
	};

	void Deoper()
	{
		if (user->Account()->o && user->Account()->o->owner == this->owner)
		{
			user->Account()->o->Delete();
			user->Account()->o = nullptr;

			Log(this->owner) << "Removed services operator from " << user->nick << " (" << user->Account()->GetDisplay() << ")";
			user->RemoveMode(Config->GetClient("OperServ"), "OPER"); // Probably not set, just incase
		}
	}

 public:
	SQLOperResult(Module *m, User *u) : SQL::Interface(m), user(u) { }

	void OnResult(const SQL::Result &r) override
	{
		SQLOperResultDeleter d(this);

		if (!user || !user->Account())
			return;

		if (r.Rows() == 0)
		{
			Log(LOG_DEBUG) << "m_sql_oper: Got 0 rows for " << user->nick;
			Deoper();
			return;
		}

		Anope::string opertype;
		try
		{
			opertype = r.Get(0, "opertype");
		}
		catch (const SQL::Exception &)
		{
			Log(this->owner) << "Expected column named \"opertype\" but one was not found";
			return;
		}

		Log(LOG_DEBUG) << "m_sql_oper: Got result for " << user->nick << ", opertype " << opertype;

		Anope::string modes;
		try
		{
			modes = r.Get(0, "modes");
		}
		catch (const SQL::Exception &) { }

		ServiceBot *OperServ = Config->GetClient("OperServ");
		if (opertype.empty())
		{
			Deoper();
			return;
		}

		OperType *ot = OperType::Find(opertype);
		if (ot == NULL)
		{
			Log(this->owner) << "m_sql_oper: Oper " << user->nick << " has type " << opertype << ", but this opertype does not exist?";
			return;
		}

		if (user->Account()->o && user->Account()->o->owner != this->owner)
		{
			Log(this->owner) << "Oper " << user->Account()->GetDisplay() << " has type " << ot->GetName() << ", but is already configured as an oper of type " << user->Account()->o->GetType()->GetName();
			return;
		}

		if (!user->Account()->o || user->Account()->o->GetType() != ot)
		{
			Log(this->owner) << "m_sql_oper: Tieing oper " << user->nick << " to type " << opertype;

			if (user->Account()->o)
				user->Account()->o->Delete();

			Oper *o = Serialize::New<Oper *>();
			o->owner = this->owner;
			o->SetName(user->Account()->GetDisplay());
			o->SetType(ot);

			user->Account()->o = o;
		}

		if (!user->HasMode("OPER"))
		{
			IRCD->SendOper(user);

			if (!modes.empty())
				user->SetModes(OperServ, "%s", modes.c_str());
		}
	}

	void OnError(const SQL::Result &r) override
	{
		SQLOperResultDeleter d(this);
		Log(this->owner) << "m_sql_oper: Error executing query " << r.GetQuery().query << ": " << r.GetError();
	}
};

class ModuleSQLOper : public Module
       , public EventHook<Event::NickIdentify>
{
	Anope::string engine;
	Anope::string query;

	ServiceReference<SQL::Provider> SQL;

 public:
	ModuleSQLOper(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
	       EventHook<Event::NickIdentify>(this)
	{
	}

	~ModuleSQLOper()
	{
		if (NickServ::service == nullptr)
			return;

		NickServ::nickcore_map& map = NickServ::service->GetAccountMap();
		for (NickServ::nickcore_map::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			NickServ::Account *nc = it->second;

			if (nc->o && nc->o->owner == this)
			{
				nc->o->Delete();
				nc->o = nullptr;
			}
		}
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *config = conf->GetModule(this);

		this->engine = config->Get<Anope::string>("engine");
		this->query = config->Get<Anope::string>("query");

		this->SQL = ServiceReference<SQL::Provider>(engine);
	}

	void OnNickIdentify(User *u) override
	{
		if (!this->SQL)
		{
			Log() << "Unable to find SQL engine: " << engine;
			return;
		}

		SQL::Query q(this->query);
		q.SetValue("a", u->Account()->GetDisplay());
		q.SetValue("i", u->ip.addr());

		this->SQL->Run(new SQLOperResult(this, u), q);

		Log(LOG_DEBUG) << "m_sql_oper: Checking authentication for " << u->Account()->GetDisplay();
	}
};

MODULE_INIT(ModuleSQLOper)
