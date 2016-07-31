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

#pragma once

class LDAPException : public ModuleException
{
 public:
	LDAPException(const Anope::string &reason) : ModuleException(reason) { }

	virtual ~LDAPException() throw() { }
};

struct LDAPModification
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

struct LDAPAttributes : public std::map<Anope::string, std::vector<Anope::string> >
{
	size_t size(const Anope::string &attr) const
	{
		const std::vector<Anope::string>& array = this->getArray(attr);
		return array.size();
	}

	const std::vector<Anope::string> keys() const
	{
		std::vector<Anope::string> k;
		for (const_iterator it = this->begin(), it_end = this->end(); it != it_end; ++it)
			k.push_back(it->first);
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

struct LDAPResult
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
	virtual ~LDAPInterface() { }

	virtual void OnResult(const LDAPResult &r) anope_abstract;
	virtual void OnError(const LDAPResult &err) anope_abstract;
	virtual void OnDelete() { }
};

class LDAPProvider : public Service
{
 public:
	LDAPProvider(Module *c, const Anope::string &n) : Service(c, "LDAPProvider", n) { }

	/** Attempt to bind to the LDAP server as an admin
	 * @param i The LDAPInterface the result is sent to
	 */
	virtual void BindAsAdmin(LDAPInterface *i) anope_abstract;

	/** Bind to LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param who The binddn
	 * @param pass The password
	 */
	virtual void Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass) anope_abstract;

	/** Search ldap for the specified filter
	 * @param i The LDAPInterface the result is sent to
	 * @param base The base DN to search
	 * @param filter The filter to apply
	 */
	virtual void Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter) anope_abstract;

	/** Add an entry to LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param dn The dn of the entry to add
	 * @param attributes The attributes
	 */
	virtual void Add(LDAPInterface *i, const Anope::string &dn, LDAPMods &attributes) anope_abstract;

	/** Delete an entry from LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param dn The dn of the entry to delete
	 */
	virtual void Del(LDAPInterface *i, const Anope::string &dn) anope_abstract;

	/** Modify an existing entry in LDAP
	 * @param i The LDAPInterface the result is sent to
	 * @param base The base DN to modify
	 * @param attributes The attributes to modify
	 */
	virtual void Modify(LDAPInterface *i, const Anope::string &base, LDAPMods &attributes) anope_abstract;
};
