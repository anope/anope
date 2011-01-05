/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"

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

const std::list<Anope::string> &OperType::GetCommands() const
{
	return this->commands;
}

const std::list<Anope::string> &OperType::GetPrivs() const
{
	return this->privs;
}

