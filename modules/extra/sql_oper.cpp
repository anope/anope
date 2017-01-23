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
		Oper *oper = user->Account()->GetOper();
		if (oper != nullptr)
		{
			oper->Delete();

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
			Log(LogType::DEBUG) << "m_sql_oper: Got 0 rows for " << user->nick;
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

		Log(LogType::DEBUG) << "m_sql_oper: Got result for " << user->nick << ", opertype " << opertype;

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

		Oper *oper = user->Account()->GetOper();
		if (oper == nullptr || oper->GetType() != ot)
		{
			Log(this->owner) << "m_sql_oper: Tieing oper " << user->nick << " to type " << opertype;

			if (oper)
				oper->Delete();

			oper = Serialize::New<Oper *>();
			oper->SetName(user->Account()->GetDisplay());
			oper->SetType(ot);

			user->Account()->SetOper(oper);
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

		Log(LogType::DEBUG) << "m_sql_oper: Checking authentication for " << u->Account()->GetDisplay();
	}
};

MODULE_INIT(ModuleSQLOper)
