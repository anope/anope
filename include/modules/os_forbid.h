/*
 *
 * (C) 2013-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

enum ForbidType
{
	FT_NICK = 1,
	FT_CHAN,
	FT_EMAIL,
	FT_REGISTER,
	FT_SIZE
};

class ForbidData : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;

	virtual ForbidType GetType() anope_abstract;
	virtual void SetType(const ForbidType &) anope_abstract;
};

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) anope_abstract;

	virtual std::vector<ForbidData *> GetForbids() anope_abstract;
};

static ServiceReference<ForbidService> forbid_service("ForbidService", "forbid");

static Serialize::TypeReference<ForbidData> forbiddata("ForbidData");

