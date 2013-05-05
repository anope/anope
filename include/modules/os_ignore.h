/* OperServ ignore interface
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


struct IgnoreData : Serializable
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t time; /* When do we stop ignoring them? */

	IgnoreData() : Serializable("IgnoreData") { }
	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data);
};

class IgnoreService : public Service
{
 protected:
	std::list<IgnoreData> ignores;

	IgnoreService(Module *c) : Service(c, "IgnoreService", "ignore") { }
	
 public:
	virtual IgnoreData* AddIgnore(const Anope::string &mask, const Anope::string &creator, const Anope::string &reason, time_t delta = Anope::CurTime) = 0;

	virtual bool DelIgnore(const Anope::string &mask) = 0;

	inline void ClearIgnores() { this->ignores.clear(); }

	virtual IgnoreData *Find(const Anope::string &mask) = 0;

	inline std::list<IgnoreData> &GetIgnores() { return this->ignores; }
};

static ServiceReference<IgnoreService> ignore_service("IgnoreService", "ignore");

void IgnoreData::Serialize(Serialize::Data &data) const
{
	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["reason"] << this->reason;
	data["time"] << this->time;
}

Serializable* IgnoreData::Unserialize(Serializable *obj, Serialize::Data &data)
{
	if (!ignore_service)
		return NULL;
	
	if (obj)
	{
		IgnoreData *ign = anope_dynamic_static_cast<IgnoreData *>(obj);
		data["mask"] >> ign->mask;
		data["creator"] >> ign->creator;
		data["reason"] >> ign->reason;
		data["time"] >> ign->time;
		return ign;
	}

	Anope::string smask, screator, sreason;
	time_t t;

	data["mask"] >> smask;
	data["creator"] >> screator;
	data["reason"] >> sreason;
	data["time"] >> t;

	return ignore_service->AddIgnore(smask, screator, sreason, t);
}

