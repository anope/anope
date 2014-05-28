/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Event
{
	struct CoreExport NickGroup : Events
	{
		/** Called when a user groups their nick
		 * @param u The user grouping
		 * @param target The target they're grouping to
		 */
		virtual void OnNickGroup(User *u, NickServ::Nick *target) anope_abstract;
	};
}
