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
	RPCRequest request;
	HTTPReply repl; /* Request holds a reference to the HTTPReply, because we might exist long enough to invalidate it
	                   we'll copy it here then reset the reference before we use it */
	Reference<HTTPClient> client;
	Reference<RPCServiceInterface> xinterface;

public:
	RPCIdentifyRequest(Module *m, RPCRequest &req, HTTPClient *c, RPCServiceInterface *iface, const Anope::string &acc, const Anope::string &pass)
		: IdentifyRequest(m, acc, pass)
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

		request.Reply("account", GetAccount());

		xinterface->Reply(request);
		client->SendReply(&request.reply);
	}

	void OnFail() override
	{
		if (!xinterface || !client)
			return;

		request.reply = this->repl;

		request.Error(-32000, "Invalid password");

		xinterface->Reply(request);
		client->SendReply(&request.reply);
	}
};

class MyRPCEvent final
	: public RPCEvent
{
public:
	bool Run(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request) override
	{
		if (request.name == "command")
			this->DoCommand(iface, client, request);
		else if (request.name == "checkAuthentication")
			return this->DoCheckAuthentication(iface, client, request);
		else if (request.name == "stats")
			this->DoStats(iface, client, request);
		else if (request.name == "channel")
			this->DoChannel(iface, client, request);
		else if (request.name == "user")
			this->DoUser(iface, client, request);
		else if (request.name == "opers")
			this->DoOperType(iface, client, request);
		else if (request.name == "notice")
			this->DoNotice(iface, client, request);

		return true;
	}

private:
	void DoCommand(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		Anope::string service = request.data.size() > 0 ? request.data[0] : "";
		Anope::string user = request.data.size() > 1 ? request.data[1] : "";
		Anope::string command = request.data.size() > 2 ? request.data[2] : "";

		if (service.empty() || user.empty() || command.empty())
			request.Error(-32602, "Invalid parameters");
		else
		{
			BotInfo *bi = BotInfo::Find(service, true);
			if (!bi)
				request.Error(-32000, "Invalid service");
			else
			{
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
					request.Reply("return", out);
			}
		}
	}

	static bool DoCheckAuthentication(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		Anope::string username = request.data.size() > 0 ? request.data[0] : "";
		Anope::string password = request.data.size() > 1 ? request.data[1] : "";

		if (username.empty() || password.empty())
			request.Error(-32602, "Invalid parameters");
		else
		{
			auto *req = new RPCIdentifyRequest(me, request, client, iface, username, password);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();
			return false;
		}

		return true;
	}

	static void DoStats(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		request.Reply("uptime", Anope::ToString(Anope::CurTime - Anope::StartTime));
		request.Reply("uplinkname", Me->GetLinks().front()->GetName());
		{
			Anope::string buf;
			for (const auto &capab : Servers::Capab)
				buf += " " + capab;
			if (!buf.empty())
				buf.erase(buf.begin());
			request.Reply("uplinkcapab", buf);
		}
		request.Reply("usercount", Anope::ToString(UserListByNick.size()));
		request.Reply("maxusercount", Anope::ToString(MaxUserCount));
		request.Reply("channelcount", Anope::ToString(ChannelList.size()));
	}

	static void DoChannel(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		if (request.data.empty())
			return;

		Channel *c = Channel::Find(request.data[0]);

		request.Reply("name", c ? c->name : request.data[0]);

		if (c)
		{
			request.Reply("bancount", Anope::ToString(c->HasMode("BAN")));
			int count = 0;
			for (auto &ban : c->GetModeList("BAN"))
				request.Reply("ban" + Anope::ToString(++count), ban);

			request.Reply("exceptcount", Anope::ToString(c->HasMode("EXCEPT")));
			count = 0;
			for (auto &except : c->GetModeList("EXCEPT"))
				request.Reply("except" + Anope::ToString(++count), except);

			request.Reply("invitecount", Anope::ToString(c->HasMode("INVITEOVERRIDE")));
			count = 0;
			for (auto &invite : c->GetModeList("INVITEOVERRIDE"))
				request.Reply("invite" + Anope::ToString(++count), invite);

			Anope::string users;
			for (Channel::ChanUserList::const_iterator it = c->users.begin(); it != c->users.end(); ++it)
			{
				ChanUserContainer *uc = it->second;
				users += uc->status.BuildModePrefixList() + uc->user->nick + " ";
			}
			if (!users.empty())
			{
				users.erase(users.length() - 1);
				request.Reply("users", users);
			}

			if (!c->topic.empty())
				request.Reply("topic", c->topic);

			if (!c->topic_setter.empty())
				request.Reply("topicsetter", c->topic_setter);

			request.Reply("topictime", Anope::ToString(c->topic_time));
			request.Reply("topicts", Anope::ToString(c->topic_ts));
		}
	}

	static void DoUser(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		if (request.data.empty())
			return;

		User *u = User::Find(request.data[0]);

		request.Reply("nick", u ? u->nick : request.data[0]);

		if (u)
		{
			request.Reply("ident", u->GetIdent());
			request.Reply("vident", u->GetVIdent());
			request.Reply("host", u->host);
			if (!u->vhost.empty())
				request.Reply("vhost", u->vhost);
			if (!u->chost.empty())
				request.Reply("chost", u->chost);
			request.Reply("ip", u->ip.addr());
			request.Reply("timestamp", Anope::ToString(u->timestamp));
			request.Reply("signon", Anope::ToString(u->signon));
			if (u->IsIdentified())
			{
				request.Reply("account", u->Account()->display);
				if (u->Account()->o)
					request.Reply("opertype", u->Account()->o->ot->GetName());
			}

			Anope::string channels;
			for (User::ChanUserList::const_iterator it = u->chans.begin(); it != u->chans.end(); ++it)
			{
				ChanUserContainer *cc = it->second;
				channels += cc->status.BuildModePrefixList() + cc->chan->name + " ";
			}
			if (!channels.empty())
			{
				channels.erase(channels.length() - 1);
				request.Reply("channels", channels);
			}
		}
	}

	static void DoOperType(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		for (auto *ot : Config->MyOperTypes)
		{
			Anope::string perms;
			for (const auto &priv : ot->GetPrivs())
				perms += " " + priv;
			for (const auto &command : ot->GetCommands())
				perms += " " + command;
			request.Reply(ot->GetName(), perms);
		}
	}

	static void DoNotice(RPCServiceInterface *iface, HTTPClient *client, RPCRequest &request)
	{
		Anope::string from = request.data.size() > 0 ? request.data[0] : "";
		Anope::string to = request.data.size() > 1 ? request.data[1] : "";
		Anope::string message = request.data.size() > 2 ? request.data[2] : "";

		BotInfo *bi = BotInfo::Find(from, true);
		User *u = User::Find(to, true);

		if (!bi || !u || message.empty())
			return;

		u->SendMessage(bi, message);
	}
};

class ModuleRPCMain final
	: public Module
{
	ServiceReference<RPCServiceInterface> rpc;

	MyRPCEvent stats;

public:
	ModuleRPCMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR), rpc("RPCServiceInterface", "rpc")
	{
		me = this;

		if (!rpc)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

		rpc->Register(&stats);
	}

	~ModuleRPCMain() override
	{
		if (rpc)
			rpc->Unregister(&stats);
	}
};

MODULE_INIT(ModuleRPCMain)
