/* NickServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/nickserv/cert.h"
#include "modules/nickserv.h"

static Anope::hash_map<NickServ::Account *> certmap;
static EventHandlers<Event::NickCertEvents> *events;

class CertServiceImpl : public CertService
{
 public:
	CertServiceImpl(Module *o) : CertService(o) { }

	NickServ::Account* FindAccountFromCert(const Anope::string &cert) override
	{
		Anope::hash_map<NickServ::Account *>::iterator it = certmap.find(cert);
		if (it != certmap.end())
			return it->second;
		return NULL;
	}

	bool Matches(User *u, NickServ::Account *nc) override
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>(certentry);
		return !u->fingerprint.empty() && FindCert(cl, u->fingerprint);
	}

	NSCertEntry *FindCert(const std::vector<NSCertEntry *> &cl, const Anope::string &certfp) override
	{
		for (NSCertEntry *e : cl)
			if (e->GetCert() == certfp)
				return e;
		return nullptr;
	}
};

class NSCertEntryImpl : public NSCertEntry
{
 public:
	NSCertEntryImpl(Serialize::TypeBase *type) : NSCertEntry(type) { }
	NSCertEntryImpl(Serialize::TypeBase *type, Serialize::ID id) : NSCertEntry(type, id) { }
	~NSCertEntryImpl();

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	Anope::string GetCert() override;
	void SetCert(const Anope::string &) override;
};

class NSCertEntryType : public Serialize::Type<NSCertEntryImpl>
{
 public:
	struct Account : Serialize::ObjectField<NSCertEntryImpl, NickServ::Account *>
	{
		using Serialize::ObjectField<NSCertEntryImpl, NickServ::Account *>::ObjectField;

		void SetField(NSCertEntryImpl *s, NickServ::Account *acc) override
		{
			const Anope::string &cert = s->GetCert();
			if (!cert.empty())
				certmap.erase(cert);

			Serialize::ObjectField<NSCertEntryImpl, NickServ::Account *>::SetField(s, acc);

			if (!cert.empty() && s->GetAccount())
				certmap[cert] = acc;
		}
	} nc;

	struct Mask : Serialize::Field<NSCertEntryImpl, Anope::string>
	{
		using Serialize::Field<NSCertEntryImpl, Anope::string>::Field;

		void SetField(NSCertEntryImpl *s, const Anope::string &m) override
		{
			const Anope::string &old = GetField(s);
			if (!old.empty())
				certmap.erase(old);

			Serialize::Field<NSCertEntryImpl, Anope::string>::SetField(s, m);

			if (!m.empty() && s->GetAccount())
				certmap[m] = s->GetAccount();
		}
	} mask;

	NSCertEntryType(Module *me) : Serialize::Type<NSCertEntryImpl>(me, "NSCertEntry")
		, nc(this, "nc", true)
		, mask(this, "mask")
	{
	}
};

NSCertEntryImpl::~NSCertEntryImpl()
{
	const Anope::string &old = GetCert();
	if (!old.empty())
		certmap.erase(old);
}

NickServ::Account *NSCertEntryImpl::GetAccount()
{
	return Get<NickServ::Account *>(&NSCertEntryType::nc);
}

void NSCertEntryImpl::SetAccount(NickServ::Account *nc)
{
	Set(&NSCertEntryType::nc, nc);
}

Anope::string NSCertEntryImpl::GetCert()
{
	return Get<Anope::string>(&NSCertEntryType::mask);
}

void NSCertEntryImpl::SetCert(const Anope::string &mask)
{
	Set(&NSCertEntryType::mask, mask);
}

class CommandNSCert : public Command
{
	NSCertEntry *FindCert(const std::vector<NSCertEntry *> &cl, const Anope::string &certfp)
	{
		for (NSCertEntry *e : cl)
			if (e->GetCert() == certfp)
				return e;
		return nullptr;
	}

	void DoAdd(CommandSource &source, NickServ::Account *nc, Anope::string certfp)
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>(certentry);
		unsigned max = Config->GetModule(this->owner)->Get<unsigned>("max", "5");

		if (cl.size() >= max)
		{
			source.Reply(_("Sorry, the maximum of \002{0}\002 certificate entries has been reached."), max);
			return;
		}

		if (source.GetAccount() == nc)
		{
			User *u = source.GetUser();

			if (!u || u->fingerprint.empty())
			{
				source.Reply(_("You are not using a client certificate."));
				return;
			}

			certfp = u->fingerprint;
		}

		if (FindCert(cl, certfp))
		{
			source.Reply(_("Fingerprint \002{0}\002 already present on the certificate list of \002{0}\002."), certfp, nc->GetDisplay());
			return;
		}

		if (certmap.find(certfp) != certmap.end())
		{
			source.Reply(_("Fingerprint \002{0}\002 is already in use."), certfp);
			return;
		}

		NSCertEntry *e = certentry.Create();
		e->SetAccount(nc);
		e->SetCert(certfp);

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD certificate fingerprint " << certfp << " to " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 added to the certificate list of \002{1}\002."), certfp, nc->GetDisplay());
	}

	void DoDel(CommandSource &source, NickServ::Account *nc, Anope::string certfp)
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>(certentry);

		if (certfp.empty())
		{
			User *u = source.GetUser();
			if (u)
				certfp = u->fingerprint;
		}

		if (certfp.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		NSCertEntry *cert = FindCert(cl, certfp);
		if (!cert)
		{
			source.Reply(_("\002{0}\002 not found on the certificate list of \002{1}\002."), certfp, nc->GetDisplay());
			return;
		}

		cert->Delete();

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE certificate fingerprint " << certfp << " from " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 deleted from the access list of \002{1}\002."), certfp, nc->GetDisplay());
	}

	void DoList(CommandSource &source, NickServ::Account *nc)
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>(certentry);

		if (cl.empty())
		{
			source.Reply(_("The certificate list of \002{0}\002 is empty."), nc->GetDisplay());
			return;
		}

		source.Reply(_("Certificate list for \002{0}\002:"), nc->GetDisplay());
		for (NSCertEntry *e : cl)
			source.Reply("    {0}", e->GetCert());
	}

 public:
	CommandNSCert(Module *creator) : Command(creator, "nickserv/cert", 1, 3)
	{
		this->SetDesc(_("Modify the nickname client certificate list"));
		this->SetSyntax(_("ADD [\037nickname\037] [\037fingerprint\037]"));
		this->SetSyntax(_("DEL [\037nickname\037] \037fingerprint\037"));
		this->SetSyntax(_("LIST [\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		Anope::string nick, certfp;

		if (cmd.equals_ci("LIST"))
			nick = params.size() > 1 ? params[1] : "";
		else
		{
			nick = params.size() == 3 ? params[1] : "";
			certfp = params.size() > 1 ? params[params.size() - 1] : "";
		}

		NickServ::Account *nc;
		if (!nick.empty() && source.HasPriv("nickserv/access"))
		{
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}

			if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.GetAccount() != na->GetAccount() && na->GetAccount()->IsServicesOper() && !cmd.equals_ci("LIST"))
			{
				source.Reply(_("You may view, but not modify, the certificate list of other Services Operators."));
				return;
			}

			nc = na->GetAccount();
		}
		else
			nc = source.nc;

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else if (nc->HasFieldS("NS_SUSPENDED"))
			source.Reply(_("\002{0}\002 is suspended."), nc->GetDisplay());
		else if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode."));
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, certfp);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, certfp);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Modifies or displays the certificate list for your account."
		               "If you connect to IRC and provide a client certificate with a matching fingerprint in the certificate list, you will be automatically identified to services."
		               " Services Operators may provide \037nickname\037 to modify other users' certificate lists.\n"
		               "\n"
		               "Examples:\n"
		               "\n"
		               "         {0} ADD\n"
		               "         Adds your current fingerprint to the certificate list and automatically identifies you when you connect to IRC using this certificate.\n"
		               "\n"
		               "         {0} DEL <fingerprint>\n"
		               "         Removes \"<fingerprint>\" from your certificate list."));
		return true;
	}
};

class NSCert : public Module
	, public EventHook<Event::Fingerprint>
	, public EventHook<NickServ::Event::NickValidate>
{
	CommandNSCert commandnscert;
	CertServiceImpl cs;

	EventHandlers<Event::NickCertEvents> onnickservevents;

 public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnscert(this)
		, cs(this)
		, onnickservevents(this)
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("Your IRCd does not support ssl client certificates");

		events = &onnickservevents;
	}

	void OnFingerprint(User *u) override
	{
		ServiceBot *NickServ = Config->GetClient("NickServ");
		if (!NickServ || u->IsIdentified())
			return;

		NickServ::Account *nc = cs.FindAccountFromCert(u->fingerprint);
		if (!nc || nc->HasFieldS("NS_SUSPENDED"))
			return;

		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na && na->GetAccount() == nc)
			u->Identify(na);
		else
			u->Login(nc);

		u->SendMessage(NickServ, _("SSL certificate fingerprint accepted, you are now identified to \002%s\002."), nc->GetDisplay().c_str());
		Log(NickServ) << u->GetMask() << " automatically identified for account " << nc->GetDisplay() << " via SSL certificate fingerprint";
	}

	EventReturn OnNickValidate(User *u, NickServ::Nick *na) override
	{
		if (u->fingerprint.empty())
			return EVENT_CONTINUE;

		if (cs.Matches(u, na->GetAccount()))
		{
			ServiceBot *NickServ = Config->GetClient("NickServ");
			u->Identify(na);
			u->SendMessage(NickServ, _("SSL certificate fingerprint accepted, you are now identified."));
			Log(NickServ) << u->GetMask() << " automatically identified for account " << na->GetAccount()->GetDisplay() << " via SSL certificate fingerprint";
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSCert)
