/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

class CSSuspendInfo : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual Anope::string GetBy() anope_abstract;
	virtual void SetBy(const Anope::string &) anope_abstract;
	
	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetWhen() anope_abstract;
	virtual void SetWhen(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;
};

static Serialize::TypeReference<CSSuspendInfo> cssuspendinfo("CSSuspendInfo");

namespace Event
{
	struct CoreExport ChanSuspend : Events
	{
		/** Called when a channel is suspended
		 * @param ci The channel
		 */
		virtual void OnChanSuspend(ChanServ::Channel *ci) anope_abstract;
	};
	struct CoreExport ChanUnsuspend : Events
	{
		/** Called when a channel is unsuspended
		 * @param ci The channel
		 */
		virtual void OnChanUnsuspend(ChanServ::Channel *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<ChanUnsuspend> OnChanUnsuspend;
}

