#ifndef BOTSERV_H
#define BOTSERV_H

struct UserData
{
	UserData()
	{
		this->Clear();
	}

	void Clear()
	{
		last_use = last_start = Anope::CurTime;
		lines = times = 0;
		lastline.clear();
	}

	/* Data validity */
	time_t last_use;

	/* for flood kicker */
	int16 lines;
	time_t last_start;

	/* for repeat kicker */
	Anope::string lastline;
	Anope::string lasttarget;
	int16 times;
};

struct BanData
{
	Anope::string mask;
	time_t last_use;
	int16 ttb[TTB_SIZE];

	BanData()
	{
		this->Clear();
	}

	void Clear()
	{
		last_use = 0;
		for (int i = 0; i < TTB_SIZE; ++i)
			this->ttb[i] = 0;
	}
};

class BotServService : public Service
{
 public:
	BotServService(Module *m) : Service(m, "BotServ") { }

	virtual UserData *GetUserData(User *u, Channel *c) = 0;

	virtual BanData *GetBanData(User *u, Channel *c) = 0;
};

static service_reference<BotServService> botserv("BotServ");

#endif // BOTSERV_H

