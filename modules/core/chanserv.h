#ifndef CHANSERV_H
#define CHANSERV_H

class ChanServService : public Service
{
 public:
	ChanServService(Module *m) : Service(m, "ChanServ") { }

	virtual BotInfo *Bot() = 0;
};

static service_reference<ChanServService> chanserv("ChanServ");

#endif // CHANSERV_H

