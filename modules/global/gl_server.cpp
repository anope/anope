/* Global core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandGLServer final
	: public Command
{
private:
	ServiceReference<GlobalService> global;

	BotInfo *GetSender(CommandSource &source)
	{
		Reference<BotInfo> sender;
		if (global)
			sender = global->GetDefaultSender();
		if (!sender)
			sender = source.service;
		return sender;
	}

public:
	CommandGLServer(Module *creator)
		: Command(creator, "global/server", 1)
		, global("GlobalService", "Global")
	{
		this->SetDesc(_("Send a message to all users on a server"));
		this->SetSyntax(_("\037server\037 [\037message\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!global)
		{
			source.Reply(SERVICE_UNAVAILABLE, source.service->nick.c_str());
			return;
		}

		auto *server = Server::Find(params[0]);
		if (!server || server == Me || server->IsJuped())
		{
			source.Reply(_("Server \002%s\002 is not linked to the network."), params[0].c_str());
			return;
		}

		auto queuesize = global->CountQueue(source.nc);
		if (!queuesize && params.size() < 2)
		{
			source.Reply(GLOBAL_NO_MESSAGE);
			return;
		}

		if (queuesize && params.size() > 1)
		{
			source.Reply(GLOBAL_QUEUE_CONFLICT);
			return;
		}

		if (params.empty())
		{
			// We are sending the message queue.
			global->SendQueue(source, GetSender(source), server);
		}
		else
		{
			// We are sending a single message.
			global->SendSingle(params[1], &source, GetSender(source), server);
			queuesize = 1;
		}

		Log(LOG_ADMIN, source, this) << "to send " << queuesize << " messages to users on " << server->GetName();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Allows sending messages to all users on a server. The message will be sent\n"
			"from \002%s\002.\n"
			"\n"
			"You can either send a message by specifying it as a parameter or provide no\n"
			"parameters to send a previously queued message.\n"
		), GetSender(source)->nick.c_str());
		return true;
	}
};

class GLServer final
	: public Module
{
private:
	CommandGLServer commandglserver;

public:
	GLServer(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandglserver(this)
	{

	}
};

MODULE_INIT(GLServer)
