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

class RPCBlock
{
public:
	/** Represents possible types of RPC value. */
	using RPCValue = std::variant<RPCBlock, Anope::string, std::nullptr_t, bool, double, int64_t, uint64_t>;

	/** Retrieves the list of RPC replies. */
	inline const auto &GetReplies() const { return this->replies; }

	template <typename Stringable>
	inline RPCBlock &Reply(const Anope::string &key, const Stringable &value)
	{
		this->replies.emplace(key, Anope::ToString(value));
		return *this;
	}

	inline RPCBlock &ReplyBlock(const Anope::string &key)
	{
		auto it = this->replies.emplace(key, RPCBlock());
		return std::get<RPCBlock>(it.first->second);
	}

	inline RPCBlock &ReplyBool(const Anope::string &key, bool value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline RPCBlock &ReplyFloat(const Anope::string &key, double value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline RPCBlock &ReplyInt(const Anope::string &key, int64_t value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

	inline RPCBlock &ReplyNull(const Anope::string &key)
	{
		this->replies.emplace(key, nullptr);
		return *this;
	}

	inline RPCBlock &ReplyUInt(const Anope::string &key, uint64_t value)
	{
		this->replies.emplace(key, value);
		return *this;
	}

private:
	Anope::map<RPCValue> replies;
};

class RPCRequest final
	: public RPCBlock
{
private:
	std::optional<std::pair<int64_t, Anope::string>> error;

public:
	Anope::string name;
	Anope::string id;
	std::deque<Anope::string> data;
	HTTPReply &reply;

	RPCRequest(HTTPReply &r)
		: reply(r)
	{
	}

	inline void Error(uint64_t errcode, const Anope::string &errstr)
	{
		this->error.emplace(errcode, errstr);
	}

	inline const auto &GetError() const { return this->error; }
};

class RPCServiceInterface;

class RPCEvent
{
private:
	Anope::string event;

protected:
	RPCEvent(const Anope::string& e)
		: event(e)
	{
	}

public:
	virtual ~RPCEvent() = default;

	const auto &GetEvent() const { return event; }

	virtual bool Run(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request) = 0;
};

class RPCServiceInterface
	: public Service
{
public:
	RPCServiceInterface(Module *creator, const Anope::string &sname) : Service(creator, "RPCServiceInterface", sname) { }

	virtual bool Register(RPCEvent *event) = 0;

	virtual bool Unregister(RPCEvent *event) = 0;

	virtual void Reply(RPCRequest &request) = 0;
};
