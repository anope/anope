/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/rpc.h"
#include "modules/httpd.h"

#include "yyjson/yyjson.c"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

inline Anope::string yyjson_get_astr(yyjson_val *val, const char *key)
{
	const auto *str = yyjson_get_str(yyjson_obj_get(val, key));
	return str ? str : "";
}

class MyJSONRPCServiceInterface final
	: public RPC::ServiceInterface
	, public HTTPPage
{
private:
	Anope::map<RPC::Event *> events;

	static void SendError(HTTPReply &reply, int64_t code, const Anope::string &message, const Anope::string &id)
	{
		Log(LOG_DEBUG) << "JSON-RPC error " << code << ": " << message;

		// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
		auto* doc = yyjson_mut_doc_new(nullptr);

		auto* root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		auto *error = yyjson_mut_obj(doc);
		yyjson_mut_obj_add_sint(doc, error, "code", code);
		yyjson_mut_obj_add_strn(doc, error, "message", message.c_str(), message.length());

		yyjson_mut_obj_add_val(doc, root, "error", error);
		yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");

		if (id.empty())
			yyjson_mut_obj_add_null(doc, root, "id");
		else
			yyjson_mut_obj_add_strn(doc, root, "id", id.c_str(), id.length());

		auto *json = yyjson_mut_write(doc, YYJSON_WRITE_ALLOW_INVALID_UNICODE | YYJSON_WRITE_NEWLINE_AT_END, nullptr);
		if (json)
		{
			reply.Write(json);
			free(json);
		}
		yyjson_mut_doc_free(doc);
	}

	static yyjson_mut_val *SerializeElement(yyjson_mut_doc *doc, const RPC::Value &value);

	static void SerializeArray(yyjson_mut_doc *doc, yyjson_mut_val *value, const RPC::Array &array)
	{
		for (const auto &elem : array.GetReplies())
		{
			auto *obj = SerializeElement(doc, elem);
			yyjson_mut_arr_add_val(value, obj);
		}
	}

	static void SerializeMap(yyjson_mut_doc *doc, yyjson_mut_val *value, const RPC::Map &map)
	{
		for (const auto &[k, v] : map.GetReplies())
		{
			auto *obj = SerializeElement(doc, v);
			yyjson_mut_obj_add_val(doc, value, k.c_str(), obj);
		}
	}

public:
	MyJSONRPCServiceInterface(Module *creator, const Anope::string &sname)
		: RPC::ServiceInterface(creator, sname)
		, HTTPPage("/jsonrpc", "application/json")
	{
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
		auto *doc = yyjson_read(message.content.c_str(), message.content.length(), YYJSON_READ_ALLOW_TRAILING_COMMAS | YYJSON_READ_ALLOW_INVALID_UNICODE);
		if (!doc)
		{
			SendError(reply, RPC::ERR_PARSE_ERROR, "JSON parse error", "");
			return true;
		}

		auto *root = yyjson_doc_get_root(doc);
		if (!yyjson_is_obj(root))
		{
			// TODO: handle an array of JSON-RPC requests
			yyjson_doc_free(doc);
			SendError(reply, RPC::ERR_INVALID_REQUEST, "Wrong JSON root element", "");
			return true;
		}

		const auto id = yyjson_get_astr(root, "id");
		const auto jsonrpc = yyjson_get_astr(root, "jsonrpc");
		if (!jsonrpc.empty() && jsonrpc != "2.0")
		{
			yyjson_doc_free(doc);
			SendError(reply, RPC::ERR_INVALID_REQUEST, "Unsupported JSON-RPC version", id);
			return true;
		}

		RPC::Request request(reply);
		request.id = id;
		request.name = yyjson_get_astr(root, "method");

		auto *params = yyjson_obj_get(root, "params");
		size_t idx, max;
		yyjson_val *val;
		yyjson_arr_foreach(params, idx, max, val)
		{
			const auto *str = yyjson_get_str(val);
			request.data.push_back(str ? str : "");
		}

		yyjson_doc_free(doc);

		auto event = this->events.find(request.name);
		if (event == this->events.end())
		{
			SendError(reply, RPC::ERR_METHOD_NOT_FOUND, "Method not found", id);
			return true;
		}

		if (!event->second->Run(this, client, request))
			return false;

		this->Reply(request);
		return true;
	}

	void Reply(RPC::Request &request) override
	{
		if (request.GetError())
		{
			SendError(request.reply, request.GetError()->first, request.GetError()->second, request.id);
			return;
		}

		// {"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]}
		auto* doc = yyjson_mut_doc_new(nullptr);

		auto* root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		if (request.id.empty())
			yyjson_mut_obj_add_null(doc, root, "id");
		else
			yyjson_mut_obj_add_strn(doc, root, "id", request.id.c_str(), request.id.length());

		if (!request.GetReplies().empty())
		{
			auto *result = yyjson_mut_obj(doc);
			SerializeMap(doc, result, request);
			yyjson_mut_obj_add_val(doc, root, "result", result);
		}

		yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");

		auto *json = yyjson_mut_write(doc, YYJSON_WRITE_ALLOW_INVALID_UNICODE | YYJSON_WRITE_NEWLINE_AT_END, nullptr);
		if (json)
		{
			request.reply.Write(json);
			free(json);
		}
		yyjson_mut_doc_free(doc);
	}
};

yyjson_mut_val *MyJSONRPCServiceInterface::SerializeElement(yyjson_mut_doc *doc, const RPC::Value &value)
{
	yyjson_mut_val *elem;
	std::visit(overloaded
	{
		[&doc, &elem](const RPC::Array &a)
		{
			elem = yyjson_mut_arr(doc);
			SerializeArray(doc, elem, a);
		},
		[&doc, &elem](const RPC::Map &m)
		{
			elem = yyjson_mut_obj(doc);
			SerializeMap(doc, elem, m);
		},
		[&doc, &elem](const Anope::string &s)
		{
			elem = yyjson_mut_strn(doc, s.c_str(), s.length());
		},
		[&doc, &elem](std::nullptr_t)
		{
			elem = yyjson_mut_null(doc);
		},
		[&doc, &elem](bool b)
		{
			elem = yyjson_mut_bool(doc, b);
		},
		[&doc, &elem](double d)
		{
			elem = yyjson_mut_real(doc, d);
		},
		[&doc, &elem](int64_t i)
		{
			elem = yyjson_mut_int(doc, i);
		},
		[&doc, &elem](uint64_t u)
		{
			elem = yyjson_mut_uint(doc, u);
		},
	}, value.Get());
	return elem;
}


class ModuleJSONRPC final
	: public Module
{
private:
	ServiceReference<HTTPProvider> httpref;
	MyJSONRPCServiceInterface jsonrpcinterface;

public:
	ModuleJSONRPC(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, jsonrpcinterface(this, "rpc")
	{
	}

	~ModuleJSONRPC() override
	{
		if (httpref)
			httpref->UnregisterPage(&jsonrpcinterface);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		if (httpref)
			httpref->UnregisterPage(&jsonrpcinterface);

		this->httpref = ServiceReference<HTTPProvider>("HTTPProvider", conf->GetModule(this)->Get<const Anope::string>("server", "httpd/main"));
		if (!httpref)
			throw ConfigException("Unable to find http reference, is httpd loaded?");

		httpref->RegisterPage(&jsonrpcinterface);
	}
};

MODULE_INIT(ModuleJSONRPC)
