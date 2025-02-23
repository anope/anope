/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "httpd.h"

#include <variant>

namespace RPC
{
	class Event;
	class Map;
	class Request;
	class ServiceInterface;

	/** Represents possible types of RPC value. */
	using Value = std::variant<Map, Anope::string, std::nullptr_t, bool, double, int64_t, uint64_t>;

	/** Represents standard RPC errors from the JSON-RPC and XML-RPC specifications. */
	enum Error
		: int64_t
	{
		ERR_CUSTOM_END       = -32000,
		ERR_CUSTOM_START     = -32099,
		ERR_PARSE_ERROR      = -32700,
		ERR_INVALID_REQUEST  = -32600,
		ERR_METHOD_NOT_FOUND = -32601,
		ERR_INVALID_PARAMS   = -32602,
	};
}

class RPC::Map
{
public:
	/** Retrieves the list of RPC replies. */
	inline const auto &GetReplies() const { return this->replies; }

	template <typename Stringable>
	inline Map &Reply(const Anope::string &key, const Stringable &value)
	{
		this->replies.emplace(key, Anope::ToString(value));
		return *this;
	}

	inline Map &ReplyMap(const Anope::string &key)
	{
		auto it = this->replies.emplace(key, Map());
		return std::get<Map>(it.first->second);
	}

	inline Map &ReplyBool(const Anope::string &key, bool value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline Map &ReplyFloat(const Anope::string &key, double value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline Map &ReplyInt(const Anope::string &key, int64_t value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline Map &ReplyNull(const Anope::string &key)
	{
		this->replies.emplace(key, nullptr);
		return *this;
	}

	inline Map &ReplyUInt(const Anope::string &key, uint64_t value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

private:
	Anope::map<Value> replies;
};

class RPC::Request final
	: public RPC::Map
{
private:
	std::optional<std::pair<int64_t, Anope::string>> error;

public:
	Anope::string name;
	Anope::string id;
	std::deque<Anope::string> data;
	HTTPReply &reply;

	Request(HTTPReply &r)
		: reply(r)
	{
	}

	inline void Error(uint64_t errcode, const Anope::string &errstr)
	{
		this->error.emplace(errcode, errstr);
	}

	inline const auto &GetError() const { return this->error; }
};

class RPC::Event
{
private:
	Anope::string event;

protected:
	Event(const Anope::string& e)
		: event(e)
	{
	}

public:
	virtual ~Event() = default;

	const auto &GetEvent() const { return event; }

	virtual bool Run(ServiceInterface *iface, HTTPClient *client, Request &request) = 0;
};

class RPC::ServiceInterface
	: public Service
{
public:
	ServiceInterface(Module *creator, const Anope::string &sname)
		: Service(creator, "RPCServiceInterface", sname)
	{
	}

	virtual bool Register(Event *event) = 0;

	virtual bool Unregister(Event *event) = 0;

	virtual void Reply(Request &request) = 0;
};
