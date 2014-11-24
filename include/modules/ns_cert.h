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

class NSCertEntry;

class CertService : public Service
{
 public:
	CertService(Module *c) : Service(c, "CertService", "certs") { }

	virtual NickServ::Account* FindAccountFromCert(const Anope::string &cert) anope_abstract;

	virtual bool Matches(User *, NickServ::Account *) anope_abstract;

	virtual NSCertEntry *FindCert(const std::vector<NSCertEntry *> &cl, const Anope::string &certfp) anope_abstract;
};

static ServiceReference<CertService> certservice("CertService", "certs");

class NSCertEntry : public Serialize::Object
{
 public:
	using Serialize::Object::Object;

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetCert() anope_abstract;
	virtual void SetCert(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<NSCertEntry> certentry("NSCertEntry");

namespace Event
{
	struct CoreExport NickCertEvents : Events
	{
		/** Called when a user adds an entry to their cert list
		 * @param nc The nick
		 * @param entry The entry
		 */
		virtual void OnNickAddCert(NickServ::Account *nc, const Anope::string &entry) anope_abstract;

		/** Called from NickServ::Account::EraseCert()
		 * @param nc pointer to the NickServ::Account
		 * @param entry The fingerprint
		 */
		virtual void OnNickEraseCert(NickServ::Account *nc, const Anope::string &entry) anope_abstract;

		/** called from NickServ::Account::ClearCert()
		 * @param nc pointer to the NickServ::Account
		 */
		virtual void OnNickClearCert(NickServ::Account *nc) anope_abstract;
	};
}

