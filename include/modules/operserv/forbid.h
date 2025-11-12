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

enum ForbidType
{
	FT_NICK = 1,
	FT_CHAN,
	FT_EMAIL,
	FT_REGISTER,
	FT_SIZE
};

struct ForbidData
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t created = 0;
	time_t expires = 0;
	ForbidType type;

	virtual ~ForbidData() = default;
protected:
	ForbidData() = default;
};

class ForbidService
	: public Service
{
public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData *CreateForbid() = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual ForbidData *FindForbidExact(const Anope::string &mask, ForbidType type) = 0;

	virtual std::vector<ForbidData *> GetForbids() = 0;
};

static ServiceReference<ForbidService> forbid_service("ForbidService", "forbid");
