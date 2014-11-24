/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

class NSSuspendInfo : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetBy() anope_abstract;
	virtual void SetBy(const Anope::string &) anope_abstract;
	
	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetWhen() anope_abstract;
	virtual void SetWhen(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;
};

static Serialize::TypeReference<NSSuspendInfo> nssuspendinfo("NSSuspendInfo");

namespace Event
{
	struct CoreExport NickSuspend : Events
	{
		/** Called when a nick is suspended
		 * @param na The nick alias
		 */
		virtual void OnNickSuspend(NickServ::Nick *na) anope_abstract;
	};

	struct CoreExport NickUnsuspended : Events
	{
		/** Called when a nick is unsuspneded
		 * @param na The nick alias
		 */
		virtual void OnNickUnsuspended(NickServ::Nick *na) anope_abstract;
	};
}

