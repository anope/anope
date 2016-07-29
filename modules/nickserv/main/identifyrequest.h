#include "modules/nickserv.h"

class IdentifyRequestImpl : public NickServ::IdentifyRequest
{
 public:
	IdentifyRequestImpl(NickServ::IdentifyRequestListener *, Module *o, const Anope::string &acc, const Anope::string &pass);
	virtual ~IdentifyRequestImpl();

	void Hold(Module *m) override;
	void Release(Module *m) override;
	void Success(Module *m) override;
	void Dispatch() override;
	void Unload(Module *);
};
