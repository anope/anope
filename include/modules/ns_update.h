/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Event
{
	struct CoreExport NickUpdate : Events
	{
		/** Called when a user does /ns update
		 * @param u The user
		 */
		virtual void OnNickUpdate(User *u) anope_abstract;
	};
}

template<> struct EventName<Event::NickUpdate> { static constexpr const char *const name = "OnNickUpdate"; };
