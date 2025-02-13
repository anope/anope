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

inline Anope::string yyjson_get_astr(yyjson_val *val, const char *key)
{
	const auto *str = yyjson_get_str(yyjson_obj_get(val, key));
	return str ? str : "";
}

class MyJSONRPCServiceInterface final
	: public RPCServiceInterface
	, public HTTPPage
{
private:
	std::deque<RPCEvent *> events;

	void SendError(HTTPReply &reply, int64_t code, const char *message, const Anope::string &id)
	{
		Log(LOG_DEBUG) << "JSON-RPC error " << code << ": " << message;

		// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
		auto* doc = yyjson_mut_doc_new(nullptr);

		auto* root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		auto *error = yyjson_mut_obj(doc);
		yyjson_mut_obj_add_sint(doc, error, "code", code);
		yyjson_mut_obj_add_str(doc, error, "message", message);

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

public:
	MyJSONRPCServiceInterface(Module *creator, const Anope::string &sname)
		: RPCServiceInterface(creator, sname)
		, HTTPPage("/jsonrpc", "application/json")
	{
	}

	void Register(RPCEvent *event) override
	{
		this->events.push_back(event);
	}

	void Unregister(RPCEvent *event) override
	{
		auto it = std::find(this->events.begin(), this->events.end(), event);
		if (it != this->events.end())
			this->events.erase(it);
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) override
	{
		auto *doc = yyjson_read(message.content.c_str(), message.content.length(), YYJSON_READ_ALLOW_TRAILING_COMMAS | YYJSON_READ_ALLOW_INVALID_UNICODE);
		if (!doc)
		{
			SendError(reply, -32700, "JSON parse error", "");
			return true;
		}

		auto *root = yyjson_doc_get_root(doc);
		if (!yyjson_is_obj(root))
		{
			// TODO: handle an array of JSON-RPC requests
			yyjson_doc_free(doc);
			SendError(reply, -32600, "Wrong JSON root element", "");
			return true;
		}

		const auto id = yyjson_get_astr(root, "id");
		const auto jsonrpc = yyjson_get_astr(root, "jsonrpc");
		if (!jsonrpc.empty() && jsonrpc != "2.0")
		{
			yyjson_doc_free(doc);
			SendError(reply, -32600, "Unsupported JSON-RPC version", id);
			return true;
		}

		RPCRequest request(reply);
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

		Log() << "id " << request.id;
		Log() << "name " << request.name;

		yyjson_doc_free(doc);

		for (auto *e : this->events)
		{
			if (!e->Run(this, client, request))
				return false;

			else if (!request.get_replies().empty())
			{
				this->Reply(request);
				return true;
			}
		}

		// If we reached this point nobody handled the event.
		SendError(reply, -32601, "Method not found", id);
		return true;
	}

	void Reply(RPCRequest &request) override
	{
		// {"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]}
		auto* doc = yyjson_mut_doc_new(nullptr);

		auto* root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		if (request.id.empty())
			yyjson_mut_obj_add_null(doc, root, "id");
		else
			yyjson_mut_obj_add_strn(doc, root, "id", request.id.c_str(), request.id.length());

		if (!request.get_replies().empty())
		{
			auto *result = yyjson_mut_obj(doc);
			for (const auto &[k, v] : request.get_replies())
				yyjson_mut_obj_add_strn(doc, result, k.c_str(), v.c_str(), v.length());
			yyjson_mut_obj_add_val(doc, root, "result", result);
		}

		yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");

		auto *json = yyjson_mut_write(doc, YYJSON_WRITE_ALLOW_INVALID_UNICODE | YYJSON_WRITE_NEWLINE_AT_END, nullptr);
		if (json)
		{
			request.r.Write(json);
			free(json);
		}
		yyjson_mut_doc_free(doc);
	}
};

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
