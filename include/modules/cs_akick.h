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
	struct CoreExport Akick : Events
	{
		/** Called after adding an akick to a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickAdd(CommandSource &source, ChannelInfo *ci, const AutoKick *ak) anope_abstract;

		/** Called before removing an akick from a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickDel(CommandSource &source, ChannelInfo *ci, const AutoKick *ak) anope_abstract;
	};
}
