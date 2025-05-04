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
	class Array;
	class Event;
	class Map;
	class Request;
	class ServiceInterface;
	class Value;

	/** Represents possible types of RPC value. */
	using ValueUnion = std::variant<Array, Map, Anope::string, std::nullptr_t, bool, double, int64_t, uint64_t>;

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

class RPC::Array final
{
private:
	std::vector<Value> replies;

public:
	/** Retrieves the list of RPC replies. */
	inline const auto &GetReplies() const { return this->replies; }

	template <typename T>
	inline Array &Reply(const T &t)
	{
		this->replies.emplace_back(RPC::Value(t));
		return *this;
	}

	inline Array &ReplyArray();

	inline Map &ReplyMap();
};

class RPC::Map final
{
private:
	Anope::map<Value> replies;

public:
	/** Retrieves the list of RPC replies. */
	inline const auto &GetReplies() const { return this->replies; }

	template <typename T>
	inline Map &Reply(const Anope::string &key, const T &t)
	{
		this->replies.emplace(key, RPC::Value(t));
		return *this;
	}

	inline Array &ReplyArray(const Anope::string &key);

	inline Map &ReplyMap(const Anope::string &key);
};

class RPC::Value final
{
private:
	RPC::ValueUnion value;

public:
	explicit Value(const ValueUnion &v)
		: value(v)
	{
	}

	explicit Value(const Array &a)
		: value(a)
	{
	}

	explicit Value(const Map &m)
		: value(m)
	{
	}

	explicit Value(std::nullptr_t)
		: value(nullptr)
	{
	}

	explicit Value(bool b)
		: value(b)
	{
	}

	Value(double d)
		: value(d)
	{
	}

	Value(int64_t i)
		: value(i)
	{
	}

	Value(uint64_t u)
		: value(u)
	{
	}

	template <typename T>
	Value(const T &t)
		: value(Anope::ToString(t))
	{
	}

	inline auto &Get() { return this->value; }

	inline const auto &Get() const { return this->value; }
};

class RPC::Request final
{
private:
	std::optional<std::pair<int64_t, Anope::string>> error;
	std::optional<Value> root;

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

	template<typename T = Map>
	inline T &Root();

	inline const auto &GetError() const { return this->error; }

	inline const auto &GetRoot() const { return this->root; }
};

#define RPC_EVENT "RPC::Event"

class RPC::Event
	: public Service
{
private:
	size_t minparams;

protected:
	Event(Module *o, const Anope::string& e, size_t mp = 0)
		: Service(o, RPC_EVENT, e)
		, minparams(mp)
	{
	}

public:
	virtual ~Event() = default;

	const auto &GetMinParams() const { return minparams; }

	virtual bool Run(ServiceInterface *iface, HTTPClient *client, Request &request) = 0;
};

class RPC::ServiceInterface
	: public Service
{
public:
	Anope::map<std::vector<Anope::string>> tokens;

	ServiceInterface(Module *creator)
		: Service(creator, "RPC::ServiceInterface", "rpc")
	{
	}

	bool CanExecute(const Anope::string &header, const Anope::string &method) const
	{
		if (header.compare(0, 7, "Bearer ", 7) != 0)
			return false; // No token provided.

		Anope::string token;
		Anope::B64Decode(header.substr(7), token);

		auto it = tokens.find(token);
		if (it == tokens.end())
			return false; // No valid token.

		for (const auto &glob : it->second)
		{
			if (glob[0] == '~' && Anope::Match(method, glob.substr(1)))
				return false; // Negative match.

			if (Anope::Match(method, glob))
				return true; // Positive match.
		}
		return false; // No match.
	}

	virtual void Reply(Request &request) = 0;
};

inline RPC::Array &RPC::Array::ReplyArray()
{
	auto &reply = this->replies.emplace_back(RPC::Array());
	return std::get<RPC::Array>(reply.Get());
}

inline RPC::Map &RPC::Array::ReplyMap()
{
	auto &reply = this->replies.emplace_back(RPC::Map());
	return std::get<RPC::Map>(reply.Get());
}

inline RPC::Array &RPC::Map::ReplyArray(const Anope::string &key)
{
	auto it = this->replies.emplace(key, RPC::Array());
	return std::get<RPC::Array>(it.first->second.Get());
}

inline RPC::Map &RPC::Map::ReplyMap(const Anope::string &key)
{
	auto it = this->replies.emplace(key, RPC::Map());
	return std::get<RPC::Map>(it.first->second.Get());
}

template<typename T>
inline T &RPC::Request::Root()
{
	if (!this->root.has_value())
		this->root = RPC::Value(T());
	return std::get<T>(this->root.value().Get());
}
