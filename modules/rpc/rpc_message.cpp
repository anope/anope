/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/global/service.h"
#include "modules/rpc.h"

enum
{
	// Used by anope.messageNetwork and anope.messageServer
	ERR_NO_GLOBAL_SERVICE = RPC::ERR_CUSTOM_START,

	// Used by anope.messageServer
	ERR_NO_SUCH_SERVER = RPC::ERR_CUSTOM_START + 1,

	// Used by anope.messageUser
	ERR_NO_SUCH_SOURCE = RPC::ERR_CUSTOM_START,
	ERR_NO_SUCH_TARGET = RPC::ERR_CUSTOM_START + 1,
};

class MessageNetworkRPCEvent final
	: public RPC::Event
{
private:
	ServiceReference<GlobalService> &global;

public:
	MessageNetworkRPCEvent(ServiceReference<GlobalService> &g)
		: RPC::Event("anope.messageNetwork", 1)
		, global(g)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (!global)
		{
			request.Error(ERR_NO_GLOBAL_SERVICE, "No global service");
			return true;
		}

		for (const auto &message : request.data)
			global->SendSingle(message);
		return true;
	}
};

class MessageServerRPCEvent final
	: public RPC::Event
{
private:
	ServiceReference<GlobalService> &global;

public:
	MessageServerRPCEvent(ServiceReference<GlobalService> &g)
		: RPC::Event("anope.messageServer", 2)
		, global(g)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (!global)
		{
			request.Error(ERR_NO_GLOBAL_SERVICE, "No global service");
			return true;
		}

		auto *s = Server::Find(request.data[0], true);
		if (!s)
		{
			request.Error(ERR_NO_SUCH_SERVER, "No such server");
			return true;
		}

		std::vector<Anope::string> messages(request.data.begin() + 1, request.data.end());
		for (const auto &message : messages)
			global->SendSingle(message, nullptr, nullptr, s);
		return true;
	}
};

class MessageUserRPCEvent final
	: public RPC::Event
{
public:
	MessageUserRPCEvent()
		: RPC::Event("anope.messageUser", 3)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto *bi = BotInfo::Find(request.data[0], true);
		if (!bi)
		{
			request.Error(ERR_NO_SUCH_SOURCE, "No such source");
			return true;
		}

		auto *u = User::Find(request.data[1], true);
		if (!u)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such target");
			return true;
		}

		u->SendMessage(bi, request.data[2]);
		return true;
	}
};

class ModuleRPCSystem final
	: public Module
{
private:
	ServiceReference<GlobalService> global;
	ServiceReference<RPC::ServiceInterface> rpc;
	MessageNetworkRPCEvent messagenetworkrpcevent;
	MessageServerRPCEvent messageserverrpcevent;
	MessageUserRPCEvent messageuserrpcevent;

public:
	ModuleRPCSystem(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, global("GlobalService", "Global")
		, rpc("RPCServiceInterface", "rpc")
		, messagenetworkrpcevent(global)
		, messageserverrpcevent(global)
	{
		if (!rpc)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

		rpc->Register(&messagenetworkrpcevent);
		rpc->Register(&messageserverrpcevent);
		rpc->Register(&messageuserrpcevent);
	}

	~ModuleRPCSystem() override
	{
		if (!rpc)
			return;

		rpc->Unregister(&messagenetworkrpcevent);
		rpc->Unregister(&messageserverrpcevent);
		rpc->Unregister(&messageuserrpcevent);
	}
};

MODULE_INIT(ModuleRPCSystem)
