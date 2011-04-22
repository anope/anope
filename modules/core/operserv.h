#ifndef OPERSERV_H
#define OPERSERV_H

class OperServService : public Service
{
 public:
	OperServService(Module *m) : Service(m, "OperServ") { }

	virtual BotInfo *Bot() = 0;
};

static service_reference<OperServService> operserv("OperServ");

#endif // OPERSERV_H

