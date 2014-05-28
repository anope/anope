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

struct NSCertList
{
 protected:
	NSCertList() { }
 public:
	virtual ~NSCertList() { }

	/** Add an entry to the nick's certificate list
	 *
	 * @param entry The fingerprint to add to the cert list
	 *
	 * Adds a new entry into the cert list.
	 */
	virtual void AddCert(const Anope::string &entry) anope_abstract;

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificaate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	virtual Anope::string GetCert(unsigned entry) const anope_abstract;

	virtual unsigned GetCertCount() const anope_abstract;

	/** Find an entry in the nick's cert list
	 *
	 * @param entry The fingerprint to search for
	 * @return True if the fingerprint is found in the cert list, false otherwise
	 *
	 * Search for an fingerprint within the cert list.
	 */
	virtual bool FindCert(const Anope::string &entry) const anope_abstract;

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	virtual void EraseCert(const Anope::string &entry) anope_abstract;

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	virtual void ClearCert() anope_abstract;

	virtual void Check() anope_abstract;
};

class CertService : public Service
{
 public:
	CertService(Module *c) : Service(c, "CertService", "certs") { }

	virtual NickServ::Account* FindAccountFromCert(const Anope::string &cert) anope_abstract;
};

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

