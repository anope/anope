#include "module.h"
#include "modules/ldap.h"

static Module *me;

static Anope::string basedn;
static Anope::string search_filter;
static Anope::string object_class;
static Anope::string email_attribute;
static Anope::string username_attribute;

struct IdentifyInfo
{
	Reference<User> user;
	IdentifyRequest *req;
	ServiceReference<LDAPProvider> lprov;
	bool admin_bind;
	Anope::string dn;

	IdentifyInfo(User *u, IdentifyRequest *r, ServiceReference<LDAPProvider> &lp) : user(u), req(r), lprov(lp), admin_bind(true)
	{
		req->Hold(me);
	}
	
	~IdentifyInfo()
	{
		req->Release(me);
	}
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

	void OnResult(const LDAPResult &r) anope_override
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		IdentifyInfo *ii = it->second;
		this->requests.erase(it);

		if (!ii->lprov)
		{
			delete ii;
			return;
		}

		switch (r.type)
		{
			case LDAPResult::QUERY_SEARCH:
			{
				if (!r.empty())
				{
					try
					{
						const LDAPAttributes &attr = r.get(0);
						ii->dn = attr.get("dn");
						Log(LOG_DEBUG) << "m_ldap_authenticationn: binding as " << ii->dn;
						LDAPQuery id = ii->lprov->Bind(this, ii->dn, ii->req->GetPassword());
						this->Add(id, ii);
						return;
					}
					catch (const LDAPException &ex)
					{
						Log(this->owner) << "Error binding after search: " << ex.GetReason();
					}
				}
				break;
			}
			case LDAPResult::QUERY_BIND:
			{
				if (ii->admin_bind)
				{
					Anope::string sf = search_filter.replace_all_cs("%account", ii->req->GetAccount()).replace_all_cs("%object_class", object_class);
					try
					{
						Log(LOG_DEBUG) << "m_ldap_authentication: searching for " << sf;
						LDAPQuery id = ii->lprov->Search(this, basedn, sf);
						this->Add(id, ii);
						ii->admin_bind = false;
						return;
					}
					catch (const LDAPException &ex)
					{
						Log(this->owner) << "Unable to search for " << sf << ": " << ex.GetReason();
					}
				}
				else
				{
					NickAlias *na = NickAlias::Find(ii->req->GetAccount());
					if (na == NULL)
					{
						na = new NickAlias(ii->req->GetAccount(), new NickCore(ii->req->GetAccount()));
						na->last_realname = ii->user ? ii->user->realname : ii->req->GetAccount();
						FOREACH_MOD(OnNickRegister, (ii->user, na, ii->req->GetPassword()));
						BotInfo *NickServ = Config->GetClient("NickServ");
						if (ii->user && NickServ)
							ii->user->SendMessage(NickServ, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
					}
					// encrypt and store the password in the nickcore
					Anope::Encrypt(ii->req->GetPassword(), na->nc->pass);

					na->nc->Extend<Anope::string>("m_ldap_authentication_dn", ii->dn);
					ii->req->Success(me);
				}
				break;
			}
			default:
				break;
		}

		delete ii;
	}

	void OnError(const LDAPResult &r) anope_override
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		IdentifyInfo *ii = it->second;
		this->requests.erase(it);
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

	void OnResult(const LDAPResult &r) anope_override
	{
		std::map<LDAPQuery, Anope::string>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		User *u = User::Find(it->second);
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
				BotInfo *NickServ = Config->GetClient("NickServ");
				if (NickServ)
					u->SendMessage(NickServ, _("Your email has been updated to \002%s\002"), email.c_str());
				Log(this->owner) << "Updated email address for " << u->nick << " (" << u->Account()->display << ") to " << email;
			}
		}
		catch (const LDAPException &ex)
		{
			Log(this->owner) << ex.GetReason();
		}
	}

	void OnError(const LDAPResult &r) anope_override
	{
		this->requests.erase(r.id);
		Log(this->owner) << r.error;
	}
};

class OnRegisterInterface : public LDAPInterface
{
 public:
	OnRegisterInterface(Module *m) : LDAPInterface(m) { }

	void OnResult(const LDAPResult &r) anope_override
	{
		Log(this->owner) << "Successfully added newly created account to LDAP";
	}

	void OnError(const LDAPResult &r) anope_override
	{
		Log(this->owner) << "Error adding newly created account to LDAP: " << r.getError();
	}
};

class NSIdentifyLDAP : public Module
{
	ServiceReference<LDAPProvider> ldap;
	IdentifyInterface iinterface;
	OnIdentifyInterface oninterface;
	OnRegisterInterface orinterface;

	PrimitiveExtensibleItem<Anope::string> dn;

	Anope::string password_attribute;
	Anope::string disable_register_reason;
	Anope::string disable_email_reason;
 public:
	NSIdentifyLDAP(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, EXTRA | VENDOR), ldap("LDAPProvider", "ldap/main"), iinterface(this), oninterface(this), orinterface(this),
		dn(this, "m_ldap_authentication_dn")
	{

		me = this;
	}

	void Prioritize() anope_override
	{
		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}

	void OnReload(Configuration::Conf *config) anope_override
	{
		Configuration::Block *conf = Config->GetModule(this);

		basedn = conf->Get<const Anope::string>("basedn");
		search_filter = conf->Get<const Anope::string>("search_filter");
		object_class = conf->Get<const Anope::string>("object_class");
		username_attribute = conf->Get<const Anope::string>("username_attribute");
		this->password_attribute = conf->Get<const Anope::string>("password_attribute");
		email_attribute = conf->Get<const Anope::string>("email_attribute");
		this->disable_register_reason = conf->Get<const Anope::string>("disable_register_reason");
		this->disable_email_reason = conf->Get<const Anope::string>("disable_email_reason");

		if (!email_attribute.empty())
			/* Don't complain to users about how they need to update their email, we will do it for them */
			config->GetModule("nickserv")->Set("forceemail", "false");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		if (!this->disable_register_reason.empty())
		{
			if (command->name == "nickserv/register" || command->name == "nickserv/group")
			{
				source.Reply(this->disable_register_reason);
				return EVENT_STOP;
			}
		}

		if (!email_attribute.empty() && !this->disable_email_reason.empty() && command->name == "nickserv/set/email")
		{
			source.Reply(this->disable_email_reason);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnCheckAuthentication(User *u, IdentifyRequest *req) anope_override
	{
		if (!this->ldap)
			return;

		IdentifyInfo *ii = new IdentifyInfo(u, req, this->ldap);
		try
		{
			LDAPQuery id = this->ldap->BindAsAdmin(&this->iinterface);
			this->iinterface.Add(id, ii);
		}
		catch (const LDAPException &ex)
		{
			delete ii;
			Log(this) << ex.GetReason();
		}
	}

	void OnNickIdentify(User *u) anope_override
	{
		if (email_attribute.empty() || !this->ldap)
			return;

		Anope::string *d = dn.Get(u->Account());
		if (!d || d->empty())
			return;

		try
		{
			LDAPQuery id = this->ldap->Search(&this->oninterface, *d, "(" + email_attribute + "=*)");
			this->oninterface.Add(id, u->nick);
		}
		catch (const LDAPException &ex)
		{
			Log(this) << ex.GetReason();
		}
	}

	void OnNickRegister(User *, NickAlias *na, const Anope::string &pass) anope_override
	{
		if (!this->disable_register_reason.empty() || !this->ldap)
			return;

		try
		{
			this->ldap->BindAsAdmin(NULL);

			LDAPMods attributes;
			attributes.resize(4);

			attributes[0].name = "objectClass";
			attributes[0].values.push_back("top");
			attributes[0].values.push_back(object_class);

			attributes[1].name = username_attribute;
			attributes[1].values.push_back(na->nick);

			if (!na->nc->email.empty())
			{
				attributes[2].name = email_attribute;
				attributes[2].values.push_back(na->nc->email);
			}

			attributes[3].name = this->password_attribute;
			attributes[3].values.push_back(pass);

			Anope::string new_dn = username_attribute + "=" + na->nick + "," + basedn;
			this->ldap->Add(&this->orinterface, new_dn, attributes);
		}
		catch (const LDAPException &ex)
		{
			Log(this) << ex.GetReason();
		}
	}
};

MODULE_INIT(NSIdentifyLDAP)
