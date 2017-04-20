/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2017 Anope Team <team@anope.org>
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
#include "modules/ldap.h"

static std::set<Oper *> my_opers;
static Anope::string opertype_attribute;

class IdentifyInterface : public LDAPInterface
{
	Reference<User> u;

 public:
	IdentifyInterface(Module *m, User *user) : LDAPInterface(m), u(user)
	{
	}

	void OnResult(const LDAPResult &r) override
	{
		if (!u || !u->Account())
			return;

		NickServ::Account *nc = u->Account();

		try
		{
			const LDAPAttributes &attr = r.get(0);

			const Anope::string &opertype = attr.get(opertype_attribute);

			OperType *ot = OperType::Find(opertype);
			if (ot != NULL && (nc->o == NULL || ot != nc->o->ot))
			{
				Oper *o = nc->o;
				if (o != NULL && my_opers.count(o) > 0)
				{
					my_opers.erase(o);
					delete o;
				}
				o = new Oper(u->nick, ot);
				my_opers.insert(o);
				nc->o = o;

				Log(this->owner) << "Tied " << u->nick << " (" << nc->GetDisplay() << ") to opertype " << ot->GetName();
			}
		}
		catch (const LDAPException &ex)
		{
			if (nc->o != NULL)
			{
				if (my_opers.count(nc->o) > 0)
				{
					my_opers.erase(nc->o);
					delete nc->o;
				}
				nc->o = NULL;

				Log(this->owner) << "Removed services operator from " << u->nick << " (" << nc->GetDisplay() << ")";
			}
		}
	}

	void OnError(const LDAPResult &r) override
	{
	}

	void OnDelete() anope_override
	{
		delete this;
	}
};

class LDAPOper : public Module
	, public EventHook<Event::NickIdentify>
	, public EventHook<Event::DelCore>
{
	ServiceReference<LDAPProvider> ldap;

	Anope::string binddn;
	Anope::string password;
	Anope::string basedn;
	Anope::string filter;
 public:
	LDAPOper(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, ldap("LDAPProvider", "ldap/main")
	{

	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *config = Config->GetModule(this);

		this->binddn = config->Get<Anope::string>("binddn");
		this->password = config->Get<Anope::string>("password");
		this->basedn = config->Get<Anope::string>("basedn");
		this->filter = config->Get<Anope::string>("filter");
		opertype_attribute = config->Get<Anope::string>("opertype_attribute");

		for (std::set<Oper *>::iterator it = my_opers.begin(), it_end = my_opers.end(); it != it_end; ++it)
			delete *it;
		my_opers.clear();
	}

	void OnNickIdentify(User *u) override
	{
		try
		{
			if (!this->ldap)
				throw LDAPException("No LDAP interface. Is m_ldap loaded and configured correctly?");
			else if (this->basedn.empty() || this->filter.empty() || opertype_attribute.empty())
				throw LDAPException("Could not search LDAP for opertype settings, invalid configuration.");

			if (!this->binddn.empty())
				this->ldap->Bind(NULL, this->binddn.replace_all_cs("%a", u->Account()->GetDisplay()), this->password.c_str());
			this->ldap->Search(new IdentifyInterface(this, u), this->basedn, this->filter.replace_all_cs("%a", u->Account()->GetDisplay()));
		}
		catch (const LDAPException &ex)
		{
			Log() << ex.GetReason();
		}
	}

	void OnDelCore(NickServ::Account *nc) override
	{
		if (nc->o != NULL && my_opers.count(nc->o) > 0)
		{
			my_opers.erase(nc->o);
			delete nc->o;
			nc->o = NULL;
		}
	}
};

MODULE_INIT(LDAPOper)
