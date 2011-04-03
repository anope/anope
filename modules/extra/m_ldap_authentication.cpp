#include "module.h"
#include "ldap.h"

static Anope::string email_attribute;

struct IdentifyInfo
{
	dynamic_reference<User> user;
	dynamic_reference<Command> command;
	std::vector<Anope::string> params;
	Anope::string account;
	Anope::string pass;

	IdentifyInfo(User *u, Command *c, const std::vector<Anope::string> &pa, const Anope::string &a, const Anope::string &p) : user(u), command(c), params(pa), account(a), pass(p) { }
};


class IdentifyInterface : public LDAPInterface
{
	std::map<LDAPQuery, IdentifyInfo *> requests;

 public:
	IdentifyInterface(Module *m) : LDAPInterface(m) { }

	void Add(LDAPQuery id, IdentifyInfo *ii)
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(id);
		if (it != this->requests.end())
			delete it->second;
		this->requests[id] = ii;
	}

	void OnResult(const LDAPResult &r)
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		IdentifyInfo *ii = it->second;
		this->requests.erase(it);

		if (!ii->user || !ii->command)
		{
			delete this;
			return;
		}

		User *u = *ii->user;
		Command *c = *ii->command;

		u->Extend("m_ldap_authentication_authenticated");

		NickAlias *na = findnick(ii->account);
		if (na == NULL)
		{
			na = new NickAlias(ii->account, new NickCore(ii->account));
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));

			u->SendMessage(NickServ, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
		}
		
		enc_encrypt(ii->pass, na->nc->pass);

		Anope::string params;
		for (unsigned i = 0; i < ii->params.size(); ++i)
			params += ii->params[i] + " ";
		mod_run_cmd(c->service, u, NULL, c, c->name, params);

		delete ii;
	}

	void OnError(const LDAPResult &r)
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		IdentifyInfo *ii = it->second;
		this->requests.erase(it);

		if (!ii->user || !ii->command)
		{
			delete ii;
			return;
		}

		User *u = *ii->user;
		Command *c = *ii->command;

		u->Extend("m_ldap_authentication_error");

		Anope::string params;
		for (unsigned i = 0; i < ii->params.size(); ++i)
			params += ii->params[i] + " ";
		mod_run_cmd(c->service, u, NULL, c, c->name, params);

		delete ii;
	}
};

class OnIdentifyInterface : public LDAPInterface
{
	std::map<LDAPQuery, Anope::string> requests;

 public:
	OnIdentifyInterface(Module *m) : LDAPInterface(m) { }

	void Add(LDAPQuery id, const Anope::string &nick)
	{
		this->requests[id] = nick;
	}

	void OnResult(const LDAPResult &r)
	{
		std::map<LDAPQuery, Anope::string>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		User *u = finduser(it->second);
		this->requests.erase(it);

		if (!u || !u->Account())
			return;

		try
		{
			const LDAPAttributes &attr = r.get(0);
			Anope::string email = attr.get(email_attribute);

			if (!email.equals_ci(u->Account()->email))
			{
				u->Account()->email = email;
				u->SendMessage(NickServ, _("Your email has been updated to \002%s\002"), email.c_str());
				Log() << "m_ldap_authentication: Updated email address for " << u->nick << " (" << u->Account()->display << ") to " << email;
			}
		}
		catch (const LDAPException &ex)
		{
			Log() << "m_ldap_authentication: " << ex.GetReason();
		}
	}

	void OnError(const LDAPResult &r)
	{
		this->requests.erase(r.id);
		Log() << "m_ldap_authentication: " << r.error;
	}
};

class NSIdentifyLDAP : public Module
{
	service_reference<LDAPProvider> ldap;
	IdentifyInterface iinterface;
	OnIdentifyInterface oninterface;

	Anope::string binddn;
	Anope::string username_attribute;
	bool disable_register;
	Anope::string disable_reason;
 public:
	NSIdentifyLDAP(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator), ldap("ldap/main"), iinterface(this), oninterface(this)
	{
		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);

		Implementation i[] = { I_OnReload, I_OnPreCommand, I_OnCheckAuthentication, I_OnNickIdentify };
		ModuleManager::Attach(i, this, 4);
		ModuleManager::SetPriority(this, PRIORITY_FIRST);

		OnReload(false);
	}

	void OnReload(bool)
	{
		ConfigReader config;

		this->binddn = config.ReadValue("m_ldap_authentication", "binddn", "", 0);
		this->username_attribute = config.ReadValue("m_ldap_authentication", "username_attribute", "", 0);
		email_attribute = config.ReadValue("m_ldap_authentication", "email_attribute", "", 0);
		this->disable_register = config.ReadFlag("m_ldap_authentication", "disable_ns_register", "false", 0);
		this->disable_reason = config.ReadValue("m_ldap_authentication", "disable_reason", "", 0);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		if (this->disable_register && command->service == NickServ && command->name == "REGISTER")
		{
			source.Reply(_(this->disable_reason.c_str()));
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnCheckAuthentication(User *u, Command *c, const std::vector<Anope::string> &params, const Anope::string &account, const Anope::string &password)
	{
		if (u == NULL || c == NULL || !this->ldap)
			return EVENT_CONTINUE;
		else if (u->GetExt("m_ldap_authentication_authenticated"))
		{
			u->Shrink("m_ldap_authentication_authenticated");
			return EVENT_ALLOW;
		}
		else if (u->GetExt("m_ldap_authentication_error"))
		{
			u->Shrink("m_ldap_authentication_error");
			return EVENT_CONTINUE;;
		}

		IdentifyInfo *ii = new IdentifyInfo(u, c, params, account, password);
		try
		{
			Anope::string full_binddn = this->username_attribute + "=" + account + "," + this->binddn;
			LDAPQuery id = this->ldap->Bind(&this->iinterface, full_binddn, password);
			this->iinterface.Add(id, ii);
		}
		catch (const LDAPException &ex)
		{
			delete ii;
			Log() << "ns_identify_ldap: " << ex.GetReason();
			return EVENT_CONTINUE;
		}

		return EVENT_STOP;
	}

	void OnNickIdentify(User *u)
	{
		if (email_attribute.empty() || !this->ldap)
			return;

		try
		{
			LDAPQuery id = this->ldap->Search(&this->oninterface, this->binddn, "(" + this->username_attribute + "=" + u->Account()->display + ")");
			this->oninterface.Add(id, u->nick);
		}
		catch (const LDAPException &ex)
		{
			Log() << "m_ldap_authentication: " << ex.GetReason();
		}
	}
};

MODULE_INIT(NSIdentifyLDAP)
