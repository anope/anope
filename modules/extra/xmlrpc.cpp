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

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class MyXMLRPCServiceInterface final
	: public RPC::ServiceInterface
	, public HTTPPage
{
private:
	RPC::Events events;

	static void SendError(HTTPReply &reply, xmlrpc_env &env)
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

	static xmlrpc_value *SerializeElement(xmlrpc_env &env, const RPC::Value &value);

	static void SerializeArray(xmlrpc_env &env, xmlrpc_value *value, const RPC::Array &array)
	{
		for (const auto &elem : array.GetReplies())
		{
			auto *obj = SerializeElement(env, elem);
			xmlrpc_array_append_item(&env, value, obj);
		}
	}

	static void SerializeMap(xmlrpc_env &env, xmlrpc_value *value, const RPC::Map &map)
	{
		for (const auto &[k, v] : map.GetReplies())
		{
			auto *obj = SerializeElement(env, v);
			xmlrpc_struct_set_value_n(&env, value, k.c_str(), k.length(), obj);
		}
	}

public:
	// Whether we should use the i8 XML-RPC extension.
	static bool enable_i8;

	// Whether we should use the nil XML-RPC extension.
	static bool enable_nil;

	MyXMLRPCServiceInterface(Module *creator, const Anope::string &sname)
		: RPC::ServiceInterface(creator, sname)
		, HTTPPage("/xmlrpc", "text/xml")
	{
	}

	const RPC::Events &GetEvents() override
	{
		return events;
	}

	bool Register(RPC::Event *event) override
	{
		return this->events.emplace(event->GetEvent(), event).second;
	}

	bool Unregister(RPC::Event *event) override
	{
		return this->events.erase(event->GetEvent()) != 0;
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

		RPC::Request request(reply);

		request.name = method;
		delete method;

		auto event = this->events.find(request.name);
		if (event == this->events.end())
		{
			xmlrpc_env_set_fault(&env, RPC::ERR_METHOD_NOT_FOUND, "Method not found");
			SendError(reply, env);
			xmlrpc_DECREF(params);
			return true;
		}

		auto paramcount = xmlrpc_array_size(&env, params);
		for (auto idx = 0; idx < paramcount; ++idx)
		{
			xmlrpc_value *value = nullptr;
			xmlrpc_array_read_item(&env, params, idx, &value);

			Anope::string param;
			if (xmlrpc_value_type(value) != XMLRPC_TYPE_STRING)
			{
				xmlrpc_env_set_fault(&env, RPC::ERR_INVALID_REQUEST, "Anope XML-RPC only supports strings");
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

		auto *eh = event->second;
		if (request.data.size() < eh->GetMinParams())
		{
			auto error = Anope::printf("Not enough parameters (given %zu, expected %zu)",
				request.data.size(), eh->GetMinParams());
			xmlrpc_env_set_fault(&env, RPC::ERR_INVALID_PARAMS, error.c_str());
			SendError(reply, env);
			return true;
		}

		if (!eh->Run(this, client, request))
			return false;

		this->Reply(request);
		return true;
	}

	void Reply(RPC::Request &request) override
	{
		xmlrpc_env env;
		xmlrpc_env_init(&env);

		if (request.GetError())
		{
			xmlrpc_env_set_fault(&env, request.GetError()->first, request.GetError()->second.c_str());
			SendError(request.reply, env);
			return;
		}

		xmlrpc_value *value;
		if (request.GetRoot())
			value = SerializeElement(env, request.GetRoot().value());
		else if (enable_nil)
			value = xmlrpc_nil_new(&env);
		else
			value = xmlrpc_struct_new(&env);

		auto *response = xmlrpc_mem_block_new(&env, 0);
		xmlrpc_serialize_response(&env, response, value);

		request.reply.Write((char *)xmlrpc_mem_block_contents(response), xmlrpc_mem_block_size(response));

		xmlrpc_DECREF(value);
		xmlrpc_mem_block_free(response);
	}
};

xmlrpc_value *MyXMLRPCServiceInterface::SerializeElement(xmlrpc_env &env, const RPC::Value &value)
{
	xmlrpc_value *elem;
	std::visit(overloaded
	{
		[&env, &elem](const RPC::Array &a)
		{
			elem = xmlrpc_array_new(&env);
			SerializeArray(env, elem, a);
		},
		[&env, &elem](const RPC::Map &m)
		{
			elem = xmlrpc_struct_new(&env);
			SerializeMap(env, elem, m);
		},
		[&env, &elem](const Anope::string &s)
		{
			elem = xmlrpc_string_new_lp(&env, s.length(), s.c_str());
		},
		[&env, &elem](std::nullptr_t)
		{
			elem = enable_nil ? xmlrpc_nil_new(&env) : xmlrpc_struct_new(&env);
		},
		[&env, &elem](bool b)
		{
			elem = xmlrpc_bool_new(&env, b);
		},
		[&env, &elem](double d)
		{
			elem = xmlrpc_double_new(&env, d);
		},
		[&env, &elem](int64_t i)
		{
			// XML-RPC does not support 64-bit integers without the use of an
			// extension.
			if (!enable_i8 && (i <= INT32_MAX && i >= INT32_MIN))
			{
				// We can fit this into a int.
				elem = xmlrpc_int_new(&env, i);
			}
			else if (enable_i8)
			{
				// We can fit this into a i8.
				elem = xmlrpc_i8_new(&env, i);
			}
			else
			{
				// We need to convert this to a string.
				auto s = Anope::ToString(i);
				elem = xmlrpc_string_new_lp(&env, s.length(), s.c_str());
			}
		},
		[&env, &elem](uint64_t u)
		{
			// XML-RPC does not support unsigned data types or 64-bit integers
			// without the use of an extension.
			if (!enable_i8 && u <= INT32_MAX)
			{
				// We can fit this into a int.
				elem = xmlrpc_int_new(&env, u);
			}
			else if (enable_i8 && u <= INT64_MAX)
			{
				// We can fit this into a i8.
				elem = xmlrpc_i8_new(&env, u);
			}
			else
			{
				// We need to convert this to a string.
				auto s = Anope::ToString(u);
				elem = xmlrpc_string_new_lp(&env, s.length(), s.c_str());
			}
		},
	}, value.Get());
	return elem;
}

bool MyXMLRPCServiceInterface::enable_i8 = true;
bool MyXMLRPCServiceInterface::enable_nil = true;

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
		if (env.fault_occurred)
		{
			Anope::string fault(env.fault_string);
			xmlrpc_env_clean(&env);
			throw ModuleException("Failed to initialise libxmlrpc: " + fault);
		}

		unsigned int xmlrpc_major, xmlrpc_minor, xmlrpc_point;
		xmlrpc_version(&xmlrpc_major, &xmlrpc_minor, &xmlrpc_point);
		Log(this) << "Module is running against xmlrpc-c version " << xmlrpc_major << '.' << xmlrpc_minor << '.' << xmlrpc_point;
	}

	~ModuleXMLRPC() override
	{
		if (httpref)
			httpref->UnregisterPage(&xmlrpcinterface);

		xmlrpc_term();
	}

	void OnReload(Configuration::Conf &conf) override
	{
		if (httpref)
			httpref->UnregisterPage(&xmlrpcinterface);

		auto &modconf = conf.GetModule(this);
		MyXMLRPCServiceInterface::enable_i8 = modconf.Get<bool>("enable_i8", "yes");
		MyXMLRPCServiceInterface::enable_nil = modconf.Get<bool>("enable_nil", "yes");

		this->httpref = ServiceReference<HTTPProvider>("HTTPProvider", modconf.Get<const Anope::string>("server", "httpd/main"));
		if (!httpref)
			throw ConfigException("Unable to find http reference, is httpd loaded?");
		httpref->RegisterPage(&xmlrpcinterface);

	}
};

MODULE_INIT(ModuleXMLRPC)
