/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace NickServ
{
	class NickServService : public Service
	{
	 public:
		NickServService(Module *m) : Service(m, "NickServService", "NickServ")
		{
		}

		virtual void Validate(User *u) anope_abstract;
		virtual void Collide(User *u, NickAlias *na) anope_abstract;
		virtual void Release(NickAlias *na) anope_abstract;
	};
	static ServiceReference<NickServService> service("NickServService", "NickServ");

	namespace Event
	{
		struct CoreExport PreNickExpire : Events
		{
			/** Called before a nick expires
			 * @param na The nick
			 * @param expire Set to true to allow the nick to expire
			 */
			virtual void OnPreNickExpire(NickAlias *na, bool &expire) anope_abstract;
		};
		static EventHandlersReference<PreNickExpire> OnPreNickExpire("OnPreNickExpire");

		struct CoreExport NickExpire : Events
		{
			/** Called when a nick drops
			 * @param na The nick
			 */
			virtual void OnNickExpire(NickAlias *na) anope_abstract;
		};
		static EventHandlersReference<NickExpire> OnNickExpire("OnNickExpire");

		struct CoreExport NickRegister : Events
		{
			/** Called when a nick is registered
			 * @param user The user registering the nick, of any
			 * @param The nick
			 */
			virtual void OnNickRegister(User *user, NickAlias *na) anope_abstract;
		};
		static EventHandlersReference<NickRegister> OnNickRegister("OnNickRegister");

		struct CoreExport NickValidate : Events
		{
			/** Called when a nick is validated. That is, to determine if a user is permissted
			 * to be on the given nick.
			 * @param u The user
			 * @param na The nick they are on
			 * @return EVENT_STOP to force the user off of the nick
			 */
			virtual EventReturn OnNickValidate(User *u, NickAlias *na) anope_abstract;
		};
		static EventHandlersReference<NickValidate> OnNickValidate("OnNickValidate");
	}
}
