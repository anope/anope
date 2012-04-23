#ifndef GLOBAL_H
#define GLOBAL_H

class GlobalService : public Service
{
 public:
	GlobalService(Module *m) : Service(m, "GlobalService", "Global") { }

	/** Send out a global message to all users
	 * @param sender Our client which should send the global
	 * @param source The sender of the global
	 * @param message The message
	 */
	virtual void SendGlobal(const BotInfo *sender, const Anope::string &source, const Anope::string &message) = 0;
};

static service_reference<GlobalService> global("GlobalService", "Global");

#endif // GLOBAL_H

