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

bool OperType::HasCommand(const std::string &cmdstr)
{

}

bool OperType::HasPriv(const std::string &privstr)
{

}

void OperType::AddCommand(const std::string &cmdstr)
{
	this->commands.push_back(cmdstr);
}

void OperType::AddPriv(const std::string &privstr)
{
	this->privs.push_back(privstr);
}

