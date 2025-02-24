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
	CommandRPCEvent()
		: RPC::Event("command")
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
				str += msg + "\n";
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
	CheckAuthenticationRPCEvent()
		: RPC::Event("checkAuthentication")
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
	StatsRPCEvent()
		: RPC::Event("stats")
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

class ChannelRPCEvent final
	: public RPC::Event
{
public:
	ChannelRPCEvent()
		: RPC::Event("channel")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Invalid parameters");
			return true;
		}

		Channel *c = Channel::Find(request.data[0]);

		auto &root = request.Root();
		root.Reply("name", c ? c->name : request.data[0]);

		if (c)
		{
			root.Reply("bancount", c->HasMode("BAN"));
			auto &bans = root.ReplyArray("bans");
			for (auto &ban : c->GetModeList("BAN"))
				bans.Reply(ban);

			root.Reply("exceptcount", c->HasMode("EXCEPT"));
			auto &excepts = root.ReplyArray("excepts");
			for (auto &except : c->GetModeList("EXCEPT"))
				excepts.Reply(except);

			root.Reply("invitecount", c->HasMode("INVITEOVERRIDE"));
			auto &invites = root.ReplyArray("invites");
			for (auto &invite : c->GetModeList("INVITEOVERRIDE"))
				invites.Reply(invite);

			auto &users = root.ReplyArray("users");
			for (const auto &[_, uc] : c->users)
				users.Reply(uc->status.BuildModePrefixList() + uc->user->nick);

			if (!c->topic.empty())
				root.Reply("topic", c->topic);

			if (!c->topic_setter.empty())
				root.Reply("topicsetter", c->topic_setter);

			root.Reply("topictime", c->topic_time);
			root.Reply("topicts", c->topic_ts);
		}
		return true;
	}
};

class UserRPCEvent final
	: public RPC::Event
{
public:
	UserRPCEvent()
		: RPC::Event("user")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Invalid parameters");
			return true;
		}

		User *u = User::Find(request.data[0]);

		auto &root = request.Root();
		root.Reply("nick", u ? u->nick : request.data[0]);

		if (u)
		{
			root.Reply("ident", u->GetIdent());
			root.Reply("vident", u->GetVIdent());
			root.Reply("host", u->host);
			if (!u->vhost.empty())
				root.Reply("vhost", u->vhost);
			if (!u->chost.empty())
				root.Reply("chost", u->chost);
			root.Reply("ip", u->ip.addr());
			root.Reply("timestamp", u->timestamp);
			root.Reply("signon", u->signon);
			if (u->IsIdentified())
			{
				root.Reply("account", u->Account()->display);
				if (u->Account()->o)
					root.Reply("opertype", u->Account()->o->ot->GetName());
			}

			auto &channels = root.ReplyArray("channels");
			for (const auto &[_, cc] : u->chans)
				channels.Reply(cc->status.BuildModePrefixList() + cc->chan->name);
		}
		return true;
	}
};

class OpersRPCEvent final
	: public RPC::Event
{
public:
	OpersRPCEvent()
		: RPC::Event("opers")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root();
		for (auto *ot : Config->MyOperTypes)
		{
			Anope::string perms;
			for (const auto &priv : ot->GetPrivs())
				perms += " " + priv;
			for (const auto &command : ot->GetCommands())
				perms += " " + command;
			root.Reply(ot->GetName(), perms);
		}
		return true;
	}
};

class NoticeRPCEvent final
	: public RPC::Event
{
public:
	NoticeRPCEvent()
		: RPC::Event("notice")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		Anope::string from    = request.data.size() > 0 ? request.data[0] : "";
		Anope::string to      = request.data.size() > 1 ? request.data[1] : "";
		Anope::string message = request.data.size() > 2 ? request.data[2] : "";

		BotInfo *bi = BotInfo::Find(from, true);
		User *u = User::Find(to, true);

		if (!bi || !u || message.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Invalid parameters");
			return true;
		}

		u->SendMessage(bi, message);
		return true;
	}
};

class ModuleRPCMain final
	: public Module
{
private:
	ServiceReference<RPC::ServiceInterface> rpc;
	CommandRPCEvent commandrpcevent;
	CheckAuthenticationRPCEvent checkauthenticationrpcevent;
	StatsRPCEvent statsrpcevent;
	ChannelRPCEvent channelrpcevent;
	UserRPCEvent userrpcevent;
	OpersRPCEvent opersrpcevent;
	NoticeRPCEvent noticerpcevent;

public:
	ModuleRPCMain(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, rpc("RPCServiceInterface", "rpc")
	{
		me = this;

		if (!rpc)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

		rpc->Register(&commandrpcevent);
		rpc->Register(&checkauthenticationrpcevent);
		rpc->Register(&statsrpcevent);
		rpc->Register(&channelrpcevent);
		rpc->Register(&userrpcevent);
		rpc->Register(&opersrpcevent);
		rpc->Register(&noticerpcevent);
	}

	~ModuleRPCMain() override
	{
		if (!rpc)
			return;

		rpc->Unregister(&commandrpcevent);
		rpc->Unregister(&checkauthenticationrpcevent);
		rpc->Unregister(&statsrpcevent);
		rpc->Unregister(&channelrpcevent);
		rpc->Unregister(&userrpcevent);
		rpc->Unregister(&opersrpcevent);
		rpc->Unregister(&noticerpcevent);
	}
};

MODULE_INIT(ModuleRPCMain)
