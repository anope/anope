#ifndef OS_FORBID_H
#define OS_FORBID_H

enum ForbidType
{
	FT_NONE,
	FT_NICK,
	FT_CHAN,
	FT_EMAIL
};

struct ForbidData : Serializable
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t created;
	time_t expires;
	ForbidType type;

	Anope::string serialize_name() const { return "ForbidData"; }
	serialized_data serialize();
	static void unserialize(serialized_data &data);
};

class ForbidService : public Service<Base>
{
 public:
	ForbidService(Module *m) : Service<Base>(m, "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual const std::vector<ForbidData *> &GetForbids() = 0;
};

static service_reference<ForbidService, Base> forbid_service("forbid");

Serializable::serialized_data ForbidData::serialize()
{
	serialized_data data;
	
	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["reason"] << this->reason;
	data["created"] << this->created;
	data["expires"] << this->expires;
	data["type"] << this->type;

	return data;
}

void ForbidData::unserialize(serialized_data &data)
{
	if (!forbid_service)
		return;

	ForbidData *fb = new ForbidData;

	data["mask"] >> fb->mask;
	data["creator"] >> fb->creator;
	data["reason"] >> fb->reason;
	data["created"] >> fb->created;
	data["expires"] >> fb->expires;
	unsigned int t;
	data["type"] >> t;
	fb->type = static_cast<ForbidType>(t);

	forbid_service->AddForbid(fb);
}

#endif

