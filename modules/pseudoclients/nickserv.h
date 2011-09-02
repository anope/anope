#ifndef NICKSERV_H
#define NICKSERV_H

class NickServService : public Service<Base>
{
 public:
	NickServService(Module *m) : Service<Base>(m, "NickServ") { }

	virtual void Validate(User *u) = 0;
};

static service_reference<NickServService, Base> nickserv("NickServ");

#endif // NICKSERV_H

