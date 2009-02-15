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

bool SVSOper::HasCommand(const std::string &cmdstr)
{

}

bool SVSOper::HasPriv(const std::string &privstr)
{

}

void SVSOper::AddCommand(const std::string &cmdstr)
{
	this->commands.push_back(cmdstr);
}

void SVSOper::AddPriv(const std::string &privstr)
{
	this->privs.push_back(privstr);
}

