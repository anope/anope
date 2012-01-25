#ifndef NICKSERV_H
#define NICKSERV_H

class NickServService : public Service
{
 public:
	NickServService(Module *m) : Service(m, "NickServService", "NickServ") { }

	virtual void Validate(User *u) = 0;
};

static service_reference<NickServService> nickserv("NickServService", "NickServ");

#endif // NICKSERV_H

