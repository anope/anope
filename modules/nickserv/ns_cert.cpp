// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/nickserv/cert.h"

#define NICKSERV_CERT_TYPE "NSCert"

struct NSCertInfo final
	: NickServ::Cert
	, Serializable
{
	NSCertInfo(Extensible *ext)
		: Serializable(NICKSERV_CERT_TYPE)
	{
		account = anope_dynamic_static_cast<NickCore *>(ext);
	}
};

static Anope::unordered_map<NSCertInfo *> certmap;

struct CertServiceImpl final
	: NickServ::CertService
{
	CertServiceImpl(Module *o)
		: NickServ::CertService(o)
	{
	}

	NickCore *FindAccountFromCert(const Anope::string &cert) override
	{
		auto it = certmap.find(cert);
		if (it != certmap.end())
			return it->second->account;
		return NULL;
	}

	void ReplaceCert(const Anope::string &oldcert, const Anope::string &newcert) override
	{
		auto *nc = FindAccountFromCert(oldcert);
		if (!nc)
			return;

		auto *cl = nc->GetExt<NickServ::CertList>(NICKSERV_CERT_EXT);
		if (cl)
			cl->ReplaceCert(oldcert, newcert);
	}
};

struct NSCertListImpl final
	: NickServ::CertList
{
	friend class NSCertInfoType;

	Serialize::Reference<NickCore> nc;
	std::vector<NSCertInfo *> certs;

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
	NickServ::Cert *AddCert(const Anope::string &entry) override
	{
		auto *cert = new NSCertInfo(nc);
		cert->fingerprint = entry;

		this->certs.push_back(cert);
		certmap[entry] = cert;
		FOREACH_MOD(OnNickAddCert, (this->nc, cert));
		return cert;
	}

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	NickServ::Cert *GetCert(unsigned entry) const override
	{
		if (entry >= this->certs.size())
			return nullptr;

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
		auto it = std::find_if(this->certs.begin(), this->certs.end(), [&entry](const NSCertInfo *cert) {
			return cert->fingerprint == entry;
		});
		return it != this->certs.end();
	}

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	void EraseCert(const Anope::string &entry) override
	{
		auto it = std::find_if(this->certs.begin(), this->certs.end(), [&entry](const NSCertInfo *cert) {
			return cert->fingerprint == entry;
		});
		if (it != this->certs.end())
		{
			FOREACH_MOD(OnNickEraseCert, (this->nc, *it));
			certmap.erase(entry);

			delete *it;
			this->certs.erase(it);
		}
	}

	void ReplaceCert(const Anope::string &oldentry, const Anope::string &newentry) override
	{
		auto oldit = std::find_if(this->certs.begin(), this->certs.end(), [&oldentry](const NSCertInfo *cert) {
			return cert->fingerprint == oldentry;
		});
		if (oldit == this->certs.end())
			return; // We can't replace a non-existent cert.

		FOREACH_MOD(OnNickEraseCert, (this->nc, *oldit));
		certmap.erase(oldentry);

		auto newit = std::find_if(this->certs.begin(), this->certs.end(), [&newentry](const NSCertInfo *cert) {
			return cert->fingerprint == newentry;
		});
		if (newit != this->certs.end())
		{
			// The cert we're upgrading to already exists.
			delete *newit;
			this->certs.erase(newit);
			return;
		}

		auto *cert = *newit;
		cert->fingerprint = newentry;
		certmap[newentry] = cert;
		FOREACH_MOD(OnNickAddCert, (this->nc, cert));
	}

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	void ClearCert() override
	{
		FOREACH_MOD(OnNickClearCert, (this->nc));
		for (const auto *cert : certs)
		{
			delete cert;
			certmap.erase(cert->fingerprint);
		}
		this->certs.clear();
	}

	void Check() override
	{
		if (this->certs.empty())
			nc->Shrink<NickServ::CertList>(NICKSERV_CERT_EXT);
	}

	struct ExtensibleItem final
		: ::ExtensibleItem<NSCertListImpl>
	{
		ExtensibleItem(Module *m, const Anope::string &ename) : ::ExtensibleItem<NSCertListImpl>(m, ename) { }

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			// Begin 2.0 compatibility.
			if (s->GetSerializableType()->GetName() != NICKCORE_TYPE)
				return;

			auto *nc = anope_dynamic_static_cast<NickCore *>(e);
			auto *cl = this->Require(nc);

			// Delete the old cert list.
			for (const auto *cert : cl->certs)
			{
				delete cert;
				certmap.erase(cert->fingerprint);
			}
			cl->certs.clear();

			// Add the new cert list
			spacesepstream sep(data.Load("cert"));
			for (Anope::string buf; sep.GetToken(buf); )
			{
				auto *cert = new NSCertInfo(e);
				cert->fingerprint = buf;
				cl->certs.push_back(cert);
				certmap[buf] = cert;
			}
			// End 2.0 compatibility.
		}
	};
};


class NSCertInfoType final
	: public Serialize::Type
{
public:
	NSCertInfoType()
		: Serialize::Type(NICKSERV_CERT_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *cert = static_cast<const NSCertInfo *>(obj);
		data.Store("account", cert->account->GetId());
		data.Store("created", cert->created);
		data.Store("creator", cert->creator);
		data.Store("description", cert->description);
		data.Store("fingerprint", cert->fingerprint);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		auto *nc = NickCore::FindId(data.Load<uint64_t>("account"));
		if (!nc)
			return nullptr; // Missing user.

		NSCertInfo *cert;
		if (obj)
			cert = anope_dynamic_static_cast<NSCertInfo *>(obj);
		else
			cert = new NSCertInfo(nc);

		cert->created = data.Load<time_t>("created");
		cert->creator = data.Load("creator");
		cert->description = data.Load("description");
		cert->fingerprint = data.Load("fingerprint");

		if (!obj)
		{
			auto *cl = nc->Require<NSCertListImpl>(NICKSERV_CERT_EXT);
			cl->certs.push_back(cert);
			certmap[cert->fingerprint] = cert;
		}

		return cert;
	}
};

class CommandNSCert final
	: public Command
{
private:
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		auto *nc = FindTarget(source, params.size() == 3 ? params[1] : "", true);
		if (!nc)
			return;

		const auto certfp = FindFingerprint(source, params, nc, false);
		if (certfp.empty())
			return;

		auto *cl = nc->Require<NickServ::CertList>(NICKSERV_CERT_EXT);
		const auto max = Config->GetModule(this->owner).Get<unsigned>("max", "5");
		if (cl->GetCertCount() >= max)
		{
			source.Reply(max, N_("The maximum of %u certificate entry has been reached.", "The maximum of %u certificate entries has been reached."),
				max);
			return;
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

		auto *cert = cl->AddCert(certfp);
		cert->created = Anope::CurTime;
		cert->creator = source.GetNick();

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD certificate fingerprint " << certfp << " to " << nc->display;
		source.Reply(_("\002%s\002 added to %s's certificate list."), certfp.c_str(), nc->display.c_str());
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		auto *nc = FindTarget(source, params.size() == 3 ? params[1] : "", true);
		if (!nc)
			return;

		const auto certfp = FindFingerprint(source, params, nc, true);
		if (certfp.empty())
			return;

		auto *cl = nc->Require<NickServ::CertList>(NICKSERV_CERT_EXT);
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

	void DoList(CommandSource &source, const std::vector<Anope::string> &params, bool full)
	{
		auto *nc = FindTarget(source, params.size() > 1 ? params[1] : "", false);
		if (!nc)
			return;

		auto *cl = nc->GetExt<NickServ::CertList>(NICKSERV_CERT_EXT);
		if (!cl || !cl->GetCertCount())
		{
			source.Reply(_("%s's certificate list is empty."), nc->display.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Fingerprint"));
		if (full)
		{
			list.AddColumn(_("Creator")).AddColumn(_("Created"));
			list.SetFlexible([](ListFormatter::ListEntry &row)
			{
				return row["Description"].empty()
					? _("\002{fingerprint}\002 -- created by {creator} at {created}")
					: _("\002{fingerprint}\002 -- created by {creator} at {created} ({description})");
			});
		}
		else
		{
			list.SetFlexible([](ListFormatter::ListEntry &row)
			{
				return row["Description"].empty()
					? _("\002{fingerprint}\002")
					: _("\002{fingerprint}\002 ({description})");
			});
		}
		list.AddColumn(_("Description"));

		for (unsigned i = 0; i < cl->GetCertCount(); ++i)
		{
			auto *cert = cl->GetCert(i);
			ListFormatter::ListEntry entry;
			entry["Fingerprint"] = cert->fingerprint;
			entry["Description"] = cert->description;
			if (full)
			{
				entry["Created"] = cert->created
					? Anope::strftime(cert->created, nullptr, true)
					: TIME_UNKNOWN;

				entry["Creator"] = cert->creator.empty()
					? TIME_UNKNOWN
					: cert->creator;
			}
			list.AddEntry(entry);
		}

		source.Reply(_("Certificate list for %s:"), nc->display.c_str());
		list.SendTo(source);
	}

	Anope::string FindFingerprint(CommandSource &source, const std::vector<Anope::string> &params, const NickCore *nc, bool del)
	{
		if (source.GetAccount() != nc || del)
		{
			if (params.size() > 1)
				return params.back();

			this->OnSyntaxError(source, params[0]);
			return "";
		}

		auto *u = source.GetUser();
		if (u && !u->fingerprint.empty())
			return u->fingerprint;

		source.Reply(_("You are not using a client certificate."));
		return "";
	}

	NickCore *FindTarget(CommandSource &source, const Anope::string &nick, bool modify)
	{
		if (!nick.empty())
		{
			const auto *na = NickAlias::Find(nick);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return nullptr;
			}

			NickCore *nc = na->nc;
			if (nc != source.GetAccount() && !source.HasPriv("nickserv/cert"))
			{
				source.Reply(ACCESS_DENIED);
				return nullptr;
			}

			if (modify)
			{
				if (nc->HasExt("NS_SUSPENDED"))
				{
					source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
					return nullptr;
				}
				if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.GetAccount() != nc && nc->IsServicesOper())
				{
					source.Reply(_("You may view but not modify the certificate list of other Services Operators."));
					return nullptr;
				}
			}
			return nc;
		}
		return source.nc;
	}

public:
	CommandNSCert(Module *creator) : Command(creator, "nickserv/cert", 1, 3)
	{
		this->SetDesc(_("Modify the nickname client certificate list"));
		this->SetSyntax(_("ADD [\037nickname\037 \037fingerprint\037]"));
		this->SetSyntax(_("DEL [\037nickname\037] \037fingerprint\037"));
		this->SetSyntax(_("LIST [\037nickname\037]"));
		this->SetSyntax(_("VIEW [\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		if (cmd.equals_ci("LIST"))
			return this->DoList(source, params, false);
		if (cmd.equals_ci("VIEW"))
			return this->DoList(source, params, true);
		else if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Modifies or displays the certificate list for your nick. If you connect to IRC and "
			"provide a client certificate with a matching fingerprint in the cert list, you will "
			"be automatically identified to services. Services Operators may provide a nick to "
			"modify other users' certificate lists."
		));

		ExampleWrapper examples;

		examples.AddEntry("ADD", _(
			"Adds your current fingerprint to your certificate list."
		));
		examples.AddEntry(_("ADD \037nickname\037 \037fingerprint\037"), _(
			"Adds the specified \037fingerprint\037 to the certificate list of \037nickname\037."
		), "nickserv/cert");

		examples.AddEntry(_("DEL \037fingerprint\037"), _(
			"Removes the specified \037fingerprint\037 from your certificate list."
		));
		examples.AddEntry(_("DEL \037nickname\037 \037fingerprint\037"), _(
			"Removes the specified \037fingerprint\037 from the certificate list of "
			"\037nickname\037."
		), "nickserv/cert");

		examples.AddEntry("LIST", _(
			"Displays your current certificate list."
		));
		examples.AddEntry(_("LIST \037nickname\037"), _(
			"Displays the current certificate list of \037nickname\037."
		), "nickserv/cert");

		examples.AddEntry("VIEW", _(
			"Displays your current certificate list as well the details about who added each entry "
			"and when they added it."
		));
		examples.AddEntry(_("VIEW \037nickname\037"), _(
			"Displays the current certificate list of \037nickname\037 as well as the details "
			"about who added each entry and when they added it."
		), "nickserv/cert");

		examples.SendTo(source);

		return true;
	}
};

class CommandNSSetAutologin
	: public Command
{
public:
	CommandNSSetAutologin(Module *creator, const Anope::string &sname = "nickserv/set/autologin", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether you should automatically be logged in when you connect using a known SSL certificate."));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autologin for " << na->nc->display;
			nc->Extend<bool>("AUTOLOGIN");
			source.Reply(_("%s will now be automatically logged in when they connect using a known SSL certificate."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autologin for " << na->nc->display;
			nc->Shrink<bool>("AUTOLOGIN");
			source.Reply(_("%s will now not be automatically logged in when they connect using a known SSL certificate."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOLOGIN");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(
			_(
				"Sets whether you should automatically be logged in when you connect using a known "
				"SSL certificate. You can configure your SSL certificate using the \002%s\002 "
				"command."
			),
			source.service->GetQueryCommand("nickserv/cert").c_str()
		);
		return true;
	}
};

class CommandNSSASetAutologin final
	: public CommandNSSetAutologin
{
public:
	CommandNSSASetAutologin(Module *creator)
		: CommandNSSetAutologin(creator, "nickserv/saset/autologin", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(
			_(
				"Sets whether the given nickname should automatically be logged in when they "
				"connect using a known SSL certificate. You can configure their SSL certificate "
				"using the \002%s\002 command."
			),
			source.service->GetQueryCommand("nickserv/cert").c_str()
		);
		return true;
	}
};

class NSCert final
	: public Module
{
private:
	CommandNSCert commandnscert;
	CommandNSSetAutologin commandnssetautologin;
	CommandNSSASetAutologin commandnssasetautologin;
	NSCertListImpl::ExtensibleItem certs;
	CertServiceImpl cs;
	NSCertInfoType cert_type;

	bool CanLogin(User *u, NickCore *nc)
	{
		if (!nc || nc->HasExt("NS_SUSPENDED"))
			return false; // Account suspended.

		if (!nc->HasExt("AUTOLOGIN"))
			return false; // Autologin disabled.

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
	NSCert(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnscert(this)
		, commandnssetautologin(this)
		, commandnssasetautologin(this)
		, certs(this, NICKSERV_CERT_EXT)
		, cs(this)
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
		Log(NickServ) << u->GetMask() << " automatically identified for account " << nc->display << " via SSL certificate fingerprint " << u->fingerprint;
	}

	void OnNickRegister(User *u, NickAlias *na, const Anope::string &pass) override
	{
		if (!Config->GetModule(this).Get<bool>("automatic", "yes") || !u || u->fingerprint.empty())
			return;

		auto *cl = certs.Require(na->nc);
		auto *cert = cl->AddCert(u->fingerprint);
		cert->created = Anope::CurTime;
		cert->creator = u->nick;

		auto *NickServ = Config->GetClient("NickServ");
		u->SendMessage(NickServ, _("Your SSL certificate fingerprint \002%s\002 has been automatically added to your certificate list."), u->fingerprint.c_str());
	}

	EventReturn OnNickValidate(User *u, NickAlias *na) override
	{
		auto *cl = certs.Get(na->nc);
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
