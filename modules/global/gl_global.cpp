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

class CommandGLGlobal final
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
	CommandGLGlobal(Module *creator)
		: Command(creator, "global/global", 0, 1)
		, global("GlobalService", "Global")
	{
		this->SetDesc(_("Send a message to all users"));
		this->SetSyntax(_("[\037message\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!global)
		{
			source.Reply(SERVICE_UNAVAILABLE, source.service->nick.c_str());
			return;
		}

		auto queuesize = global->CountQueue(source.nc);
		if (!queuesize && params.empty())
		{
			source.Reply(GLOBAL_NO_MESSAGE);
			return;
		}

		if (queuesize && !params.empty())
		{
			source.Reply(GLOBAL_QUEUE_CONFLICT);
			return;
		}

		if (params.empty())
		{
			// We are sending the message queue.
			global->SendQueue(source, GetSender(source));
		}
		else
		{
			// We are sending a single message.
			global->SendSingle(params[0], &source, GetSender(source));
			queuesize = 1;
		}

		Log(LOG_ADMIN, source, this) << "to send " << queuesize << " messages to all users";
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Allows sending messages to all users on the network. The message will be sent "
				"from \002%s\002."
				"\n\n"
				"You can either send a message by specifying it as a parameter or provide no "
				"parameters to send a previously queued message."
			),
			GetSender(source)->nick.c_str());
		return true;
	}
};

class GLGlobal final
	: public Module
{
private:
	CommandGLGlobal commandglglobal;

public:
	GLGlobal(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandglglobal(this)
	{

	}
};

MODULE_INIT(GLGlobal)
