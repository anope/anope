/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "modules/nickserv/cert.h"
#include "modules/nickserv.h"

static Anope::hash_map<NickServ::Account *> certmap;

class CertServiceImpl : public CertService
{
 public:
	CertServiceImpl(Module *o) : CertService(o) { }

	NickServ::Account* FindAccountFromCert(const Anope::string &cert) override
	{
#warning "use serialize find"
		Anope::hash_map<NickServ::Account *>::iterator it = certmap.find(cert);
		if (it != certmap.end())
			return it->second;
		return NULL;
	}

	bool Matches(User *u, NickServ::Account *nc) override
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>();
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
	friend class NSCertEntryType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> cert;

 public:
	using NSCertEntry::NSCertEntry;
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

		void OnSet(NSCertEntryImpl *s, NickServ::Account *acc) override
		{
			const Anope::string &cert = s->GetCert();

			if (!cert.empty())
				certmap[cert] = acc;
		}
	} account;

	struct Mask : Serialize::Field<NSCertEntryImpl, Anope::string>
	{
		using Serialize::Field<NSCertEntryImpl, Anope::string>::Field;

		void OnSet(NSCertEntryImpl *s, const Anope::string &m) override
		{
			Anope::string *old = this->Get_(s);
			if (old != nullptr)
				certmap.erase(*old);

			if (!m.empty() && s->GetAccount())
				certmap[m] = s->GetAccount();
		}
	} mask;

	NSCertEntryType(Module *me) : Serialize::Type<NSCertEntryImpl>(me)
		, account(this, "account", &NSCertEntryImpl::account, true)
		, mask(this, "mask", &NSCertEntryImpl::cert)
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
	return Get<NickServ::Account *>(&NSCertEntryType::account);
}

void NSCertEntryImpl::SetAccount(NickServ::Account *nc)
{
	Set(&NSCertEntryType::account, nc);
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
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>();
		unsigned max = Config->GetModule(this->GetOwner())->Get<unsigned>("max", "5");

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

		NSCertEntry *e = Serialize::New<NSCertEntry *>();
		e->SetAccount(nc);
		e->SetCert(certfp);
		
#warning "events?"

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD certificate fingerprint " << certfp << " to " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 added to the certificate list of \002{1}\002."), certfp, nc->GetDisplay());
	}

	void DoDel(CommandSource &source, NickServ::Account *nc, Anope::string certfp)
	{
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>();

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
		std::vector<NSCertEntry *> cl = nc->GetRefs<NSCertEntry *>();

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

			if (Config->GetModule("nickserv/main")->Get<bool>("secureadmins", "yes") && source.GetAccount() != na->GetAccount() && na->GetAccount()->GetOper() && !cmd.equals_ci("LIST"))
			{
				source.Reply(_("You may view, but not modify, the certificate list of other Services Operators."));
				return;
			}

			nc = na->GetAccount();
		}
		else
		{
			nc = source.nc;
		}

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

 public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::Fingerprint>(this)
		, EventHook<NickServ::Event::NickValidate>(this)
		, commandnscert(this)
		, cs(this)
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("Your IRCd does not support ssl client certificates");
	}

	void OnFingerprint(User *u) override
	{
		ServiceBot *NickServ = Config->GetClient("NickServ");
		if (!NickServ || u->IsIdentified())
			return;

		NickServ::Account *nc = cs.FindAccountFromCert(u->fingerprint);
		if (!nc || nc->HasFieldS("NS_SUSPENDED"))
			return;

		unsigned int maxlogins = Config->GetModule("nickserv/identify")->Get<unsigned int>("maxlogins");
		if (maxlogins && nc->users.size() >= maxlogins)
		{
			u->SendMessage(NickServ, _("Account \002{0}\002 has already reached the maximum number of simultaneous logins ({1})."), nc->GetDisplay(), maxlogins);
			return;
		}

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

			unsigned int maxlogins = Config->GetModule("nickserv/identify")->Get<unsigned int>("maxlogins");
			if (maxlogins && na->GetAccount()->users.size() >= maxlogins)
			{
				u->SendMessage(NickServ, _("Account \002{0}\002 has already reached the maximum number of simultaneous logins ({1})."), na->GetAccount()->GetDisplay(), maxlogins);
				return EVENT_CONTINUE;
			}

			u->Identify(na);
			u->SendMessage(NickServ, _("SSL certificate fingerprint accepted, you are now identified."));
			Log(NickServ) << u->GetMask() << " automatically identified for account " << na->GetAccount()->GetDisplay() << " via SSL certificate fingerprint";
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSCert)
