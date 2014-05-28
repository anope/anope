/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

 /* AutoKick data. */
class CoreExport AutoKick : public Serializable
{
 public:
	/* Channel this autokick is on */
	Serialize::Reference<ChanServ::Channel> ci;

	Anope::string mask;
	Serialize::Reference<NickServ::Account> nc;

	Anope::string reason;
	Anope::string creator;
	time_t addtime;
	time_t last_used;

 protected:
	AutoKick() : Serializable("AutoKick") { }
 public:
	virtual ~AutoKick() { }
};

class AutoKickService : public Service
{
 public:
	AutoKickService(Module *o) : Service(o, "AutoKickService", "AutoKickService") { }

	virtual AutoKick* Create() anope_abstract;
};
static ServiceReference<AutoKickService> akick("AutoKickService", "AutoKickService");

namespace Event
{
	struct CoreExport Akick : Events
	{
		/** Called after adding an akick to a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickAdd(CommandSource &source, ChanServ::Channel *ci, const AutoKick *ak) anope_abstract;

		/** Called before removing an akick from a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickDel(CommandSource &source, ChanServ::Channel *ci, const AutoKick *ak) anope_abstract;
	};
}
