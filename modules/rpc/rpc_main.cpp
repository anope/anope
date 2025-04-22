/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/rpc.h"

static Module *me;

class RPCIdentifyRequest final
	: public IdentifyRequest
{
	RPC::Request request;
	HTTPReply repl; /* Request holds a reference to the HTTPReply, because we might exist long enough to invalidate it
	                   we'll copy it here then reset the reference before we use it */
	Reference<HTTPClient> client;
	Reference<RPC::ServiceInterface> xinterface;

public:
	RPCIdentifyRequest(Module *m, RPC::Request &req, HTTPClient *c, RPC::ServiceInterface *iface, const Anope::string &acc, const Anope::string &pass)
		: IdentifyRequest(m, acc, pass, c->GetIP())
		, request(req)
		, repl(request.reply)
		, client(c)
		, xinterface(iface)
	{
	}

	void OnSuccess() override
	{
		if (!xinterface || !client)
			return;

		request.reply = this->repl;

		request.Root().Reply("account", GetAccount());

		xinterface->Reply(request);
		client->SendReply(&request.reply);
	}

	void OnFail() override
	{
		if (!xinterface || !client)
			return;

		request.reply = this->repl;

		request.Error(RPC::ERR_CUSTOM_START, "Invalid password");

		xinterface->Reply(request);
		client->SendReply(&request.reply);
	}
};

class CommandRPCEvent final
	: public RPC::Event
{
public:
	CommandRPCEvent(Module *o)
		: RPC::Event(o, "command")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		Anope::string service = request.data.size() > 0 ? request.data[0] : "";
		Anope::string user    = request.data.size() > 1 ? request.data[1] : "";
		Anope::string command = request.data.size() > 2 ? request.data[2] : "";

		if (service.empty() || user.empty() || command.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Invalid parameters");
			return true;
		}

		BotInfo *bi = BotInfo::Find(service, true);
		if (!bi)
		{
			request.Error(RPC::ERR_CUSTOM_START, "Invalid service");
			return true;
		}

		NickAlias *na = NickAlias::Find(user);

		Anope::string out;

		struct RPCommandReply final
			: CommandReply
		{
			Anope::string &str;

			RPCommandReply(Anope::string &s) : str(s) { }

			void SendMessage(BotInfo *source, const Anope::string &msg) override
			{
				str += msg.replace_all_cs("\x1A", "\x20") + "\n";
			};
		}
		reply(out);

		User *u = User::Find(user, true);
		CommandSource source(user, u, na ? *na->nc : NULL, &reply, bi);
		Command::Run(source, command);

		if (!out.empty())
			request.Root().Reply("return", out);

		return true;
	}
};

class CheckAuthenticationRPCEvent final
	: public RPC::Event
{
public:
	CheckAuthenticationRPCEvent(Module *o)
		: RPC::Event(o, "checkAuthentication")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		Anope::string username = request.data.size() > 0 ? request.data[0] : "";
		Anope::string password = request.data.size() > 1 ? request.data[1] : "";

		if (username.empty() || password.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Invalid parameters");
			return true;
		}

		auto *req = new RPCIdentifyRequest(me, request, client, iface, username, password);
		FOREACH_MOD(OnCheckAuthentication, (NULL, req));
		req->Dispatch();
		return false;
	}
};

class StatsRPCEvent final
	: public RPC::Event
{
public:
	StatsRPCEvent(Module *o)
		: RPC::Event(o, "stats")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root();
		root.Reply("uptime", Anope::CurTime - Anope::StartTime);
		root.Reply("uplinkname", Me->GetLinks().front()->GetName());
		{
			auto &uplinkcapab = root.ReplyArray("uplinkcapab");
			for (const auto &capab : Servers::Capab)
				uplinkcapab.Reply(capab);
		}
		root.Reply("usercount", UserListByNick.size());
		root.Reply("maxusercount", MaxUserCount);
		root.Reply("channelcount", ChannelList.size());
		return true;
	}
};

class ModuleRPCMain final
	: public Module
{
private:
	CommandRPCEvent commandrpcevent;
	CheckAuthenticationRPCEvent checkauthenticationrpcevent;
	StatsRPCEvent statsrpcevent;

public:
	ModuleRPCMain(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, commandrpcevent(this)
		, checkauthenticationrpcevent(this)
		, statsrpcevent(this)
	{
		me = this;
	}
};

MODULE_INIT(ModuleRPCMain)
