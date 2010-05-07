/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 *
 */

#include "services.h"

OperType::OperType(const ci::string &nname) : name(nname)
{
}

bool OperType::HasCommand(const std::string &cmdstr) const
{
	for (std::list<std::string>::const_iterator it = this->commands.begin(); it != this->commands.end(); ++it)
	{
		if (Anope::Match(cmdstr, *it))
		{
			return true;
		}
	}
	for (std::set<OperType *>::const_iterator iit = this->inheritances.begin(); iit != this->inheritances.end(); ++iit)
	{
		OperType *ot = *iit;

		if (ot->HasCommand(cmdstr))
		{
			return true;
		}
	}

	return false;
}

bool OperType::HasPriv(const std::string &privstr) const
{
	for (std::list<std::string>::const_iterator it = this->privs.begin(); it != this->privs.end(); ++it)
	{
		if (Anope::Match(privstr, *it))
		{
			return true;
		}
	}
	for (std::set<OperType *>::const_iterator iit = this->inheritances.begin(); iit != this->inheritances.end(); ++iit)
	{
		OperType *ot = *iit;

		if (ot->HasPriv(privstr))
		{
			return true;
		}
	}

	return false;
}

void OperType::AddCommand(const std::string &cmdstr)
{
	this->commands.push_back(cmdstr);
}

void OperType::AddPriv(const std::string &privstr)
{
	this->privs.push_back(privstr);
}

const ci::string &OperType::GetName() const
{
	return this->name;
}

void OperType::Inherits(OperType *ot)
{
	this->inheritances.insert(ot);
}

