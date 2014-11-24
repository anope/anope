/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Event
{
	struct CoreExport BotCreate : Events
	{
		/** Called when a new bot is made
		 * @param bi The bot
		 */
		virtual void OnBotCreate(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotChange : Events
	{
		/** Called when a bot is changed
		 * @param bi The bot
		 */
		virtual void OnBotChange(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotDelete : Events
	{
		/** Called when a bot is deleted
		 * @param bi The bot
		 */
		virtual void OnBotDelete(ServiceBot *bi) anope_abstract;
	};
}
