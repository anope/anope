#ifndef NICKSERV_H
#define NICKSERV_H

class NickServService : public Service<NickServService>
{
 public:
	NickServService(Module *m) : Service<NickServService>(m, "NickServ") { }

	virtual void Validate(User *u) = 0;
};

static service_reference<NickServService> nickserv("NickServ");

#endif // NICKSERV_H

