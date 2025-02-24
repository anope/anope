/*
 *
 * (C) 2010-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/rpc.h"

enum
{
	// Used by anope.channel, anope.oper, anope.server, and anope.user
	ERR_NO_SUCH_TARGET = RPC::ERR_CUSTOM_START,
};

class AnopeListChannelsRPCEvent final
	: public RPC::Event
{
public:
	AnopeListChannelsRPCEvent()
		: RPC::Event("anope.listChannels")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root<RPC::Array>();
		for (auto &[_, c] : ChannelList)
			root.Reply(c->name);
		return true;
	}
};

class AnopeChannelRPCEvent final
	: public RPC::Event
{
public:
	AnopeChannelRPCEvent()
		: RPC::Event("anope.channel")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Not enough parameters");
			return true;
		}

		auto *c = Channel::Find(request.data[0]);
		if (!c)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such channel");
			return true;
		}

		auto &root = request.Root();
		root.Reply("created", c->creation_time)
			.Reply("name", c->name)
			.Reply("registered", !!c->ci);

		std::map<char, RPC::Array&> modemap;
		auto &listmodes = root.ReplyMap("listmodes");
		for (auto *cm : ModeManager::GetChannelModes())
		{
			if (cm->type != MODE_LIST)
				continue;

			for (auto &entry : c->GetModeList(cm->name))
			{
				auto *wcm = cm->Wrap(entry);

				auto it = modemap.find(wcm->mchar);
				if (it == modemap.end())
					it = modemap.emplace(wcm->mchar, listmodes.ReplyArray(wcm->mchar)).first;

				it->second.Reply(entry);
			}
		}

		std::vector<Anope::string> modelist = { "+" };
		for (const auto &[mname, mvalue] : c->GetModes())
		{
			auto *cm = ModeManager::FindChannelModeByName(mname);
			if (!cm || cm->type == MODE_LIST)
				continue;

			modelist.front().push_back(cm->mchar);
			if (!mvalue.empty())
				modelist.push_back(mvalue);
		}
		auto &modes = root.ReplyArray("modes");
		for (const auto &modeparam : modelist)
			modes.Reply(modeparam);

		if (c->topic.empty())
			root.Reply("topic", nullptr);
		else
		{
			auto &topic = root.ReplyMap("topic");
			topic.Reply("setat", c->topic_ts)
				.Reply("setby", c->topic_setter)
				.Reply("value", c->topic);
		}

		auto &users = root.ReplyArray("users");
		for (const auto &[_, uc] : c->users)
			users.Reply(uc->status.BuildModePrefixList() + uc->user->nick);

		return true;
	}
};

class AnopeListOpersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListOpersRPCEvent()
		: RPC::Event("anope.listOpers")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root<RPC::Array>();
		for (auto *oper : Oper::opers)
			root.Reply(oper->name);
		return true;
	}
};

class AnopeOperRPCEvent final
	: public RPC::Event
{
public:
	AnopeOperRPCEvent()
		: RPC::Event("anope.oper")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Not enough parameters");
			return true;
		}

		auto *o = Oper::Find(request.data[0]);
		if (!o)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such oper");
			return true;
		}

		auto &root = request.Root();
		root
			.Reply("name", o->name)
			.Reply("operonly", o->require_oper)
			.Reply("password", !o->password.empty());

		if (o->certfp.empty())
			root.Reply("fingerprints", nullptr);
		else
		{
			auto &fingerprints = root.ReplyArray("fingerprints");
			for (const auto &fingerprint : o->certfp)
				fingerprints.Reply(fingerprint);
		}

		if (o->hosts.empty())
			root.Reply("hosts", nullptr);
		else
		{
			auto &hosts = root.ReplyArray("hosts");
			for (const auto &host : o->hosts)
				hosts.Reply(host);
		}

		auto &opertype = root.ReplyMap("opertype");
		opertype.Reply("name", o->ot->GetName());
		{
			auto &commands = opertype.ReplyArray("commands");
			for (const auto &command : o->ot->GetCommands())
				commands.Reply(command);

			auto &privileges = opertype.ReplyArray("privileges");
			for (const auto &privilege : o->ot->GetPrivs())
				privileges.Reply(privilege);
		}

		if (o->vhost.empty())
			root.Reply("vhost", nullptr);
		else
			root.Reply("vhost", o->vhost);

		return true;
	}
};

class AnopeListServersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListServersRPCEvent()
		: RPC::Event("anope.listServers")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root<RPC::Array>();
		for (auto &[_, s] : Servers::ByName)
			root.Reply(s->GetName());
		return true;
	}
};

class AnopeServerRPCEvent final
	: public RPC::Event
{
public:
	AnopeServerRPCEvent()
		: RPC::Event("anope.server")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Not enough parameters");
			return true;
		}

		auto *s = Server::Find(request.data[0]);
		if (!s)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such server");
			return true;
		}

		auto &root = request.Root();
		root.Reply("description", s->GetDescription())
			.Reply("juped", s->IsJuped())
			.Reply("name", s->GetName())
			.Reply("synced", s->IsSynced())
			.Reply("ulined", s->IsULined());

		auto &downlinks = root.ReplyArray("downlinks");
		for (const auto *s : s->GetLinks())
			downlinks.Reply(s->GetName());

		if (IRCD->RequiresID)
			root.Reply("sid", s->GetSID());
		else
			root.Reply("sid", nullptr);

		if (s->GetUplink())
			root.Reply("uplink", s->GetUplink()->GetName());
		else
			root.Reply("uplink", nullptr);

		return true;
	}
};

class AnopeListUsersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListUsersRPCEvent()
		: RPC::Event("anope.listUsers")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		auto &root = request.Root<RPC::Array>();
		for (auto &[_, u] : UserListByNick)
			root.Reply(u->nick);
		return true;
	}
};

class AnopeUserRPCEvent final
	: public RPC::Event
{
public:
	AnopeUserRPCEvent()
		: RPC::Event("anope.user")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTPClient *client, RPC::Request &request) override
	{
		if (request.data.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Not enough parameters");
			return true;
		}

		auto *u = User::Find(request.data[0]);
		if (!u)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such user");
			return true;
		}

		auto &root = request.Root();
		root.Reply("address", u->ip.addr())
			.Reply("host", u->host)
			.Reply("ident", u->GetIdent())
			.Reply("nick", u->nick)
			.Reply("nickchanged", u->timestamp)
			.Reply("real", u->realname)
			.Reply("server", u->server->GetName())
			.Reply("signon", u->signon);

		if (u->IsIdentified())
		{
			auto &account = root.ReplyMap("account");
			account.Reply("display", u->Account()->display)
				.Reply("uniqueid", u->Account()->GetId());

			if (u->Account()->o)
				account.Reply("opertype", u->Account()->o->ot->GetName());
			else
				account.Reply("opertype", nullptr);
		}
		else
		{
			root.Reply("account", nullptr);
		}

		auto &channels = root.ReplyArray("channels");
		for (const auto &[_, cc] : u->chans)
			channels.Reply(cc->status.BuildModePrefixList() + cc->chan->name);

		if (u->chost.empty())
			root.Reply("chost", nullptr);
		else
			root.Reply("chost", u->chost);

		if (u->fingerprint.empty())
			root.Reply("fingerprint", nullptr);
		else
			root.Reply("fingerprint", u->fingerprint);

		std::vector<Anope::string> modelist = { "+" };
		for (const auto &[mname, mvalue] : u->GetModeList())
		{
			auto *um = ModeManager::FindUserModeByName(mname);
			if (!um || um->type == MODE_LIST)
				continue;

			modelist.front().push_back(um->mchar);
			if (!mvalue.empty())
				modelist.push_back(mvalue);
		}
		auto &modes = root.ReplyArray("modes");
		for (const auto &modeparam : modelist)
			modes.Reply(modeparam);

		if (IRCD->RequiresID)
			root.Reply("uid", u->GetUID());
		else
			root.Reply("uid", nullptr);

		if (u->vhost.empty() || u->vhost.equals_cs(u->host))
			root.Reply("vhost", nullptr);
		else
			root.Reply("vhost", u->vhost);

		if (u->GetVIdent().equals_cs(u->GetIdent()))
			root.Reply("vident", nullptr);
		else
			root.Reply("vident", u->GetIdent());

		return true;
	}
};

class ModuleRPCData final
	: public Module
{
private:
	ServiceReference<RPC::ServiceInterface> rpc;

	AnopeListChannelsRPCEvent anopelistchannelsrpcevent;
	AnopeChannelRPCEvent anopechannelrpcevent;

	AnopeListOpersRPCEvent anopelistopersrpcevent;
	AnopeOperRPCEvent anopeoperrpcevent;

	AnopeListServersRPCEvent anopelistserversrpcevent;
	AnopeServerRPCEvent anopeserverrpcevent;

	AnopeListUsersRPCEvent anopelistusersrpcevent;
	AnopeUserRPCEvent anopeuserrpcevent;

public:
	ModuleRPCData(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, rpc("RPCServiceInterface", "rpc")
	{
		if (!rpc)
			throw ModuleException("Unable to find RPC interface, is jsonrpc/xmlrpc loaded?");

		rpc->Register(&anopelistchannelsrpcevent);
		rpc->Register(&anopechannelrpcevent);

		rpc->Register(&anopelistopersrpcevent);
		rpc->Register(&anopeoperrpcevent);

		rpc->Register(&anopelistserversrpcevent);
		rpc->Register(&anopeserverrpcevent);

		rpc->Register(&anopelistusersrpcevent);
		rpc->Register(&anopeuserrpcevent);

	}

	~ModuleRPCData() override
	{
		if (!rpc)
			return;

		rpc->Unregister(&anopelistchannelsrpcevent);
		rpc->Unregister(&anopechannelrpcevent);

		rpc->Unregister(&anopelistopersrpcevent);
		rpc->Unregister(&anopeoperrpcevent);

		rpc->Unregister(&anopelistserversrpcevent);
		rpc->Unregister(&anopeserverrpcevent);

		rpc->Unregister(&anopelistusersrpcevent);
		rpc->Unregister(&anopeuserrpcevent);
	}
};

MODULE_INIT(ModuleRPCData)
