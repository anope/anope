/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Global
{
	class GlobalService : public Service
	{
	 public:
		GlobalService(Module *m) : Service(m, "GlobalService", "Global")
		{
		}

		/** Send out a global message to all users
		 * @param sender Our client which should send the global
		 * @param source The sender of the global
		 * @param message The message
		 */
		virtual void SendGlobal(BotInfo *sender, const Anope::string &source, const Anope::string &message) anope_abstract;
	};
	static ServiceReference<GlobalService> service("GlobalService", "Global");
}


