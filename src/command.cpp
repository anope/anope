/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"

Command::Command(const Anope::string &sname, size_t min_params, size_t max_params, const Anope::string &spermission) : MaxParams(max_params), MinParams(min_params), name(sname), permission(spermission)
{
	this->module = NULL;
	this->service = NULL;
}

Command::~Command()
{
}

CommandReturn Command::Execute(User *u, const std::vector<Anope::string> &)
{
	return MOD_CONT;
}

void Command::OnServHelp(User *u) { }

bool Command::OnHelp(User *u, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(User *u, const Anope::string &subcommand) { }

void Command::SetPermission(const Anope::string &reststr)
{
	this->permission = reststr;
}

bool Command::AddSubcommand(Command *c)
{
	return false;
}

bool Command::DelSubcommand(const Anope::string &cname)
{
	return false;
}
