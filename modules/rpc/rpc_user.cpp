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
	// Used by anope.checkCredentials, anope.identify, and anope.command.
	ERR_INVALID_ACCOUNT = RPC::ERR_CUSTOM_START,

	// Used by anope.checkCredentials
	ERR_INVALID_PASSWORD = RPC::ERR_CUSTOM_START + 1,
	ERR_ACCOUNT_SUSPENDED = RPC::ERR_CUSTOM_START + 2,

	// Used by anope.identify
	ERR_INVALID_USER = RPC::ERR_CUSTOM_START + 1,

	// Used by anope.listCommands, and anope.command
	ERR_INVALID_SERVICE = RPC::ERR_CUSTOM_START + 1,

	// Used by anope.command
	ERR_INVALID_COMMAND = RPC::ERR_CUSTOM_START + 2,
};


class AnopeCheckCredentialsRPCEvent final
	: public RPC::Event
{
private:
	class RPCIdentifyRequest final
		: public IdentifyRequest
	{
	private:
		RPC::Request request;
		Reference<HTTP::Client> client;
		Reference<RPC::ServiceInterface> rpcinterface;

	public:
		RPCIdentifyRequest(Module *m, RPC::Request &r, HTTP::Client *c, RPC::ServiceInterface *i, const Anope::string &a, const Anope::string &p)
			: IdentifyRequest(m, a, p, c->GetIP())
			, request(r)
			, client(c)
			, rpcinterface(i)
		{
		}

		void OnSuccess() override
		{
			if (!rpcinterface || !client)
				return;

			auto *na = NickAlias::Find(GetAccount());
			if (!na)
				return; // Should never happen.

			if (na->nc->HasExt("NS_SUSPENDED"))
			{
				request.Error(ERR_ACCOUNT_SUSPENDED, "Account suspended");
				rpcinterface->Reply(request);
				client->SendReply(&request.reply);
				return;
			}

			auto &root = request.Root();
			root.Reply("account", na->nc->display)
				.Reply("confirmed", !na->nc->HasExt("UNCONFIRMED"))
				.Reply("uniqueid", na->nc->GetId());

			rpcinterface->Reply(request);
			client->SendReply(&request.reply);
		}

		void OnFail() override
		{
			if (!rpcinterface || !client)
				return;

			if (NickAlias::Find(GetAccount()))
				request.Error(ERR_INVALID_ACCOUNT, "Invalid account");
			else
				request.Error(ERR_INVALID_PASSWORD, "Invalid password");

			rpcinterface->Reply(request);
			client->SendReply(&request.reply);
		}
	};

public:
	AnopeCheckCredentialsRPCEvent(Module *o)
		: RPC::Event(o, "anope.checkCredentials", 2)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		const auto &username = request.data[0];
		const auto &password = request.data[1];
		if (username.empty() || password.empty())
		{
			request.Error(RPC::ERR_INVALID_PARAMS, "Not enough parameters");
			return true;
		}

		auto *req = new RPCIdentifyRequest(this->owner, request, client, iface, username, password);
		FOREACH_MOD(OnCheckAuthentication, (nullptr, req));
		req->Dispatch();
		return false;
	}
};

class AnopeIdentifyRPCEvent final
	: public RPC::Event
{
public:
	AnopeIdentifyRPCEvent(Module *o)
		: RPC::Event(o, "anope.identify", 2)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		auto *na = request.data[0].is_pos_number_only()
				? NickAlias::FindId(Anope::Convert(request.data[0], 0))
				: NickAlias::Find(request.data[0]);
		if (!na)
		{
			request.Error(ERR_INVALID_ACCOUNT, "No such account");
			return true;
		}

		auto *u = User::Find(request.data[1]);
		if (!u)
		{
			request.Error(ERR_INVALID_USER, "No such user");
			return true;
		}

		u->Identify(na);
		return true;
	}
};

class AnopeListCommandsRPCEvent final
	: public RPC::Event
{
public:
	AnopeListCommandsRPCEvent(Module *o)
		: RPC::Event(o, "anope.listCommands")
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		std::vector<BotInfo *> bots;
		if (request.data.empty())
		{
			for (const auto &[_, bi] : *BotListByNick)
				bots.push_back(bi);
		}
		else
		{
			for (const auto &bot : request.data)
			{
				auto *bi = BotInfo::Find(bot);
				if (!bi)
				{
					request.Error(ERR_INVALID_SERVICE, "No such service");
					return true;
				}
				bots.push_back(bi);
			}
		}

		auto &root = request.Root();
		for (const auto *bi : bots)
		{
			if (bi->commands.empty())
				continue;

			auto &commands = root.ReplyMap(bi->nick);
			for (const auto &[command, info] : bi->commands)
			{
				ServiceReference<Command> cmdref("Command", info.name);
				if (!cmdref)
					continue;

				auto &cmdinfo = commands.ReplyMap(command);
				cmdinfo.Reply("hidden", info.hide)
					.Reply("minparams", cmdref->min_params)
					.Reply("requiresaccount", !cmdref->AllowUnregistered())
					.Reply("requiresuser", cmdref->RequireUser());

				if (info.group.empty())
					cmdinfo.Reply("group", nullptr);
				else
					cmdinfo.Reply("group", info.group);

				if (cmdref->max_params)
					cmdinfo.Reply("maxparams", cmdref->max_params);
				else
					cmdinfo.Reply("maxparams", nullptr);

				if (info.permission.empty())
					cmdinfo.Reply("permission", nullptr);
				else
					cmdinfo.Reply("permission", info.permission);
			}
		}

		return true;
	}
};

class AnopeCommandRPCEvent final
	: public RPC::Event
{
private:
	class RPCCommandReply final
		: public CommandReply
	{
	private:
		RPC::Array &root;

	public:
		RPCCommandReply(RPC::Array &r)
			: root(r)
		{
		}

		void SendMessage(BotInfo *source, const Anope::string &msg) override
		{
			root.Reply(NormalizeBuffer(msg.replace_all_cs("\x1A", "\x20")));
		};
	};

public:
	static bool pretenduser;

	AnopeCommandRPCEvent(Module *o)
		: RPC::Event(o, "anope.command", 3)
	{
	}

	bool Run(RPC::ServiceInterface *iface, HTTP::Client *client, RPC::Request &request) override
	{
		NickAlias *na = nullptr;
		if (!request.data[0].empty())
		{
			na = request.data[0].is_pos_number_only()
				? NickAlias::FindId(Anope::Convert(request.data[0], 0))
				: NickAlias::Find(request.data[0]);
			if (!na)
			{
				request.Error(ERR_INVALID_ACCOUNT, "No such account");
				return true;
			}
		}

		auto *bi = BotInfo::Find(request.data[1], true);
		if (!bi)
		{
			request.Error(ERR_INVALID_SERVICE, "No such service");
			return true;
		}

		Anope::string command;
		for (size_t i = 2; i < request.data.size(); ++i)
		{
			if (!command.empty())
				command.push_back(' ');
			command.append(request.data[i]);
		}

		User *u = nullptr;
		if (pretenduser && na && !na->nc->users.empty())
		{
			// Try and find the nick user first.
			for (auto *user : na->nc->users)
			{
				if (user->nick.equals_ci(na->nick))
				{
					u = user;
					break;
				}
			}

			// No nick user, fallback to the first.
			if (!u)
				u = na->nc->users.front();
		}

		RPCCommandReply reply(request.Root<RPC::Array>());
		CommandSource source(na ? na->nick : "RPC", u, na ? *na->nc : nullptr, &reply, bi, request.id);

		if (!Command::Run(source, command))
			request.Error(ERR_INVALID_COMMAND, "No such command");

		return true;
	}
};

bool AnopeCommandRPCEvent::pretenduser = false;

class ModuleRPCAccount final
	: public Module
{
private:
	AnopeCheckCredentialsRPCEvent anopecheckcredentialsrpcevent;
	AnopeIdentifyRPCEvent anopeidentifyrpcevent;
	AnopeListCommandsRPCEvent anopelistcommandsrpcevent;
	AnopeCommandRPCEvent anopecommandrpcevent;

public:
	ModuleRPCAccount(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, anopecheckcredentialsrpcevent(this)
		, anopeidentifyrpcevent(this)
		, anopelistcommandsrpcevent(this)
		, anopecommandrpcevent(this)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		AnopeCommandRPCEvent::pretenduser = conf.GetModule(this).Get<bool>("pretenduser");
	}
};

MODULE_INIT(ModuleRPCAccount)
