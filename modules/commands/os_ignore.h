/* OperServ ignore interface
 *
 * (C) 2003-2011 Anope Team
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
	serialized_data serialize();
	static void unserialize(serialized_data &data);
};

class IgnoreService : public Service<Base>
{
 protected:
	std::list<IgnoreData> ignores;

	IgnoreService(Module *c) : Service<Base>(c, "ignore") { }
	
 public:
	virtual void AddIgnore(const Anope::string &mask, const Anope::string &creator, const Anope::string &reason, time_t delta = Anope::CurTime) = 0;

	virtual bool DelIgnore(const Anope::string &mask) = 0;

	inline void ClearIgnores() { this->ignores.clear(); }

	virtual IgnoreData *Find(const Anope::string &mask) = 0;

	inline std::list<IgnoreData> &GetIgnores() { return this->ignores; }
};

static service_reference<IgnoreService, Base> ignore_service("ignore");

Serializable::serialized_data IgnoreData::serialize()
{
	serialized_data data;

	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["reason"] << this->reason;
	data["time"] << this->time;
		
	return data;
}

void IgnoreData::unserialize(serialized_data &data)
{
	if (!ignore_service)
		return;

	time_t t;
	data["time"] >> t;

	ignore_service->AddIgnore(data["mask"].astr(), data["creator"].astr(), data["reason"].astr(), t);
}

