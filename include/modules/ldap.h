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

class DllExport LDAPException : public ModuleException
{
public:
	LDAPException(const Anope::string &reason) : ModuleException(reason) { }

	virtual ~LDAPException() noexcept = default;
};

struct LDAPModification final
{
	enum LDAPOperation
	{
		LDAP_ADD,
		LDAP_DEL,
		LDAP_REPLACE
	};

	LDAPOperation op;
	Anope::string name;
	std::vector<Anope::string> values;
};
typedef std::vector<LDAPModification> LDAPMods;

struct LDAPAttributes final
	: public std::map<Anope::string, std::vector<Anope::string>>
{
	size_t size(const Anope::string &attr) const
	{
		const std::vector<Anope::string>& array = this->getArray(attr);
		return array.size();
	}

	const std::vector<Anope::string> keys() const
	{
		std::vector<Anope::string> k;
		for (const auto &[key, _] : *this)
			k.push_back(key);
		return k;
	}

	const Anope::string &get(const Anope::string &attr) const
	{
		const std::vector<Anope::string>& array = this->getArray(attr);
		if (array.empty())
			throw LDAPException("Empty attribute " + attr + " in LDAPResult::get");
		return array[0];
	}

	const std::vector<Anope::string>& getArray(const Anope::string &attr) const
	{
		const_iterator it = this->find(attr);
		if (it == this->end())
			throw LDAPException("Unknown attribute " + attr + " in LDAPResult::getArray");
		return it->second;
	}
};

enum QueryType
{
	QUERY_UNKNOWN,
	QUERY_BIND,
	QUERY_SEARCH,
	QUERY_ADD,
	QUERY_DELETE,
	QUERY_MODIFY
};

struct LDAPResult final
{
	std::vector<LDAPAttributes> messages;
	Anope::string error;

	QueryType type;

	LDAPResult()
	{
		this->type = QUERY_UNKNOWN;
	}

	size_t size() const
	{
		return this->messages.size();
	}

	bool empty() const
	{
		return this->messages.empty();
	}

	const LDAPAttributes &get(size_t sz) const
	{
		if (sz >= this->messages.size())
			throw LDAPException("Index out of range");
		return this->messages[sz];
	}

	const Anope::string &getError() const
	{
		return this->error;
	}
};

class LDAPInterface
{
public:
	Module *owner;

	LDAPInterface(Module *m) : owner(m) { }
	virtual ~LDAPInterface() = default;

	virtual void OnResult(const LDAPResult &r) = 0;
	virtual void OnError(const LDAPResult &err) = 0;
	virtual void OnDelete() { }
};

class LDAPProvider
	: public Service
{
public:
	LDAPProvider(Module *c, const Anope::string &n) : Service(c, "LDAPProvider", n) { }

	/** Attempt to bind to the LDAP server as an admin
	 * @param i The LDAPInterface the result is sent to
	 */
	virtual void BindAsAdmin(LDAPInterface *i) = 0;

	/** Bind to LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param who The binddn
	 * @param pass The password
	 */
	virtual void Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass) = 0;

	/** Search ldap for the specified filter
	 * @param i The LDAPInterface the result is sent to
	 * @param base The base DN to search
	 * @param filter The filter to apply
	 */
	virtual void Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter) = 0;

	/** Add an entry to LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param dn The dn of the entry to add
	 * @param attributes The attributes
	 */
	virtual void Add(LDAPInterface *i, const Anope::string &dn, LDAPMods &attributes) = 0;

	/** Delete an entry from LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param dn The dn of the entry to delete
	 */
	virtual void Del(LDAPInterface *i, const Anope::string &dn) = 0;

	/** Modify an existing entry in LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param base The base DN to modify
	 * @param attributes The attributes to modify
	 */
	virtual void Modify(LDAPInterface *i, const Anope::string &base, LDAPMods &attributes) = 0;

	/** Escapes a LDAP string for use in a DN.
	 * @param str The string to escape.
	 */
	virtual Anope::string EscapeDN(const Anope::string &str) const = 0;

	/** Escapes a LDAP string for use in a search filter.
	 * @param str The string to escape.
	 */
	virtual Anope::string EscapeSF(const Anope::string &str) const = 0;
};
