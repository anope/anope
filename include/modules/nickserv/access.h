/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

class NickAccess : public Serialize::Object
{
 public:
	using Serialize::Object::Object;

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<NickAccess> nsaccess("NSAccess");

