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
	std::map<Anope::string, Anope::string> replies;

public:
	Anope::string name;
	Anope::string id;
	std::deque<Anope::string> data;
	HTTPReply &r;

	RPCRequest(HTTPReply &_r) : r(_r) { }
	inline void reply(const Anope::string &dname, const Anope::string &ddata) { this->replies.emplace(dname, ddata); }
	inline const std::map<Anope::string, Anope::string> &get_replies() { return this->replies; }
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
