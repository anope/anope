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

struct ForbidData : Serializable
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t created;
	time_t expires;
	ForbidType type;

	ForbidData() : Serializable("ForbidData") { }
	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data);
};

class ForbidService : public Service
{
 public:
	ForbidService(Module *m) : Service(m, "ForbidService", "forbid") { }

	virtual void AddForbid(ForbidData *d) = 0;

	virtual void RemoveForbid(ForbidData *d) = 0;

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) = 0;

	virtual std::vector<ForbidData *> GetForbids() = 0;
};

static ServiceReference<ForbidService> forbid_service("ForbidService", "forbid");

void ForbidData::Serialize(Serialize::Data &data) const
{
	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["reason"] << this->reason;
	data["created"] << this->created;
	data["expires"] << this->expires;
	data["type"] << this->type;
}

Serializable* ForbidData::Unserialize(Serializable *obj, Serialize::Data &data)
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

	if (t > FT_SIZE - 1)
		return NULL;

	if (!obj)
		forbid_service->AddForbid(fb);
	return fb;
}

#endif

