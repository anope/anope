/* OperServ ignore interface
 *
 * (C) 2003-2010 Anope Team
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
};

class IgnoreService : public Service
{
 protected:
	std::list<IgnoreData> ignores;

	IgnoreService(Module *c, const Anope::string &n) : Service(c, n) { }
	
 public:
	virtual void AddIgnore(const Anope::string &mask, const Anope::string &creator, const Anope::string &reason, time_t delta = Anope::CurTime) = 0;

	virtual bool DelIgnore(const Anope::string &mask) = 0;

	inline void ClearIgnores() { this->ignores.clear(); }

	virtual IgnoreData *Find(const Anope::string &mask) = 0;

	inline std::list<IgnoreData> &GetIgnores() { return this->ignores; }
};

