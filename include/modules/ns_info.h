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
	struct CoreExport NickInfo : Events
	{
		/** Called when a user requests info for a nick
		 * @param source The user requesting info
		 * @param na The nick the user is requesting info from
		 * @param info Data to show the user requesting information
		 * @param show_hidden true if we should show the user everything
		 */
		virtual void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) anope_abstract;
	};
}
