/* OperServ ignore interface
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


class Ignore : public Serialize::Object
{
 public:
	using Serialize::Object::Object;

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetTime() anope_abstract;
	virtual void SetTime(const time_t &) anope_abstract;
};

static Serialize::TypeReference<Ignore> ignoretype("IgnoreData");

class IgnoreService : public Service
{
 protected:
	IgnoreService(Module *c) : Service(c, "IgnoreService", "ignore") { }

 public:
	virtual Ignore *Find(const Anope::string &mask) anope_abstract;
};

static ServiceReference<IgnoreService> ignore_service("IgnoreService", "ignore");

