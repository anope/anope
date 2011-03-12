#include "module.h"
#include "ldap.h"

static Anope::string email_attribute;

struct IdentifyInfo
{
	dynamic_reference<User> user;
	Anope::string account;
	Anope::string pass;

	IdentifyInfo(User *u, const Anope::string &a, const Anope::string &p) : user(u), account(a), pass(p) { }
};


class IdentifyInterface : public LDAPInterface, public Command
{
	std::map<LDAPQuery, IdentifyInfo *> requests;

 public:
	IdentifyInterface(Module *m) : LDAPInterface(m), Command("IDENTIFY", 0, 0)
	{
		this->service = NickServ;
	}

	CommandReturn Execute(CommandSource &, const std::vector<Anope::string> &) { return MOD_STOP; }

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

		User *u = *ii->user;
		NickAlias *na = findnick(ii->account);

		if (!na)
		{
			na = new NickAlias(ii->account, new NickCore(ii->account));
			enc_encrypt(ii->pass, na->nc->pass);

			Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = last_usermask;
			na->last_realname = u->realname;
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));

			u->SendMessage(NickServ, _("Your account \002%s\002 has been successfully created."), ii->account.c_str());
		}
		
		if (u->Account())
			Log(LOG_COMMAND, u, this) << "to log out of account " << u->Account()->display;
		Log(LOG_COMMAND, u, this) << "and identified for account " << na->nc->display << " using LDAP";
		u->SendMessage(NickServ, _("Password accepted - you are now recognized."));
		u->Identify(na);
		delete ii;
	}

	void OnError(const LDAPResult &r)
	{
		std::map<LDAPQuery, IdentifyInfo *>::iterator it = this->requests.find(r.id);
		if (it == this->requests.end())
			return;
		IdentifyInfo *ii = it->second;
		this->requests.erase(it);

		if (!ii->user)
		{
			delete this;
			return;
		}

		User *u = *ii->user;

		Log(LOG_COMMAND, u, this) << "and failed to identify for account " << ii->account << ". LDAP error: " << r.getError();
		u->SendMessage(NickServ, _(PASSWORD_INCORRECT));
		bad_password(u);
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
				u->SendMessage(NickServ, "Your email has been updated to \002%s\002", email.c_str());
				Log() << "ns_identify_ldap: Updated email address for " << u->nick << " (" << u->Account()->display << ") to " << email;
			}
		}
		catch (const LDAPException &ex)
		{
			Log() << "ns_identify_ldap: " << ex.GetReason();
		}
	}

	void OnError(const LDAPResult &r)
	{
		this->requests.erase(r.id);
		Log() << "ns_identify_ldap: " << r.error;
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

		Implementation i[] = { I_OnReload, I_OnPreCommand, I_OnNickIdentify };
		ModuleManager::Attach(i, this, 3);

		OnReload(false);
	}

	void OnReload(bool)
	{
		ConfigReader config;

		this->binddn = config.ReadValue("ns_identify_ldap", "binddn", "", 0);
		this->username_attribute = config.ReadValue("ns_identify_ldap", "username_attribute", "", 0);
		email_attribute = config.ReadValue("ns_identify_ldap", "email_attribute", "", 0);
		this->disable_register = config.ReadFlag("ns_identify_ldap", "disable_ns_register", "false", 0);
		this->disable_reason = config.ReadValue("ns_identify_ldap", "disable_reason", "", 0);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		if (command->service == NickServ)
		{
			if (this->disable_register && command->name == "REGISTER")
			{
				source.Reply(_(this->disable_reason.c_str()));
				return EVENT_STOP;
			}
			else if (command->name == "IDENTIFY" && !params.empty() && this->ldap)
			{
				Anope::string account = params.size() > 1 ? params[0] : source.u->nick;
				Anope::string pass = params.size() > 1 ? params[1] : params[0];

				NickAlias *na = findnick(account);
				if (na)
				{
					account = na->nc->display;

					if (na->HasFlag(NS_FORBIDDEN) || na->nc->HasFlag(NI_SUSPENDED) || source.u->Account() == na->nc)
						return EVENT_CONTINUE;
				}

				IdentifyInfo *ii = new IdentifyInfo(source.u, account, pass);
				try
				{
					Anope::string full_binddn = this->username_attribute + "=" + account + "," + this->binddn;
					LDAPQuery id = this->ldap->Bind(&this->iinterface, full_binddn, pass);
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
		}

		return EVENT_CONTINUE;
	}

	void OnNickIdentify(User *u)
	{
		if (email_attribute.empty())
			return;

		try
		{
			LDAPQuery id = this->ldap->Search(&this->oninterface, this->binddn, "(" + this->username_attribute + "=" + u->Account()->display + ")");
			this->oninterface.Add(id, u->nick);
		}
		catch (const LDAPException &ex)
		{
			Log() << "ns_identify_ldap: " << ex.GetReason();
		}
	}
};

MODULE_INIT(NSIdentifyLDAP)
