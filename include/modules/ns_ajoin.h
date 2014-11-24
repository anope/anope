/*
 *
 * (C) 2013-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

class AutoJoin : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual NickServ::Account *GetOwner() anope_abstract;
	virtual void SetOwner(NickServ::Account *acc) anope_abstract;

	virtual Anope::string GetChannel() anope_abstract;
	virtual void SetChannel(const Anope::string &c) anope_abstract;

	virtual Anope::string GetKey() anope_abstract;
	virtual void SetKey(const Anope::string &k) anope_abstract;
};

static Serialize::TypeReference<AutoJoin> autojoin("AutoJoin");
