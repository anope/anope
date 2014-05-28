#include "identifyrequest.h"

IdentifyRequestImpl::IdentifyRequestImpl(NickServ::IdentifyRequestListener *li, Module *o, const Anope::string &acc, const Anope::string &pass) : NickServ::IdentifyRequest(li, o, acc, pass)
{
	std::set<NickServ::IdentifyRequest *> &requests = NickServ::service->GetIdentifyRequests();
	requests.insert(this);
}

IdentifyRequestImpl::~IdentifyRequestImpl()
{
	std::set<NickServ::IdentifyRequest *> &requests = NickServ::service->GetIdentifyRequests();
	requests.erase(this);
	delete l;
}

void IdentifyRequestImpl::Hold(Module *m)
{
	holds.insert(m);
}

void IdentifyRequestImpl::Release(Module *m)
{
	holds.erase(m);
	if (holds.empty() && dispatched)
	{
		if (!success)
			l->OnFail(this);
		delete this;
	}
}

void IdentifyRequestImpl::Success(Module *m)
{
	if (!success)
	{
		l->OnSuccess(this);
		success = true;
	}
}

void IdentifyRequestImpl::Dispatch()
{
	if (holds.empty())
	{
		if (!success)
			l->OnFail(this);
		delete this;
	}
	else
		dispatched = true;
}
