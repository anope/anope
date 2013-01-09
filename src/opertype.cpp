/*
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "anope.h"
#include "opertype.h"
#include "config.h"

Oper *Oper::Find(const Anope::string &name)
{
	for (unsigned i = 0; i < Config->Opers.size(); ++i)
	{
		Oper *o = Config->Opers[i];

		if (o->name.equals_ci(name))
			return o;
	}

	return NULL;
}

OperType *OperType::Find(const Anope::string &name)
{
	for (std::list<OperType *>::iterator it = Config->MyOperTypes.begin(), it_end = Config->MyOperTypes.end(); it != it_end; ++it)
	{
		OperType *ot = *it;

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
		if (Anope::Match(cmdstr, *it))
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
		if (Anope::Match(privstr, *it))
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
	this->inheritances.insert(ot);
}

const std::list<Anope::string> OperType::GetCommands() const
{
	std::list<Anope::string> cmd_list = this->commands;
	for (std::set<OperType *>::const_iterator it = this->inheritances.begin(), it_end = this->inheritances.end(); it != it_end; ++it)
	{
		OperType *ot = *it;
		std::list<Anope::string> cmds = ot->GetPrivs();
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

