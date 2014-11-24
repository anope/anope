/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

class OperInfo : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual Serialize::Object *GetTarget() anope_abstract;
	virtual void SetTarget(Serialize::Object *) anope_abstract;

	virtual Anope::string GetInfo() anope_abstract;
	virtual void SetInfo(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &) anope_abstract;
};

static Serialize::TypeReference<OperInfo> operinfo("OperInfo");

