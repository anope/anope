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

#include "services.h"
#include "anope.h"
#include "opertype.h"
#include "config.h"

Anope::string Oper::GetName()
{
	return Get(&OperBlockType::name);
}

void Oper::SetName(const Anope::string &n)
{
	Set(&OperBlockType::name, n);
}

Anope::string Oper::GetPassword()
{
	return Get(&OperBlockType::password);
}

void Oper::SetPassword(const Anope::string &s)
{
	Set(&OperBlockType::password, s);
}

Anope::string Oper::GetCertFP()
{
	return Get(&OperBlockType::certfp);
}

void Oper::SetCertFP(const Anope::string &s)
{
	Set(&OperBlockType::certfp, s);
}

Anope::string Oper::GetHost()
{
	return Get(&OperBlockType::host);
}

void Oper::SetHost(const Anope::string &h)
{
	Set(&OperBlockType::host, h);
}

Anope::string Oper::GetVhost()
{
	return Get(&OperBlockType::vhost);
}

void Oper::SetVhost(const Anope::string &s)
{
	Set(&OperBlockType::vhost, s);
}

OperType *Oper::GetType()
{
	return OperType::Find(Get(&OperBlockType::type));
}

void Oper::SetType(OperType *t)
{
	Set(&OperBlockType::type, t->GetName());
}

bool Oper::GetRequireOper()
{
	return Get(&OperBlockType::require_oper);
}

void Oper::SetRequireOper(const bool &b)
{
	Set(&OperBlockType::require_oper, b);
}

bool Oper::HasCommand(const Anope::string &cmdstr)
{
	OperType *type = GetType();
	if (type != nullptr)
		return type->HasCommand(cmdstr);
	return false;
}

bool Oper::HasPriv(const Anope::string &cmdstr)
{
	OperType *type = GetType();
	if (type != nullptr)
		return type->HasPriv(cmdstr);
	return false;
}

Oper *Oper::Find(const Anope::string &name)
{
	for (Oper *o : Serialize::GetObjects<Oper *>())
		if (o->GetName() == name)
			return o;

	return nullptr;
}

OperType *OperType::Find(const Anope::string &name)
{
	for (unsigned i = 0; i < Config->MyOperTypes.size(); ++i)
	{
		OperType *ot = Config->MyOperTypes[i];

		if (ot->GetName() == name)
			return ot;
	}

	return NULL;
}

OperType::OperType(const Anope::string &nname) : name(nname)
{
}

bool OperType::HasCommand(const Anope::string &cmdstr) const
{
	for (std::list<Anope::string>::const_iterator it = this->commands.begin(), it_end = this->commands.end(); it != it_end; ++it)
	{
		const Anope::string &s = *it;

		if (!s.find('~') && Anope::Match(cmdstr, s.substr(1)))
			return false;
		else if (Anope::Match(cmdstr, s))
			return true;
	}
	for (std::set<OperType *>::const_iterator iit = this->inheritances.begin(), iit_end = this->inheritances.end(); iit != iit_end; ++iit)
	{
		OperType *ot = *iit;

		if (ot->HasCommand(cmdstr))
			return true;
	}

	return false;
}

bool OperType::HasPriv(const Anope::string &privstr) const
{
	for (std::list<Anope::string>::const_iterator it = this->privs.begin(), it_end = this->privs.end(); it != it_end; ++it)
	{
		const Anope::string &s = *it;

		if (!s.find('~') && Anope::Match(privstr, s.substr(1)))
			return false;
		else if (Anope::Match(privstr, s))
			return true;
	}
	for (std::set<OperType *>::const_iterator iit = this->inheritances.begin(), iit_end = this->inheritances.end(); iit != iit_end; ++iit)
	{
		OperType *ot = *iit;

		if (ot->HasPriv(privstr))
			return true;
	}

	return false;
}

void OperType::AddCommand(const Anope::string &cmdstr)
{
	this->commands.push_back(cmdstr);
}

void OperType::AddPriv(const Anope::string &privstr)
{
	this->privs.push_back(privstr);
}

const Anope::string &OperType::GetName() const
{
	return this->name;
}

void OperType::Inherits(OperType *ot)
{
	if (ot != this)
		this->inheritances.insert(ot);
}

const std::list<Anope::string> OperType::GetCommands() const
{
	std::list<Anope::string> cmd_list = this->commands;
	for (std::set<OperType *>::const_iterator it = this->inheritances.begin(), it_end = this->inheritances.end(); it != it_end; ++it)
	{
		OperType *ot = *it;
		std::list<Anope::string> cmds = ot->GetCommands();
		for (std::list<Anope::string>::const_iterator it2 = cmds.begin(), it2_end = cmds.end(); it2 != it2_end; ++it2)
			cmd_list.push_back(*it2);
	}
	return cmd_list;
}

const std::list<Anope::string> OperType::GetPrivs() const
{
	std::list<Anope::string> priv_list = this->privs;
	for (std::set<OperType *>::const_iterator it = this->inheritances.begin(), it_end = this->inheritances.end(); it != it_end; ++it)
	{
		OperType *ot = *it;
		std::list<Anope::string> priv = ot->GetPrivs();
		for (std::list<Anope::string>::const_iterator it2 = priv.begin(), it2_end = priv.end(); it2 != it2_end; ++it2)
			priv_list.push_back(*it2);
	}
	return priv_list;
}

