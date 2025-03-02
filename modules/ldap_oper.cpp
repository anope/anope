/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/ldap.h"

static std::set<Oper *> my_opers;
static Anope::string opertype_attribute;

class IdentifyInterface final
	: public LDAPInterface
{
	Reference<User> u;

public:
	IdentifyInterface(Module *m, User *user) : LDAPInterface(m), u(user)
	{
	}

	void OnResult(const LDAPResult &r) override
	{
		if (!u || !u->IsIdentified())
			return;

		NickCore *nc = u->Account();

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
				Log(this->owner) << "Tied " << u->nick << " (" << nc->display << ") to opertype " << ot->GetName();
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

				Log(this->owner) << "Removed services operator from " << u->nick << " (" << nc->display << ")";
			}
		}
	}

	void OnError(const LDAPResult &r) override
	{
	}

	void OnDelete() override
	{
		delete this;
	}
};

class LDAPOper final
	: public Module
{
	ServiceReference<LDAPProvider> ldap;

	Anope::string binddn;
	Anope::string password;
	Anope::string basedn;
	Anope::string filter;
public:
	LDAPOper(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, EXTRA | VENDOR), ldap("LDAPProvider", "ldap/main")
	{

	}

	void OnReload(Configuration::Conf &conf) override
	{
		Configuration::Block &config = Config->GetModule(this);

		this->binddn = config.Get<const Anope::string>("binddn");
		this->password = config.Get<const Anope::string>("password");
		this->basedn = config.Get<const Anope::string>("basedn");
		this->filter = config.Get<const Anope::string>("filter");
		opertype_attribute = config.Get<const Anope::string>("opertype_attribute");

		for (const auto *oper : my_opers)
			delete oper;
		my_opers.clear();
	}

	void OnNickIdentify(User *u) override
	{
		try
		{
			if (!this->ldap)
				throw LDAPException("No LDAP interface. Is ldap loaded and configured correctly?");
			else if (this->basedn.empty() || this->filter.empty() || opertype_attribute.empty())
				throw LDAPException("Could not search LDAP for opertype settings, invalid configuration.");

			if (!this->binddn.empty())
				this->ldap->Bind(NULL, this->binddn.replace_all_cs("%a", u->Account()->display), this->password.c_str());
			this->ldap->Search(new IdentifyInterface(this, u), this->basedn, this->filter.replace_all_cs("%a", u->Account()->display));
		}
		catch (const LDAPException &ex)
		{
			Log() << ex.GetReason();
		}
	}

	void OnDelCore(NickCore *nc) override
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
