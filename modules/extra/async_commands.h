
class CommandMutex : public Thread
{
 public:
	// Mutex used by this command to allow the core to drop and pick up processing of it at will
	Mutex mutex;
	// Set to true when this thread is processing data that is not thread safe (eg, the command)
	bool processing;
	CommandSource source;
	Command *command;
	std::vector<Anope::string> params;

	CommandMutex(CommandSource &s, Command *c, const std::vector<Anope::string> &p) : Thread(), processing(true), source(s), command(c), params(p) { }

	~CommandMutex() { }

	virtual void Run() = 0;

	virtual void Lock() = 0;
	
	virtual void Unlock() = 0;
};

class AsynchCommandsService : public Service
{
 public:
	AsynchCommandsService(Module *o, const Anope::string &n) : Service(o, n) { }
	virtual CommandMutex *CurrentCommand() = 0;
};

