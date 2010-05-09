
extern CoreExport bool Mail(User *u, NickRequest *nr, const std::string &service, const std::string &subject, const std::string &message);
extern CoreExport bool Mail(User *u, NickCore *nc, const std::string &service, const std::string &subject, const std::string &message);
extern CoreExport bool Mail(NickCore *nc, const std::string &subject, const std::string &message);
extern CoreExport bool MailValidate(const std::string &email);

class MailThread : public Thread
{
 private:
	const std::string MailTo;
	const std::string Addr;
	const std::string Subject;
	const std::string Message;
	
	bool Success;
 public:
	MailThread(const std::string &mailto, const std::string &addr, const std::string &subject, const std::string &message) : Thread(), MailTo(mailto), Addr(addr), Subject(subject), Message(message), Success(false)
	{
	}

	~MailThread();

	void Run();
};
