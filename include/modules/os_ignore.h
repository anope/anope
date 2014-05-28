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


struct IgnoreData
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t time; /* When do we stop ignoring them? */

	virtual ~IgnoreData() { }
 protected:
	IgnoreData() : time(0) { }
};

class IgnoreService : public Service
{
 protected:
	IgnoreService(Module *c) : Service(c, "IgnoreService", "ignore") { }

 public:
	virtual void AddIgnore(IgnoreData *) anope_abstract;

	virtual void DelIgnore(IgnoreData *) anope_abstract;

	virtual void ClearIgnores() anope_abstract;

	virtual IgnoreData *Create() anope_abstract;

	virtual IgnoreData *Find(const Anope::string &mask) anope_abstract;

	virtual std::vector<IgnoreData *> &GetIgnores() anope_abstract;
};

static ServiceReference<IgnoreService> ignore_service("IgnoreService", "ignore");

