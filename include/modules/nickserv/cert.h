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

#pragma once

#define NICKSERV_CERT_EXT "certificates"
#define NICKSERV_CERT_SERVICE "NickServ::CertService"

namespace NickServ
{
	struct Cert;
	class CertList;
	class CertService;

	ServiceReference<CertService> cert_service(NICKSERV_CERT_SERVICE, NICKSERV_CERT_SERVICE);
}

struct NickServ::Cert
{
	/** The account this cert is for. */
	Serialize::Reference<NickCore> account;

	/** The time at which this certificate was created. */
	time_t created = 0;

	/** The user who created this certificate. */
	Anope::string creator;

	/** If non-empty then a description of the certificate. */
	Anope::string description;

	/** The TLS fingerprint for the certificate. */
	Anope::string fingerprint;
};

class NickServ::CertList
{
protected:
	CertList() = default;

public:
	virtual ~CertList() = default;

	/** Add an entry to the nick's certificate list
	 *
	 * @param entry The fingerprint to add to the cert list
	 *
	 * Adds a new entry into the cert list.
	 */
	virtual NickServ::Cert *AddCert(const Anope::string &entry) = 0;

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	virtual NickServ::Cert *GetCert(unsigned entry) const = 0;

	virtual unsigned GetCertCount() const = 0;

	/** Find an entry in the nick's cert list
	 *
	 * @param entry The fingerprint to search for
	 * @return True if the fingerprint is found in the cert list, false otherwise
	 *
	 * Search for an fingerprint within the cert list.
	 */
	virtual bool FindCert(const Anope::string &entry) const = 0;

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	virtual void EraseCert(const Anope::string &entry) = 0;

	/** Replaces a fingerprint in the nick's certificate list
	 *
	 * @param oldentry The old fingerprint to remove
	 * @param newentry The new fingerprint to add
	 *
	 * Replaces the specified fingerprint in the cert list.
	 */
	virtual void ReplaceCert(const Anope::string &oldentry, const Anope::string &newentry) = 0;

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	virtual void ClearCert() = 0;

	virtual void Check() = 0;
};

class NickServ::CertService
	: public ::Service
{
public:
	CertService(Module *m)
		: ::Service(m, NICKSERV_CERT_SERVICE, NICKSERV_CERT_SERVICE)
	{
	}

	virtual NickCore *FindAccountFromCert(const Anope::string &cert) = 0;
	virtual void ReplaceCert(const Anope::string &oldcert, const Anope::string &newcert) = 0;
};
