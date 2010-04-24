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
#include "modules.h"

Command::Command(const ci::string &sname, size_t min_params, size_t max_params, const std::string &spermission) : MaxParams(max_params), MinParams(min_params), name(sname), permission(spermission)
{
	this->core = 0;
	this->mod_name = NULL;
	this->service = NULL;
	this->next = NULL;
}

Command::~Command()
{
	if (this->mod_name) {
		delete [] this->mod_name;
	}
	if (this->service) {
		delete [] this->service;
	}
}

CommandReturn Command::Execute(User *u, const std::vector<ci::string> &) { return MOD_CONT; }

bool Command::OnHelp(User *u, const ci::string &subcommand) { return false; }

void Command::OnSyntaxError(User *u, const ci::string &subcommand) { }

void Command::SetPermission(const std::string &reststr)
{
	this->permission = reststr;
}

