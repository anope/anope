#ifndef NICKSERV_H
#define NICKSERV_H

class NickServService : public Service
{
 public:
	NickServService(Module *m) : Service(m, "NickServService", "NickServ") { }

	virtual void Validate(User *u) = 0;
};

static ServiceReference<NickServService> NickServService("NickServService", "NickServ");
static Reference<BotInfo> NickServ;

#endif // NICKSERV_H

