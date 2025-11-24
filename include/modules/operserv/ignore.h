// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#define OPERSERV_IGNORE_DATA_TYPE "IgnoreData"
#define OPERSERV_IGNORE_SERVICE "OperServ::IgnoreService"

namespace OperServ
{
	class IgnoreData;
	class IgnoreService;

	ServiceReference<IgnoreService> ignore_service(OPERSERV_IGNORE_SERVICE, OPERSERV_IGNORE_SERVICE);
}

class OperServ::IgnoreData
{
protected:
	IgnoreData() = default;

public:
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t time = 0; /* When do we stop ignoring them? */

	virtual ~IgnoreData() = default;
};

class OperServ::IgnoreService
	: public Service
{
protected:
	IgnoreService(Module *c)
		: Service(c, OPERSERV_IGNORE_SERVICE, OPERSERV_IGNORE_SERVICE)
	{
	}

public:
	virtual void AddIgnore(OperServ::IgnoreData *) = 0;

	virtual void DelIgnore(OperServ::IgnoreData *) = 0;

	virtual void ClearIgnores() = 0;

	virtual OperServ::IgnoreData *Create() = 0;

	virtual OperServ::IgnoreData *Find(const Anope::string &mask) = 0;

	virtual std::vector<OperServ::IgnoreData *> &GetIgnores() = 0;
};
