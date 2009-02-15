/*
 * Copyright (C) 2008-2009 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2009 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */
 
 #include "services.h"

OperType::OperType(const std::string &nname) : name(nname)
{
}

bool OperType::HasCommand(const std::string &cmdstr) const
{
	for (std::list<std::string>::const_iterator it = this->commands.begin(); it != this->commands.end(); it++)
	{
		if (Anope::Match(cmdstr, *it))
		{
			return true;
		}
	}

	return false;
}

bool OperType::HasPriv(const std::string &privstr) const
{
	for (std::list<std::string>::const_iterator it = this->privs.begin(); it != this->privs.end(); it++)
	{
		if (Anope::Match(privstr, *it))
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

const std::string &OperType::GetName() const
{
	return this->name;
}

