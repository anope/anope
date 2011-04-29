#include "module.h"
#include "async_commands.h"

static bool ignore_pre_command = false;
static Pipe *me;
static CommandMutex *current_command = NULL;
static std::list<CommandMutex *> commands;
/* Mutex held by the core when it is processing. Used by threads to halt the core */
static Mutex main_mutex;

class AsynchCommandMutex : public CommandMutex
{
 public:
	bool destroy;
	bool started;

	AsynchCommandMutex(CommandSource &s, Command *c, const std::vector<Anope::string> &p) : CommandMutex(s, c, p), destroy(false), started(false)
	{
		commands.push_back(this);

		this->mutex.Lock();
	}

	~AsynchCommandMutex()
	{
		std::list<CommandMutex *>::iterator it = std::find(commands.begin(), commands.end(), this);
		if (it != commands.end())
			commands.erase(it);
		if (this == current_command)
			current_command = NULL;
	}

	void Run()
	{
		this->started = true;

		User *u = this->source.u;
		BotInfo *bi = this->source.owner;
	
		ignore_pre_command = true;
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreCommand, OnPreCommand(source, command, params));
		if (MOD_RESULT == EVENT_STOP)
		{
			source.DoReply();
			return;
		}

		if (!command->permission.empty() && !u->HasCommand(command->permission))
		{
			u->SendMessage(bi, _(ACCESS_DENIED));
			Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command;
		}
		else
		{
			CommandReturn ret = command->Execute(source, params);
			if (ret != MOD_STOP)
			{
				FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, command, params));
				source.DoReply();
			}
		}

		main_mutex.Unlock();
	}

	void Lock()
	{
		if (this->destroy)
			this->Exit();

		this->processing = true;
		me->Notify();
		this->mutex.Lock();

		if (this->destroy)
		{
			this->Unlock();
			this->Exit();
		}
	}

	void Unlock()
	{
		this->processing = false;
		main_mutex.Unlock();
	}
	
	void Destroy()
	{
		this->destroy = true;
	}
};


class ModuleAsynchCommands : public Module, public Pipe, public AsynchCommandsService
{
	bool reset;

	void Reset()
	{
		this->reset = false;

		unsigned count = 0, size = commands.size();
		for (std::list<CommandMutex *>::iterator it = commands.begin(); count < size; ++count, ++it)
		{
			AsynchCommandMutex *cm = debug_cast<AsynchCommandMutex *>(*it);
			cm->Destroy();

			new AsynchCommandMutex(cm->source, cm->command, cm->params);
		}
	}

 public:
	ModuleAsynchCommands(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), Pipe(), AsynchCommandsService(this, "asynch_commands"), reset(false)
	{
		me = this;

		this->SetPermanent(true);

		main_mutex.Lock();

		Implementation i[] = { I_OnObjectDestroy, I_OnPreCommand };
		ModuleManager::Attach(i, this, 2);

		ModuleManager::SetPriority(this, PRIORITY_FIRST);

		ModuleManager::RegisterService(this);
	}
	
	void OnObjectDestroy(Base *b)
	{
		for (std::list<CommandMutex *>::iterator it = commands.begin(), it_end = commands.end(); it != it_end; ++it)
		{
			AsynchCommandMutex *cm = debug_cast<AsynchCommandMutex *>(*it);

			if (cm->started && (cm->command == b || cm->source.u == b || cm->source.owner == b || cm->source.service == b || cm->source.ci == b))
				cm->Destroy();
		}

		this->reset = true;
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		if (ignore_pre_command)
		{
			ignore_pre_command = false;
			return EVENT_CONTINUE;
		}
		else if (current_command)
			return EVENT_CONTINUE;
		else if (command->name == "RESTART")
			return EVENT_CONTINUE;

		CommandSource source_copy = source;
		AsynchCommandMutex *cm = new AsynchCommandMutex(source_copy, command, params);

		try
		{
			// Give processing to the command thread
			Log(LOG_DEBUG_2) << "Waiting for command thread " << cm->command->name << " from " << source_copy.u->nick;
			current_command = cm;
			threadEngine.Start(cm);
			main_mutex.Lock();
			current_command = NULL;

			return EVENT_STOP;
		}
		catch (const CoreException &ex)
		{
			delete cm;
			Log() << "Unable to thread for command: " << ex.GetReason();
		}

		return EVENT_CONTINUE;
	}

	void OnNotify()
	{
		for (std::list<CommandMutex *>::iterator it = commands.begin(), it_end = commands.end(); it != it_end;)
		{
			AsynchCommandMutex *cm = debug_cast<AsynchCommandMutex *>(*it++);

			// Thread engine will pick this up later
			if (cm->GetExitState() || !cm->processing)
				continue;
			else if (cm->destroy)
			{
				if (cm->started)
				{
					cm->mutex.Unlock();
					continue;
				}
				else
					delete cm;
			}

			if (this->reset)
			{
				this->Reset();
				return this->OnNotify();
			}

			Log(LOG_DEBUG_2) << "Waiting for command thread " << cm->command->name << " from " << cm->source.u->nick;
			current_command = cm;

			// Unlock to give processing back to the command thread
			PushLanguage("anope", cm->source.u->Account() ? cm->source.u->Account()->language : "");
			if (!cm->started)
			{
				try
				{
					threadEngine.Start(cm);
				}
				catch (const CoreException &)
				{
					delete cm;
					PopLanguage();
					continue;
				}
			}
			else
				cm->mutex.Unlock();
			// Relock to regain processing once the command thread hangs for any reason
			main_mutex.Lock();
			PopLanguage();

			current_command = NULL;
		}
	}

	CommandMutex *CurrentCommand()
	{
		return current_command;
	}
};

MODULE_INIT(ModuleAsynchCommands)
