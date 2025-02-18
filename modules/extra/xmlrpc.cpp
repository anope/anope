/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */


/* RequiredLibraries: xmlrpc */

#include <xmlrpc-c/base.h>

#include "module.h"
#include "modules/rpc.h"
#include "modules/httpd.h"

class MyXMLRPCServiceInterface final
	: public RPCServiceInterface
	, public HTTPPage
{
private:
	std::deque<RPCEvent *> events;

	void SendError(HTTPReply &reply, xmlrpc_env &env)
	{
		Log(LOG_DEBUG) << "XML-RPC error " << env.fault_code << ": " << env.fault_string;

		xmlrpc_env fault;
		xmlrpc_env_init(&fault);

		auto *error = xmlrpc_mem_block_new(&fault, 0);
		xmlrpc_serialize_fault(&fault, error, &env);

		reply.Write((char *)xmlrpc_mem_block_contents(error), xmlrpc_mem_block_size(error));

		xmlrpc_mem_block_free(error);
		xmlrpc_env_clean(&fault);
		xmlrpc_env_clean(&env);
	}

public:
	MyXMLRPCServiceInterface(Module *creator, const Anope::string &sname)
		: RPCServiceInterface(creator, sname)
		, HTTPPage("/xmlrpc", "text/xml")
	{
	}

	void Register(RPCEvent *event) override
	{
		this->events.push_back(event);
	}

	void Unregister(RPCEvent *event) override
	{
		std::deque<RPCEvent *>::iterator it = std::find(this->events.begin(), this->events.end(), event);

		if (it != this->events.end())
			this->events.erase(it);
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) override
	{
		xmlrpc_env env;
		xmlrpc_env_init(&env);

		const char *method = nullptr;
		xmlrpc_value *params = nullptr;
		xmlrpc_parse_call(&env, message.content.c_str(), message.content.length(), &method, &params);

		if (env.fault_occurred)
		{
			SendError(reply, env);
			return true;
		}

		RPCRequest request(reply);

		request.name = method;
		delete method;

		auto paramcount = xmlrpc_array_size(&env, params);
		for (auto idx = 0; idx < paramcount; ++idx)
		{
			xmlrpc_value *value = nullptr;
			xmlrpc_array_read_item(&env, params, idx, &value);

			Anope::string param;
			if (xmlrpc_value_type(value) != XMLRPC_TYPE_STRING)
			{
				// TODO: error;
				xmlrpc_env_set_fault(&env, 0, "Anope XML-RPC only supports strings");
				SendError(reply, env);
				xmlrpc_DECREF(value);
				xmlrpc_DECREF(params);
				return true;
			}

			size_t length = 0;
			const char *string = nullptr;
			xmlrpc_read_string_lp(&env, value, &length, &string);

			if (env.fault_occurred)
			{
				SendError(reply, env);
				xmlrpc_DECREF(value);
				xmlrpc_DECREF(params);
				continue;
			}

			request.data.push_back(Anope::string(string, length));
			xmlrpc_DECREF(value);
		}
		xmlrpc_DECREF(params);

		for (auto *e : this->events)
		{
			if (!e->Run(this, client, request))
				return false;

			if (request.GetError())
			{
				xmlrpc_env_set_fault(&env, request.GetError()->first, request.GetError()->second.c_str());
				SendError(reply, env);
				return true;
			}

			if (!request.GetReplies().empty())
			{
				this->Reply(request);
				return true;
			}
		}

		// If we reached this point nobody handled the event.
		xmlrpc_env_set_fault(&env, -32601, "Method not found");
		SendError(reply, env);
		return true;
	}

	void Reply(RPCRequest &request) override
	{
		xmlrpc_env env;
		xmlrpc_env_init(&env);

		if (request.GetError())
		{
			xmlrpc_env_set_fault(&env, request.GetError()->first, request.GetError()->second.c_str());
			SendError(request.reply, env);
			return;
		}

		auto *value = xmlrpc_struct_new(&env);
		for (const auto &[k, v] : request.GetReplies())
		{
			auto *str = xmlrpc_string_new_lp(&env, v.length(), v.c_str());
			xmlrpc_struct_set_value_n(&env, value, k.c_str(), k.length(), str);
		}

		auto *response = xmlrpc_mem_block_new(&env, 0);
		xmlrpc_serialize_response(&env, response, value);

		request.reply.Write((char *)xmlrpc_mem_block_contents(response), xmlrpc_mem_block_size(response));

		xmlrpc_DECREF(value);
		xmlrpc_mem_block_free(response);
	}
};

class ModuleXMLRPC final
	: public Module
{
private:
	ServiceReference<HTTPProvider> httpref;
	MyXMLRPCServiceInterface xmlrpcinterface;

public:
	ModuleXMLRPC(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, xmlrpcinterface(this, "rpc")
	{
		xmlrpc_env env;
		xmlrpc_env_init(&env);
		xmlrpc_init(&env);
		if (!env.fault_occurred)
			return	;

		Anope::string fault(env.fault_string);
		xmlrpc_env_clean(&env);
		throw ModuleException("Failed to initialise libxmlrpc: " + fault);
	}

	~ModuleXMLRPC() override
	{
		if (httpref)
			httpref->UnregisterPage(&xmlrpcinterface);

		xmlrpc_term();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		if (httpref)
			httpref->UnregisterPage(&xmlrpcinterface);

		this->httpref = ServiceReference<HTTPProvider>("HTTPProvider", conf->GetModule(this)->Get<const Anope::string>("server", "httpd/main"));
		if (!httpref)
			throw ConfigException("Unable to find http reference, is httpd loaded?");

		httpref->RegisterPage(&xmlrpcinterface);
	}
};

MODULE_INIT(ModuleXMLRPC)
