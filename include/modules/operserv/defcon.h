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
	struct CoreExport DefconLevel : Events
	{
		/** Called when defcon level changes
		 * @param level The level
		 */
		virtual void OnDefconLevel(int level) anope_abstract;
	};
}

template<> struct EventName<Event::DefconLevel> { static constexpr const char *const name = "OnDefconLevel"; };
