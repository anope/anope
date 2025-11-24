// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/global/service.h"

class CommandGLGlobal final
	: public Command
{
private:
	BotInfo *GetSender(CommandSource &source)
	{
		Reference<BotInfo> sender;
		if (Global::service)
			sender = Global::service->GetDefaultSender();
		if (!sender)
			sender = source.service;
		return sender;
	}

public:
	CommandGLGlobal(Module *creator)
		: Command(creator, "global/global", 0, 1)
	{
		this->SetDesc(_("Send a message to all users"));
		this->SetSyntax(_("[\037message\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!Global::service)
		{
			source.Reply(SERVICE_UNAVAILABLE, source.service->nick.c_str());
			return;
		}

		auto queuesize = Global::service->CountQueue(source.nc);
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
			Global::service->SendQueue(source, GetSender(source));
		}
		else
		{
			// We are sending a single message.
			Global::service->SendSingle(params[0], &source, GetSender(source));
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
