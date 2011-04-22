#ifndef HOSTSERV_H
#define HOSTSERV_H

class HostServService : public Service
{
 public:
	HostServService(Module *m) : Service(m, "HostServ") { }

	virtual BotInfo *Bot() = 0;

	virtual void Sync(NickAlias *na) = 0;
};

static service_reference<HostServService> hostserv("HostServ");

#endif // HOSTSERV_H

