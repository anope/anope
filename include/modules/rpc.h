/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "httpd.h"

class RPCRequest final
{
private:
	std::optional<std::pair<int64_t, Anope::string>> error;
	std::map<Anope::string, Anope::string> replies;

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

	inline void Reply(const Anope::string &dname, const Anope::string &ddata)
	{
		this->replies.emplace(dname, ddata);
	}

	inline const auto &GetError() { return this->error; }

	inline const auto &GetReplies() { return this->replies; }
};

class RPCServiceInterface;

class RPCEvent
{
public:
	virtual ~RPCEvent() = default;
	virtual bool Run(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request) = 0;
};

class RPCServiceInterface
	: public Service
{
public:
	RPCServiceInterface(Module *creator, const Anope::string &sname) : Service(creator, "RPCServiceInterface", sname) { }

	virtual void Register(RPCEvent *event) = 0;

	virtual void Unregister(RPCEvent *event) = 0;

	virtual void Reply(RPCRequest &request) = 0;
};
