#include "module.h"
#include "async_commands.h"

static Pipe *me;
static CommandMutex *current_command = NULL;
static std::list<CommandMutex *> commands;
/* Mutex held by the core when it is processing. Used by threads to halt the core */
static Mutex main_mutex;

class AsynchCommandMutex : public CommandMutex
{
 public:
	AsynchCommandMutex() : CommandMutex()
	{
		commands.push_back(this);
		current_command = this;
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
		User *u = this->source.u;
		BotInfo *bi = this->source.owner;

		if (!command->permission.empty() && !u->Account()->HasCommand(command->permission))
		{
			u->SendMessage(bi, LanguageString::ACCESS_DENIED);
			Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command;
		}
		else
		{
			CommandReturn ret = command->Execute(source, params);

			if (ret == MOD_CONT)
			{
				FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, command, params));
			}
		}

		main_mutex.Unlock();
	}

	void Lock()
	{
		this->processing = true;
		me->Notify();
		this->mutex.Lock();
	}

	void Unlock()
	{
		this->processing = false;
		main_mutex.Unlock();
	}
};


class ModuleAsynchCommands : public Module, public Pipe, public AsynchCommandsService
{
 public:
	ModuleAsynchCommands(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), Pipe(), AsynchCommandsService(this, "asynch_commands") 
	{
		me = this;

		this->SetPermanent(true);

		main_mutex.Lock();

		Implementation i[] = { I_OnPreCommand };
		ModuleManager::Attach(i, this, 1);

		ModuleManager::RegisterService(this);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		AsynchCommandMutex *cm = new AsynchCommandMutex();
		try
		{
			cm->mutex.Lock();
			cm->command = command;
			cm->source = source;
			cm->params = params;

			// Give processing to the command thread
			Log(LOG_DEBUG_2) << "Waiting for command thread " << cm->command->name << " from " << source.u->nick;
			threadEngine.Start(cm);
			main_mutex.Lock();

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
		for (std::list<CommandMutex *>::iterator it = commands.begin(), it_end = commands.end(); it != it_end; ++it)
		{
			CommandMutex *cm = *it;

			// Thread engine will pick this up later
			if (cm->GetExitState() || !cm->processing)
				continue;

			Log(LOG_DEBUG_2) << "Waiting for command thread " << cm->command->name << " from " << cm->source.u->nick;
			current_command = cm;

			// Unlock to give processing back to the command thread
			cm->mutex.Unlock();
			// Relock to regain processing once the command thread hangs for any reason
			main_mutex.Lock();

			current_command = NULL;
		}
	}

	CommandMutex *CurrentCommand()
	{
		return current_command;
	}
};

MODULE_INIT(ModuleAsynchCommands)
