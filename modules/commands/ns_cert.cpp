/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/ns_cert.h"

struct NSCertListImpl : NSCertList
{
	Serialize::Reference<NickCore> nc;
	std::vector<Anope::string> certs;

 public:
 	NSCertListImpl(Extensible *obj) : nc(anope_dynamic_static_cast<NickCore *>(obj)) { }

	/** Add an entry to the nick's certificate list
	 *
	 * @param entry The fingerprint to add to the cert list
	 *
	 * Adds a new entry into the cert list.
	 */
	void AddCert(const Anope::string &entry) anope_override
	{
		this->certs.push_back(entry);
		FOREACH_MOD(OnNickAddCert, (this->nc, entry));
	}

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificaate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	Anope::string GetCert(unsigned entry) const anope_override
	{
		if (entry >= this->certs.size())
			return "";
		return this->certs[entry];
	}

	unsigned GetCertCount() const anope_override
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
	bool FindCert(const Anope::string &entry) const anope_override
	{
		return std::find(this->certs.begin(), this->certs.end(), entry) != this->certs.end();
	}

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	void EraseCert(const Anope::string &entry) anope_override
	{
		std::vector<Anope::string>::iterator it = std::find(this->certs.begin(), this->certs.end(), entry);
		if (it != this->certs.end())
		{
			FOREACH_MOD(OnNickEraseCert, (this->nc, entry));
			this->certs.erase(it);
		}
	}

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	void ClearCert() anope_override
	{
		FOREACH_MOD(OnNickClearCert, (this->nc));
		this->certs.clear();
	}

	void Check() anope_override
	{
		if (this->certs.empty())
			nc->Shrink<NSCertList>("certificates");
	}

	struct ExtensibleItem : ::ExtensibleItem<NSCertListImpl>
	{
		ExtensibleItem(Module *m, const Anope::string &ename) : ::ExtensibleItem<NSCertListImpl>(m, ename) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const anope_override
		{
			if (s->GetSerializableType()->GetName() != "NickCore")
				return;

			const NickCore *nc = anope_dynamic_static_cast<const NickCore *>(e);
			NSCertList *certs = this->Get(nc);
			if (certs == NULL || !certs->GetCertCount())
				return;

			for (unsigned i = 0; i < certs->GetCertCount(); ++i)
				data["cert"] << certs->GetCert(i) << " ";
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) anope_override
		{
			if (s->GetSerializableType()->GetName() != "NickCore")
				return;

			NickCore *nc = anope_dynamic_static_cast<NickCore *>(e);
			NSCertListImpl *certs = this->Require(nc);

			Anope::string buf;
			data["cert"] >> buf;
			spacesepstream sep(buf);
			certs->certs.clear();
			while (sep.GetToken(buf))
				certs->certs.push_back(buf);
		}
	};
};

class CommandNSCert : public Command
{
 private:
	void DoServAdminList(CommandSource &source, const NickCore *nc)
	{
		NSCertList *cl = nc->GetExt<NSCertList>("certificates");
		
		if (!cl || !cl->GetCertCount())
		{
			source.Reply(_("Certificate list for \002%s\002 is empty."), nc->display.c_str());
			return;
		}

		if (nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
			return;
		}

		ListFormatter list;
		list.AddColumn("Certificate");

		for (unsigned i = 0; i < cl->GetCertCount(); ++i)
		{
			const Anope::string &fingerprint = cl->GetCert(i);
			ListFormatter::ListEntry entry;
			entry["Certificate"] = fingerprint;
			list.AddEntry(entry);
		}

		source.Reply(_("Certificate list for \002%s\002:"), nc->display.c_str());

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	void DoAdd(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{
		NSCertList *cl = nc->Require<NSCertList>("certificates");

		if (cl->GetCertCount() >= Config->GetModule(this->owner)->Get<unsigned>("accessmax"))
		{
			source.Reply(_("Sorry, you can only have %d certificate entries for a nickname."), Config->GetModule(this->owner)->Get<unsigned>("accessmax"));
			return;
		}

		if (mask.empty())
		{
			if (source.GetUser() && !source.GetUser()->fingerprint.empty() && !cl->FindCert(source.GetUser()->fingerprint))
			{
				cl->AddCert(source.GetUser()->fingerprint);
				source.Reply(_("\002%s\002 added to your certificate list."), source.GetUser()->fingerprint.c_str());
			}
			else
				this->OnSyntaxError(source, "ADD");

			return;
		}

		if (cl->FindCert(mask))
		{
			source.Reply(_("Fingerprint \002%s\002 already present on your certificate list."), mask.c_str());
			return;
		}

		cl->AddCert(mask);
		source.Reply(_("\002%s\002 added to your certificate list."), mask.c_str());
	}

	void DoDel(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{
		NSCertList *cl = nc->Require<NSCertList>("certificates");

		if (mask.empty())
		{
			if (source.GetUser() && !source.GetUser()->fingerprint.empty() && cl->FindCert(source.GetUser()->fingerprint))
			{
				cl->EraseCert(source.GetUser()->fingerprint);
				source.Reply(_("\002%s\002 deleted from your certificate list."), source.GetUser()->fingerprint.c_str());
			}
			else
				this->OnSyntaxError(source, "DEL");

			return;
		}

		if (!cl->FindCert(mask))
		{
			source.Reply(_("\002%s\002 not found on your certificate list."), mask.c_str());
			return;
		}

		source.Reply(_("\002%s\002 deleted from your certificate list."), mask.c_str());
		cl->EraseCert(mask);
		cl->Check();
	}

	void DoList(CommandSource &source, const NickCore *nc)
	{
		NSCertList *cl = nc->GetExt<NSCertList>("certificates");

		if (!cl || !cl->GetCertCount())
		{
			source.Reply(_("Your certificate list is empty."));
			return;
		}

		ListFormatter list;
		list.AddColumn("Certificate");

		for (unsigned i = 0; i < cl->GetCertCount(); ++i)
		{
			ListFormatter::ListEntry entry;
			entry["Certificate"] = cl->GetCert(i);
			list.AddEntry(entry);
		}

		source.Reply(_("Certificate list:"));
		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

 public:
	CommandNSCert(Module *creator) : Command(creator, "nickserv/cert", 1, 2)
	{
		this->SetDesc("Modify the nickname client certificate list");
		this->SetSyntax("ADD \037fingerprint\037");
		this->SetSyntax("DEL \037fingerprint\037");
		this->SetSyntax("LIST");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &cmd = params[0];
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		const NickAlias *na;
		if (cmd.equals_ci("LIST") && source.IsServicesOper() && !mask.empty() && (na = NickAlias::Find(mask)))
			return this->DoServAdminList(source, na->nc);

		NickCore *nc = source.nc;

		if (source.nc->HasExt("NS_SUSPENDED"))
			source.Reply(NICK_X_SUSPENDED, source.nc->display.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, mask);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else
			this->OnSyntaxError(source, cmd);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Modifies or displays the certificate list for your nick.\n"
				"If you connect to IRC and provide a client certificate with a\n"
				"matching fingerprint in the cert list, your nick will be\n"
				"automatically identified to services.\n"
				" \n"));
		source.Reply(_("Examples:\n"
				" \n"
				"    \002CERT ADD <fingerprint>\002\n"
				"        Adds this fingerprint to the certificate list and\n"
				"        automatically identifies you when you connect to IRC\n"
				"        using this certificate.\n"
				" \n"
				"    \002CERT DEL <fingerprint>\002\n"
				"        Reverses the previous command.\n"
				" \n"
				"    \002CERT LIST\002\n"
				"        Displays the current certificate list."));
		return true;
	}
};

class NSCert : public Module
{
	CommandNSCert commandnscert;
	NSCertListImpl::ExtensibleItem certs;

 public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnscert(this), certs(this, "certificates")
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("Your IRCd does not support ssl client certificates");
	}

	void OnFingerprint(User *u) anope_override
	{
		NickAlias *na = NickAlias::Find(u->nick);
		BotInfo *NickServ = Config->GetClient("NickServ");
		if (!NickServ || !na)
			return;
		if (u->IsIdentified() && u->Account() == na->nc)
			return;
		if (na->nc->HasExt("NS_SUSPENDED"))
			return;

		NSCertList *cl = certs.Get(na->nc);
		if (!cl || !cl->FindCert(u->fingerprint))
			return;

		u->Identify(na);
		u->SendMessage(NickServ, _("SSL Fingerprint accepted. You are now identified."));
		Log(u) << "automatically identified for account " << na->nc->display << " via SSL certificate fingerprint";
	}

	EventReturn OnNickValidate(User *u, NickAlias *na) anope_override
	{
		NSCertList *cl = certs.Get(na->nc);
		if (!u->fingerprint.empty() && cl && cl->FindCert(u->fingerprint))
		{
			u->Identify(na);
			u->SendMessage(Config->GetClient("NickServ"), _("SSL fingerprint accepted, you are now identified."));
			Log(u) << "automatically identified for account " << na->nc->display << " via SSL fingerprint.";
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSCert)
