/*
 * Anope IRC Services
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2009-2017 Anope Team <team@anope.org>
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

#include "services.h"
#include "serialize.h"

class Oper : public Serialize::Object
{
	friend class OperBlockType;

	Serialize::Storage<Anope::string> name, password, certfp, host, vhost, type;
	Serialize::Storage<bool> require_oper;

 public:
	static constexpr const char *const NAME = "oper";

	using Serialize::Object::Object;

	Anope::string GetName();
	void SetName(const Anope::string &);

	Anope::string GetPassword();
	void SetPassword(const Anope::string &);

	Anope::string GetCertFP();
	void SetCertFP(const Anope::string &);

	Anope::string GetHost();
	void SetHost(const Anope::string &);

	Anope::string GetVhost();
	void SetVhost(const Anope::string &);

	OperType *GetType();
	void SetType(OperType *);

	bool GetRequireOper();
	void SetRequireOper(const bool &);

	bool HasCommand(const Anope::string &cmdstr);
	bool HasPriv(const Anope::string &cmdstr);

	/** Find an oper block by name
	 * @param name The name
	 * @return the oper block
	 */
	static Oper *Find(const Anope::string &name);
};

class OperBlockType : public Serialize::Type<Oper>
{
 public:
	Serialize::Field<Oper, Anope::string> name, password, certfp, host, vhost, type;
	Serialize::Field<Oper, bool> require_oper;

	OperBlockType() : Serialize::Type<Oper>(nullptr)
		, name(this, "name", &Oper::name)
		, password(this, "password", &Oper::password)
		, certfp(this, "certfp", &Oper::certfp)
		, host(this, "host", &Oper::host)
		, vhost(this, "vhost", &Oper::vhost)
		, type(this, "type", &Oper::type)
		, require_oper(this, "require_oper", &Oper::require_oper)
	{
	}
};

class CoreExport OperType
{
 private:
	/** The name of this opertype, e.g. "sra".
	 */
	Anope::string name;

	/** Privs that this opertype may use, e.g. 'users/auspex'.
	 * This *must* be std::list, see commands comment for details.
	 */
	std::list<Anope::string> privs;

	/** Commands this user may execute, e.g:
	 * botserv/set/ *, botserv/set/private, botserv/ *
	 * et cetera.
	 *
	 * This *must* be std::list, not std::map, because
	 * we support full globbing here. This shouldn't be a problem
	 * as we don't invoke it often.
	 */
	std::list<Anope::string> commands;

	/** Set of opertypes we inherit from
	 */
	std::set<OperType *> inheritances;
 public:
 	/** Modes to set when someone identifys using this opertype
	 */
	Anope::string modes;

	/** Find an oper type by name
	 * @param name The name
	 * @return The oper type
	 */
	static OperType *Find(const Anope::string &name);

	/** Create a new opertype of the given name.
	 * @param nname The opertype name, e.g. "sra".
	 */
	OperType(const Anope::string &nname);

	/** Check whether this opertype has access to run the given command string.
	 * @param cmdstr The string to check, e.g. botserv/set/private.
	 * @return True if this opertype may run the specified command, false otherwise.
	 */
	bool HasCommand(const Anope::string &cmdstr) const;

	/** Check whether this opertype has access to the given special permission.
	 * @param privstr The priv to check for, e.g. users/auspex.
	 * @return True if this opertype has the specified priv, false otherwise.
	 */
	bool HasPriv(const Anope::string &privstr) const;

	/** Add the specified command to this opertype.
	 * @param cmdstr The command mask to grant this opertype access to, e.g: nickserv/ *, chanserv/set/ *, botserv/set/private.
	 */
	void AddCommand(const Anope::string &cmdstr);

	/** Add the specified priv mask to this opertype.
	 * @param privstr The specified mask of privs to grant this opertype access to,  e.g. users/auspex, users/ *, etc.
	 */
	void AddPriv(const Anope::string &privstr);

	/** Returns the name of this opertype.
	 */
	const Anope::string &GetName() const;

	/** Make this opertype inherit commands and privs from another opertype
	 * @param ot The opertype to inherit from
	 */
	void Inherits(OperType *ot);

	/** Gets the icommands for this opertype
	 * @return A list of commands
	 */
	const std::list<Anope::string> GetCommands() const;

	/** Gets the privileges for this opertype
	 * @return A list of privileges
	 */
	const std::list<Anope::string> GetPrivs() const;
};

