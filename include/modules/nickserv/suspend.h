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
	static constexpr const char *const NAME = "nssuspendinfo";

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

namespace Event
{
	struct CoreExport NickSuspend : Events
	{
		static constexpr const char *NAME = "nicksuspend";

		using Events::Events;
		
		/** Called when a nick is suspended
		 * @param na The nick alias
		 */
		virtual void OnNickSuspend(NickServ::Nick *na) anope_abstract;
	};

	struct CoreExport NickUnsuspend : Events
	{
		static constexpr const char *NAME = "nickunsuspend";

		using Events::Events;
		
		/** Called when a nick is unsuspneded
		 * @param na The nick alias
		 */
		virtual void OnNickUnsuspend(NickServ::Nick *na) anope_abstract;
	};
}
