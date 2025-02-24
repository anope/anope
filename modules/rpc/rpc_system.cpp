/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/rpc.h"

// TODO:
//
// * system.getCapabilities
// * system.methodHelp
// * system.methodSignature


class SystemListMethodsRPCEvent final
	: public RPC::Event
{
public:
	SystemListMethodsRPCEvent()
		: RPC::Event("system.listMethods")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root<RPC::Array>();
		for (const auto &[event, _] : iface->GetEvents())
			root.Reply(event);
		return true;
	}
};

class ModuleRPCSystem final
	: public Module
{
private:
	ServiceReference<RPC::ServiceInterface> rpc;
	SystemListMethodsRPCEvent systemlistmethodsrpcevent;

public:
	ModuleRPCSystem(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, rpc("RPCServiceInterface", "rpc")
	{
		if (!rpc)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

		rpc->Register(&systemlistmethodsrpcevent);
	}

	~ModuleRPCSystem() override
	{
		if (!rpc)
			return;

		rpc->Unregister(&systemlistmethodsrpcevent);
	}
};

MODULE_INIT(ModuleRPCSystem)
