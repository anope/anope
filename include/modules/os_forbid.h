/*
 *
 * (C) 2011-2022 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

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

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData* CreateForbid() = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual ForbidData *FindForbidExact(const Anope::string &mask, ForbidType type) = 0;

	virtual std::vector<ForbidData *> GetForbids() = 0;
};

static ServiceReference<ForbidService> forbid_service("ForbidService", "forbid");
