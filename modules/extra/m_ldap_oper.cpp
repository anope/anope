#include "module.h"
#include "ldap.h"

static std::set<Oper *> my_opers;
static Anope::string opertype_attribute;

class IdentifyInterface : public LDAPInterface
{
	std::map<LDAPQuery, Anope::string> requests;

 public:
	IdentifyInterface(Module *m) : LDAPInterface(m)
	{
	}

	void Add(LDAPQuery id, const Anope::string &nick)
	{
		std::map<LDAPQuery, Anope::string>::iterator it = this->requests.find(id);
		this->requests[id] = nick;
	}

	void OnResult(const LDAPResult &r) anope_override
	{
		std::map<LDAPQuery, Anope::string>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		User *u = finduser(it->second);
		this->requests.erase(it);


		if (!u || !u->Account())
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
				Log() << "m_ldap_oper: Tied " << u->nick << " (" << nc->display << ") to opertype " << ot->GetName();
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

				Log() << "m_ldap_oper: Removed services operator from " << u->nick << " (" << nc->display << ")";
			}
		}
	}

	void OnError(const LDAPResult &r) anope_override
	{
		this->requests.erase(r.id);
	}
};

class LDAPOper : public Module
{
	service_reference<LDAPProvider> ldap;
	IdentifyInterface iinterface;

	Anope::string binddn;
	Anope::string password;
	Anope::string basedn;
	Anope::string filter;
 public:
	LDAPOper(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, SUPPORTED), ldap("LDAPProvider", "ldap/main"), iinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNickIdentify, I_OnDelCore };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->binddn = config.ReadValue("m_ldap_oper", "binddn", "", 0);
		this->password = config.ReadValue("m_ldap_oper", "password", "", 0);
		this->basedn = config.ReadValue("m_ldap_oper", "basedn", "", 0);
		this->filter = config.ReadValue("m_ldap_oper", "filter", "", 0);
		opertype_attribute = config.ReadValue("m_ldap_oper", "opertype_attribute", "", 0);

		for (std::set<Oper *>::iterator it = my_opers.begin(), it_end = my_opers.end(); it != it_end; ++it)
			delete *it;
		my_opers.clear();
	}

	void OnNickIdentify(User *u) anope_override
	{
		try
		{
			if (!this->ldap)
				throw LDAPException("No LDAP interface. Is m_ldap loaded and configured correctly?");
			else if (this->basedn.empty() || this->filter.empty() || opertype_attribute.empty())
				throw LDAPException("Could not search LDAP for opertype settings, invalid configuration.");

			if (!this->binddn.empty())
				this->ldap->Bind(NULL, this->binddn.replace_all_cs("%a", u->Account()->display), this->password.c_str());
			LDAPQuery id = this->ldap->Search(&this->iinterface, this->basedn, this->filter.replace_all_cs("%a", u->Account()->display));
			this->iinterface.Add(id, u->nick);
		}
		catch (const LDAPException &ex)
		{
			Log() << "m_ldapoper: " << ex.GetReason();
		}
	}

	void OnDelCore(NickCore *nc) anope_override
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
