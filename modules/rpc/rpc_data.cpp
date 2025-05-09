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
	// Used by anope.{account,channel,oper,server,user}.
	ERR_NO_SUCH_TARGET = RPC::ERR_CUSTOM_START,

	// Used by anope.list{Accounts,Channels,Opers,Servers,Users}.
	ERR_NO_SUCH_DETAIL = RPC::ERR_CUSTOM_START,
};

class SaveData final
	: public Serialize::Data
{
public:
	Anope::map<std::stringstream> data;

	std::iostream &operator[](const Anope::string &key) override
	{
		return data[key];
	}

	static void Serialize(const Extensible *e, const Serializable *s, RPC::Map &map)
	{
		SaveData data;
		Extensible::ExtensibleSerialize(e, s, data);
		for (const auto &[k, v] : data.data)
		{
			auto vs = v.str();
			switch (data.GetType(k))
			{
				case Serialize::DataType::BOOL:
					map.Reply(k, Anope::Convert<bool>(vs, false));
					break;
				case Serialize::DataType::FLOAT:
					map.Reply(k, Anope::Convert<double>(vs, 0.0));
					break;
				case Serialize::DataType::INT:
					map.Reply(k, Anope::Convert<int64_t>(vs, 0));
					break;
				case Serialize::DataType::TEXT:
				{
					if (vs.empty())
						map.Reply(k, nullptr);
					else
						map.Reply(k, vs);
					break;
				}
				case Serialize::DataType::UINT:
					map.Reply(k, Anope::Convert<uint64_t>(vs, 0));
					break;
			}
		}
	}
};

class AnopeListAccountsRPCEvent final
	: public RPC::Event
{
public:
	AnopeListAccountsRPCEvent(Module *o)
		: RPC::Event(o, "anope.listAccounts")
	{
	}

	static void GetInfo(NickCore *nc, RPC::Map &root)
	{
		root
			.Reply("display", nc->display)
			.Reply("lastmail", nc->lastmail)
			.Reply("registered", nc->registered)
			.Reply("uniqueid", nc->GetId());

		if (nc->email.empty())
			root.Reply("email", nullptr);
		else
			root.Reply("email", nc->email);

		SaveData::Serialize(nc, nc, root.ReplyMap("extensions"));

		if (nc->language.empty())
			root.Reply("language", nullptr);
		else
			root.Reply("language", nc->language);

		auto &nicks = root.ReplyMap("nicks");
		for (const auto *na : *nc->aliases)
		{
			auto &nick = nicks.ReplyMap(na->nick);
			nick.Reply("lastseen", na->last_seen)
				.Reply("registered", nc->registered);

			SaveData::Serialize(na, na, nick.ReplyMap("extensions"));
			if (na->HasVHost())
			{
				auto &vhost = nick.ReplyMap("vhost");
				vhost
					.Reply("created", na->GetVHostCreated())
					.Reply("creator", na->GetVHostCreator())
					.Reply("host", na->GetVHostHost())
					.Reply("mask", na->GetVHostMask());

				if (na->GetVHostIdent().empty())
					vhost.Reply("ident", nullptr);
				else
					vhost.Reply("ident", na->GetVHostIdent());
			}
			else
				nick.Reply("vhost", nullptr);
		}

		if (nc->o)
		{
			auto &opertype = root.ReplyMap("opertype");
			opertype.Reply("name", nc->o->ot->GetName());

			auto &commands = opertype.ReplyArray("commands");
			for (const auto &command : nc->o->ot->GetCommands())
				commands.Reply(command);

			auto &privileges = opertype.ReplyArray("privileges");
			for (const auto &privilege : nc->o->ot->GetPrivs())
				privileges.Reply(privilege);
		}
		else
			root.Reply("opertype", nullptr);

		auto &users = root.ReplyArray("users");
		for (const auto *u : nc->users)
			users.Reply(u->nick);
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto detail = request.data.empty() ? "name" : request.data[0];
		if (detail.equals_ci("name"))
		{
			auto &root = request.Root<RPC::Array>();
			for (auto &[_, nc] : *NickCoreList)
				root.Reply(nc->display);
		}
		else if (detail.equals_ci("full"))
		{
			auto &root = request.Root<RPC::Map>();
			for (auto &[_, nc] : *NickCoreList)
				GetInfo(nc, root.ReplyMap(nc->display));
		}
		else
		{
			request.Error(ERR_NO_SUCH_DETAIL, "No such detail level");
		}
		return true;
	}
};

class AnopeAccountRPCEvent final
	: public RPC::Event
{
public:
	AnopeAccountRPCEvent(Module *o)
		: RPC::Event(o, "anope.account", 1)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *na = NickAlias::Find(request.data[0]);
		if (!na)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such account");
			return true;
		}

		AnopeListAccountsRPCEvent::GetInfo(na->nc, request.Root());
		return true;
	}
};

class AnopeListChannelsRPCEvent final
	: public RPC::Event
{
public:
	AnopeListChannelsRPCEvent(Module *o)
		: RPC::Event(o, "anope.listChannels")
	{
	}

	static void GetInfo(Channel *c, RPC::Map &root)
	{
		root.Reply("created", c->created)
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
		for (const auto &[mname, mdata] : c->GetModes())
		{
			auto *cm = ModeManager::FindChannelModeByName(mname);
			if (!cm || cm->type == MODE_LIST)
				continue;

			modelist.front().push_back(cm->mchar);
			if (!mdata.value.empty())
				modelist.push_back(mdata.value);
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
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto detail = request.data.empty() ? "name" : request.data[0];
		if (detail.equals_ci("name"))
		{
			auto &root = request.Root<RPC::Array>();
			for (auto &[_, c] : ChannelList)
				root.Reply(c->name);
		}
		else if (detail.equals_ci("full"))
		{
			auto &root = request.Root<RPC::Map>();
			for (auto &[_, c] : ChannelList)
				GetInfo(c, root.ReplyMap(c->name));
		}
		else
		{
			request.Error(ERR_NO_SUCH_DETAIL, "No such detail level");
		}
		return true;
	}
};

class AnopeChannelRPCEvent final
	: public RPC::Event
{
public:
	AnopeChannelRPCEvent(Module *o)
		: RPC::Event(o, "anope.channel", 1)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *c = Channel::Find(request.data[0]);
		if (!c)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such channel");
			return true;
		}

		AnopeListChannelsRPCEvent::GetInfo(c, request.Root());
		return true;
	}
};

class AnopeListOpersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListOpersRPCEvent(Module *o)
		: RPC::Event(o, "anope.listOpers")
	{
	}

	static void GetInfo(Oper *o, RPC::Map &root)
	{
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
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto detail = request.data.empty() ? "name" : request.data[0];
		if (detail.equals_ci("name"))
		{
			auto &root = request.Root<RPC::Array>();
			for (auto *o : Oper::opers)
				root.Reply(o->name);
		}
		else if (detail.equals_ci("full"))
		{
			auto &root = request.Root<RPC::Map>();
			for (auto *o : Oper::opers)
				GetInfo(o, root.ReplyMap(o->name));
		}
		else
		{
			request.Error(ERR_NO_SUCH_DETAIL, "No such detail level");
		}
		return true;
	}
};

class AnopeOperRPCEvent final
	: public RPC::Event
{
public:
	AnopeOperRPCEvent(Module *o)
		: RPC::Event(o, "anope.oper", 1)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *o = Oper::Find(request.data[0]);
		if (!o)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such oper");
			return true;
		}

		AnopeListOpersRPCEvent::GetInfo(o, request.Root());
		return true;
	}
};

class AnopeListServersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListServersRPCEvent(Module *o)
		: RPC::Event(o, "anope.listServers")
	{
	}

	static void GetInfo(Server *s, RPC::Map &root)
	{
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
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto detail = request.data.empty() ? "name" : request.data[0];
		if (detail.equals_ci("name"))
		{
			auto &root = request.Root<RPC::Array>();
			for (auto &[_, s] : Servers::ByName)
				root.Reply(s->GetName());
		}
		else if (detail.equals_ci("full"))
		{
			auto &root = request.Root<RPC::Map>();
			for (auto &[_, s] : Servers::ByName)
				GetInfo(s, root.ReplyMap(s->GetName()));
		}
		else
		{
			request.Error(ERR_NO_SUCH_DETAIL, "No such detail level");
		}
		return true;
	}
};

class AnopeServerRPCEvent final
	: public RPC::Event
{
public:
	AnopeServerRPCEvent(Module *o)
		: RPC::Event(o, "anope.server")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *s = Server::Find(request.data[0]);
		if (!s)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such server");
			return true;
		}

		AnopeListServersRPCEvent::GetInfo(s, request.Root());
		return true;
	}
};

class AnopeListUsersRPCEvent final
	: public RPC::Event
{
public:
	AnopeListUsersRPCEvent(Module *o)
		: RPC::Event(0, "anope.listUsers")
	{
	}

	static void GetInfo(User *u, RPC::Map &root)
	{
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
		for (const auto &[mname, mdata] : u->GetModeList())
		{
			auto *um = ModeManager::FindUserModeByName(mname);
			if (!um || um->type == MODE_LIST)
				continue;

			modelist.front().push_back(um->mchar);
			if (!mdata.value.empty())
				modelist.push_back(mdata.value);
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
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto detail = request.data.empty() ? "name" : request.data[0];
		if (detail.equals_ci("name"))
		{
			auto &root = request.Root<RPC::Array>();
			for (auto &[_, u] : UserListByNick)
				root.Reply(u->nick);
		}
		else if (detail.equals_ci("full"))
		{
			auto &root = request.Root<RPC::Map>();
			for (auto &[_, u] : UserListByNick)
				GetInfo(u, root.ReplyMap(u->nick));
		}
		else
		{
			request.Error(ERR_NO_SUCH_DETAIL, "No such detail level");
		}
		return true;
	}
};

class AnopeUserRPCEvent final
	: public RPC::Event
{
public:
	AnopeUserRPCEvent(Module *o)
		: RPC::Event(o, "anope.user", 1)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *u = User::Find(request.data[0]);
		if (!u)
		{
			request.Error(ERR_NO_SUCH_TARGET, "No such user");
			return true;
		}

		AnopeListUsersRPCEvent::GetInfo(u, request.Root());
		return true;
	}
};

class ModuleRPCData final
	: public Module
{
private:
	AnopeListAccountsRPCEvent anopelistaccountsrpcevent;
	AnopeAccountRPCEvent anopeaccountrpcevent;

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
		, anopelistaccountsrpcevent(this)
		, anopeaccountrpcevent(this)
		, anopelistchannelsrpcevent(this)
		, anopechannelrpcevent(this)
		, anopelistopersrpcevent(this)
		, anopeoperrpcevent(this)
		, anopelistserversrpcevent(this)
		, anopeserverrpcevent(this)
		, anopelistusersrpcevent(this)
		, anopeuserrpcevent(this)
	{
	}

};

MODULE_INIT(ModuleRPCData)
