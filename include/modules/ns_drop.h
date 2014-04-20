/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace Event
{
	struct CoreExport NickDrop : Events
	{
		/** Called when a nick is dropped
		 * @param source The source of the command
		 * @param na The nick
		 */
		virtual void OnNickDrop(CommandSource &source, NickAlias *na) anope_abstract;
	};
	extern CoreExport EventHandlers<NickDrop> OnNickDrop;
}

