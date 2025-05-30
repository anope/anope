/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/nickserv/cert.h"

static Anope::unordered_map<NickCore *> certmap;

struct CertServiceImpl final
	: CertService
{
	CertServiceImpl(Module *o) : CertService(o) { }

	NickCore *FindAccountFromCert(const Anope::string &cert) override
	{
		Anope::unordered_map<NickCore *>::iterator it = certmap.find(cert);
		if (it != certmap.end())
			return it->second;
		return NULL;
	}

	void ReplaceCert(const Anope::string &oldcert, const Anope::string &newcert) override
	{
		auto *nc = FindAccountFromCert(oldcert);
		if (!nc)
			return;

		auto *cl = nc->GetExt<NSCertList>("certificates");
		if (cl)
			cl->ReplaceCert(oldcert, newcert);
	}
};

struct NSCertListImpl final
	: NSCertList
{
	Serialize::Reference<NickCore> nc;
	std::vector<Anope::string> certs;

public:
	NSCertListImpl(Extensible *obj) : nc(anope_dynamic_static_cast<NickCore *>(obj)) { }

	~NSCertListImpl() override
	{
		ClearCert();
	}

	/** Add an entry to the nick's certificate list
	 *
	 * @param entry The fingerprint to add to the cert list
	 *
	 * Adds a new entry into the cert list.
	 */
	void AddCert(const Anope::string &entry) override
	{
		this->certs.push_back(entry);
		certmap[entry] = nc;
		FOREACH_MOD(OnNickAddCert, (this->nc, entry));
	}

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	Anope::string GetCert(unsigned entry) const override
	{
		if (entry >= this->certs.size())
			return "";
		return this->certs[entry];
	}

	unsigned GetCertCount() const override
	{
		return this->certs.size();
	}

	/** Find an entry in the nick's cert list
	 *
	 * @param entry The fingerprint to search for
	 * @return True if the fingerprint is found in the cert list, false otherwise
	 *
	 * Search for an fingerprint within the cert list.
	 */
	bool FindCert(const Anope::string &entry) const override
	{
		return std::find(this->certs.begin(), this->certs.end(), entry) != this->certs.end();
	}

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	void EraseCert(const Anope::string &entry) override
	{
		std::vector<Anope::string>::iterator it = std::find(this->certs.begin(), this->certs.end(), entry);
		if (it != this->certs.end())
		{
			FOREACH_MOD(OnNickEraseCert, (this->nc, entry));
			certmap.erase(entry);
			this->certs.erase(it);
		}
	}

	void ReplaceCert(const Anope::string &oldentry, const Anope::string &newentry) override
	{
		auto it = std::find(this->certs.begin(), this->certs.end(), oldentry);
		if (it == this->certs.end())
			return; // We can't replace a non-existent cert.

		FOREACH_MOD(OnNickEraseCert, (this->nc, oldentry));
		certmap.erase(oldentry);

		if (std::find(this->certs.begin(), this->certs.end(), newentry) != this->certs.end())
		{
			// The cert we're upgrading to already exists.
			this->certs.erase(it);
			return;
		}

		*it = newentry;
		certmap[newentry] = nc;
		FOREACH_MOD(OnNickAddCert, (this->nc, newentry));
	}

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	void ClearCert() override
	{
		FOREACH_MOD(OnNickClearCert, (this->nc));
		for (const auto &cert : certs)
			certmap.erase(cert);
		this->certs.clear();
	}

	void Check() override
	{
		if (this->certs.empty())
			nc->Shrink<NSCertList>("certificates");
	}

	struct ExtensibleItem final
		: ::ExtensibleItem<NSCertListImpl>
	{
		ExtensibleItem(Module *m, const Anope::string &ename) : ::ExtensibleItem<NSCertListImpl>(m, ename) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			if (s->GetSerializableType()->GetName() != NICKCORE_TYPE)
				return;

			const NickCore *n = anope_dynamic_static_cast<const NickCore *>(e);
			NSCertList *c = this->Get(n);
			if (c == NULL || !c->GetCertCount())
				return;

			std::ostringstream oss;
			for (unsigned i = 0; i < c->GetCertCount(); ++i)
				oss << c->GetCert(i) << " ";
			data.Store("cert", oss.str());
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			if (s->GetSerializableType()->GetName() != NICKCORE_TYPE)
				return;

			NickCore *n = anope_dynamic_static_cast<NickCore *>(e);
			NSCertListImpl *c = this->Require(n);

			Anope::string buf;
			data["cert"] >> buf;
			spacesepstream sep(buf);
			for (const auto &cert : c->certs)
				certmap.erase(cert);
			c->certs.clear();
			while (sep.GetToken(buf))
			{
				c->certs.push_back(buf);
				certmap[buf] = n;
			}
		}
	};
};

class CommandNSCert final
	: public Command
{
private:
	void DoAdd(CommandSource &source, NickCore *nc, Anope::string certfp)
	{
		NSCertList *cl = nc->Require<NSCertList>("certificates");
		unsigned max = Config->GetModule(this->owner).Get<unsigned>("max", "5");

		if (cl->GetCertCount() >= max)
		{
			source.Reply(_("Sorry, the maximum of %d certificate entries has been reached."), max);
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

		if (cl->FindCert(certfp))
		{
			source.Reply(_("Fingerprint \002%s\002 already present on %s's certificate list."), certfp.c_str(), nc->display.c_str());
			return;
		}

		if (certmap.find(certfp) != certmap.end())
		{
			source.Reply(_("Fingerprint \002%s\002 is already in use."), certfp.c_str());
			return;
		}

		cl->AddCert(certfp);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD certificate fingerprint " << certfp << " to " << nc->display;
		source.Reply(_("\002%s\002 added to %s's certificate list."), certfp.c_str(), nc->display.c_str());
	}

	void DoDel(CommandSource &source, NickCore *nc, Anope::string certfp)
	{
		NSCertList *cl = nc->Require<NSCertList>("certificates");

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

		if (!cl->FindCert(certfp))
		{
			source.Reply(_("\002%s\002 not found on %s's certificate list."), certfp.c_str(), nc->display.c_str());
			return;
		}

		cl->EraseCert(certfp);
		cl->Check();
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE certificate fingerprint " << certfp << " from " << nc->display;
		source.Reply(_("\002%s\002 deleted from %s's certificate list."), certfp.c_str(), nc->display.c_str());
	}

	static void DoList(CommandSource &source, const NickCore *nc)
	{
		NSCertList *cl = nc->GetExt<NSCertList>("certificates");

		if (!cl || !cl->GetCertCount())
		{
			source.Reply(_("%s's certificate list is empty."), nc->display.c_str());
			return;
		}

		source.Reply(_("Certificate list for %s:"), nc->display.c_str());
		for (unsigned i = 0; i < cl->GetCertCount(); ++i)
		{
			Anope::string fingerprint = cl->GetCert(i);
			source.Reply("    %s", fingerprint.c_str());
		}
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

		NickCore *nc;
		if (!nick.empty())
		{
			const NickAlias *na = NickAlias::Find(nick);
			if (na == NULL)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return;
			}
			else if (na->nc != source.GetAccount() && !source.HasPriv("nickserv/cert"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			else if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.GetAccount() != na->nc && na->nc->IsServicesOper() && !cmd.equals_ci("LIST"))
			{
				source.Reply(_("You may view but not modify the certificate list of other Services Operators."));
				return;
			}

			nc = na->nc;
		}
		else
			nc = source.nc;

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else if (nc->HasExt("NS_SUSPENDED"))
			source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
		else if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, certfp);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, certfp);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Modifies or displays the certificate list for your nick. "
				"If you connect to IRC and provide a client certificate with a "
				"matching fingerprint in the cert list, you will be "
				"automatically identified to services. Services Operators "
				"may provide a nick to modify other users' certificate lists."
				"\n\n"
				"Examples:"
				"\n\n"
				"    \002%s\032ADD\002\n"
				"        Adds your current fingerprint to the certificate list and\n"
				"        automatically identifies you when you connect to IRC\n"
				"        using this fingerprint."
				"\n\n"
				"    \002%s\032DEL\032<fingerprint>\002\n"
				"        Removes the fingerprint <fingerprint> from your certificate list."
				"\n\n"
				"    \002%s\032LIST\002\n"
				"        Displays the current certificate list."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());
		return true;
	}
};

class NSCert final
	: public Module
{
	CommandNSCert commandnscert;
	NSCertListImpl::ExtensibleItem certs;
	CertServiceImpl cs;

	bool CanLogin(User *u, NickCore *nc)
	{
		if (!nc || nc->HasExt("NS_SUSPENDED"))
			return false; // Account suspended.

		const auto maxlogins = Config->GetModule("ns_identify").Get<unsigned int>("maxlogins");
		if (maxlogins && nc->users.size() >= maxlogins)
		{
			auto *nickserv = Config->GetClient("NickServ");
			u->SendMessage(nickserv, _("Account \002%s\002 has already reached the maximum number of simultaneous logins (%u)."),
				nc->display.c_str(), maxlogins);
			return false;
		}

		return true;
	}

public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnscert(this), certs(this, "certificates"), cs(this)
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("Your IRCd does not support ssl client certificates");
	}

	void OnFingerprint(User *u) override
	{
		if (u->IsIdentified())
			return;

		NickCore *nc = cs.FindAccountFromCert(u->fingerprint);
		if (!CanLogin(u, nc))
			return;

		NickAlias *na = NickAlias::Find(u->nick);
		if (na && na->nc == nc)
			u->Identify(na);
		else
			u->Login(nc);

		auto *NickServ = Config->GetClient("NickServ");
		u->SendMessage(NickServ, _("SSL certificate fingerprint accepted, you are now identified to \002%s\002."), nc->display.c_str());
		Log(NickServ) << u->GetMask() << " automatically identified for account " << nc->display << " via SSL certificate fingerprint";
	}

	void OnNickRegister(User *u, NickAlias *na, const Anope::string &pass) override
	{
		if (!Config->GetModule(this).Get<bool>("automatic", "yes") || !u || u->fingerprint.empty())
			return;

		auto *cl = certs.Require(na->nc);
		cl->AddCert(u->fingerprint);

		auto *NickServ = Config->GetClient("NickServ");
		u->SendMessage(NickServ, _("Your SSL certificate fingerprint \002%s\002 has been automatically added to your certificate list."), u->fingerprint.c_str());
	}

	EventReturn OnNickValidate(User *u, NickAlias *na) override
	{
		NSCertList *cl = certs.Get(na->nc);
		if (!u->fingerprint.empty() && cl && cl->FindCert(u->fingerprint))
		{
			if (!CanLogin(u, na->nc))
				return EVENT_CONTINUE;

			u->Identify(na);

			auto *NickServ = Config->GetClient("NickServ");
			u->SendMessage(NickServ, _("SSL certificate fingerprint accepted, you are now identified."));
			Log(NickServ) << u->GetMask() << " automatically identified for account " << na->nc->display << " via SSL certificate fingerprint";
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSCert)
