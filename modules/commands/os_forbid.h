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

	const Anope::string serialize_name() const anope_override { return "ForbidData"; }
	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &data);
};

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual const std::vector<ForbidData *> &GetForbids() = 0;
};

static service_reference<ForbidService> forbid_service("ForbidService", "forbid");

Serialize::Data ForbidData::serialize() const
{
	Serialize::Data data;
	
	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["reason"] << this->reason;
	data["created"] << this->created;
	data["expires"] << this->expires;
	data["type"] << this->type;

	return data;
}

Serializable* ForbidData::unserialize(Serializable *obj, Serialize::Data &data)
{
	if (!forbid_service)
		return NULL;

	ForbidData *fb;
	if (obj)
		fb = anope_dynamic_static_cast<ForbidData *>(obj);
	else
		fb = new ForbidData;

	data["mask"] >> fb->mask;
	data["creator"] >> fb->creator;
	data["reason"] >> fb->reason;
	data["created"] >> fb->created;
	data["expires"] >> fb->expires;
	unsigned int t;
	data["type"] >> t;
	fb->type = static_cast<ForbidType>(t);

	if (!obj)
		forbid_service->AddForbid(fb);
	return fb;
}

#endif

