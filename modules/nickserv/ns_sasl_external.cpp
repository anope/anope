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
#include "modules/nickserv/sasl.h"

class External final
	: public SASL::Mechanism
{
private:
	struct Session final
		: SASL::Session
	{
		std::vector<Anope::string> certs;

		Session(SASL::Mechanism *m, const Anope::string &u)
			: SASL::Session(m, u)
		{
		}
	};

public:
	External(Module *o)
		: SASL::Mechanism(o, "EXTERNAL")
	{
	}

	Session *CreateSession(const Anope::string &uid) override
	{
		return new Session(this, uid);
	}

	bool ProcessMessage(SASL::Session *sess, const SASL::Message &m) override
	{
		Session *mysess = anope_dynamic_static_cast<Session *>(sess);

		if (m.type == "S")
		{
			if (m.data.size() < 2)
				return false; // No client certs.

			mysess->certs.assign(m.data.begin() + 1, m.data.end());
			SASL::service->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			if (!NickServ::cert_service || mysess->certs.empty())
				return false;

			for (auto it = mysess->certs.begin(); it != mysess->certs.end(); ++it)
			{
				auto *nc = NickServ::cert_service->FindAccountFromCert(*it);
				if (nc && !nc->HasExt("NS_SUSPENDED") && !nc->HasExt("UNCONFIRMED"))
				{
					// If we are using a fallback cert then upgrade it.
					if (it != mysess->certs.begin())
					{
						auto *cl = nc->GetExt<NickServ::CertList>(NICKSERV_CERT_EXT);
						if (cl)
							cl->ReplaceCert(*it, mysess->certs[0]);
					}

					Log(this->owner, "sasl", Config->GetClient("NickServ")) << sess->GetUserInfo() << " identified to account " << nc->display << " using SASL EXTERNAL";
					SASL::service->Succeed(sess, nc);
					delete sess;
					return true;
				}
			}

			Log(this->owner, "sasl", Config->GetClient("NickServ")) << sess->GetUserInfo() << " failed to identify using certificate " << mysess->certs.front() << " using SASL EXTERNAL";
			return false;
		}
		return true;
	}
};

class ModuleSASLExternal final
	: public Module
{
private:
	External external;

public:
	ModuleSASLExternal(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, external(this)
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("No CertFP");

		if (!SASL::protocol_interface)
			throw ModuleException("Your IRCd does not support SASL");
	}
};

MODULE_INIT(ModuleSASLExternal)
