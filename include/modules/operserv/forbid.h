// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#define OPERSERV_FORBID_DATA_TYPE "ForbidData"
#define OPERSERV_FORBID_SERVICE "OperServ::ForbidService"

namespace OperServ
{
	class ForbidData;
	class ForbidService;

	enum ForbidType
	{
		FT_NICK = 1,
		FT_CHAN,
		FT_EMAIL,
		FT_REGISTER,
		FT_PASSWORD,
		FT_SIZE
	};

	ServiceReference<ForbidService> forbid_service(OPERSERV_FORBID_SERVICE, OPERSERV_FORBID_SERVICE);
}

class OperServ::ForbidData
{
protected:
	ForbidData() = default;

public:
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t created = 0;
	time_t expires = 0;
	bool immutable = false;
	OperServ::ForbidType type;

	virtual ~ForbidData() = default;
};

class OperServ::ForbidService
	: public Service
{
public:
	ForbidService(Module *m)
		: Service(m, OPERSERV_FORBID_SERVICE, OPERSERV_FORBID_SERVICE)
	{
	}

	virtual void AddForbid(OperServ::ForbidData *d) = 0;

	virtual void RemoveForbid(OperServ::ForbidData *d) = 0;

	virtual OperServ::ForbidData *CreateForbid() = 0;

	virtual OperServ::ForbidData *FindForbid(const Anope::string &mask, OperServ::ForbidType type) = 0;

	virtual OperServ::ForbidData *FindForbidExact(const Anope::string &mask, OperServ::ForbidType type) = 0;

	virtual std::vector<OperServ::ForbidData *> GetForbids() = 0;
};
