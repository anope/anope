#include "module.h"
#include "nickserv.h"
#include "ldap.h"

static Anope::string email_attribute;

struct IdentifyInfo
{
	dynamic_reference<User> user;
	dynamic_reference<Command> command;
	CommandSource source;
	std::vector<Anope::string> params;
	Anope::string account;
	Anope::string pass;

	IdentifyInfo(User *u, Command *c, CommandSource &s, const std::vector<Anope::string> &pa, const Anope::string &a, const Anope::string &p) : user(u), command(c), source(s), params(pa), account(a), pass(p) { }
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

		User *u = ii->user;
		Command *c = ii->command;

		u->Extend("m_ldap_authentication_authenticated", NULL);

		NickAlias *na = findnick(ii->account);
		if (na == NULL)
		{
			na = new NickAlias(ii->account, new NickCore(ii->account));
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));

			BotInfo *bi = findbot(Config->NickServ);
			if (bi)
				u->SendMessage(bi, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
		}
		
		enc_encrypt(ii->pass, na->nc->pass);

		c->Execute(ii->source, ii->params);

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

		User *u = ii->user;
		Command *c = ii->command;

		u->Extend("m_ldap_authentication_error", NULL);

		c->Execute(ii->source, ii->params);

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

		if (!u || !u->Account() || r.empty())
			return;

		try
		{
			const LDAPAttributes &attr = r.get(0);
			Anope::string email = attr.get(email_attribute);

			if (!email.equals_ci(u->Account()->email))
			{
				u->Account()->email = email;
				BotInfo *bi = findbot(Config->NickServ);
				if (bi)
					u->SendMessage(bi, _("Your email has been updated to \002%s\002"), email.c_str());
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

class OnRegisterInterface : public LDAPInterface
{
 public:
	OnRegisterInterface(Module *m) : LDAPInterface(m) { }

	void OnResult(const LDAPResult &r)
	{
		Log() << "m_ldap_authentication: Successfully added newly created account to LDAP";
	}

	void OnError(const LDAPResult &r)
	{
		Log() << "m_ldap_authentication: Error adding newly created account to LDAP: " << r.getError();
	}
};

class NSIdentifyLDAP : public Module
{
	service_reference<LDAPProvider> ldap;
	IdentifyInterface iinterface;
	OnIdentifyInterface oninterface;
	OnRegisterInterface orinterface;

	Anope::string binddn;
	Anope::string object_class;
	Anope::string username_attribute;
	Anope::string password_attribute;
	bool disable_register;
	Anope::string disable_reason;
 public:
	NSIdentifyLDAP(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, SUPPORTED), ldap("LDAPProvider", "ldap/main"), iinterface(this), oninterface(this), orinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnPreCommand, I_OnCheckAuthentication, I_OnNickIdentify, I_OnNickRegister };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		ModuleManager::SetPriority(this, PRIORITY_FIRST);

		OnReload();
	}

	void OnReload()
	{
		ConfigReader config;

		this->binddn = config.ReadValue("m_ldap_authentication", "binddn", "", 0);
		this->object_class = config.ReadValue("m_ldap_authentication", "object_class", "", 0);
		this->username_attribute = config.ReadValue("m_ldap_authentication", "username_attribute", "", 0);
		this->password_attribute = config.ReadValue("m_ldap_authentication", "password_attribute", "", 0);
		email_attribute = config.ReadValue("m_ldap_authentication", "email_attribute", "", 0);
		this->disable_register = config.ReadFlag("m_ldap_authentication", "disable_ns_register", "false", 0);
		this->disable_reason = config.ReadValue("m_ldap_authentication", "disable_reason", "", 0);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params)
	{
		if (this->disable_register && !this->disable_reason.empty() && command->name == "nickserv/register")
		{
			source.Reply(_(this->disable_reason.c_str()));
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnCheckAuthentication(Command *c, CommandSource *source, const std::vector<Anope::string> &params, const Anope::string &account, const Anope::string &password)
	{

		if (c == NULL || source == NULL || !this->ldap)
			return EVENT_CONTINUE;

		User *u = source->u;
		if (u->HasExt("m_ldap_authentication_authenticated"))
		{
			u->Shrink("m_ldap_authentication_authenticated");
			return EVENT_ALLOW;
		}
		else if (u->HasExt("m_ldap_authentication_error"))
		{
			u->Shrink("m_ldap_authentication_error");
			return EVENT_CONTINUE;
		}

		IdentifyInfo *ii = new IdentifyInfo(u, c, *source,  params, account, password);
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

	void OnNickRegister(NickAlias *na)
	{
		if (this->disable_register || !this->ldap)
			return;

		try
		{
			this->ldap->BindAsAdmin(NULL);

			LDAPMods attributes;
			attributes.resize(4);

			attributes[0].name = "objectClass";
			attributes[0].values.push_back("top");
			attributes[0].values.push_back(this->object_class);

			attributes[1].name = this->username_attribute;
			attributes[1].values.push_back(na->nick);

			if (!na->nc->email.empty())
			{
				attributes[2].name = email_attribute;
				attributes[2].values.push_back(na->nc->email);
			}

			attributes[3].name = this->password_attribute;
			attributes[3].values.push_back(na->nc->pass);

			Anope::string new_dn = this->username_attribute + "=" + na->nick + "," + this->binddn;
			this->ldap->Add(&this->orinterface, new_dn, attributes);
		}
		catch (const LDAPException &ex)
		{
			Log() << "m_ldap_authentication: " << ex.GetReason();
		}
	}
};

MODULE_INIT(NSIdentifyLDAP)
