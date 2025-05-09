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

#define QUEUE_EMPTY _("You have no messages queued.")

class QueueDelCallback final
	: public NumberList
{
private:
	unsigned deleted = 0;
	ServiceReference<GlobalService> &global;
	CommandSource &source;

public:
	QueueDelCallback(CommandSource &src, ServiceReference<GlobalService>& gl, const Anope::string &list)
		: NumberList(list, true)
		, global(gl)
		, source(src)
	{
	}

	~QueueDelCallback() override
	{
		if (deleted)
			source.Reply(deleted, N_("Deleted %u entry from your message queue.", "Deleted %u entries from your message queue."), deleted);
		else
			source.Reply(_("No matching entries in your message queue."));
	}

	void HandleNumber(unsigned number) override
	{
		if (!number || number > global->CountQueue(source.nc))
			return;

		if (global->Unqueue(source.nc, number - 1))
			deleted++;
	}
};

class CommandGLQueue final
	: public Command
{
private:
	ServiceReference<GlobalService> global;

	void DoAdd(CommandSource &source, const Anope::string &message)
	{
		if (message.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		auto maxqueue = Config->GetModule(this->module).Get<size_t>("maxqueue", "10");
		if (global->CountQueue(source.nc) >= maxqueue)
		{
			source.Reply(_("You can not queue any more messages."));
			return;
		}

		global->Queue(source.nc, message);
		source.Reply(_("Your message has been queued."));
		Log(LOG_ADMIN, source, this) << "to queue: " << message;
	}

	void DoClear(CommandSource &source)
	{
		if (!global->CountQueue(source.nc))
		{
			source.Reply(_("You do not have any queued messages."));
			return;
		}

		global->ClearQueue(source.nc);
		source.Reply(_("Your message queue has been cleared."));
		Log(LOG_ADMIN, source, this) << "to clear their queue.";
	}

	void DoDel(CommandSource &source, const Anope::string &what)
	{
		if (what.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (!global->CountQueue(source.nc))
		{
			source.Reply(QUEUE_EMPTY);
			return;
		}

		QueueDelCallback(source, global, what).Process();
	}

	void DoList(CommandSource &source)
	{
		const auto *q = global->GetQueue(source.nc);
		if (!q || q->empty())
		{
			source.Reply(QUEUE_EMPTY);
			return;
		}

		ListFormatter list(source.nc);
		list.AddColumn(_("Number")).AddColumn(_("Message"));
		for (size_t i = 0; i < q->size(); ++i)
		{
			ListFormatter::ListEntry entry;
			entry["Number"] = Anope::ToString(i + 1);
			entry["Message"] = (*q)[i];
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (const auto &reply : replies)
			source.Reply(reply);
	}

public:
	CommandGLQueue(Module *creator)
		: Command(creator, "global/queue", 1, 2)
		, global("GlobalService", "Global")
	{
		this->SetDesc(_("Manages your pending message queue."));
		this->SetSyntax(_("ADD \037message\037"));
		this->SetSyntax(_("DEL \037entry-num\037"));
		this->SetSyntax("LIST");
		this->SetSyntax("CLEAR");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!global)
		{
			source.Reply(SERVICE_UNAVAILABLE, source.service->nick.c_str());
			return;
		}

		const auto &cmd = params[0];
		const auto &what = params.size() > 1 ? params[1] : "";
		if (cmd.equals_ci("ADD"))
			this->DoAdd(source, what);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, what);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply("");
		source.Reply(_(
				"Allows queueing messages to send to users on the network."
				"\n\n"
				"The \002%s\032ADD\002 command adds the given message to the message queue."
				"\n\n"
				"The \002%s\032CLEAR\002 command clears the message queue."
				"\n\n"
				"The \002%s\032DEL\002 command removes the specified message from the message queue. The "
				"message number can be obtained from the output of the \002%s\032LIST\002 command."
				"\n\n"
				"The \002%s\032LIST\002 command lists all messages that are currently in the message queue."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());
		return true;
	}
};

class GLQueue final
	: public Module
{
private:
	CommandGLQueue commandglqueue;

public:
	GLQueue(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandglqueue(this)
	{

	}
};

MODULE_INIT(GLQueue)
