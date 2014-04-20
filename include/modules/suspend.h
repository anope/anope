/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

struct SuspendInfo
{
	Anope::string what, by, reason;
	time_t when, expires;

	SuspendInfo() { }
	virtual ~SuspendInfo() { }
};

namespace Event
{
	struct CoreExport ChanSuspend : Events
	{
		/** Called when a channel is suspended
		 * @param ci The channel
		 */
		virtual void OnChanSuspend(ChannelInfo *ci) anope_abstract;
	};
	struct CoreExport ChanUnsuspend : Events
	{
		/** Called when a channel is unsuspended
		 * @param ci The channel
		 */
		virtual void OnChanUnsuspend(ChannelInfo *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<ChanUnsuspend> OnChanUnsuspend;

	struct CoreExport NickSuspend : Events
	{
		/** Called when a nick is suspended
		 * @param na The nick alias
		 */
		virtual void OnNickSuspend(NickAlias *na) anope_abstract;
	};

	struct CoreExport NickUnsuspended : Events
	{
		/** Called when a nick is unsuspneded
		 * @param na The nick alias
		 */
		virtual void OnNickUnsuspended(NickAlias *na) anope_abstract;
	};
}

