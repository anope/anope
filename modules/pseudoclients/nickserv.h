#ifndef NICKSERV_H
#define NICKSERV_H

class NickServService : public Service
{
 public:
	NickServService(Module *m) : Service(m, "NickServ") { }

	virtual void Validate(User *u) = 0;
};

static service_reference<NickServService> nickserv("NickServ");

#endif // NICKSERV_H

