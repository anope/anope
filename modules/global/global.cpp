/* Global core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class GlobalCore final
	: public Module
	, public GlobalService
{
private:
	Reference<BotInfo> global;
	PrimitiveExtensibleItem<std::vector<Anope::string>> queue;

	void ServerGlobal(BotInfo *sender, Server *server, bool children, const Anope::string &message)
	{
		if (server != Me && !server->IsJuped())
			server->Notice(sender, message);

		if (children)
		{
			for (auto *link : server->GetLinks())
				this->ServerGlobal(sender, link, true, message);
		}
	}

public:
	GlobalCore(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, GlobalService(this)
		, queue(this, "global-queue")
	{
	}

	void ClearQueue(NickCore *nc) override
	{
		queue.Unset(nc);
	}

	Reference<BotInfo> GetDefaultSender() const override
	{
		return global;
	}

	const std::vector<Anope::string> *GetQueue(NickCore* nc) const override
	{
		return queue.Get(nc);
	}

	size_t Queue(NickCore *nc, const Anope::string &message) override
	{
		auto *q = queue.Require(nc);
		q->push_back(message);
		return q->size();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const auto glnick = conf->GetModule(this)->Get<const Anope::string>("client");
		if (glnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		auto *bi = BotInfo::Find(glnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + glnick);

		global = bi;
	}

	void OnRestart() override
	{
		const auto msg = Config->GetModule(this)->Get<const Anope::string>("globaloncycledown");
		if (!msg.empty())
			this->SendSingle(msg, nullptr, nullptr, nullptr);
	}

	void OnShutdown() override
	{
		const auto msg = Config->GetModule(this)->Get<const Anope::string>("globaloncycledown");
		if (!msg.empty())
			this->SendSingle(msg, nullptr, nullptr, nullptr);
	}

	void OnNewServer(Server *s) override
	{
		const auto msg = Config->GetModule(this)->Get<const Anope::string>("globaloncycleup");
		if (!msg.empty() && !Me->IsSynced())
			s->Notice(global, msg);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *global)
			return EVENT_CONTINUE;

		source.Reply(_("%s commands:"), global->nick.c_str());
		return EVENT_CONTINUE;
	}

	bool SendQueue(CommandSource &source, BotInfo *sender, Server *server) override
	{
		// We MUST have an account.
		if (!source.nc)
			return false;

		// We MUST have a message queue.
		auto *q = queue.Get(source.nc);
		if (!q || q->empty())
			return false;

		auto success = true;
		for (const auto &message : *q)
		{
			if (!SendSingle(message, &source, sender, server))
			{
				success = false;
				break;
			}
		}

		queue.Unset(source.nc);
		return success;
	}

	bool SendSingle(const Anope::string &message, CommandSource *source, BotInfo* sender, Server* server) override
	{
		// We MUST have a sender.
		if (sender)
			sender = global;
		if (!sender)
			return false;

		Anope::string line;
		if (source && !Config->GetModule(this)->Get<bool>("anonymousglobal"))
		{
			// A source is available and they're not anonymous.
			line = Anope::printf("[%s] %s", source->GetNick().c_str(), message.c_str());
		}
		else
		{
			// A source isn't available or they're anonymous.
			line = message.empty() ? " " : message;
		}

		if (server)
			this->ServerGlobal(sender, server, false, line);
		else
			this->ServerGlobal(sender, Servers::GetUplink(), true, line);
		return true;
	}

	bool Unqueue(NickCore *nc, size_t idx) override
	{
		auto *q = queue.Get(nc);
		if (!q || idx > q->size())
			return false;

		q->erase(q->begin() + idx);
		if (q->empty())
			queue.Unset(nc);

		return true;
	}
};

MODULE_INIT(GlobalCore)
