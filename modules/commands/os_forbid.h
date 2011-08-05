#ifndef OS_FORBID_H
#define OS_FORBID_H

enum ForbidType
{
	FT_NONE,
	FT_NICK,
	FT_CHAN,
	FT_EMAIL
};

struct ForbidData
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t created;
	time_t expires;
	ForbidType type;
};

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual const std::vector<ForbidData *> &GetForbids() = 0;
};

#endif

