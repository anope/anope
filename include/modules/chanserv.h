/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace ChanServ
{
	class ChanServService : public Service
	{
	 public:
		ChanServService(Module *m) : Service(m, "ChanServService", "ChanServ")
		{
		}

		/* Have ChanServ hold the channel, that is, join and set +nsti and wait
		 * for a few minutes so no one can join or rejoin.
		 */
		virtual void Hold(Channel *c) anope_abstract;
	};
	static ServiceReference<ChanServService> service("ChanServService", "ChanServ");

	namespace Event
	{
		struct CoreExport PreChanExpire : Events
		{
			/** Called before a channel expires
			 * @param ci The channel
			 * @param expire Set to true to allow the chan to expire
			 */
			virtual void OnPreChanExpire(ChannelInfo *ci, bool &expire) anope_abstract;
		};
		static EventHandlersReference<PreChanExpire> OnPreChanExpire("OnPreChanExpire");

		struct CoreExport ChanExpire : Events
		{
			/** Called before a channel expires
			 * @param ci The channel
			 */
			virtual void OnChanExpire(ChannelInfo *ci) anope_abstract;
		};
		static EventHandlersReference<ChanExpire> OnChanExpire("OnChanExpire");
	}
}
