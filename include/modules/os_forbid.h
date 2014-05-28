#ifndef OS_FORBID_H
#define OS_FORBID_H

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
	time_t created;
	time_t expires;
	ForbidType type;

	virtual ~ForbidData() { }
 protected:
	ForbidData() : created(0), expires(0) { }
};

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual void AddForbid(ForbidData *d) anope_abstract;

	virtual void RemoveForbid(ForbidData *d) anope_abstract;

	virtual ForbidData* CreateForbid() anope_abstract;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) anope_abstract;

	virtual std::vector<ForbidData *> GetForbids() anope_abstract;
};

static ServiceReference<ForbidService> forbid_service("ForbidService", "forbid");

#endif

