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

class AnopeDebugTypesRPCEvent final
	: public RPC::Event
{
public:
	AnopeDebugTypesRPCEvent()
		: RPC::Event("anope.debugTypes")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		// For both the map type and array type we test that we can handle:
		//
		// * Arrays
		// * Maps
		// * Strings
		// * Null
		// * Boolean true
		// * Boolean false
		// * Doubles
		// * Signed integer that can fit into a 32-bit type.
		// * Signed integer that needs a 64-bit type (not supported by some
		//   XML-RPC clients).
		// * Unsigned integer that can fit into a 32-bit signed type.
		// * Unsigned integer that needs a 64-bit signed type (not supported by
		//   some XML-RPC clients).
		// * Unsigned integer that needs a 64-bit unsigned type (converted to a
		//   string when using the xmlrpc module).

		auto &root = request.Root();

		auto &array = root.ReplyArray("array");
		array.ReplyArray();
		array.ReplyMap();
		array.Reply("string");
		array.Reply(nullptr);
		array.Reply(true);
		array.Reply(false);
		array.Reply(42.42);
		array.Reply(int64_t(42));
		array.Reply(int64_t(INT32_MAX) + 42);
		array.Reply(uint64_t(42));
		array.Reply(uint64_t(INT32_MAX) + 42);
		array.Reply(uint64_t(INT64_MAX) + 42);

		root.ReplyMap("map");
		root.Reply("string", "string");
		root.Reply("null", nullptr);
		root.Reply("true", true);
		root.Reply("false", false);
		root.Reply("double", 42.42);
		root.Reply("int-small", int64_t(42));
		root.Reply("int-large", int64_t(INT32_MAX) + 42);
		root.Reply("uint-small", uint64_t(42));
		root.Reply("uint-medium", uint64_t(INT32_MAX) + 42);
		root.Reply("uint-large", uint64_t(INT64_MAX) + 42);

		return true;
	}
};

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
	AnopeDebugTypesRPCEvent anopedebugtypesrpcevent;
	SystemListMethodsRPCEvent systemlistmethodsrpcevent;

public:
	ModuleRPCSystem(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
	{
		if (!RPC::service)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

#if DEBUG_BUILD
		RPC::service->Register(&anopedebugtypesrpcevent);
#endif
		RPC::service->Register(&systemlistmethodsrpcevent);
	}

	~ModuleRPCSystem() override
	{
		if (!RPC::service)
			return;

#if DEBUG_BUILD
		RPC::service->Unregister(&anopedebugtypesrpcevent);
#endif
		RPC::service->Unregister(&systemlistmethodsrpcevent);
	}
};

MODULE_INIT(ModuleRPCSystem)
