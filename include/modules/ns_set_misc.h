/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

class NSMiscData : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual Anope::string GetData() anope_abstract;
	virtual void SetData(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<NSMiscData> nsmiscdata("NSMiscData");

